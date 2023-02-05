//
// Created by Ofir Gilad on 05/02/2023.
//

#include "Snake.h"
#include "IglMeshLoader.h"


using namespace cg3d;
using namespace std;
using namespace Eigen;

Snake::Snake(std::shared_ptr<Movable> root, std::shared_ptr<Camera> camera) {
    this->root = root;
    this->camera = camera;
}

void Snake::InitSnake()
{
    // Creating meshes
    auto cylMesh{ IglLoader::MeshFromFiles("cyl_igl", "data/zcylinder.obj") };
    auto snakeMesh{ IglLoader::MeshFromFiles("cyl_igl", "data/snake1.obj") };

    // Creating textures
    auto program = std::make_shared<Program>("shaders/phongShader");
    auto material{ std::make_shared<Material>("material", program) }; // empty material
    auto snake_material{ std::make_shared<Material>("snake_material", program) }; // empty material

    material->AddTexture(0, "textures/box0.bmp", 2);
    snake_material->AddTexture(0, "textures/snake.jpg", 2);

    // Building bones models
    float scaleFactor = 1;
    int i = 0;
    snake_bones.push_back(Model::Create("bone " + to_string(i), cylMesh, material));
    snake_bones[i]->Scale(scaleFactor, Movable::Axis::X);
    snake_bones[i]->SetCenter(Eigen::Vector3f(0, 0, -half_size * scaleFactor));
    root->AddChild(snake_bones[i]);
    i++;

    while(i < number_of_bones)
    {
        snake_bones.push_back(Model::Create("bone " + to_string(i), cylMesh, material));
        snake_bones[i]->Scale(scaleFactor, Movable::Axis::X);
        snake_bones[i]->Translate(size * scaleFactor, Movable::Axis::Z);
        snake_bones[i]->SetCenter(Eigen::Vector3f(0, 0, -half_size * scaleFactor));
        snake_bones[i - 1]->AddChild(snake_bones[i]);
        i++;
    }
    snake_bones[0]->Translate({ 0,0,half_size * scaleFactor });

    // Building snake model
    snake_body = Model::Create("snake", snakeMesh, snake_material);
    //root->AddChild(snake_body);
}


// Snake Movement
void Snake::MoveUp() {
    snake_bones[first_index]->Rotate(0.1f, Movable::Axis::X);

    if (view_state == 1) {
        ChangeCameraView(view_state);
    }
}

void Snake::MoveDown() {
    snake_bones[first_index]->Rotate(-0.1f, Movable::Axis::X);

    if (view_state == 1) {
        ChangeCameraView(view_state);
    }
}

void Snake::MoveLeft() {
    snake_bones[first_index]->Rotate(0.1f, Movable::Axis::Y);

    if (view_state == 1) {
        ChangeCameraView(view_state);
    }
}

void Snake::MoveRight() {
    snake_bones[first_index]->Rotate(-0.1f, Movable::Axis::Y);

    if (view_state == 1) {
        ChangeCameraView(view_state);
    }
}

void Snake::SwitchView() {
    if (view_state == 0) {
        view_state = 1;
        ChangeCameraView(view_state);
    }
    else {
        view_state = 0;
        ChangeCameraView(view_state);
    }
}

void Snake::ChangeCameraView(int state) {
    // Global View: state=0
    if (state == 0) {
        Matrix4f center = Matrix4f();
        center.setIdentity();
        camera->SetTransform(center);
        camera->Translate(22, Movable::Axis::Z);
    }
    // Snake View: state=1
    else {
        camera->SetTransform(snake_bones[first_index]->GetAggregatedTransform());
        Vector3f camera_translation = camera->GetRotation() * Vector3f(0, 0.5, 0);
        camera->Translate(camera_translation);
    }
}
