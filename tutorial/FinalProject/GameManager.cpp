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
    InitStageParameters(true);
    InitCustomObject();
    BuildGameObjects();

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

void GameManager::InitStageParameters(bool new_stage) {
    if (new_stage) {
        stats.current_health = stats.max_health;
        //stats.current_health = 20;
        stats.current_score = 0;
    }
}

void GameManager::InitCustomObject() {
    auto program = std::make_shared<Program>("shaders/basicShader");

    // health
    auto material1 = std::make_shared<Material>("material", program);
    material1->AddTexture(0, "../tutorial/objects/health.png", 2);
    health_model = ObjLoader::ModelFromObj("health", "../tutorial/objects/health.obj", material1);

    health_model->Scale(0.3, Movable::Axis::XYZ);
    health_model->Translate(6, Movable::Axis::Y);
    health_model->Translate(-1.5, Movable::Axis::Z);

    // Score
    auto material2 = std::make_shared<Material>("material", program);
    material2->AddTexture(0, "../tutorial/objects/score.jpg", 2);
    score_model = ObjLoader::ModelFromObj("score", "data/circle.obj", material2);

    // Gold
    auto material3 = std::make_shared<Material>("material", program);
    material3->AddTexture(0, "../tutorial/objects/gold.jpg", 2);
    gold_model = ObjLoader::ModelFromObj("gold", "data/circle.obj", material3);

    // Bonus
    auto material4 = std::make_shared<Material>("material", program);
    material4->AddTexture(0, "../tutorial/objects/bonus.jpg", 2);
    bonus_model = ObjLoader::ModelFromObj("bonus", "data/circle.obj", material4);

    // Obstacle
    auto material5 = std::make_shared<Material>("material", program);
    material5->AddTexture(0, "../tutorial/objects/obstacle.jpg", 2);
    obstacle_model = ObjLoader::ModelFromObj("obstacle", "data/circle.obj", material5);
}

void GameManager::BuildGameObjects() {
    BuildHealthObjects();
    //BuildScoreObjects();
    //BuildGoldObjects();
    //BuildBonusObjects();
    //BuildObstacleObjects();
}

void GameManager::BuildHealthObjects() {
    for (int i = 0; i < 3; i++) {
        // Init meshes
        temp_object1 = Model::Create("health", health_model->GetMesh(), health_model->material);
        temp_object1->SetTransform(health_model->GetTransform());
        temp_object2 = Model::Create("HealthObject", Mesh::Cube(), health_model->material);
        root->AddChild(temp_object2);

        // Setting Positions
        temp_object2->AddChild(temp_object1);
        temp_object2->Translate(-10 * (i + 1), Movable::Axis::Z); // Need to generate randomly
        
        // Fix Scaling
        temp_object2->Scale(Eigen::Vector3f(3.4, 3.4, 3.4));
        temp_object2->isHidden = true;
        
        // Adding to Stage objects list
        stage_objects.push_back(temp_object2);
        number_of_objects++;
    }
}

void GameManager::BuildScoreObjects() {
    for (int i = 0; i < 3; i++) {
        // Init meshes
        temp_object1 = Model::Create("score", score_model->GetMesh(), score_model->material);
        temp_object1->SetTransform(score_model->GetTransform());
        temp_object2 = Model::Create("ScoreObject", Mesh::Cube(), score_model->material);
        root->AddChild(temp_object2);

        // Setting Positions
        temp_object2->AddChild(temp_object1);
        temp_object2->Translate(-10 * (i + 1), Movable::Axis::Z); // Need to generate randomly

        // Fix Scaling
        //temp_object2->Scale(Eigen::Vector3f(3.4, 3.4, 3.4));
        temp_object2->isHidden = true;

        // Adding to Stage objects list
        stage_objects.push_back(temp_object2);
        number_of_objects++;
    }
}

void GameManager::BuildGoldObjects() {
    for (int i = 0; i < 3; i++) {
        // Init meshes
        temp_object1 = Model::Create("gold", gold_model->GetMesh(), gold_model->material);
        temp_object1->SetTransform(gold_model->GetTransform());
        temp_object2 = Model::Create("GoldObject", Mesh::Cube(), gold_model->material);
        root->AddChild(temp_object2);

        // Setting Positions
        temp_object2->AddChild(temp_object1);
        temp_object2->Translate(-10 * (i + 1), Movable::Axis::Z); // Need to generate randomly

        // Fix Scaling
        //temp_object2->Scale(Eigen::Vector3f(3.4, 3.4, 3.4));
        temp_object2->isHidden = true;

        // Adding to Stage objects list
        stage_objects.push_back(temp_object2);
        number_of_objects++;
    }
}

void GameManager::BuildBonusObjects() {
    for (int i = 0; i < 3; i++) {
        // Init meshes
        temp_object1 = Model::Create("bonus", bonus_model->GetMesh(), bonus_model->material);
        temp_object1->SetTransform(bonus_model->GetTransform());
        temp_object2 = Model::Create("BonusObject", Mesh::Cube(), bonus_model->material);
        root->AddChild(temp_object2);

        // Setting Positions
        temp_object2->AddChild(temp_object1);
        temp_object2->Translate(-10 * (i + 1), Movable::Axis::Z); // Need to generate randomly

        // Fix Scaling
        //temp_object2->Scale(Eigen::Vector3f(3.4, 3.4, 3.4));
        temp_object2->isHidden = true;

        // Adding to Stage objects list
        stage_objects.push_back(temp_object2);
        number_of_objects++;
    }
}

void GameManager::BuildObstacleObjects() {
    for (int i = 0; i < 3; i++) {
        // Init meshes
        temp_object1 = Model::Create("obstacle", obstacle_model->GetMesh(), obstacle_model->material);
        temp_object1->SetTransform(obstacle_model->GetTransform());
        temp_object2 = Model::Create("ObstacleObject", Mesh::Cube(), obstacle_model->material);
        root->AddChild(temp_object2);

        // Setting Positions
        temp_object2->AddChild(temp_object1);
        temp_object2->Translate(-10 * (i + 1), Movable::Axis::Z); // Need to generate randomly

        // Fix Scaling
        //temp_object2->Scale(Eigen::Vector3f(3.4, 3.4, 3.4));
        temp_object2->isHidden = true;

        // Adding to Stage objects list
        stage_objects.push_back(temp_object2);
        number_of_objects++;
    }
}