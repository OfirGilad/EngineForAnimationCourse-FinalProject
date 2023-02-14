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

    // Init Snake
    snake = Snake(root, camera_list);
    snake.InitSnake(number_of_bones);
    snake.HideSnake();

    // Init Sound Manager
    sound_manager = SoundManager();

    // Init Stats
    stats = Stats();
    stats.InitStats();

    // Init Leaderboard
    leaderboard = Leaderboard();
    leaderboard.InitLeaderboard();
}

void GameManager::LoadStage(int stage_number)
{
    // Set Select Stage
    selected_stage = stage_number;

    InitBackground();
    InitAxis();

    auto program = std::make_shared<Program>("shaders/basicShader");
    auto material{ std::make_shared<Material>("material", program) }; // empty material
    auto sphereMesh{ IglLoader::MeshFromFiles("sphere_igl", "../tutorial/objects/heart.obj") };
    auto sphereMesh1{ IglLoader::MeshFromFiles("sphere_igl", "data/sphere.obj") };
    material->AddTexture(0, "../tutorial/objects/heart.png", 2);

    temp_object1 = Model::Create("heart", sphereMesh, material);
    temp_object2 = Model::Create("HealthObject", sphereMesh1, material);
    

    
    temp_object1->Translate(0, Movable::Axis::Z);

    root->AddChild(temp_object2);

    temp_object2->AddChild(temp_object1);
    temp_object2->Scale(Eigen::Vector3f(4, 4, 4));
    temp_object2->Translate(-15, Movable::Axis::Z);

    temp_object1->Scale(0.3, Movable::Axis::XYZ);
    temp_object1->Translate(6, Movable::Axis::Y);
    temp_object1->Translate(-1.5, Movable::Axis::Z);
    temp_object2->isHidden = true;


    stage_objects.push_back(temp_object2);
    number_of_objects = 1;


    
    snake.ShowSnake();
}

void GameManager::UnloadStage() {
    root->RemoveChild(background);
    root->RemoveChild(axis);
    snake.HideSnake();
    
    // And unload all other objects
}


void GameManager::InitCollisionBoxes() {
    auto program = std::make_shared<Program>("shaders/basicShader");
    auto material{ std::make_shared<Material>("material", program) }; // empty material
    auto cubeMesh1{ IglLoader::MeshFromFiles("cube1", "data/cube.off") };
    auto cubeMesh2{ IglLoader::MeshFromFiles("cube2", "data/cube.off") };
    cube1 = Model::Create("cube1", cubeMesh1, material);
    cube2 = Model::Create("cube2", cubeMesh1, material);
}

void GameManager::InitBackground() {
    auto daylight{ std::make_shared<Material>("daylight", "shaders/cubemapShader") };
    daylight->AddTexture(0, "textures/cubemaps/Daylight Box_", 3);

    background = Model::Create("background", Mesh::Cube(), daylight);
    root->AddChild(background);
    background->Scale(120, Movable::Axis::XYZ);
    background->SetPickable(false);
    background->SetStatic();
}

void GameManager::InitAxis() {
    auto program = std::make_shared<Program>("shaders/basicShader");
    auto material{ std::make_shared<Material>("material", program) }; // empty material

    Eigen::MatrixXd vertices(6, 3);
    vertices << -1, 0, 0, 1, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, -1, 0, 0, 1;
    Eigen::MatrixXi faces(3, 2);
    faces << 0, 1, 2, 3, 4, 5;
    Eigen::MatrixXd vertexNormals = Eigen::MatrixXd::Ones(6, 3);
    Eigen::MatrixXd textureCoords = Eigen::MatrixXd::Ones(6, 2);
    std::shared_ptr<Mesh> coordsys = std::make_shared<Mesh>("coordsys", vertices, faces, vertexNormals, textureCoords);
    axis = Model::Create("axis", coordsys, material);
    axis->mode = 1;
    axis->Scale(60, Movable::Axis::XYZ);
    root->AddChild(axis);
    axis->Translate(Eigen::Vector3f(0, 0, 0));
}

void GameManager::BuildGameObjects() {
    //// Creating textures
    //auto program = std::make_shared<Program>("shaders/basicShader");
    //auto snake_material = std::make_shared<Material>("snake_material", program);
    //snake_material->AddTexture(0, "textures/snake.jpg", 2);

    //auto snake_bones = ObjLoader::ModelFromObj("HealthObject", "data/zcylinder.obj", snake_material);
}