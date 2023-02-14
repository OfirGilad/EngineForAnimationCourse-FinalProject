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
    i++;

    while(i < number_of_bones)
    {
        snake_bones.push_back(ObjLoader::ModelFromObj("bone " + to_string(i), "data/zcylinder.obj", snake_material));
        snake_bones[i]->Scale(scaleFactor, Scene::Axis::X);
        snake_bones[i]->Translate(bone_size * scaleFactor, Scene::Axis::Z);
        snake_bones[i]->SetCenter(Eigen::Vector3f(0, 0, -(bone_size / 2) * scaleFactor));
        snake_bones[i]->showWireframe = false;
        snake_bones[i - 1]->AddChild(snake_bones[i]);
        i++;
    }
    snake_bones[first_index]->Translate({ 0,0,(bone_size / 2) * scaleFactor });

    // Creating snake meshe
    snake_body = ObjLoader::ModelFromObj("snake", "data/snake1.obj", snake_material);
    snake_body->Scale(Vector3f(1, 1, number_of_bones));
    auto mesh = snake_body->GetMeshList();
    OV = mesh[0]->data[0].vertices;
    OF = mesh[0]->data[0].faces;
    root->AddChild(snake_body);
    snake_body->Translate((bone_size * number_of_bones)/2.f, Scene::Axis::Z);

    UpdateCameraView();
    //InitBonesData();
    //SkinningInit();
}


void Snake::ShowSnake() {
    for (int i = 0; i < number_of_bones; i++) {
        snake_bones[i]->isHidden = false;
    }
}

void Snake::HideSnake() {
    for (int i = 0; i < number_of_bones; i++) {
        snake_bones[i]->isHidden = true;
    }
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
    Vector3f camera_translation = camera_list[1]->GetRotation() * Vector3f(0, 0.5, 0);
    camera_list[1]->Translate(camera_translation);
}

//void Snake::InitBonesData() {
    //vC.resize(number_of_bones + 1);
    //vT.resize(number_of_bones + 1);
    //vQ.resize(number_of_bones + 1, Eigen::Quaterniond::Identity());
    //Eigen::Vector3d min = V.colwise().minCoeff();
    //double min_z = min[2];

    ////set parents 
    ///*P.clear();
    //P.push_back(-1);
    //for (int i = 1; i < number_of_bones + 1; i++) {
    //    P.push_back(i - 1);
    //}*/
    //BE.resize(16, 2);
    //C.resize(17, 3);
    //CT.resize(32, 3);
    //BE << 0, 1,
    //    1, 2,
    //    2, 3,
    //    3, 4,
    //    4, 5,
    //    5, 6,
    //    6, 7,
    //    7, 8,
    //    8, 9,
    //    9, 10,
    //    10, 11,
    //    11, 12,
    //    12, 13,
    //    13, 14,
    //    14, 15,
    //    15, 16;


    //RestartSnake();
    //viewer->data_list[0].add_points(points, Eigen::RowVector3d(0, 0, 1));
    //CalculateWeight(V, min_z);
//}

void Snake::RestartData() {
    vT.resize(number_of_bones + 1);
    vQ.resize(number_of_bones + 1, Eigen::Quaterniond::Identity());

    //viewer->data(0).set_vertices(V);
    Eigen::Vector3d min = V.colwise().minCoeff();
    Eigen::Vector3d max = V.colwise().maxCoeff();
    double min_z = min[2];
    float max_z = max[2];
    Eigen::Vector3d pos;
    Eigen::MatrixXd points(17, 3);
    for (int i = 0; i < number_of_bones; i++) {
        //vC[i] = pos;
        pos = GetBonePosition(i, -1).cast<double>();
        vT[i] = pos;
        points.row(i) = pos;
        //pos = pos + Eigen::Vector3d(0, 0, bone_size);
    }

    pos = GetBonePosition(last_index, 1).cast<double>();
    vT[number_of_bones] = pos;
    points.row(number_of_bones) = pos;


    //pos = pos + Eigen::Vector3d(0, 0, bone_size);

    //for (int i = 1; i < number_of_bones + 1; i++) {
    //    /*viewer->data_list[i].resetTranslation();
    //    viewer->data_list[i].SetCenterOfRotation(Eigen::Vector3d(0, 0, min_z + (i - 1) * bone_size));
    //    viewer->data_list[i].MyTranslate(vC[i - 1], true);*/
    //}
    //for (int i = 0; i < number_of_bones + 1; i++) {
    //    C.row(i) = vC[i];
    //}
    //for (int i = 1; i < number_of_bones + 1; i++) {
    //    CT.row(i * 2 - 1) = vC[i];
    //}
}

///

void Snake::CalculateWeight() 
{
    int n = V.rows();
    W = Eigen::MatrixXd::Zero(n, number_of_bones + 1);

    //Eigen::Vector3d min_values = V.colwise().minCoeff();
    //double min_z = min_values[2];

    for (int i = 0; i < n; i++) 
    {
        double current_z = V.row(i)[2];

        for (int j = 0; j < number_of_bones; j++) 
        {
            if ((current_z >= GetBonePosition(j, -1).z()) && (current_z <= GetBonePosition(j, 1).z()))
            {
                double result = 1 - (abs(GetBonePosition(j, 1).z() - current_z) / bone_size);
                W.row(i)[j] = result;
                W.row(i)[j + 1] = 1 - result;
                break;
            }
        }
    }
}

#include <igl/directed_edge_parents.h>
#include <igl/per_vertex_normals.h>

void Snake::SkinningInit() {
    // Init new V
    V = Eigen::MatrixXd::Zero(OV.rows(), OV.cols());
    for (int i = 0; i < OV.rows(); i++) {
        Eigen::Vector4f OV_i = Eigen::Vector4f(OV.row(i).x(), OV.row(i).y(), OV.row(i).z(), 1);
        Eigen::Vector4f V_i = snake_body->GetAggregatedTransform() * OV_i;
        V.row(i) = Eigen::Vector3d(V_i.x(), V_i.y(), V_i.z());
    }
    U = V;

    // Set Snake Mesh new data
    igl::per_vertex_normals(V, OF, VN);
    T = Eigen::MatrixXd::Zero(V.rows(), 2);
    auto mesh = snake_body->GetMeshList();
    mesh[0]->data.pop_back();
    mesh[0]->data.push_back({ V, OF, VN, T });
    snake_body->SetMeshList(mesh);
    snake_body->SetTransform(Eigen::Matrix4f::Identity());

    // Init C
    C.resize(number_of_bones + 1, 3);
    for (int i = 0; i < number_of_bones; i++)
    {
        C.row(i) = GetBonePosition(i, -1).cast<double>();
    }
    C.row(number_of_bones) = GetBonePosition(last_index, 1).cast<double>();

    // Init BE
    //BE.resize(number_of_bones, 2);
    //for (int i = 0; i < number_of_bones; i++) {
    //    BE.row(i) = Vector2i(i, i + 1);
    //}

    CalculateWeight();

    cout << "W" << endl;
    cout << W << endl;
    cout << "V" << endl;
    cout << V << endl;
    cout << "C" << endl;
    cout << C << endl;

    //igl::lbs_matrix(V, W, M);
}

#include "igl/per_vertex_normals.h"

void Snake::Skinning() {
  
    // Propagate relative rotations via FK to retrieve absolute transformations
    // vQ - rotations of joints
    // vT - translation of joints

    vT.resize(number_of_bones + 1);
    vQ.resize(number_of_bones + 1, Eigen::Quaterniond::Identity());


    for (int i = 0; i < number_of_bones; i++) {
        Vector3d X = GetBonePosition(i, -1).cast<double>();
        Vector3d Y = C.row(i);
        vT[i] = X - Y;
    }
    Vector3d X = GetBonePosition(last_index, 1).cast<double>();
    Vector3d Y = C.row(last_index + 1);

    vT[last_index + 1] = X - Y;


    //// Compute deformation via LBS as matrix multiplication

    igl::dqs(V, W, vQ, vT, U);

    
    
    igl::per_vertex_normals(U, OF, VN);
    T = Eigen::MatrixXd::Zero(U.rows(), 2);
    auto mesh = snake_body->GetMeshList();
    mesh[0]->data.pop_back();
    mesh[0]->data.push_back({ U, OF, VN, T });
    snake_body->SetMeshList(mesh);
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
