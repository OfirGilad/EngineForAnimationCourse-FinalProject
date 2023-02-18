#pragma once
#include "AutoMorphingModel.h"
#include "Scene.h"

#include <memory>
#include <utility>

#include "SceneWithImGui.h"
#include "imgui.h"
#include "GameManager.h"

class BasicScene : public cg3d::SceneWithImGui
{
public:
    BasicScene(std::string name, cg3d::Display* display);
    void Init(float fov, int width, int height, float near, float far);
    void Update(const cg3d::Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model) override;
    void MouseCallback(cg3d::Viewport* viewport, int x, int y, int button, int action, int mods, int buttonState[]) override;
    void ScrollCallback(cg3d::Viewport* viewport, int x, int y, int xoffset, int yoffset, bool dragging, int buttonState[]) override;
    void CursorPosCallback(cg3d::Viewport* viewport, int x, int y, bool dragging, int* buttonState)  override;
    void KeyCallback(cg3d::Viewport* viewport, int x, int y, int key, int scancode, int action, int mods) override;

    // SceneWithImGui expend
    void SetCamera(int index);
    void AddViewportCallback(cg3d::Viewport* _viewport) override;
    void ViewportSizeCallback(cg3d::Viewport* _viewport) override;
    

    // Camera View Management
    void SwitchView(bool next);


    // Game Manager
    GameManager* game_manager;


    // Menu Managmenet
    void MenuManager();
    enum MenuMapping { 
        LoginMenu, 
        MainMenu, 
        StageSelectionMenu, 
        ShopMenu, 
        StatsMenu, 
        HallOfFameMenu, 
        CreditsMenu, 
        StageMenu, 
        StageCompletedMenu, 
        StageFailedMenu, 
        NewHighScoreMenu
    };

    void LoginMenuHandler();
    void MainMenuHandler();
    void ShopMenuHandler();
    void StatsMenuHandler();
    void StageSelectionMenuHandler();
    void HallOfFameMenuHandler();
    void CreditsMenuHandler();
    void StageMenuHandler();
    void StageCompletedMenuHandler();
    void StageFailedMenuHandler();
    void NewHighScoreMenuHandler();
    void Spacing(int number_of_spacing);

    // Programs Handler
    bool ProgramHandler(const Program& program);

private:
    // SceneWithImGui expend
    void BuildImGui() override;
    cg3d::Viewport* viewport = nullptr;


    // Scene root
    std::shared_ptr<Movable> root;


    // Camera parameters
    int distance = 50;
    int degree = -15;
    Eigen::Vector3f camera_translation = Eigen::Vector3f(0, 15, 50);


    // Camera list parameters
    std::vector<std::shared_ptr<cg3d::Camera>> camera_list{ 3 };
    int camera_index = 0;
    int number_of_cameras = 0;


    // Menu parameters
    int width, height;
    ImVec2 buttons_size1, buttons_size2, window_size1;
    float font_scale1, font_scale2, text_position1, text_position2, text_position3;

    int menu_index = NewHighScoreMenu;
    int last_menu_index = MainMenu;

    bool display_keys = false;
    bool payed_credits = false;
    bool display_new_game = true;
};
