//
// Created by Ofir Gilad on 06/02/2023.
//

#include "AnimationVisitor.h"

#include <GL.h>
#include "Model.h"
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
        Visitor::Visit(model); // draw children first
        if (scene->IsAnimate()) {
            if (model->name == std::string("bone 0")) {
                model->TranslateInSystem(system, Eigen::Vector3f(0, 0, -0.1f));
            }
        }
    }
}