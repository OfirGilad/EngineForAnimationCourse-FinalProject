//
// Created by Ofir Gilad on 06/02/2023.
//

#ifndef ENGINEFORANIMATIONCOURSE_FINALPROJECT_ANIMATIONVISITOR_H
#define ENGINEFORANIMATIONCOURSE_FINALPROJECT_ANIMATIONVISITOR_H

#endif //ENGINEFORANIMATIONCOURSE_FINALPROJECT_ANIMATIONVISITOR_H

#include "Visitor.h"
#include "Model.h"
#include <vector>
#include "../tutorial/FinalProject/BasicScene.h"


namespace cg3d
{
    class AnimationVisitor : public Visitor
    {
    public:
        void Run(Scene* scene, Camera* camera) override;
        void Visit(Model* model) override;

    private:
        BasicScene* scene;
        Eigen::Quaternionf quaternionx, quaterniony;
        int bone_index = 0;
        int number_of_bones = 15; // without the first bone
        bool bones_found_flag = false;
    };
}