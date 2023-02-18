//
// Created by Ofir Gilad on 05/02/2023.
//

#include "Snake.h"


using namespace cg3d;
using namespace std;
using namespace Eigen;

Snake::Snake(std::shared_ptr<Movable> root, vector<std::shared_ptr<Camera>> camera_list)
{
    this->root = root;
    this->camera_list = camera_list;
}

void Snake::InitSnake(int num_of_bones)
{
    number_of_bones = num_of_bones;
    last_index = number_of_bones - 1;
    snake_length = bone_size * number_of_bones;

    // Creating textures
    auto program = std::make_shared<Program>("shaders/basicShader");
    auto snake_material = std::make_shared<Material>("snake_material", program);
    snake_material->AddTexture(0, "textures/snake.jpg", 2);

    // Creating bone meshes
    float scaleFactor = 1;
    int i = 0;
    snake_bones.push_back(ObjLoader::ModelFromObj("bone " + to_string(i), "data/zcylinder.obj", snake_material));
    snake_bones[i]->Scale(scaleFactor, Scene::Axis::X);
    snake_bones[i]->SetCenter(Eigen::Vector3f(0, 0, -(bone_size / 2) *scaleFactor));
    snake_bones[i]->showWireframe = false;
    root->AddChild(snake_bones[i]);
    snake_bones[i]->isHidden = true;
    i++;

    // Add Snake Head custom mesh
    auto program1 = std::make_shared<Program>("shaders/basicShader");
    auto material1 = std::make_shared<Material>("snake_material", program1);
    material1->AddTexture(0, "textures/carbon.jpg", 2);
    snake_head = ObjLoader::ModelFromObj("snake head", "../tutorial/objects/snake_head.obj", material1);
    snake_bones[first_index]->AddChild(snake_head);
    snake_head->Scale(0.09);
    snake_head->RotateByDegree(180, Movable::Axis::Y);
    snake_head->Translate(0.8, Movable::Axis::Z);

    while(i < number_of_bones)
    {
        snake_bones.push_back(ObjLoader::ModelFromObj("bone " + to_string(i), "data/zcylinder.obj", snake_material));
        snake_bones[i]->Scale(scaleFactor, Scene::Axis::X);
        snake_bones[i]->Translate(bone_size * scaleFactor, Scene::Axis::Z);
        snake_bones[i]->SetCenter(Eigen::Vector3f(0, 0, -(bone_size / 2) * scaleFactor));
        snake_bones[i]->showWireframe = false;
        snake_bones[i - 1]->AddChild(snake_bones[i]);
        snake_bones[i]->isHidden = true;
        i++;
    }
    snake_bones[first_index]->Translate({ 0,0,(bone_size / 2) * scaleFactor });

    UpdateCameraView();

    SkinningInit();
}


void Snake::ShowSnake() {
    if (enable_skinning) {
        snake_body->isHidden = false;
    }
    else {
        for (int i = 1; i < number_of_bones; i++) {
            snake_bones[i]->isHidden = false;
        }
    }
}

void Snake::HideSnake() {
    if (enable_skinning) {
        snake_body->isHidden = true;
    }
    else {
        for (int i = 1; i < number_of_bones; i++) {
            snake_bones[i]->isHidden = true;
        }
    }
}

void Snake::ResetSnakePosition() {
    snake_bones[first_index]->SetTransform(Matrix4f::Identity());

    /*int i = 1;
    while (i < number_of_bones)
    {
        snake_bones[i]->SetTransform(Matrix4f::Identity());
        snake_bones[i]->Translate(bone_size, Movable::Axis::Z);
        i++;
    }*/
    Skinning();
}

// Snake Movement
void Snake::MoveUp()
{
    snake_bones[first_index]->Rotate(0.1f, Scene::Axis::X);
    snake_bones[first_index + 1]->Rotate(-0.1f, Scene::Axis::X);
}

void Snake::MoveDown()
{
    snake_bones[first_index]->Rotate(-0.1f, Scene::Axis::X);
    snake_bones[first_index + 1]->Rotate(0.1f, Scene::Axis::X);
}

void Snake::MoveLeft()
{
    snake_bones[first_index]->Rotate(0.1f, Scene::Axis::Y);
    snake_bones[first_index + 1]->Rotate(-0.1f, Scene::Axis::Y);
}

void Snake::MoveRight()
{
    snake_bones[first_index]->Rotate(-0.1f, Scene::Axis::Y);
    snake_bones[first_index + 1]->Rotate(0.1f, Scene::Axis::Y);
}

void Snake::RollLeft()
{
    snake_bones[first_index]->Rotate(0.1f, Scene::Axis::Z);
    snake_bones[first_index + 1]->Rotate(-0.1f, Scene::Axis::Z);
}

void Snake::RollRight()
{
    snake_bones[first_index]->Rotate(-0.1f, Scene::Axis::Z);
    snake_bones[first_index + 1]->Rotate(0.1f, Scene::Axis::Z);
}


void Snake::UpdateCameraView()
{
    snake_bones[first_index]->AddChild(camera_list[0]);
    snake_bones[first_index]->AddChild(camera_list[1]);
    Vector3f camera_translation = camera_list[1]->GetRotation() * Vector3f(0, 0.6, 0.8);
    camera_list[1]->Translate(camera_translation);
}


void Snake::CalculateWeight() 
{
    int n = V.rows();
    W = Eigen::MatrixXd::Zero(n, number_of_bones + 1);

    float snake_length = V.colwise().maxCoeff()[2] - V.colwise().minCoeff()[2];
    float bone_length = snake_length / 16.f;

    double min_z = V.colwise().minCoeff()[2];

    for (int i = 0; i < n; i++) {
        double current_z = V.row(i)[2];
        for (int j = 0; j < number_of_bones + 1; j++) {
            if ((current_z >= min_z + bone_length * j) && (current_z <= min_z + bone_length * (j + 1))) {
                double dist = abs(current_z - (min_z + bone_length * j));
                W.row(i)[j] = (bone_length - dist) / bone_length;
                W.row(i)[j + 1] = 1 - W.row(i)[j];
                break;
            }
        }
    }
}


void Snake::SkinningInit() {
    if (!enable_skinning) {
        return;
    }

    // Create snake mesh
    auto program = std::make_shared<Program>("shaders/basicShader");
    auto material = std::make_shared<Material>("snake_material", program);
    auto snake_mesh{IglLoader::MeshFromFiles("snake_igl", "data/snake1.obj")};
    snake_body = Model::Create("snake", snake_mesh, material);
    root->AddChild(snake_body);
    snake_body->isHidden = true;

    // Find transform
    Eigen::MatrixXd OV = snake_body->GetMeshList()[0]->data[0].vertices;
    Eigen::Vector3d translate = Eigen::Vector3d(0.0f, 0.0f, number_of_bones * (bone_size / 2));
    Eigen::Vector3d scale = Eigen::Vector3d(1.0f, 1.0f, number_of_bones);
    Eigen::Affine3d Tout{ Eigen::Affine3d::Identity() };
    Eigen::Affine3d Tin{ Eigen::Affine3d::Identity() };
    Tout.pretranslate(translate);
    Tin.scale(scale);
    Eigen::MatrixX4d transform = Tout.matrix() * Tin.matrix();


    // Find the new V
    V = Eigen::MatrixXd::Zero(OV.rows(), OV.cols());
    for (int i = 0; i < OV.rows(); i++) {
        Eigen::Vector4d OV_i = Eigen::Vector4d(OV.row(i).x(), OV.row(i).y(), OV.row(i).z(), 1);
        Eigen::Vector4d V_i = transform * OV_i;
        V.row(i) = Eigen::Vector3d(V_i[0], V_i[1], V_i[2]);
    }


    // Set Snake Mesh new data
    std::shared_ptr<cg3d::Mesh> new_mesh = std::make_shared<cg3d::Mesh>(
        snake_body->name,
        V,
        snake_body->GetMeshList()[0]->data[0].faces,
        snake_body->GetMeshList()[0]->data[0].vertexNormals,
        snake_body->GetMeshList()[0]->data[0].textureCoords);
    snake_body->SetMeshList({ new_mesh });
    snake_body->SetTransform(Eigen::Matrix4f::Identity());
    

    // Init C
    C.resize(number_of_bones + 1, 3);
    for (int i = 0; i < number_of_bones; i++)
    {
        C.row(i) = GetBonePosition(i, -1).cast<double>();
    }
    C.row(number_of_bones) = GetBonePosition(last_index, 1).cast<double>();


    // Init W
    CalculateWeight();


    // vQ - rotations of joints
    // vT - translation of joints
    vT.resize(number_of_bones + 1);
    vQ.resize(number_of_bones + 1, Eigen::Quaterniond::Identity());
}


void Snake::Skinning() 
{
    if (!enable_skinning) {
        return;
    }

    // Calculate vT
    for (int i = 0; i < number_of_bones; i++) {
        vT[i] = GetBonePosition(i, -1).cast<double>() - (Vector3d)C.row(i);
    }
    vT[number_of_bones] = GetBonePosition(last_index, 1).cast<double>() - (Vector3d)C.row(number_of_bones);

    // Calling DQS
    igl::dqs(V, W, vQ, vT, U);

    // Update mesh
    std::shared_ptr<cg3d::Mesh> new_mesh = std::make_shared<cg3d::Mesh>(
        snake_body->name,
        U,
        snake_body->GetMeshList()[0]->data[0].faces,
        snake_body->GetMeshList()[0]->data[0].vertexNormals,
        snake_body->GetMeshList()[0]->data[0].textureCoords);
    snake_body->SetMeshList({ new_mesh });
}


// Get the position of snake_bones[bone_id] 
// (-1) - start
// 0 - center
// 1 - end
Eigen::Vector3f Snake::GetBonePosition(int bone_id, int position)
{
    Eigen::Vector3f half_length = Eigen::Vector3f(0, 0, 0.8);

    Eigen::Matrix4f bone_transform = snake_bones[bone_id]->GetAggregatedTransform();
    Eigen::Vector3f bone_center = Eigen::Vector3f(bone_transform.col(3).x(), bone_transform.col(3).y(), bone_transform.col(3).z());
    Eigen::Vector3f bone_requested_position = bone_center + snake_bones[bone_id]->GetRotation() * half_length * position;

    return bone_requested_position;
}
