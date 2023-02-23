//
// Created by Ofir Gilad on 10/02/2023.
//

#include "CollisionDetectionVisitor.h"
#include "Utility.h"

using namespace std;

namespace cg3d
{
    void CollisionDetectionVisitor::Run(cg3d::Scene* _scene, cg3d::Camera* camera)
    {
        collision_logic = GameLogics();
        Visitor::Run(_scene, camera);
    }

    void CollisionDetectionVisitor::Visit(Scene* _scene) 
    {
        game_manager = ((BasicScene*)_scene)->game_manager;

        float stage_size = game_manager->stats->stage_size;
        backgound_cube_space = Eigen::Vector3f(stage_size, stage_size, stage_size);

        if (_scene->GetAnimate()) {
            std::shared_ptr<cg3d::Model> snake_head = game_manager->snake.GetBones()[0];

            if (CheckSelfCollision()) {
                _scene->SetAnimate(false);
                int current_health = game_manager->stats->current_health;
                game_manager->stats->total_health_points_lost += current_health;
                game_manager->stats->current_health -= current_health;
                cout << "Self Collision" << endl;

                return;
            }

            if (CheckBackgoroundCollision()) {
                _scene->SetAnimate(false);
                int current_health = game_manager->stats->current_health;
                game_manager->stats->total_health_points_lost += current_health;
                game_manager->stats->current_health -= current_health;
                cout << "Backgound Collision" << endl;

                return;
            }

            for (int i = 0; i < int(game_manager->alive_objects.size()); i++) {
                GameObject* current_game_object = game_manager->alive_objects[i];

                collision_logic.InitCollisionDetection(snake_head, current_game_object->model, game_manager->cube1, game_manager->cube2);
                bool collision_check = collision_logic.CollisionCheck(snake_head->GetAABBTree(), current_game_object->model->GetAABBTree(), 0);

                if (collision_check) {
                    // Reset Timers
                    current_game_object->alive_timer.ResetTimer();
                    current_game_object->dead_timer.StartTimer();

                    // Remove Collision Boxes
                    snake_head->RemoveChild(game_manager->cube1);
                    current_game_object->model->RemoveChild(game_manager->cube2);

                    // Move to Dead Objects
                    game_manager->alive_objects.erase(game_manager->alive_objects.begin() + i);
                    game_manager->dead_objects.push_back(current_game_object);

                    // Handle Object Event
                    current_game_object->CollisionWithObject();
                    current_game_object->SetDead();

                    return;
                }
            }
        }
    }

    bool CollisionDetectionVisitor::CheckSelfCollision() {
        std::shared_ptr<cg3d::Model> snake_head = game_manager->snake.GetBones()[0];
        int number_of_bones = int(game_manager->snake.GetBones().size());

        for (int i = 2; i < number_of_bones; i++)
        {
            std::shared_ptr<cg3d::Model> inner_bone = game_manager->snake.GetBones()[i];

            collision_logic.InitCollisionDetection(snake_head, inner_bone, game_manager->cube1, game_manager->cube2);
            bool collision_check = collision_logic.CollisionCheck(snake_head->GetAABBTree(), inner_bone->GetAABBTree(), 0);

            if (collision_check) {
                // Remove Collision Boxes
                snake_head->RemoveChild(game_manager->cube1);
                inner_bone->RemoveChild(game_manager->cube2);

                return true;
            }
        }
        return false;
    }

    bool CollisionDetectionVisitor::CheckBackgoroundCollision() {
        Eigen::Vector3f snake_head_end = game_manager->snake.GetBonePosition(0, -1);

        for (int i = 0; i < 3; i++) {
            if (abs(snake_head_end[i]) > backgound_cube_space[i]) {
                return true;
            }
        }
        return false;
    }
}
