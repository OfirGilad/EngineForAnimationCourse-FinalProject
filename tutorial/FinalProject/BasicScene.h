#pragma once
#include "AutoMorphingModel.h"
#include "Scene.h"

#include <memory>
#include <utility>

#include "Snake.h"
#include "SceneWithImGui.h"
#include "imgui.h"


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


    void SetCamera(int index);
    void AddViewportCallback(cg3d::Viewport* _viewport) override;
    void ViewportSizeCallback(cg3d::Viewport* _viewport) override;
    
    void SetAnimate(bool status) { animate = status; }

    void SwitchView();
    void InitRotationModes();

    // Menu Managmenet
    void MenuManager();
    enum MenuMapping { MainMenu, StageSelectionMenu, GameMenu, ShopMenu, HallOfFameMenu, CreditsMenu };

    void MainMenuHandler();
    void StageSelectionMenuHandler();
    void GameMenuHandler();
    void ShopMenuHandler();
    void HallOfFameMenuHandler();
    void CreditsMenuHandler();

    Snake snake;

    std::shared_ptr<cg3d::Model> sphere1;

private:
    void BuildImGui() override;
    cg3d::Viewport* viewport = nullptr;


    std::shared_ptr<Movable> root;
    //std::shared_ptr<cg3d::Model> sphere1 ,cube;
    //std::shared_ptr<cg3d::AutoMorphingModel> autoCube;
    std::vector<std::shared_ptr<cg3d::Model>> axis;
    std::shared_ptr<cg3d::Model> cube;
    int pickedIndex = 0;
    int tipIndex = 0;
    Eigen::VectorXi EMAP;
    Eigen::MatrixXi F,E,EF,EI;
    Eigen::VectorXi EQ;
    // If an edge were collapsed, we'd collapse it to these points:
    Eigen::MatrixXd V, C, N, T, points,edges,colors;

    int distance = 50;

    // Snake Params
    int number_of_bones = 16;
    std::vector<std::shared_ptr<cg3d::Camera>> camera_list{ 2 };
    int camera_index = 0;
    int number_of_cameras = 0;
    
    std::vector<std::vector<std::pair<int, Axis>>> translation_modes;
    int up_down_mode = 0, left_right_mode = 0;


    // Menu Params
    int width, height;
    ImVec2 buttons_size1;
    float font_scale1;

    int menu_index = 0;
    int display_keys = 0;
    bool payed_credits = false;
};
