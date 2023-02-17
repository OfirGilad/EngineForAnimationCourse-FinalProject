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
        object_handler = ObjectHandler();
        Visitor::Run(_scene, camera);
    }

    void CollisionDetectionVisitor::Visit(Scene* _scene) {
        game_manager = ((BasicScene*)_scene)->game_manager;
        
        if (_scene->GetAnimate())
        {
            std::shared_ptr<cg3d::Model> snake_head = game_manager->snake.GetBones()[0];

            if (CheckSelfCollision()) {
                _scene->SetAnimate(false);
                cout << "Self Collision" << endl;
            }

            if (CheckBackgoroundCollision()) {
                _scene->SetAnimate(false);
                cout << "Backgound Collision" << endl;
            }

            if (game_manager->number_of_objects != 0) {
                for (int i = 0; i < game_manager->number_of_objects; i++) {
                    std::shared_ptr<cg3d::Model> current_game_object = game_manager->stage_objects[i];

                    collision_logic.InitCollisionDetection(snake_head, current_game_object, game_manager->cube1, game_manager->cube2);
                    bool collision_check = collision_logic.CollisionCheck(snake_head->GetAABBTree(), current_game_object->GetAABBTree(), 0);

                    if (collision_check) {
                        // Remove Collision Boxes
                        snake_head->RemoveChild(game_manager->cube1);
                        current_game_object->RemoveChild(game_manager->cube2);

                        // Handle Object Event
                        object_handler.InitObjectHandler(current_game_object->name, game_manager);
                        object_handler.HandleCollision();
                        game_manager->root->RemoveChild(current_game_object);
                        game_manager->stage_objects.erase(game_manager->stage_objects.begin() + i);
                        game_manager->number_of_objects--;
                    }
                }
            }
        }
    }

    bool CollisionDetectionVisitor::CheckSelfCollision() {
        std::shared_ptr<cg3d::Model> snake_head = game_manager->snake.GetBones()[0];
        int number_of_bones = game_manager->snake.GetBones().size();

        for (int i = 2; i < number_of_bones; i++)
        {
            std::shared_ptr<cg3d::Model> inner_bone = game_manager->snake.GetBones()[i];

            collision_logic.InitCollisionDetection(snake_head, inner_bone, game_manager->cube1, game_manager->cube2);
            bool collision_check = collision_logic.CollisionCheck(snake_head->GetAABBTree(), inner_bone->GetAABBTree(), 0);

            if (collision_check) {
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
