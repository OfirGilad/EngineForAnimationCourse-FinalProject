//
// Created by Ofir Gilad on 13/02/2023.
//

#include "GameManager.h"

void GameManager::InitGameManager(std::shared_ptr<Movable> _root, std::vector<std::shared_ptr<Camera>> _camera_list)
{
	root = _root;
	camera_list = _camera_list;

    // Init Collision Boxes
    InitCollisionBoxes();

    // Init Sound Manager
    sound_manager = SoundManager();

    // Init stats
    stats = Stats();
    stats.InitStats();

    // Init leaderboard
    leaderboard = Leaderboard();
    leaderboard.InitLeaderboard();
}

void GameManager::InitStage(int stage_number)
{
    // Set Select Stage
    selected_stage = stage_number;

    auto daylight{ std::make_shared<Material>("daylight", "shaders/cubemapShader") };
    daylight->AddTexture(0, "textures/cubemaps/Daylight Box_", 3);
    auto background{ Model::Create("background", Mesh::Cube(), daylight) };
    root->AddChild(background);
    background->Scale(120, Movable::Axis::XYZ);
    background->SetPickable(false);
    background->SetStatic();

    auto program = std::make_shared<Program>("shaders/phongShader");
    auto material{ std::make_shared<Material>("material", program) }; // empty material
    auto sphereMesh{ IglLoader::MeshFromFiles("sphere_igl", "data/sphere.obj") };

    sphere1 = Model::Create("HealthObject", sphereMesh, material);
    root->AddChild(sphere1);
    sphere1->Translate(-10, Movable::Axis::Z);
    stage_objects.push_back(sphere1);
    number_of_objects = 1;

    auto program1 = std::make_shared<Program>("shaders/pickingShader");
    auto material1{ std::make_shared<Material>("material", program1) }; // empty material

    //Axis
    Eigen::MatrixXd vertices(6, 3);
    vertices << -1, 0, 0, 1, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, -1, 0, 0, 1;
    Eigen::MatrixXi faces(3, 2);
    faces << 0, 1, 2, 3, 4, 5;
    Eigen::MatrixXd vertexNormals = Eigen::MatrixXd::Ones(6, 3);
    Eigen::MatrixXd textureCoords = Eigen::MatrixXd::Ones(6, 2);
    std::shared_ptr<Mesh> coordsys = std::make_shared<Mesh>("coordsys", vertices, faces, vertexNormals, textureCoords);
    axis.push_back(Model::Create("axis", coordsys, material1));
    axis[0]->mode = 1;
    axis[0]->Scale(4, Movable::Axis::XYZ);
    root->AddChild(axis[0]);


    // Init snake
    snake = Snake(root, camera_list);
    snake.InitSnake(number_of_bones);
}


void GameManager::InitCollisionBoxes() {
    auto program = std::make_shared<Program>("shaders/basicShader");
    auto material{ std::make_shared<Material>("material", program) }; // empty material
    auto cubeMesh1{ IglLoader::MeshFromFiles("cube1", "data/cube.off") };
    auto cubeMesh2{ IglLoader::MeshFromFiles("cube2", "data/cube.off") };
    cube1 = Model::Create("cube1", cubeMesh1, material);
    cube2 = Model::Create("cube2", cubeMesh1, material);
}