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
    camera_list[1] = Camera::Create("snake view", fov, float(width) / height, near, far);

    camera = camera_list[0];
    number_of_cameras = camera_list.size();

    AddChild(root = Movable::Create("root")); // a common (invisible) parent object for all the shapes
    auto daylight{std::make_shared<Material>("daylight", "shaders/cubemapShader")}; 
    daylight->AddTexture(0, "textures/cubemaps/Daylight Box_", 3);
    auto background{Model::Create("background", Mesh::Cube(), daylight)};
    AddChild(background);
    background->Scale(120, Axis::XYZ);
    background->SetPickable(false);
    background->SetStatic();

    root->AddChild(camera_list[1]);
 
    auto program = std::make_shared<Program>("shaders/phongShader");
    auto program1 = std::make_shared<Program>("shaders/pickingShader");
    
    auto material{ std::make_shared<Material>("material", program)}; // empty material
    auto material1{ std::make_shared<Material>("material", program1)}; // empty material
    //SetNamedObject(cube, Model::Create, Mesh::Cube(), material, shared_from_this());
 
    material->AddTexture(0, "textures/box0.bmp", 2);
    auto sphereMesh{IglLoader::MeshFromFiles("sphere_igl", "data/sphere.obj")};
    auto cylMesh{IglLoader::MeshFromFiles("cyl_igl", "data/zcylinder.obj")};
    auto snakeMesh{ IglLoader::MeshFromFiles("cyl_igl", "data/snake1.obj") };
    //auto cubeMesh{IglLoader::MeshFromFiles("cube_igl","data/cube_old.obj")};
    sphere1 = Model::Create( "sphere",sphereMesh, material);    
    //cube = Model::Create( "cube", cubeMesh, material);
    
    root->AddChild(sphere1);

    sphere1->Translate(-distance, Axis::Z);

    //Axis
    Eigen::MatrixXd vertices(6,3);
    vertices << -1,0,0,1,0,0,0,-1,0,0,1,0,0,0,-1,0,0,1;
    Eigen::MatrixXi faces(3,2);
    faces << 0,1,2,3,4,5;
    Eigen::MatrixXd vertexNormals = Eigen::MatrixXd::Ones(6,3);
    Eigen::MatrixXd textureCoords = Eigen::MatrixXd::Ones(6,2);
    std::shared_ptr<Mesh> coordsys = std::make_shared<Mesh>("coordsys",vertices,faces,vertexNormals,textureCoords);
    axis.push_back(Model::Create("axis",coordsys,material1));
    axis[0]->mode = 1;   
    axis[0]->Scale(4,Axis::XYZ);
    root->AddChild(axis[0]);

    camera->Translate(distance, Axis::Z);
    camera->Translate(10, Axis::Y);
    camera->RotateByDegree(-10, Axis::X);

    // Init camera rotations modes
    InitRotationModes();

    // Init snake
    snake = Snake(root, camera_list);
    snake.InitSnake(number_of_bones);
}

void BasicScene::Update(const Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model)
{
    Scene::Update(program, proj, view, model);
    program.SetUniform4f("lightColor", 0.8f, 0.3f, 0.0f, 0.5f);
    program.SetUniform4f("Kai", 1.0f, 0.3f, 0.6f, 1.0f);
    program.SetUniform4f("Kdi", 0.5f, 0.5f, 0.0f, 1.0f);
    program.SetUniform1f("specular_exponent", 5.0f);
    program.SetUniform4f("light_position", 0.0, 15.0f, 0.0, 1.0f);
}

void BasicScene::MouseCallback(Viewport* viewport, int x, int y, int button, int action, int mods, int buttonState[])
{
    // Handle ImGui Menu
    if (ImGui::GetIO().WantCaptureMouse) return;
    
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
    auto system = camera->GetRotation().transpose();

    int translation = translation_modes[up_down_mode][left_right_mode].first;
    Axis axis = translation_modes[up_down_mode][left_right_mode].second;

    Matrix3f Kp = Matrix3f();

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) // NOLINT(hicpp-multiway-paths-covered)
        {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                break;
            case GLFW_KEY_W:
                if (IsAnimate()) {
                    snake.MoveUp();
                }
                break;
            case GLFW_KEY_S:
                if (IsAnimate()) {
                    snake.MoveDown();
                }
                break;
            case GLFW_KEY_A:
                if (IsAnimate()) {
                    snake.MoveLeft();
                }
                break;
            case GLFW_KEY_D:
                if (IsAnimate()) {
                    snake.MoveRight();
                }
                break;
            case GLFW_KEY_UP:
                if ((camera_list[0] == camera) && ((up_down_mode == 0 || up_down_mode == 2) && left_right_mode == 0)) {
                    camera->Translate(-translation, axis);
                    camera->RotateByDegree(90, Vector3f(-1, 0, 0));
                    up_down_mode = (up_down_mode + 1) % 3;

                    translation = translation_modes[up_down_mode][left_right_mode].first;
                    axis = translation_modes[up_down_mode][left_right_mode].second;
                    camera->Translate(translation, axis);
                }

                //camera->TranslateInSystem(system, {0, 0.1f, 0});
                break;
            case GLFW_KEY_DOWN:
                if ((camera_list[0] == camera) && ((up_down_mode == 0 || up_down_mode == 1) && left_right_mode == 0)) {
                    camera->Translate(-translation, axis);
                    camera->RotateByDegree(90, Vector3f(1, 0, 0));
                    up_down_mode = (up_down_mode + 2) % 3;

                    translation = translation_modes[up_down_mode][left_right_mode].first;
                    axis = translation_modes[up_down_mode][left_right_mode].second;
                    camera->Translate(translation, axis);
                }

                //camera->TranslateInSystem(system, {0, -0.1f, 0});
                break;
            case GLFW_KEY_LEFT:
                //if ((camera_list[0] == camera) && (up_down_mode == 0)) {
                //    camera->Translate(-translation, axis);
                //    camera->RotateByDegree(90, Vector3f(0, -1, 0));
                //    left_right_mode = (left_right_mode + 1) % 4;
                //
                //    translation = translation_modes[up_down_mode][left_right_mode].first;
                //    axis = translation_modes[up_down_mode][left_right_mode].second;
                //    camera->Translate(translation, axis);
                //}
                
                snake.RollLeft();

                //camera->TranslateInSystem(system, {-0.1f, 0, 0});
                break;
            case GLFW_KEY_RIGHT:
                //if ((camera_list[0] == camera) && (up_down_mode == 0)) {
                //    camera->Translate(-translation, axis);
                //    camera->RotateByDegree(90, Vector3f(0, 1, 0));
                //    left_right_mode = (left_right_mode + 3) % 4;
                //
                //    translation = translation_modes[up_down_mode][left_right_mode].first;
                //    axis = translation_modes[up_down_mode][left_right_mode].second;
                //    camera->Translate(translation, axis);
                //}

                snake.RollRight();

                //camera->TranslateInSystem(system, {0.1f, 0, 0});
                break;
            //case GLFW_KEY_B:
            //    camera->TranslateInSystem(system, {0, 0, 0.1f});
            //    break;
            //case GLFW_KEY_F:
            //    camera->TranslateInSystem(system, {0, 0, -0.1f});
            //    break;
            case GLFW_KEY_V:
                SwitchView();
                break;
            case GLFW_KEY_SPACE:
                this->animate = !this->animate;
                cout << this->animate << endl;
                break;
        }
    }
}

// Game Menu
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



void BasicScene::SwitchView()
{
    camera_index = (camera_index + 1) % number_of_cameras;
    camera = camera_list[camera_index];
    viewport->camera = camera;
}

void BasicScene::InitRotationModes() {
    // Global camera static view modes:
    vector<pair<int, Axis>> sub_translation_modes1;
    sub_translation_modes1.push_back({ distance, Axis::Z });  // Front
    sub_translation_modes1.push_back({ -distance, Axis::X }); // Right
    sub_translation_modes1.push_back({ -distance, Axis::Z }); // Back
    sub_translation_modes1.push_back({ distance, Axis::X });  // Left
    translation_modes.push_back(sub_translation_modes1);

    vector<pair<int, Axis>> sub_translation_modes2;
    sub_translation_modes2.push_back({ distance, Axis::Y });  // Top
    //sub_translation_modes2.push_back({ -distance, Axis::X });
    //sub_translation_modes2.push_back({ -distance, Axis::X });
    //sub_translation_modes2.push_back({ distance, Axis::Y });
    translation_modes.push_back(sub_translation_modes2);

    vector<pair<int, Axis>> sub_translation_modes3;
    sub_translation_modes3.push_back({ -distance, Axis::Y }); // Bottom
    //sub_translation_modes3.push_back({ -distance, Axis::X });
    //sub_translation_modes3.push_back({ distance, Axis::Y });
    //sub_translation_modes3.push_back({ distance, Axis::X });
    translation_modes.push_back(sub_translation_modes3);
}


// Menu
void BasicScene::MenuManager() {
    width = viewport->width;
    height = viewport->height;
    buttons_size1 = ImVec2(width / 4, height / 8);
    font_scale1 = (2.f * width) / 800.f;

    switch (menu_index)
    {
        case MainMenu:
            MainMenuHandler();
            break;
        case StageSelectionMenu:
            StageSelectionMenuHandler();
            break;
        case GameMenu:
            GameMenuHandler();
            break;
        case ShopMenu:
            ShopMenuHandler();
            break;
        case HallOfFameMenu:
            HallOfFameMenuHandler();
            break;
        case CreditsMenu:
            CreditsMenuHandler();
            break;
    }
}

void BasicScene::MainMenuHandler() {
    int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    bool* pOpen = nullptr;
    string gui_text;

    ImGui::Begin("Menu", pOpen, flags);
    ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    //ImGui::SetWindowSize(ImVec2(width, height), ImGuiCond_Always);
    ImGui::SetWindowSize(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetWindowFontScale(font_scale1);

    ImGui::TextColored(ImVec4(0.6, 1.0, 0.4, 1.0), "Main Menu");

    // Handle Gold
    gui_text = "Gold: " + std::to_string(0);
    ImGui::Text(gui_text.c_str());

    ImGui::Spacing();

    // Move to stage selection menu
    if (ImGui::Button("Shop", buttons_size1)) {
        menu_index = ShopMenu;
    }

    ImGui::Spacing();

    // Move to stage selection menu
    if (ImGui::Button("Select Stage", buttons_size1)) {
        menu_index = StageSelectionMenu;
    }

    ImGui::Spacing();

    if (ImGui::Button("Hall Of Fame", buttons_size1)) {
        menu_index = HallOfFameMenu;
    }

    ImGui::Spacing();

    if (ImGui::Button("Credits", buttons_size1)) {
        menu_index = CreditsMenu;
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::Button("Exit", buttons_size1)) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    
    ImGui::End();
}

void BasicScene::StageSelectionMenuHandler() {
    int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    bool* pOpen = nullptr;
    string gui_text;

    ImGui::Begin("Menu", pOpen, flags);
    ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetWindowSize(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetWindowFontScale(font_scale1);

    ImGui::TextColored(ImVec4(0.6, 1.0, 0.4, 1.0), "Stage Selection Menu");

    // Handle Gold
    gui_text = "Gold: " + std::to_string(0);
    ImGui::Text(gui_text.c_str());

    ImGui::Spacing();

    // Handle Stages
    ImGui::Text("Select Stage: ");
    for (int i = 1; i <= 3; i++) {
        gui_text = "Stage " + std::to_string(i);

        if (ImGui::Button(gui_text.c_str(), buttons_size1)) {
            cout << gui_text.c_str() << endl;
            menu_index = GameMenu;
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::Button("Back", buttons_size1)) {
        menu_index = MainMenu;
    }
    ImGui::End();
}

void BasicScene::GameMenuHandler() {
    int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    bool* pOpen = nullptr;
    string gui_text;

    ImGui::Begin("Menu", pOpen, flags);
    ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetWindowSize(ImVec2(0, 0), ImGuiCond_Always);
    //ImGui::SetWindowFontScale(font_scale1);

    ImGui::TextColored(ImVec4(0.0, 0.5, 1.0, 1.0), "Game Menu");

    // Handle Gold
    gui_text = "Gold: " + std::to_string(0);
    ImGui::Text(gui_text.c_str());

    ImGui::Spacing();

    // Handle Score
    gui_text = "Score: " + std::to_string(0);
    ImGui::Text(gui_text.c_str());

    ImGui::Spacing();

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

    ImGui::Spacing();

    if (display_keys == 0) {
        if (ImGui::Button("Show Keys")) {
            display_keys = 1;
        }
    }
    else {
        if (ImGui::Button("Hide Keys")) {
            display_keys = 0;
        }
        ImGui::Text("Keyboard Keys: ");
        ImGui::Text("W - Move snake up");
        ImGui::Text("S - Move snake down");
        ImGui::Text("A - Move snake left");
        ImGui::Text("D - Move snake right");
        ImGui::Text("V - Switch view");
        ImGui::Text("UP - Switch camera view up");
        ImGui::Text("DOWN - Switch camera view down");
        ImGui::Text("LEFT - Rotate snake left");
        ImGui::Text("RIGHT - Rotate snake right");
        ImGui::Text("ESC - Exit game");
    }

    ImGui::Spacing();

    if (ImGui::Button("Pasue")) {
        animate = false;
        cout << "Pause Game" << endl;
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::Button("Back to Main Menu")) {
        menu_index = MainMenu;
    }

    ImGui::End();
}

void BasicScene::ShopMenuHandler() {
    int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    bool* pOpen = nullptr;
    string gui_text;

    ImGui::Begin("Menu", pOpen, flags);
    ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetWindowSize(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetWindowFontScale(font_scale1);

    ImGui::TextColored(ImVec4(0.6, 1.0, 0.4, 1.0), "Shop");

    // Handle Gold
    gui_text = "Gold: " + std::to_string(0);
    ImGui::Text(gui_text.c_str());

    ImGui::Spacing();

    ImGui::Text("Refill Health");
    if (ImGui::Button("50 Gold", buttons_size1)) {
        cout << "Refill Health" << endl;
    }

    ImGui::Spacing();

    ImGui::Text("Increase Max Health");
    if (ImGui::Button("100 Gold", buttons_size1)) {
        cout << "Increase Max Health" << endl;
    }

    ImGui::Spacing();

    ImGui::Text("Increase Max Speed");
    if (ImGui::Button("25 Gold", buttons_size1)) {
        cout << "Increase Max Speed" << endl;
    }

    ImGui::Spacing();

    ImGui::Text("X2 Gold");
    if (ImGui::Button("100 Gold", buttons_size1)) {
        cout << "Increase Max Speed" << endl;
    }
    
    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::Button("Back", buttons_size1)) {
        menu_index = MainMenu;
    }

    ImGui::End();
}

void BasicScene::HallOfFameMenuHandler() {
    int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    bool* pOpen = nullptr;
    string gui_text;

    ImGui::Begin("Menu", pOpen, flags);
    ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetWindowSize(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetWindowFontScale(font_scale1);

    ImGui::TextColored(ImVec4(0.6, 1.0, 0.4, 1.0), "Hall of Fame");

    ImGui::Text("1.  AAA - 1000");
    ImGui::Text("2.  AAA - 900");
    ImGui::Text("3.  AAA - 800");
    ImGui::Text("4.  AAA - 700");
    ImGui::Text("5.  AAA - 600");
    ImGui::Text("6.  AAA - 500");
    ImGui::Text("7.  AAA - 400");
    ImGui::Text("8.  AAA - 300");
    ImGui::Text("9.  AAA - 200");
    ImGui::Text("10. AAA - 100");

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::Button("Back", buttons_size1)) {
        menu_index = MainMenu;
    }

    ImGui::End();
}

void BasicScene::CreditsMenuHandler() {
    int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    bool* pOpen = nullptr;
    string gui_text;

    ImGui::Begin("Menu", pOpen, flags);
    ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetWindowSize(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetWindowFontScale(font_scale1);

    ImGui::TextColored(ImVec4(0.6, 1.0, 0.4, 1.0), "Credits");

    ImGui::Text("Created by: Ofir Gilad");

    ImGui::Spacing();

    ImGui::Text("Course Teacher: Tamir Grossinger");

    ImGui::Spacing();

    ImGui::Text("Game Engine: OpenGL");

    ImGui::Spacing();

    if (payed_credits == true) {
        ImGui::Text("Honorable Contributor: You");
    }
    else {
        if (ImGui::Button("Contribute Credit")) {
            payed_credits = true;
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::Button("Back", buttons_size1)) {
        menu_index = MainMenu;
    }

    ImGui::End();
}