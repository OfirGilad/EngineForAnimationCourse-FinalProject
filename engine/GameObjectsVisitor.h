//
// Created by Ofir Gilad on 21/02/2023.
//

#ifndef ENGINEFORANIMATIONCOURSE_FINALPROJECT_GAMEOBJECTSVISITOR_H
#define ENGINEFORANIMATIONCOURSE_FINALPROJECT_GAMEOBJECTSVISITOR_H

#endif //ENGINEFORANIMATIONCOURSE_FINALPROJECT_GAMEOBJECTSVISITOR_H

#pragma once
#include "../tutorial/FinalProject/BasicScene.h"
#include "Visitor.h"
#include "Model.h"
#include <vector>

#include <igl/per_vertex_normals.h>
#include <iostream>
#include <random>
#include <utility>


namespace cg3d 
{
    class GameObjectsVisitor : public Visitor
	{
    public:
        void Run(Scene* scene, Camera* camera) override;
        void Visit(Scene* scene) override;
		
    private:
        void moveAccordingToBeizerCurve(Model* model);
        void setModelBezier(Eigen::Vector3f vectors, std::shared_ptr<Model> model);
        //std::shared_ptr<Model> generateObjectBezier(int material_id, int model_id, std::string name, float scale);
        void drawTheBeizerCurve(std::shared_ptr<Model> model);

        std::shared_ptr<Material> material;
        std::shared_ptr<Program> program;
        BasicScene *basicScene;
        bool is_visitor_inited = false;
        float minx = -10.0f;
        float maxx = 10.0f;
        float miny = -5.0f;
        float maxy = 5.0f;
        float minz = -5.0f;
        float maxz = 5.0f;
        //float generate_random_number(float min, float max);

        GameManager* game_manager;
    };
}


