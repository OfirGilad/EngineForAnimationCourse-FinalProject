//
// Created by Ofir Gilad on 05/02/2023.
//

#include "Snake.h"
#include "IglMeshLoader.h"


using namespace cg3d;
using namespace std;
using namespace Eigen;

Snake::Snake(std::shared_ptr<Movable> root, vector<std::shared_ptr<Camera>> camera_list)
{
    this->root = root;
    this->camera_list = camera_list;
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
    snake_bones[i]->Scale(scaleFactor, Scene::Axis::X);
    snake_bones[i]->SetCenter(Eigen::Vector3f(0, 0, -half_size * scaleFactor));
    root->AddChild(snake_bones[i]);
    i++;

    while(i < number_of_bones)
    {
        snake_bones.push_back(Model::Create("bone " + to_string(i), cylMesh, material));
        snake_bones[i]->Scale(scaleFactor, Scene::Axis::X);
        snake_bones[i]->Translate(size * scaleFactor, Scene::Axis::Z);
        snake_bones[i]->SetCenter(Eigen::Vector3f(0, 0, -half_size * scaleFactor));
        snake_bones[i - 1]->AddChild(snake_bones[i]);
        i++;
    }
    snake_bones[first_index]->Translate({ 0,0,half_size * scaleFactor });
    UpdateCameraView();
    InitCollisionBoxes();

    // Building snake model
    snake_body = Model::Create("snake", snakeMesh, snake_material);
    //root->AddChild(snake_body);
}

// Snake Movement
void Snake::MoveUp()
{
    snake_bones[first_index]->Rotate(0.1f, Scene::Axis::X);

    snake_bones[first_index + 1]->Rotate(-0.1f, Scene::Axis::X);

    /*for (int i = first_index + 1; i <= last_index; i++) {
        snake_bones[i]->Rotate(-0.1f, Scene::Axis::X);
    }*/
}

void Snake::MoveDown()
{
    snake_bones[first_index]->Rotate(-0.1f, Scene::Axis::X);

    snake_bones[first_index + 1]->Rotate(0.1f, Scene::Axis::X);

    /*for (int i = first_index + 1; i <= last_index; i++) {
        snake_bones[i]->Rotate(0.1f, Scene::Axis::X);
    }*/
}

void Snake::MoveLeft()
{
    snake_bones[first_index]->Rotate(0.1f, Scene::Axis::Y);

    snake_bones[first_index + 1]->Rotate(-0.1f, Scene::Axis::Y);

    /*for (int i = first_index + 1; i <= last_index; i++) {
        snake_bones[i]->Rotate(-0.1f, Scene::Axis::Y);
    }*/
}

void Snake::MoveRight()
{
    snake_bones[first_index]->Rotate(-0.1f, Scene::Axis::Y);

    snake_bones[first_index + 1]->Rotate(0.1f, Scene::Axis::Y);

    /*for (int i = first_index + 1; i <= last_index; i++) {
        snake_bones[i]->Rotate(0.1f, Scene::Axis::Y);
    }*/
}


void Snake::UpdateCameraView()
{
    snake_bones[first_index]->AddChild(camera_list[0]);
    snake_bones[first_index]->AddChild(camera_list[1]);
    Vector3f camera_translation = camera_list[1]->GetRotation() * Vector3f(0, 0.5, 0);
    camera_list[1]->Translate(camera_translation);
}

void Snake::InitCollisionBoxes() {
    bones_trees.clear();
    bones_trees.resize(number_of_bones);

    for (int i = 0; i < number_of_bones; i++) {
        auto mesh = snake_bones[i]->GetMeshList();
        V.push_back(mesh[0]->data[0].vertices);
        F.push_back(mesh[0]->data[0].faces);
        bones_trees[i].init(V[0], F[0]);
    }
}
