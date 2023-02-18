#include "BasicScene.h"
#include <Eigen/src/Core/Matrix.h>
#include <edges.h>
#include <memory>
#include <per_face_normals.h>
#include <read_triangle_mesh.h>
#include <utility>
#include <vector>
#include "GLFW/glfw3.h"
#include "Mesh.h"
#include "PickVisitor.h"
#include "Renderer.h"
#include "ObjLoader.h"
#include "IglMeshLoader.h"

#include "igl/per_vertex_normals.h"
#include "igl/per_face_normals.h"
#include "igl/unproject_onto_mesh.h"
#include "igl/edge_flaps.h"
#include "igl/loop.h"
#include "igl/upsample.h"
#include "igl/AABB.h"
#include "igl/parallel_for.h"
#include "igl/shortest_edge_and_midpoint.h"
#include "igl/circulation.h"
#include "igl/edge_midpoints.h"
#include "igl/collapse_edge.h"
#include "igl/edge_collapse_is_valid.h"
#include "igl/write_triangle_mesh.h"

// #include "AutoMorphingModel.h"

#include "file_dialog_open.h"
#include "GLFW/glfw3.h"

using namespace std;
using namespace cg3d;
using namespace Eigen;


BasicScene::BasicScene(std::string name, Display* display) : SceneWithImGui(std::move(name), display)
{
    ImGui::GetIO().IniFilename = nullptr;
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 5.0f;
}

void BasicScene::Init(float fov, int width, int height, float near, float far)
{
    // Set camera list
    camera_list.resize(camera_list.capacity());
    camera_list[0] = Camera::Create("global view", fov, float(width) / height, near, far);
    camera_list[1] = Camera::Create("snake front view", fov, float(width) / height, near, far);
    camera_list[2] = Camera::Create("snake back view", fov, float(width) / height, near, far);

    camera = camera_list[0];
    number_of_cameras = camera_list.size();

    AddChild(root = Movable::Create("root")); // a common (invisible) parent object for all the shapes


    // Camera Setup
    camera->Translate(camera_translation);
    camera->RotateByDegree(degree, Axis::X);
 

    // Create Game Manager
    game_manager = new GameManager();
    game_manager->InitGameManager(root, camera_list);
}

void BasicScene::Update(const Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model)
{
    Scene::Update(program, proj, view, model);
    
    // If ProgramHandler finds known program -> skip default setup
    if (ProgramHandler(program)) {
        program.SetUniform4f("lightColor", 0.8f, 0.3f, 0.0f, 0.5f);
        program.SetUniform4f("Kai", 1.0f, 0.3f, 0.6f, 1.0f);
        program.SetUniform4f("Kdi", 0.5f, 0.5f, 0.0f, 1.0f);
        program.SetUniform1f("specular_exponent", 5.0f);
        program.SetUniform4f("light_position", 0.0, 15.0f, 0.0, 1.0f);
    }
}

void BasicScene::MouseCallback(Viewport* viewport, int x, int y, int button, int action, int mods, int buttonState[])
{
    // Handle ImGui Menu
    if (ImGui::GetIO().WantCaptureMouse) return;

    // Handle coming back 
    if (menu_index != StageMenu) {
        return;
    }
    else if ((x < window_size1.x) && (y < window_size1.y)) {
        return;
    }

    // note: there's a (small) chance the button state here precedes the mouse press/release event

    if (action == GLFW_PRESS) { // default mouse button press behavior
        PickVisitor visitor;
        visitor.Init();
        renderer->RenderViewportAtPos(x, y, &visitor); // pick using fixed colors hack
        auto modelAndDepth = visitor.PickAtPos(x, renderer->GetWindowHeight() - y);
        renderer->RenderViewportAtPos(x, y); // draw again to avoid flickering
        pickedModel = modelAndDepth.first ? std::dynamic_pointer_cast<Model>(modelAndDepth.first->shared_from_this()) : nullptr;
        pickedModelDepth = modelAndDepth.second;
        camera->GetRotation().transpose();
        xAtPress = x;
        yAtPress = y;

        // if (pickedModel)
        //     debug("found ", pickedModel->isPickable ? "pickable" : "non-pickable", " model at pos ", x, ", ", y, ": ",
        //           pickedModel->name, ", depth: ", pickedModelDepth);
        // else
        //     debug("found nothing at pos ", x, ", ", y);

        if (pickedModel && !pickedModel->isPickable)
            pickedModel = nullptr; // for non-pickable models we need only pickedModelDepth for mouse movement calculations later

        if (pickedModel)
            pickedToutAtPress = pickedModel->GetTout();
        else
            cameraToutAtPress = camera->GetTout();
    }
}

void BasicScene::ScrollCallback(Viewport* viewport, int x, int y, int xoffset, int yoffset, bool dragging, int buttonState[])
{
    //// note: there's a (small) chance the button state here precedes the mouse press/release event
    //auto system = camera->GetRotation().transpose();
    //if (pickedModel) {
    //    pickedModel->TranslateInSystem(system, { 0, 0, -float(yoffset) });
    //    pickedToutAtPress = pickedModel->GetTout();
    //}
    //else {
    //    camera->TranslateInSystem(system, { 0, 0, -float(yoffset) });
    //    cameraToutAtPress = camera->GetTout();
    //}

    // Handle ImGui Menu
    if (ImGui::GetIO().WantCaptureMouse) return;

    // Enable scrolling only for global camera
    if (camera_index == 0) {
        camera->Translate({ 0, 0, -float(yoffset) });
        cameraToutAtPress = camera->GetTout();
    }
}

void BasicScene::CursorPosCallback(Viewport* viewport, int x, int y, bool dragging, int* buttonState)
{
    // Handle ImGui Menu
    if (ImGui::GetIO().WantCaptureMouse) return;

    if (dragging) {
        auto system = camera->GetRotation().transpose() * GetRotation();
        auto moveCoeff = camera->CalcMoveCoeff(pickedModelDepth, viewport->width);
        auto angleCoeff = camera->CalcAngleCoeff(viewport->width);
        if (pickedModel) {
            //pickedModel->SetTout(pickedToutAtPress);
            if (buttonState[GLFW_MOUSE_BUTTON_RIGHT] != GLFW_RELEASE)
                pickedModel->TranslateInSystem(system, {-float(xAtPress - x) / moveCoeff, float(yAtPress - y) / moveCoeff, 0});
            if (buttonState[GLFW_MOUSE_BUTTON_MIDDLE] != GLFW_RELEASE)
                pickedModel->RotateInSystem(system, float(xAtPress - x) / angleCoeff, Axis::Z);
            if (buttonState[GLFW_MOUSE_BUTTON_LEFT] != GLFW_RELEASE) {
                pickedModel->RotateInSystem(system, float(xAtPress - x) / angleCoeff, Axis::Y);
                pickedModel->RotateInSystem(system, float(yAtPress - y) / angleCoeff, Axis::X);
            }
        } else {
           // camera->SetTout(cameraToutAtPress);
            if (buttonState[GLFW_MOUSE_BUTTON_RIGHT] != GLFW_RELEASE)
                root->TranslateInSystem(system, {-float(xAtPress - x) / moveCoeff/10.0f, float( yAtPress - y) / moveCoeff/10.0f, 0});
            if (buttonState[GLFW_MOUSE_BUTTON_MIDDLE] != GLFW_RELEASE)
                root->RotateInSystem(system, float(x - xAtPress) / 180.0f, Axis::Z);
            if (buttonState[GLFW_MOUSE_BUTTON_LEFT] != GLFW_RELEASE) {
                root->RotateInSystem(system, float(x - xAtPress) / angleCoeff, Axis::Y);
                root->RotateInSystem(system, float(y - yAtPress) / angleCoeff, Axis::X);
            }
        }
        xAtPress =  x;
        yAtPress =  y;
    }
}

void BasicScene::KeyCallback(Viewport* viewport, int x, int y, int key, int scancode, int action, int mods)
{
    // Handle ImGui Menu
    if (ImGui::GetIO().WantCaptureKeyboard) return;

    auto system = camera->GetRotation().transpose();

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) // NOLINT(hicpp-multiway-paths-covered)
        {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                break;
            case GLFW_KEY_W:
                if (GetAnimate()) {
                    game_manager->snake.MoveUp();
                }
                break;
            case GLFW_KEY_S:
                if (GetAnimate()) {
                    game_manager->snake.MoveDown();
                }
                break;
            case GLFW_KEY_A:
                if (GetAnimate()) {
                    game_manager->snake.MoveLeft();
                }
                break;
            case GLFW_KEY_D:
                if (GetAnimate()) {
                    game_manager->snake.MoveRight();
                }
                break;
            case GLFW_KEY_UP:
                if (GetAnimate()) {
                    if (game_manager->stats.current_movement_speed < game_manager->stats.max_movement_speed) {
                        game_manager->stats.current_movement_speed += 1;
                    }
                    else {
                        cout << "Max movement speed already reached" << endl;
                    }
                }
                break;
            case GLFW_KEY_DOWN:
                if (GetAnimate()) {
                    if (game_manager->stats.current_movement_speed > game_manager->stats.min_movement_speed) {
                        game_manager->stats.current_movement_speed -= 1;
                    }
                    else {
                        cout << "Min movement speed already reached" << endl;
                    }
                }
                break;
            case GLFW_KEY_LEFT:
                if (GetAnimate()) {
                    game_manager->snake.RollLeft();
                }
                break;
            case GLFW_KEY_RIGHT:
                if (GetAnimate()) {
                    game_manager->snake.RollRight();
                }
                break;
            case GLFW_KEY_V:
                SwitchView(true);
                break;
            case GLFW_KEY_B:
                SwitchView(false);
                break;
            case GLFW_KEY_SPACE:
                this->animate = !this->animate;
                cout << this->animate << endl;
                break;
        }
    }
}

void BasicScene::BuildImGui() {
    MenuManager();
}

void BasicScene::SetCamera(int index)
{
    camera = camera_list[index];
    viewport->camera = camera;
}

void BasicScene::ViewportSizeCallback(Viewport* _viewport)
{
    for (auto& cam : camera_list) {
        cam->SetProjection(float(_viewport->width) / float(_viewport->height));
    }

    // note: we don't need to call Scene::ViewportSizeCallback since we are setting the projection of all the cameras
}

void BasicScene::AddViewportCallback(Viewport* _viewport)
{
    viewport = _viewport;

    Scene::AddViewportCallback(viewport);
}


void BasicScene::SwitchView(bool next)
{
    if (next) {
        camera_index = (camera_index + 1) % number_of_cameras;
    }
    else {
        camera_index = (camera_index + 2) % number_of_cameras;
    }
    
    camera = camera_list[camera_index];
    viewport->camera = camera;
}


void BasicScene::MenuManager() {
    width = viewport->width;
    height = viewport->height;
    if (width != 0 && height != 0) {
        buttons_size1 = ImVec2(width / 4, height / 8);
        buttons_size2 = ImVec2(width / 4, height / 16);

        window_size1 = ImVec2((float(width)) * 0.22 , (float(height)) * 0.3);

        font_scale1 = (2.f * width) / 800.f;
        font_scale2 = (1.f * width) / 800.f;
        text_position1 = width * 0.4f;
        text_position2 = width * 0.35f;
        text_position3 = width * 0.3f;
    }

    switch (menu_index)
    {
        case LoginMenu:
            LoginMenuHandler();
            break;
        case MainMenu:
            MainMenuHandler();
            break;
        case ShopMenu:
            ShopMenuHandler();
            break;
        case StatsMenu:
            StatsMenuHandler();
            break;
        case StageSelectionMenu:
            StageSelectionMenuHandler();
            break;
        case HallOfFameMenu:
            HallOfFameMenuHandler();
            break;
        case CreditsMenu:
            CreditsMenuHandler();
            break;
        case StageMenu:
            StageMenuHandler();
            break;
        case StageCompletedMenu:
            StageCompletedMenuHandler();
            break;
        case StageFailedMenu:
            StageFailedMenuHandler();
            break;
        case NewHighScoreMenu:
            NewHighScoreMenuHandler();
            break;
    }
}

void BasicScene::LoginMenuHandler() {
    if (game_manager->sound_manager.playing_index != LoginMenu) {
        game_manager->sound_manager.MusicHandler("opening_theme.mp3");
        game_manager->sound_manager.playing_index = LoginMenu;
    }

    int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    bool* pOpen = nullptr;
    string gui_text;

    ImGui::Begin("Menu", pOpen, flags);
    ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetWindowSize(ImVec2(width, height), ImGuiCond_Always);
    //ImGui::SetWindowSize(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetWindowFontScale(font_scale1);

    ImGui::SetCursorPosX(text_position1);
    ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "S");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), "n");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.0, 0.0, 1.0, 1.0), "a");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "k");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), "e");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.0, 0.0, 1.0, 1.0), " 3");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "D");

    Spacing(5);

    ImGui::SetCursorPosX(text_position2);
    if (display_new_game) {
        if (ImGui::Button("New Game", buttons_size1)) {
            display_new_game = false;
        }
    }
    else {
        ImGui::Text("Your Name: ");

        ImGui::SetCursorPosX(text_position2);
        static char name[21] = "";
        //ImGui::InputTextMultiline("", name, IM_ARRAYSIZE(name), ImVec2(200 * (width / 4), 35 * (height / 8)));
        ImGui::InputTextMultiline("", name, IM_ARRAYSIZE(name), ImVec2((width / 4), (height / 16)));

        Spacing(5);

        ImGui::SetCursorPosX(text_position2);
        if (ImGui::Button("Start New Game", buttons_size1)) {
            display_new_game = true;
            game_manager->stats.NewGame(name);
            game_manager->leaderboard.ResetLeaderboard();
            menu_index = MainMenu;
        }
    }

    Spacing(5);

    ImGui::SetCursorPosX(text_position2);
    if (display_new_game) {
        if (!game_manager->stats.save_data_available) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));

            if (ImGui::Button("Continue Game", buttons_size1)) {
                cout << "No save data available" << endl;
            }
            ImGui::PopStyleColor();
        }
        else {
            if (ImGui::Button("Continue Game", buttons_size1)) {
                // If load detected
                display_new_game = true;
                menu_index = MainMenu;
            }
        }
    }
    else {
        if (ImGui::Button("Cancel", buttons_size1)) {
            display_new_game = true;
        }
    }

    Spacing(10);

    ImGui::SetCursorPosX(text_position2);
    if (ImGui::Button("Exit", buttons_size1)) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    ImGui::End();
}

void BasicScene::MainMenuHandler() {
    if (game_manager->sound_manager.playing_index != MainMenu) {
        game_manager->sound_manager.MusicHandler("main_menu.mp3");
        game_manager->sound_manager.playing_index = MainMenu;
    }

    int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    bool* pOpen = nullptr;
    string gui_text;

    ImGui::Begin("Menu", pOpen, flags);
    ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetWindowSize(ImVec2(width, height), ImGuiCond_Always);
    //ImGui::SetWindowSize(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetWindowFontScale(font_scale1);

    ImGui::SetCursorPosX(text_position1);
    ImGui::TextColored(ImVec4(0.6, 1.0, 0.4, 1.0), "Main Menu");

    Spacing(5);

    // Handel User name
    gui_text = "Welcome back, " + game_manager->stats.user_name;
    ImGui::SetCursorPosX(text_position2);
    ImGui::Text(gui_text.c_str());

    Spacing(5);

    // Handle Gold
    gui_text = "Gold: " + std::to_string(game_manager->stats.gold);
    ImGui::SetCursorPosX(text_position2);
    ImGui::TextColored(ImVec4(1.0, 1.0, 0.0, 1.0), gui_text.c_str());

    Spacing(5);

    ImGui::SetCursorPosX(text_position2);
    if (ImGui::Button("Select Stage", buttons_size2)) {
        menu_index = StageSelectionMenu;
    }

    Spacing(5);

    ImGui::SetCursorPosX(text_position2);
    if (ImGui::Button("Shop", buttons_size2)) {
        last_menu_index = MainMenu;
        menu_index = ShopMenu;
    }

    Spacing(5);

    ImGui::SetCursorPosX(text_position2);
    if (ImGui::Button("Stats", buttons_size2)) {
        menu_index = StatsMenu;
    }

    Spacing(5);

    ImGui::SetCursorPosX(text_position2);
    if (ImGui::Button("Hall Of Fame", buttons_size2)) {
        menu_index = HallOfFameMenu;
    }

    Spacing(5);

    ImGui::SetCursorPosX(text_position2);
    if (ImGui::Button("Credits", buttons_size2)) {
        menu_index = CreditsMenu;
    }

    Spacing(10);

    ImGui::SetCursorPosX(text_position2);
    if (ImGui::Button("Logout", buttons_size1)) {
        game_manager->stats.InitStats();
        game_manager->leaderboard.InitLeaderboard();
        menu_index = LoginMenu;
    }
    
    ImGui::End();
}

void BasicScene::StageSelectionMenuHandler() {
    if (game_manager->sound_manager.playing_index != MainMenu) {
        game_manager->sound_manager.MusicHandler("main_menu.mp3");
        game_manager->sound_manager.playing_index = MainMenu;
    }

    int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    bool* pOpen = nullptr;
    string gui_text;

    ImGui::Begin("Menu", pOpen, flags);
    ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetWindowSize(ImVec2(width, height), ImGuiCond_Always);
    //ImGui::SetWindowSize(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetWindowFontScale(font_scale1);

    ImGui::SetCursorPosX(text_position3);
    ImGui::TextColored(ImVec4(0.6, 1.0, 0.4, 1.0), "Stage Selection Menu");

    Spacing(5);

    // Handle Gold
    ImGui::SetCursorPosX(text_position2);
    gui_text = "Gold: " + std::to_string(game_manager->stats.gold);
    ImGui::TextColored(ImVec4(1.0, 1.0, 0.0, 1.0), gui_text.c_str());

    Spacing(5);

    // Handle Stages
    ImGui::SetCursorPosX(text_position2);
    ImGui::Text("Select Stage: ");

    // Stages buttons
    for (int i = 1; i <= 3; i++) {
        Spacing(5);

        ImGui::SetCursorPosX(text_position2);
        gui_text = "Stage " + std::to_string(i);

        if (ImGui::Button(gui_text.c_str(), buttons_size1)) {
            cout << gui_text.c_str() << endl;
            game_manager->sound_manager.stage_index = i;
            game_manager->LoadStage(i);
            menu_index = StageMenu;
        }
    }

    Spacing(10);

    ImGui::SetCursorPosX(text_position2);
    if (ImGui::Button("Back", buttons_size1)) {
        menu_index = MainMenu;
    }
    ImGui::End();
}

void BasicScene::ShopMenuHandler() {
    if (game_manager->sound_manager.playing_index != ShopMenu) {
        game_manager->sound_manager.MusicHandler("shop.mp3");
        game_manager->sound_manager.playing_index = ShopMenu;
    }

    int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    bool* pOpen = nullptr;
    string gui_text;

    ImGui::Begin("Menu", pOpen, flags);
    ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    //ImGui::SetWindowSize(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetWindowSize(ImVec2(width, height), ImGuiCond_Always);
    ImGui::SetWindowFontScale(font_scale1);

    ImGui::SetCursorPosX(text_position1);
    ImGui::TextColored(ImVec4(0.6, 1.0, 0.4, 1.0), "Shop");

    Spacing(5);

    // Handle Gold
    ImGui::SetCursorPosX(text_position2);
    gui_text = "Gold: " + std::to_string(game_manager->stats.gold);
    ImGui::TextColored(ImVec4(1.0, 1.0, 0.0, 1.0), gui_text.c_str());

    Spacing(5);

    ImGui::SetCursorPosX(text_position2);
    ImGui::Text("Increase Max Health");
    ImGui::SetCursorPosX(text_position2);
    if (ImGui::Button("100 Gold", buttons_size2)) {
        cout << "Increase Max Health" << endl;
    }

    Spacing(5);

    ImGui::SetCursorPosX(text_position2);
    ImGui::Text("X2 Score");
    ImGui::SetCursorPosX(text_position2);
    if (ImGui::Button("60 Gold", buttons_size2)) {
        cout << "Increase X2 Score" << endl;
    }

    Spacing(5);

    ImGui::SetCursorPosX(text_position2);
    ImGui::Text("X2 Gold");
    ImGui::SetCursorPosX(text_position2);
    if (ImGui::Button("70 Gold", buttons_size2)) {
        cout << "Increase X2 Gold" << endl;
    }

    Spacing(5);

    ImGui::SetCursorPosX(text_position2);
    ImGui::Text("Increase Bonuses Duration");
    ImGui::SetCursorPosX(text_position2);
    if (ImGui::Button("50 Gold", buttons_size2)) {
        cout << "Increase Bonuses Duration" << endl;
    }

    Spacing(5);

    ImGui::SetCursorPosX(text_position2);
    ImGui::Text("Increase Max Speed");
    ImGui::SetCursorPosX(text_position2);
    if (ImGui::Button("25 Gold", buttons_size2)) {
        cout << "Increase Max Speed" << endl;
    }

    Spacing(10);

    ImGui::SetCursorPosX(text_position2);
    if (ImGui::Button("Back", buttons_size1)) {
        menu_index = last_menu_index;
    }

    ImGui::End();
}

void BasicScene::StatsMenuHandler() {
    if (game_manager->sound_manager.playing_index != StatsMenu) {
        game_manager->sound_manager.MusicHandler("stats.mp3");
        game_manager->sound_manager.playing_index = StatsMenu;
    }

    int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    bool* pOpen = nullptr;
    string gui_text;

    ImGui::Begin("Menu", pOpen, flags);
    ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    //ImGui::SetWindowSize(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetWindowSize(ImVec2(width, height), ImGuiCond_Always);
    ImGui::SetWindowFontScale(font_scale1);

    ImGui::SetCursorPosX(text_position1);
    ImGui::TextColored(ImVec4(0.6, 1.0, 0.4, 1.0), "Stats");

    Spacing(5);

    ImGui::SetCursorPosX(text_position3);
    ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), "Snake Stats");

    ImGui::SetCursorPosX(text_position3);
    gui_text = "Max Health: " + std::to_string(game_manager->stats.max_health);
    ImGui::Text(gui_text.c_str());

    ImGui::SetCursorPosX(text_position3);
    gui_text = "Score Multiplier: X" + std::to_string(game_manager->stats.score_multiplier);
    ImGui::Text(gui_text.c_str());

    ImGui::SetCursorPosX(text_position3);
    gui_text = "Gold Multiplier: X" + std::to_string(game_manager->stats.gold_multiplier);
    ImGui::Text(gui_text.c_str());

    ImGui::SetCursorPosX(text_position3);
    gui_text = "Bonuses Duration: " + std::to_string(game_manager->stats.bonuses_duration) + " sec";
    ImGui::Text(gui_text.c_str());

    ImGui::SetCursorPosX(text_position3);
    gui_text = "Max Movement Speed: " + std::to_string(game_manager->stats.max_movement_speed);
    ImGui::Text(gui_text.c_str());

    Spacing(5);

    ImGui::SetCursorPosX(text_position3);
    ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "Statistics");

    ImGui::SetCursorPosX(text_position3);
    gui_text = "Total Health Points Healed: " + std::to_string(game_manager->stats.total_health_points_healed);
    ImGui::Text(gui_text.c_str());

    ImGui::SetCursorPosX(text_position3);
    gui_text = "Total Health Points Lost: " + std::to_string(game_manager->stats.total_health_points_lost);
    ImGui::Text(gui_text.c_str());

    ImGui::SetCursorPosX(text_position3);
    gui_text = "Total Score Points Earned: " + std::to_string(game_manager->stats.total_score_points_earned);
    ImGui::Text(gui_text.c_str());

    ImGui::SetCursorPosX(text_position3);
    gui_text = "Total Gold Earned: " + std::to_string(game_manager->stats.total_gold_earned);
    ImGui::Text(gui_text.c_str());

    ImGui::SetCursorPosX(text_position3);
    gui_text = "Total Gold Spent: " + std::to_string(game_manager->stats.total_gold_spent);
    ImGui::Text(gui_text.c_str());

    ImGui::SetCursorPosX(text_position3);
    gui_text = "Total Bonuses Collected: " + std::to_string(game_manager->stats.total_bonuses_collected);
    ImGui::Text(gui_text.c_str());

    ImGui::SetCursorPosX(text_position3);
    gui_text = "Total Deaths: " + std::to_string(game_manager->stats.total_deaths);
    ImGui::Text(gui_text.c_str());

    Spacing(10);

    ImGui::SetCursorPosX(text_position2);
    if (ImGui::Button("Back", buttons_size1)) {
        menu_index = MainMenu;
    }

    ImGui::End();
}

void BasicScene::HallOfFameMenuHandler() {
    if (game_manager->sound_manager.playing_index != HallOfFameMenu) {
        game_manager->sound_manager.MusicHandler("hall_of_fame.mp3");
        game_manager->sound_manager.playing_index = HallOfFameMenu;
    }

    int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    bool* pOpen = nullptr;
    string gui_text;

    ImGui::Begin("Menu", pOpen, flags);
    ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    //ImGui::SetWindowSize(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetWindowSize(ImVec2(width, height), ImGuiCond_Always);
    ImGui::SetWindowFontScale(font_scale1);

    ImGui::SetCursorPosX(text_position2);
    ImGui::TextColored(ImVec4(0.6, 1.0, 0.4, 1.0), "Hall of Fame");

    Spacing(5);

    for (int i = 0; i < 10; i++) {
        ImGui::SetCursorPosX(text_position3);
        string space = "  ";
        if (i == 9) {
            space = " ";
        }
        gui_text = to_string(i + 1) + "." + space +            // Place
            game_manager->leaderboard.leaderboard_list[i].first + " - " +    // Name
            to_string(game_manager->leaderboard.leaderboard_list[i].second); // Score

        
        if (i == 0) {
            ImGui::TextColored(ImVec4(201.f / 176.f, 149.f / 255.f, 55.f / 255.f, 1.0), gui_text.c_str());
        }
        else if (i == 1) {
            ImGui::TextColored(ImVec4(180.f / 255.f, 180.f / 255.f, 180.f / 255.f, 1.0), gui_text.c_str());
        }
        else if (i == 2) {
            ImGui::TextColored(ImVec4(173.f / 255.f, 138.f / 255.f, 86.f / 255.f, 1.0), gui_text.c_str());
        }
        else {
            ImGui::Text(gui_text.c_str());
        }
    }

    Spacing(10);

    ImGui::SetCursorPosX(text_position2);
    if (ImGui::Button("Back", buttons_size1)) {
        menu_index = MainMenu;
    }

    ImGui::End();
}

void BasicScene::CreditsMenuHandler() {
    if (game_manager->sound_manager.playing_index != CreditsMenu) {
        game_manager->sound_manager.MusicHandler("credits.mp3");
        game_manager->sound_manager.playing_index = CreditsMenu;
    }

    int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    bool* pOpen = nullptr;
    string gui_text;

    ImGui::Begin("Menu", pOpen, flags);
    ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    //ImGui::SetWindowSize(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetWindowSize(ImVec2(width, height), ImGuiCond_Always);
    ImGui::SetWindowFontScale(font_scale1);

    ImGui::SetCursorPosX(text_position1);
    ImGui::TextColored(ImVec4(0.6, 1.0, 0.4, 1.0), "Credits");

    Spacing(10);

    ImGui::SetCursorPosX(text_position3);
    ImGui::Text("Created by: Ofir Gilad");

    Spacing(5);

    ImGui::SetCursorPosX(text_position3);
    ImGui::Text("Course Teacher: Tamir Grossinger");

    Spacing(5);

    ImGui::SetCursorPosX(text_position3);
    ImGui::Text("Game Engine: OpenGL");

    Spacing(5);

    ImGui::SetCursorPosX(text_position3);
    if (payed_credits) {
        gui_text = "Honorable Contributor: " + game_manager->stats.user_name;
        ImGui::Text(gui_text.c_str());
    }
    else {
        if (ImGui::Button("Contribute Credit")) {
            payed_credits = true;
        }
    }

    Spacing(10);

    ImGui::SetCursorPosX(text_position2);
    if (ImGui::Button("Back", buttons_size1)) {
        menu_index = MainMenu;
    }

    ImGui::End();
}

void BasicScene::StageMenuHandler() {
    string stage_music;
    if (game_manager->sound_manager.playing_index != -game_manager->sound_manager.stage_index) {
        stage_music = "stage" + std::to_string(game_manager->sound_manager.stage_index) + ".mp3";
        game_manager->sound_manager.MusicHandler(stage_music);
        game_manager->sound_manager.playing_index = -game_manager->sound_manager.stage_index;
    }

    int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    bool* pOpen = nullptr;
    string gui_text;

    ImGui::Begin("Menu", pOpen, flags);
    ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetWindowSize(window_size1, ImGuiCond_Always);
    //ImGui::SetWindowFontScale(font_scale1);
    ImGui::SetWindowFontScale(font_scale2);

    ImGui::TextColored(ImVec4(1.0, 0.5, 1.0, 1.0), "Stage Menu");

    Spacing(1);

    // Handle Stage
    gui_text = "Stage: " + std::to_string(game_manager->selected_stage);
    ImGui::TextColored(ImVec4(0.0, 0.0, 1.0, 1.0), gui_text.c_str());

    Spacing(1);

    // Handle Health
    gui_text = "Health: " + std::to_string(game_manager->stats.current_health);
    ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), gui_text.c_str());

    Spacing(1);

    // Handle Score
    gui_text = "Score: " + std::to_string(game_manager->stats.current_score);
    ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), gui_text.c_str());

    Spacing(1);

    // Handle Gold
    gui_text = "Gold: " + std::to_string(game_manager->stats.gold);
    ImGui::TextColored(ImVec4(1.0, 1.0, 0.0, 1.0), gui_text.c_str());
    
    Spacing(1);

    // Handle View
    ImGui::Text("Camera List: ");
    for (int i = 0; i < camera_list.size(); i++) {
        ImGui::SameLine(0);
        bool selected_camera = camera_list[i] == camera;
        if (selected_camera) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
        }
        if (ImGui::Button(std::to_string(i + 1).c_str())) {
            SetCamera(i);
        }
        if (selected_camera) {
            ImGui::PopStyleColor();
        }
    }

    Spacing(1);

    if (!display_keys) {
        if (ImGui::Button("Show Keys")) {
            display_keys = true;
        }
    }
    else {
        if (ImGui::Button("Hide Keys")) {
            display_keys = false;
        }
        ImGui::Text("Keyboard Keys: ");
        ImGui::Text("W - Move snake up");
        ImGui::Text("S - Move snake down");
        ImGui::Text("A - Move snake left");
        ImGui::Text("D - Move snake right");
        ImGui::Text("V - Switch view forward");
        ImGui::Text("B - Switch view backward");
        ImGui::Text("UP - Switch camera view up");
        ImGui::Text("DOWN - Switch camera view down");
        ImGui::Text("LEFT - Rotate snake left");
        ImGui::Text("RIGHT - Rotate snake right");
        ImGui::Text("ESC - Exit game");
    }

    Spacing(1);

    if (ImGui::Button("Pause")) {
        animate = false;
        cout << "Pause Game" << endl;
    }

    Spacing(2);

    if (ImGui::Button("Back To Main Menu")) {
        game_manager->UnloadStage();
        menu_index = MainMenu;
    }


    if (game_manager->stats.current_health == 0) {
        game_manager->sound_manager.SoundHandler("game_over.mp3");
        menu_index = StageFailedMenu;
    }

    ImGui::End();
}

void BasicScene::StageCompletedMenuHandler() {
    // Menu with no sound
    game_manager->sound_manager.StopMusic();

    int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    bool* pOpen = nullptr;
    string gui_text;

    ImVec2 window_pos_x = ImVec2(float(width - 0.25 * width) / 2.f, float(height - 0.4 * height) / 2.f);
    ImVec2 window_size_x = ImVec2((float(width)) * 0.22, (float(height)) * 0.4);

    float position_x1x = float(width) * 0.045f;
    float position_x2 = float(width) * 0.03f;

    ImVec2 buttons_size_x = ImVec2(width / 6, height / 16);

    ImGui::Begin("Menu", pOpen, flags);
    ImGui::SetWindowPos(window_pos_x, ImGuiCond_Always);
    ImGui::SetWindowSize(window_size_x, ImGuiCond_Always);
    //ImGui::SetWindowFontScale(font_scale1);
    ImGui::SetWindowFontScale(font_scale2);

    ImGui::SetCursorPosX(position_x1x);
    ImGui::TextColored(ImVec4(0.0, 1.0, 0.5, 1.0), "Stage Completed");

    Spacing(5);

    // Handle Score
    ImGui::SetCursorPosX(position_x2);
    gui_text = "Score: " + std::to_string(game_manager->stats.current_score);
    ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), gui_text.c_str());

    Spacing(1);

    // Handle Gold
    ImGui::SetCursorPosX(position_x2);
    gui_text = "Gold: " + std::to_string(game_manager->stats.gold);
    ImGui::TextColored(ImVec4(1.0, 1.0, 0.0, 1.0), gui_text.c_str());

    Spacing(5);

    // If there is no more stages -> Display end game -> Move to credits
    ImGui::SetCursorPosX(position_x2);
    if (ImGui::Button("Continue", buttons_size_x)) {
        menu_index = StageMenu;
    }

    Spacing(5);

    ImGui::SetCursorPosX(position_x2);
    if (ImGui::Button("Shop", buttons_size_x)) {
        last_menu_index = StageCompletedMenu;
        menu_index = ShopMenu;
    }

    Spacing(5);

    ImGui::SetCursorPosX(position_x2);
    if (ImGui::Button("Back To Main Menu", buttons_size_x)) {
        menu_index = MainMenu;
    }

    // Continue -> move to next stage OR trigger end of game -> Trigger High Score (IF END OF GAME)
    // Shop
    // Back to main menu -> Trigger High Score

    ImGui::End();
}

void BasicScene::StageFailedMenuHandler() {
    // Menu with no sound
    game_manager->sound_manager.StopMusic();

    int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    bool* pOpen = nullptr;
    string gui_text;

    ImVec2 window_pos_x = ImVec2(float(width - 0.25 * width) / 2.f, float(height - 0.4 * height) / 2.f);
    ImVec2 window_size_x = ImVec2((float(width)) * 0.22, (float(height)) * 0.4);

    float position_x1 = float(width) * 0.06f;
    float position_x2 = float(width) * 0.03f;

    ImVec2 buttons_size_x = ImVec2(width / 6, height / 16);

    ImGui::Begin("Menu", pOpen, flags);
    ImGui::SetWindowPos(window_pos_x, ImGuiCond_Always);
    ImGui::SetWindowSize(window_size_x, ImGuiCond_Always);
    //ImGui::SetWindowFontScale(font_scale1);
    ImGui::SetWindowFontScale(font_scale2);

    ImGui::SetCursorPosX(position_x1);
    ImGui::TextColored(ImVec4(1.0, 0.5, 0.0, 1.0), "Stage Failed");

    Spacing(5);

    // Handle Score
    ImGui::SetCursorPosX(position_x2);
    gui_text = "Score: " + std::to_string(game_manager->stats.current_score);
    ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), gui_text.c_str());

    Spacing(1);

    // Handle Gold
    ImGui::SetCursorPosX(position_x2);
    gui_text = "Gold: " + std::to_string(game_manager->stats.gold);
    ImGui::TextColored(ImVec4(1.0, 1.0, 0.0, 1.0), gui_text.c_str());

    Spacing(5);

    ImGui::SetCursorPosX(position_x2);
    if (ImGui::Button("Retry", buttons_size_x)) {
        menu_index = StageSelectionMenu;
    }

    Spacing(5);

    ImGui::SetCursorPosX(position_x2);
    if (ImGui::Button("Shop", buttons_size_x)) {
        last_menu_index = StageFailedMenu;
        menu_index = ShopMenu;
    }

    Spacing(5);

    ImGui::SetCursorPosX(position_x2);
    if (ImGui::Button("Back To Main Menu", buttons_size_x)) {
        menu_index = MainMenu;
    }

    // Display Score
    // Display Gold
    // Retry -> -> Trigger High Score -> Sends to stage selection menu
    // Shop
    // Back to main menu -> Trigger High Score

    ImGui::End();
}

void BasicScene::NewHighScoreMenuHandler() {
    // Menu with no sound
    game_manager->sound_manager.StopMusic();

    int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    bool* pOpen = nullptr;
    string gui_text;

    ImVec2 window_pos_x = ImVec2(float(width - 0.4 * width) / 2.f, float(height - 0.5 * height) / 2.f);
    ImVec2 window_size_xx = ImVec2((float(width)) * 0.4, (float(height)) * 0.45);

    float position_x1xx = float(width) * 0.025f;
    float position_x2 = float(width) * 0.03f;

    ImVec2 buttons_size_xx = ImVec2(width / 3, height / 13);

    ImGui::Begin("Menu", pOpen, flags);
    ImGui::SetWindowPos(window_pos_x, ImGuiCond_Always);
    ImGui::SetWindowSize(window_size_xx, ImGuiCond_Always);
    //ImGui::SetWindowFontScale(font_scale1);
    ImGui::SetWindowFontScale(font_scale1);


    ImGui::SetCursorPosX(position_x1xx);
    ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "N");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), "e");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.0, 0.0, 1.0, 1.0), "w");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), " H");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), "i");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.0, 0.0, 1.0, 1.0), "g");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "h");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), " S");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.0, 0.0, 1.0, 1.0), "c");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "o");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), "r");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.0, 0.0, 1.0, 1.0), "e");

    Spacing(5);

    ImGui::SetCursorPosX(position_x1xx);
    ImGui::Text("Code Name: ");

    Spacing(5);

    ImGui::SetCursorPosX(position_x1xx);
    int i = 0;
    gui_text = to_string(i + 1) + ".";
    if (i == 0) {
        ImGui::TextColored(ImVec4(201.f / 176.f, 149.f / 255.f, 55.f / 255.f, 1.0), gui_text.c_str());
    }
    else if (i == 1) {
        ImGui::TextColored(ImVec4(180.f / 255.f, 180.f / 255.f, 180.f / 255.f, 1.0), gui_text.c_str());
    }
    else if (i == 2) {
        ImGui::TextColored(ImVec4(173.f / 255.f, 138.f / 255.f, 86.f / 255.f, 1.0), gui_text.c_str());
    }
    else {
        ImGui::Text(gui_text.c_str());
    }

    ImGui::SameLine();
    static char name[4] = "";
    ImGui::InputTextMultiline("", name, IM_ARRAYSIZE(name), ImVec2((width / 15), (height / 16)));

    ImGui::SameLine();
    gui_text = "-Score: " + to_string(game_manager->stats.current_score);
    ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), gui_text.c_str());

    Spacing(5);

    ImGui::SetCursorPosX(position_x1xx);
    if (ImGui::Button("Apply", buttons_size_xx)) {
        //name
        //game_manager->leaderboard.ResetLeaderboard();
        menu_index = MainMenu;
    }

    Spacing(5);

    ImGui::SetCursorPosX(position_x1xx);
    if (ImGui::Button("Back To Main Menu", buttons_size_xx)) {
        menu_index = MainMenu;
    }

    // Display score
    // Text to fill 3 letters
    // Apply name
    // Back to main menu 

    ImGui::End();
}

void BasicScene::Spacing(int number_of_spacing) {
    for (int i = 0; i < number_of_spacing; i++) {
        ImGui::Spacing();
    }
}

bool BasicScene::ProgramHandler(const Program& program) {
    bool default_behavior = true;

    if (program.name == "snake head program") {
        program.SetUniform4f("lightColor", 0.5f, 0.25f, 0.0f, 0.5f);
        program.SetUniform4f("Kai", 0.5f, 0.25f, 0.0f, 1.0f);
        program.SetUniform4f("Kdi", 0.5f, 0.5f, 0.5f, 1.0f);
        program.SetUniform1f("specular_exponent", 5.0f);
        program.SetUniform4f("light_position", 0.0, -15.0f, 0.0, 1.0f);

        default_behavior = false;
    }
    if (program.name == "snake body program") {
        program.SetUniform4f("lightColor", 0.5f, 0.25f, 0.0f, 0.5f);
        program.SetUniform4f("Kai", 0.5f, 0.25f, 0.0f, 1.0f);
        program.SetUniform4f("Kdi", 0.5f, 0.5f, 0.5f, 1.0f);
        program.SetUniform1f("specular_exponent", 5.0f);
        program.SetUniform4f("light_position", 0.0, 15.0f, 0.0, 1.0f);

        default_behavior = false;
    }

    return default_behavior;
}