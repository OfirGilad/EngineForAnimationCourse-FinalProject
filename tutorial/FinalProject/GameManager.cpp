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

    // Init Sound Manager
    sound_manager = new SoundManager();

    // Init Stats
    stats = new Stats();
    stats->InitStats();

    // Init Leaderboard
    leaderboard = Leaderboard();
    leaderboard.InitLeaderboard();

    // Init ObjectsBuilder
    objects_builder = ObjectsBuilder();
    objects_builder.InitObjectsBuilder(stats, sound_manager);

    // Build all required objects
    InitCustomObjects();
    BuildGameObjects();
    BuildExit();
}

void GameManager::LoadStage(int stage_number)
{
    // Set Select Stage
    stats->selected_stage = stage_number;

    InitBackground();
    InitAxis();
    InitStageParameters(true);

    

    snake.ShowSnake();
}

void GameManager::UnloadStage() {
    root->RemoveChild(background);
    root->RemoveChild(axis);

    snake.HideSnake();
    snake.ResetSnakePosition();

    // And unload all other objects
}


void GameManager::InitCollisionBoxes() {
    auto program = std::make_shared<Program>("shaders/basicShader");
    auto material{ std::make_shared<Material>("material", program) }; // empty material
    auto cubeMesh1{ IglLoader::MeshFromFiles("cube1", "data/cube.off") };
    auto cubeMesh2{ IglLoader::MeshFromFiles("cube2", "data/cube.off") };
    cube1 = Model::Create("cube1", cubeMesh1, material);
    cube2 = Model::Create("cube2", cubeMesh1, material);
    cube1->showFaces = false;
    cube2->showFaces = false;
    cube1->showWireframe = true;
    cube2->showWireframe = true;
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

void GameManager::InitStageParameters(bool new_stage) {
    if (new_stage) {
        stats->current_health = stats->max_health;
        //stats->current_health = 20;
        stats->current_score = 0;
    }
}

void GameManager::InitCustomObjects() {
    // health
    auto program1 = std::make_shared<Program>("shaders/phongShader");
    auto material1 = std::make_shared<Material>("material", program1);
    material1->program->name = "health";
    health_model = ObjLoader::ModelFromObj("health", "../tutorial/objects/health.obj", material1);

    health_model->Scale(0.3f, Movable::Axis::XYZ);
    health_model->Translate(6.f, Movable::Axis::Y);
    health_model->Translate(-1.5f, Movable::Axis::Z);

    // Score
    auto program2 = std::make_shared<Program>("shaders/phongShader");
    auto material2 = std::make_shared<Material>("material", program2);
    material2->program->name = "score";
    score_model = ObjLoader::ModelFromObj("score", "data/circle.obj", material2);

    score_model->Scale(2, Movable::Axis::XYZ);

    // Gold
    auto program3 = std::make_shared<Program>("shaders/phongShader");
    auto material3 = std::make_shared<Material>("material", program3);
    material3->program->name = "gold";
    gold_model = ObjLoader::ModelFromObj("gold", "data/circle.obj", material3);

    gold_model->Scale(2.f, Movable::Axis::XYZ);

    // Bonus
    auto program4 = std::make_shared<Program>("shaders/phongShader");
    auto material4 = std::make_shared<Material>("material", program4);
    material4->program->name = "bonus";
    bonus_model = ObjLoader::ModelFromObj("bonus", "data/circle.obj", material4);

    bonus_model->Scale(2.f, Movable::Axis::XYZ);

    // Obstacle
    auto program5 = std::make_shared<Program>("shaders/phongShader");
    auto material5 = std::make_shared<Material>("material", program5);
    material5->program->name = "obstacle";
    obstacle_model = ObjLoader::ModelFromObj("obstacle", "data/circle.obj", material5);

    obstacle_model->Scale(6, Movable::Axis::XYZ);

    // Exit
    auto program6 = std::make_shared<Program>("shaders/phongShader");
    auto material6 = std::make_shared<Material>("material", program6);
    material6->program->name = "exit";
    exit_model = ObjLoader::ModelFromObj("obstacle", "../tutorial/objects/exit.obj", material6);
}

void GameManager::BuildGameObjects() {
    BuildHealthObjects();
    BuildScoreObjects();
    BuildGoldObjects();
    BuildBonusObjects();
    BuildObstacleObjects();
}

void GameManager::BuildHealthObjects() {
    for (int i = 0; i < 3; i++) {
        // Init meshes
        temp_object1 = Model::Create("health", health_model->GetMesh(), health_model->material);
        temp_object1->SetTransform(health_model->GetTransform());
        temp_object2 = Model::Create("HealthObject", Mesh::Cube(), health_model->material);
        //root->AddChild(temp_object2);

        // Setting Positions
        temp_object2->AddChild(temp_object1);
        //temp_object2->Translate(GenerateRandomPosition()); // Need to generate randomly
        
        // Fix Scaling
        temp_object2->Scale(Eigen::Vector3f(3.4f, 3.4f, 3.4f));
        temp_object2->isHidden = true;

        // Adding to Stage objects list;
        all_objects.push_back(objects_builder.BuildGameObject(temp_object2));
    }
}

void GameManager::BuildScoreObjects() {
    for (int i = 0; i < 3; i++) {
        // Init meshes
        temp_object1 = Model::Create("score", score_model->GetMesh(), score_model->material);
        temp_object1->SetTransform(score_model->GetTransform());
        temp_object2 = Model::Create("ScoreObject", Mesh::Cube(), score_model->material);
        //root->AddChild(temp_object2);

        // Setting Positions
        temp_object2->AddChild(temp_object1);
        //temp_object2->Translate(GenerateRandomPosition()); // Need to generate randomly

        // Fix Scaling
        temp_object2->Scale(Eigen::Vector3f(3.4f, 3.4f, 3.4f));
        temp_object2->isHidden = true;

        // Adding to Stage objects list
        all_objects.push_back(objects_builder.BuildGameObject(temp_object2));
    }
}

void GameManager::BuildGoldObjects() {
    for (int i = 0; i < 3; i++) {
        // Init meshes
        temp_object1 = Model::Create("gold", gold_model->GetMesh(), gold_model->material);
        temp_object1->SetTransform(gold_model->GetTransform());
        temp_object2 = Model::Create("GoldObject", Mesh::Cube(), gold_model->material);
        //root->AddChild(temp_object2);

        // Setting Positions
        temp_object2->AddChild(temp_object1);
        //temp_object2->Translate(GenerateRandomPosition()); // Need to generate randomly

        // Fix Scaling
        temp_object2->Scale(Eigen::Vector3f(3.4f, 3.4f, 3.4f));
        temp_object2->isHidden = true;

        // Adding to Stage objects list
        all_objects.push_back(objects_builder.BuildGameObject(temp_object2));
    }
}

void GameManager::BuildBonusObjects() {
    for (int i = 0; i < 3; i++) {
        // Init meshes
        temp_object1 = Model::Create("bonus", bonus_model->GetMesh(), bonus_model->material);
        temp_object1->SetTransform(bonus_model->GetTransform());
        temp_object2 = Model::Create("BonusObject", Mesh::Cube(), bonus_model->material);
        //root->AddChild(temp_object2);

        // Setting Positions
        temp_object2->AddChild(temp_object1);
        //temp_object2->Translate(GenerateRandomPosition()); // Need to generate randomly

        // Fix Scaling
        temp_object2->Scale(Eigen::Vector3f(3.4f, 3.4f, 3.4f));
        temp_object2->isHidden = true;

        // Adding to Stage objects list
        all_objects.push_back(objects_builder.BuildGameObject(temp_object2));
    }
}

void GameManager::BuildObstacleObjects() {
    for (int i = 0; i < 3; i++) {
        // Init meshes
        temp_object1 = Model::Create("obstacle", obstacle_model->GetMesh(), obstacle_model->material);
        temp_object1->SetTransform(obstacle_model->GetTransform());
        temp_object2 = Model::Create("ObstacleObject", Mesh::Cube(), obstacle_model->material);
        //root->AddChild(temp_object2);

        // Setting Positions
        temp_object2->AddChild(temp_object1);
        //temp_object2->Translate(GenerateRandomPosition()); // Need to generate randomly

        // Fix Scaling
        temp_object2->Scale(Eigen::Vector3f(6, 6, 6));
        temp_object2->isHidden = true;

        // Adding to Stage objects list
        all_objects.push_back(objects_builder.BuildGameObject(temp_object2));
    }
}

void GameManager::BuildExit() {
    // Init meshes
    temp_object1 = Model::Create("exit", exit_model->GetMesh(), exit_model->material);
    temp_object1->SetTransform(exit_model->GetTransform());
    exit = Model::Create("ExitObject", Mesh::Cube(), exit_model->material);
    root->AddChild(exit);

    // Setting Positions
    exit->AddChild(temp_object1);
    //temp_object2->Translate(GenerateRandomPosition()); // Need to generate randomly

    // Fix Scaling
    //exit->Scale(Eigen::Vector3f(6, 6, 6));
    exit->isHidden = true;
}

Eigen::Vector3f GameManager::GenerateRandomPosition() {
    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_real_distribution<float> distribution(-50, 50);

    return Eigen::Vector3f(distribution(generator), distribution(generator), distribution(generator));
}
