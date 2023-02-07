//
// Created by Ofir Gilad on 06/02/2023.
//

#include "AnimationVisitor.h"

#include <GL.h>
#include "Scene.h"
#include "Utility.h"


namespace cg3d
{
    void AnimationVisitor::Run(Scene* _scene, Camera* camera)
    {
        Visitor::Run(scene = _scene, camera);
    }

    void AnimationVisitor::Visit(Model* model)
    {
        Eigen::Matrix3f system = model->GetRotation().transpose();

        Eigen::Vector3f vector1 = Eigen::Vector3f(1, 0, 0);
        Eigen::Vector3f vector2;

        if (scene->IsAnimate())
        {
            if (model->name.find("bone") != std::string::npos)
            {
                bones_found_flag = true;

                if (model->name == std::string("bone 0"))
                {
                    model->TranslateInSystem(system, Eigen::Vector3f(0, 0, -0.1f));
                }
                else {
                    std::string bone_name1 = std::string("bone ") + std::to_string(bone_index + 1);
                    std::string bone_name2 = std::string("bone ") + std::to_string(bone_index + 2);

                    if (model->name == bone_name1) {
                        vector2 = model->Tout.rotation() * Eigen::Vector3f(1, 0, 0);
                        quaternion = Eigen::Quaternionf::FromTwoVectors(vector2, vector1);
                        quaternion = quaternion.slerp(0.9, Eigen::Quaternionf::Identity());
                        model->Rotate(quaternion);
                    }
                    else if (model->name == bone_name2) {
                        quaternion = quaternion.conjugate().slerp(0.9, Eigen::Quaternionf::Identity());
                        model->Rotate(quaternion);
                    }
                }
                Visitor::Visit(model); // draw children first
            }
            else {
                // If flag is on - cycle ended
                if (bones_found_flag) {
                    bones_found_flag = false;
                    bone_index = (bone_index + 1) % number_of_bones;
                }
            }
        }
    }
}