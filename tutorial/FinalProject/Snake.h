//
// Created by Ofir Gilad on 05/02/2023.
//

#ifndef ENGINEFORANIMATIONCOURSE_FINALPROJECT_SNAKE_H
#define ENGINEFORANIMATIONCOURSE_FINALPROJECT_SNAKE_H

#endif //ENGINEFORANIMATIONCOURSE_FINALPROJECT_SNAKE_H

#pragma once
#include "Scene.h"

#include <memory>
#include <utility>

using namespace cg3d;
using namespace std;

class Snake
{
public:
    Snake() {};
    Snake(std::shared_ptr<Movable> root, std::shared_ptr<Camera> camera);
    void InitSnake();
    void UpdateCameraView();

    void MoveUp();
    void MoveDown();
    void MoveLeft();
    void MoveRight();

private:
    std::shared_ptr<Camera> camera;
    std::shared_ptr<Movable> root;
    std::vector<std::shared_ptr<cg3d::Model>> snake_bones;
    std::shared_ptr <cg3d::Model> snake_body;
    float size = 1.6;
    float half_size = size / 2;
    int number_of_bones = 3;
    int first_index = 0;
    int last_index = number_of_bones;
    int view_state = 0;
};