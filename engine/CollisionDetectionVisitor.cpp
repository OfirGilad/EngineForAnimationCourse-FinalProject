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
        scene = (BasicScene*) _scene;
        std::shared_ptr<cg3d::Model> snake_head = scene->snake.GetBones()[0];

        if (scene->IsAnimate())
        {
            CheckSelfCollision();

            if (scene->number_of_objects != 0) {
                for (int i = 0; i < scene->number_of_objects; i++) {
                    std::shared_ptr<cg3d::Model> current_game_object = scene->stage_objects[i];

                    collision_logic.InitCollisionDetection(snake_head, current_game_object, scene->cube1, scene->cube2);
                    bool collision_check = collision_logic.CollisionCheck(snake_head->GetAABBTree(), current_game_object->GetAABBTree(), 0);

                    if (collision_check) {
                        // Remove Collision Boxes
                        snake_head->RemoveChild(scene->cube1);
                        current_game_object->RemoveChild(scene->cube2);

                        // Handle Object Event
                        object_handler.InitObjectHandler(current_game_object->name, scene);
                        object_handler.HandleCollision();
                        scene->root->RemoveChild(current_game_object);
                        scene->stage_objects.erase(scene->stage_objects.begin() + i);
                        scene->number_of_objects--;

                        cout << "Collision" << endl;
                    }
                }
            }
        }
    }

    void CollisionDetectionVisitor::CheckSelfCollision() {
        std::shared_ptr<cg3d::Model> snake_head = scene->snake.GetBones()[0];
        int number_of_bones = scene->snake.GetBones().size();

        for (int i = 2; i < number_of_bones; i++)
        {
            std::shared_ptr<cg3d::Model> inner_bone = scene->snake.GetBones()[i];

            collision_logic.InitCollisionDetection(snake_head, inner_bone, scene->cube1, scene->cube2);
            bool collision_check = collision_logic.CollisionCheck(snake_head->GetAABBTree(), inner_bone->GetAABBTree(), 0);

            if (collision_check) {
                cout << "Collision" << endl;
                scene->SetAnimate(false);
                return;
            }
        }
    }
}
