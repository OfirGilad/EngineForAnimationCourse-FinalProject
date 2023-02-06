//
// Created by Ofir Gilad on 06/02/2023.
//

#ifndef ENGINEFORANIMATIONCOURSE_FINALPROJECT_ANIMATIONVISITOR_H
#define ENGINEFORANIMATIONCOURSE_FINALPROJECT_ANIMATIONVISITOR_H

#endif //ENGINEFORANIMATIONCOURSE_FINALPROJECT_ANIMATIONVISITOR_H

#include "Visitor.h"
#include <vector>


namespace cg3d
{
    class AnimationVisitor : public Visitor
    {
    public:
        void Run(Scene* scene, Camera* camera) override;
        void Visit(Model* model) override;

    private:
        std::vector<Model*> models;
        Scene* scene;
    };
}