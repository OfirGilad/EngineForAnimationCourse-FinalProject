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
        number_of_bones = scene->snake.GetBones().size() - 1;
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
                    std::string curr_bone_name = std::string("bone ") + std::to_string(bone_index + 1);
                    //std::string prev_bone_name = std::string("bone ") + std::to_string(bone_index);

                    if (model->name == "bone 1" && bone_index == 0) {
                        vector2x = model->Tout.rotation() * vector1x;
                        quaternionx = Eigen::Quaternionf::FromTwoVectors(vector2x, vector1x);
                        quaternionx = quaternionx.slerp(0.95, Eigen::Quaternionf::Identity());
                        model->Rotate(quaternionx);

                        vector2y = model->Tout.rotation() * vector1y;
                        quaterniony = Eigen::Quaternionf::FromTwoVectors(vector2y, vector1y);
                        quaterniony = quaterniony.slerp(0.95, Eigen::Quaternionf::Identity());
                        model->Rotate(quaterniony);

                        bone_index = (bone_index + 1) % number_of_bones;
                    }
                    else if (model->name == curr_bone_name) {
                        //quaternion = quaternion.conjugate().slerp(0.9, Eigen::Quaternionf::Identity());

                        quaternionx = quaternionx.conjugate();
                        model->Rotate(quaternionx);

                        quaterniony = quaterniony.conjugate();
                        model->Rotate(quaterniony);

                        //

                        vector2x = model->Tout.rotation() * vector1x;
                        quaternionx = Eigen::Quaternionf::FromTwoVectors(vector2x, vector1x);
                        quaternionx = quaternionx.slerp(0.95, Eigen::Quaternionf::Identity());
                        model->Rotate(quaternionx);

                        vector2y = model->Tout.rotation() * vector1y;
                        quaterniony = Eigen::Quaternionf::FromTwoVectors(vector2y, vector1y);
                        quaterniony = quaterniony.slerp(0.95, Eigen::Quaternionf::Identity());
                        model->Rotate(quaterniony);
                        
                        bone_index = (bone_index + 1) % number_of_bones;
                    }
                    
                }
                Visitor::Visit(model); // draw children first
            }
        }
    }
}