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


#include "Snake.h"
#include "SoundManager.h"
#include "Stats.h"
#include "Leaderboard.h"


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
    std::shared_ptr<cg3d::Model> background, axis, temp_object1, temp_object2;

    std::vector<std::shared_ptr<cg3d::Model>> health_objects;
    std::vector<std::shared_ptr<cg3d::Model>> score_objects;
    std::vector<std::shared_ptr<cg3d::Model>> gold_objects;
    std::vector<std::shared_ptr<cg3d::Model>> bonus_objects;
    std::vector<std::shared_ptr<cg3d::Model>> obstacle_objects;


    std::vector<std::shared_ptr<cg3d::Model>> stage_objects;
    int number_of_objects = 0;
    int selected_stage = 0;


    // CollisionDetectionVisitor Params
    std::shared_ptr<cg3d::Model> cube1, cube2;

    // Snake
    Snake snake;
    int number_of_bones = 16;

    // Sound Manager
    SoundManager sound_manager;

    // Stats
    Stats stats;

    // Leaderboard
    Leaderboard leaderboard;

private:
    // Collision boxes for CollisionDetectionVisitor
    void InitCollisionBoxes();

    // Init Stage
    void InitBackground();
    void InitAxis();

    void BuildGameObjects();

    // Stage Builder
    int objective_score;
};