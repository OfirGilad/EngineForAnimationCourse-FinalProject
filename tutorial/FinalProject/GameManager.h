//
// Created by Ofir Gilad on 13/02/2023.
//

#ifndef ENGINEFORANIMATIONCOURSE_FINALPROJECT_STAGEHANDLER_H
#define ENGINEFORANIMATIONCOURSE_FINALPROJECT_STAGEHANDLER_H

#endif //ENGINEFORANIMATIONCOURSE_FINALPROJECT_STAGEHANDLER_H

#include <string>
#include <memory>
#include <utility>
#include "Scene.h"
#include "ObjLoader.h"
#include "IglMeshLoader.h"
#include <random>


#include "Snake.h"
#include "Leaderboard.h"
#include "GameObjects.h" // Includes: Stats and SoundManager 


using namespace std;
using namespace cg3d;

class GameManager
{
public:
    GameManager() {};
    void InitGameManager(std::shared_ptr<Movable> _root, std::vector<std::shared_ptr<Camera>> _camera_list);

    void LoadStage(int stage_number);
    void UnloadStage();

    // Stage Builder
    std::shared_ptr<Movable> root;
    std::vector<std::shared_ptr<Camera>> camera_list;
    std::shared_ptr<cg3d::Model> background, axis, exit, temp_object1, temp_object2;
    std::shared_ptr<cg3d::Model> health_model, score_model, gold_model, bonus_model, obstacle_model, exit_model;

    std::vector<std::vector<GameObject*>> all_objects;
    std::vector<GameObject*> alive_objects;
    std::vector<GameObject*> dead_objects;

    float base_length = 60.f;
    float stage_size = 0.f;

    // CollisionDetectionVisitor Params
    std::shared_ptr<cg3d::Model> cube1, cube2;

    // Snake
    Snake snake;
    int number_of_bones = 16;

    // Sound Manager
    SoundManager* sound_manager = 0;

    // Stats
    Stats* stats = 0;

    // Leaderboard
    Leaderboard leaderboard;

    // Objects Builder
    ObjectsBuilder objects_builder;

    Eigen::Vector3f GenerateRandomPosition();

private:
    // Collision boxes for CollisionDetectionVisitor
    void InitCollisionBoxes();

    // Init Stage
    void InitBackground();
    void InitAxis();
    void InitStageParameters(bool new_stage);
    void LoadGameObjects();

    // Building Game Objects
    void InitCustomObjects();
    void BuildGameObjects();
    void BuildHealthObjects();
    void BuildScoreObjects();
    void BuildGoldObjects();
    void BuildBonusObjects();
    void BuildObstacleObjects();
    void BuildExit();
};