//
// Created by Ofir Gilad on 06/02/2023.
//

#include "AnimationVisitor.h"
#include "Utility.h"


namespace cg3d
{
    void AnimationVisitor::Run(Scene* _scene, Camera* camera)
    {
        scene = (BasicScene *) _scene;
        Visitor::Run(scene, camera);
    }

    void AnimationVisitor::Visit(Model* model)
    {
        Eigen::Matrix3f system = model->GetRotation().transpose();

        Eigen::Vector3f vector1x = Eigen::Vector3f(1, 0, 0);
        Eigen::Vector3f vector2x;

        Eigen::Vector3f vector1y = Eigen::Vector3f(0, 1, 0);
        Eigen::Vector3f vector2y;

        if (scene->IsAnimate())
        {
            if (model->name.find("bone") != std::string::npos)
            {
                bones_found_flag = true;

                if (model->name == std::string("bone 0"))
                {
                    model->TranslateInSystem(system, Eigen::Vector3f(0, 0, -0.1f));
                    //scene->snake.Skinning();
                }
                else {
                    std::string bone_name1 = std::string("bone ") + std::to_string(bone_index + 1);
                    std::string bone_name2 = std::string("bone ") + std::to_string(bone_index + 2);

                    if (model->name == bone_name1) {
                        vector2x = model->Tout.rotation() * Eigen::Vector3f(1, 0, 0);
                        quaternionx = Eigen::Quaternionf::FromTwoVectors(vector2x, vector1x);
                        quaternionx = quaternionx.slerp(0.5, Eigen::Quaternionf::Identity());
                        model->Rotate(quaternionx);

                        vector2y = model->Tout.rotation() * Eigen::Vector3f(0, 1, 0);
                        quaterniony = Eigen::Quaternionf::FromTwoVectors(vector2y, vector1y);
                        quaterniony = quaterniony.slerp(0.5, Eigen::Quaternionf::Identity());
                        model->Rotate(quaterniony);

                        
                    }
                    else if (model->name == bone_name2) {
                        //quaternion = quaternion.conjugate().slerp(0.9, Eigen::Quaternionf::Identity());


                        quaternionx = quaternionx.conjugate();
                        model->Rotate(quaternionx);

                        quaterniony = quaterniony.conjugate();
                        model->Rotate(quaterniony);

                        //

                        /*vector2x = model->Tout.rotation() * Eigen::Vector3f(1, 0, 0);
                        quaternionx = Eigen::Quaternionf::FromTwoVectors(vector2x, vector1x);
                        quaternionx = quaternionx.slerp(0.9, Eigen::Quaternionf::Identity());
                        model->Rotate(quaternionx);

                        vector2y = model->Tout.rotation() * Eigen::Vector3f(0, 1, 0);
                        quaterniony = Eigen::Quaternionf::FromTwoVectors(vector2y, vector1y);
                        quaterniony = quaterniony.slerp(0.9, Eigen::Quaternionf::Identity());
                        model->Rotate(quaterniony);*/

                        /*vector2 = model->Tout.rotation() * Eigen::Vector3f(1, 0, 0);
                        quaternion = Eigen::Quaternionf::FromTwoVectors(vector2, vector1);
                        quaternion = quaternion.slerp(0.9, Eigen::Quaternionf::Identity());
                        model->Rotate(quaternion);*/
                    }
                    
                }
                Visitor::Visit(model); // draw children first
            }
            else {
                // If flag is on - cycle ended
                /*if (bones_found_flag) {
                    bones_found_flag = false;
                    bone_index = (bone_index + 1) % number_of_bones;
                }*/
            }
        }
    }
}