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
        scene = (BasicScene*) _scene;
        collision_logic = GameLogics();
        Visitor::Run(scene, camera);
    }

    void CollisionDetectionVisitor::Visit(Scene* _scene) {
        std::shared_ptr<cg3d::Model> snake_head = scene->snake.GetBones()[0];

        if (scene->IsAnimate())
        {
            CheckSelfCollision();

            collision_logic.InitCollisionDetection(snake_head, scene->sphere1, scene->cube1, scene->cube2);
            bool collision_check = collision_logic.CollisionCheck(snake_head->GetAABBTree(), scene->sphere1->GetAABBTree(), 0);

            if (collision_check) {
                cout << "Collision" << endl;
                scene->SetAnimate(false);
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
