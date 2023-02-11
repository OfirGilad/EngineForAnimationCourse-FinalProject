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
    snake_bones[i]->SetCenter(Eigen::Vector3f(0, 0, -(bone_size / 2) *scaleFactor));
    snake_bones[i]->showWireframe = true;
    snake_bones[i]->showFaces = false;
    root->AddChild(snake_bones[i]);
    i++;

    while(i < number_of_bones)
    {
        snake_bones.push_back(Model::Create("bone " + to_string(i), cylMesh, material));
        snake_bones[i]->Scale(scaleFactor, Scene::Axis::X);
        snake_bones[i]->Translate(bone_size * scaleFactor, Scene::Axis::Z);
        snake_bones[i]->SetCenter(Eigen::Vector3f(0, 0, -(bone_size / 2) * scaleFactor));
        snake_bones[i]->showWireframe = true;

        snake_bones[i]->showFaces = false;
        snake_bones[i - 1]->AddChild(snake_bones[i]);
        i++;
    }
    snake_bones[first_index]->Translate({ 0,0,(bone_size / 2) * scaleFactor });

    // Building snake model
    snake_body = Model::Create("snake", snakeMesh, snake_material);
    snake_body->Scale(Vector3f(1, 1, number_of_bones));
    auto mesh = snake_body->GetMeshList();
    V = mesh[0]->data[0].vertices;
    F = mesh[0]->data[0].faces;
    root->AddChild(snake_body);
    snake_body->Translate((bone_size * number_of_bones)/2.f, Scene::Axis::Z);

    UpdateCameraView();
    InitBonesData();
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

void Snake::RollLeft()
{
    snake_bones[first_index]->Rotate(0.1f, Scene::Axis::Z);

    snake_bones[first_index + 1]->Rotate(-0.1f, Scene::Axis::Z);

    /*for (int i = first_index + 1; i <= last_index; i++) {
        snake_bones[i]->Rotate(-0.1f, Scene::Axis::Z);
    }*/
}

void Snake::RollRight()
{
    snake_bones[first_index]->Rotate(-0.1f, Scene::Axis::Z);

    snake_bones[first_index + 1]->Rotate(0.1f, Scene::Axis::Z);

    /*for (int i = first_index + 1; i <= last_index; i++) {
        snake_bones[i]->Rotate(0.1f, Scene::Axis::Z);
    }*/
}


void Snake::UpdateCameraView()
{
    snake_bones[first_index]->AddChild(camera_list[0]);
    snake_bones[first_index]->AddChild(camera_list[1]);
    Vector3f camera_translation = camera_list[1]->GetRotation() * Vector3f(0, 0.5, 0);
    camera_list[1]->Translate(camera_translation);
}

void Snake::InitBonesData() {
    vC.resize(number_of_bones + 1);
    vT.resize(number_of_bones + 1);
    vQ.resize(number_of_bones + 1, Eigen::Quaterniond::Identity());
    Eigen::Vector3d min = V.colwise().minCoeff();
    double min_z = min[2];

    //set parents 
    P.clear();
    P.push_back(-1);
    for (int i = 1; i < number_of_bones + 1; i++) {
        P.push_back(i - 1);
    }
    BE.resize(16, 2);
    C.resize(17, 3);
    CT.resize(32, 3);
    BE << 0, 1,
        1, 2,
        2, 3,
        3, 4,
        4, 5,
        5, 6,
        6, 7,
        7, 8,
        8, 9,
        9, 10,
        10, 11,
        11, 12,
        12, 13,
        13, 14,
        14, 15,
        15, 16;


    RestartSnake();
    //viewer->data_list[0].add_points(points, Eigen::RowVector3d(0, 0, 1));
    CalculateWeight(V, min_z);
}

void Snake::RestartSnake() {
    //viewer->data(0).set_vertices(V);
    Eigen::Vector3d min = V.colwise().minCoeff();
    Eigen::Vector3d max = V.colwise().maxCoeff();
    double min_z = min[2];
    float max_z = max[2];
    Eigen::Vector3d pos(0, 0, min_z);
    Eigen::MatrixXd points(17, 3);
    for (int i = 0; i < number_of_bones + 1; i++) {
        vC[i] = pos;
        vT[i] = pos;
        points.row(i) = pos;
        pos = pos + Eigen::Vector3d(0, 0, bone_size);
    }
    for (int i = 1; i < number_of_bones + 1; i++) {
        /*viewer->data_list[i].resetTranslation();
        viewer->data_list[i].SetCenterOfRotation(Eigen::Vector3d(0, 0, min_z + (i - 1) * bone_size));
        viewer->data_list[i].MyTranslate(vC[i - 1], true);*/
    }
    for (int i = 0; i < number_of_bones + 1; i++) {
        C.row(i) = vC[i];
    }
    for (int i = 1; i < number_of_bones + 1; i++) {
        CT.row(i * 2 - 1) = vC[i];
    }
}

void Snake::CalculateWeight(Eigen::MatrixXd& V, double min_z) 
{
    int n = V.rows();
    W = Eigen::MatrixXd::Zero(n, number_of_bones + 1);
    for (int i = 0; i < n; i++) 
    {
        double curr_z = V.row(i)[2];
        for (int j = 0; j < number_of_bones + 1; j++) 
        {
            if (curr_z >= min_z + bone_size * j && curr_z <= min_z + bone_size * (j + 1))
            {
                double res = abs(curr_z - (min_z + bone_size * (j + 1))) * 10;
                W.row(i)[j] = res;
                W.row(i)[j + 1] = 1 - res;
                break;
            }
        }
    }
}


#include "igl/per_vertex_normals.h"

void Snake::Skinning() {
    // igl::forward_kinematics(C, BE, P, anim_pose, vQ, vT);

    const int dim = C.cols();
    Eigen::MatrixXd T(BE.rows() * (dim + 1), dim);

    for (int e = 0; e < BE.rows(); e++)
    {
        Eigen::Affine3d a = Eigen::Affine3d::Identity();
        a.translate(vT[e]);
        a.rotate(vQ[e]);
        T.block(e * (dim + 1), 0, dim + 1, dim) = a.matrix().transpose().block(0, 0, dim + 1, dim);
    }
    igl::dqs(V, W, vQ, vT, U);
    //move joints according to T, returns new position in CT and BET
    igl::deform_skeleton(C, BE, T, CT, BET);

    
    //igl::opengl::glfw::Viewer viewer;
    //viewer.data().set_mesh(V, F);
    //viewer.data().set_vertices(U);
    //viewer.data().set_edges(CT, BET, Eigen::RowVector3d(70. / 255., 252. / 255., 167. / 255.));
    //
    //V = viewer.data().V;
    //F = viewer.data().F;
    //
    //igl::per_vertex_normals(V, F, VN);
    //T = Eigen::MatrixXd::Zero(V.rows(), 2);
    //auto mesh = snake_body->GetMeshList();
    //mesh[0]->data.pop_back();
    //mesh[0]->data.push_back({ V, F, VN, T });
    //snake_body->SetMeshList(mesh);
    //snake_body->Translate(0, Scene::Axis::Z);


    //viewer->data(0).set_vertices(U);
    //viewer->data(0).set_edges(CT, BET, Eigen::RowVector3d(70. / 255., 252. / 255., 167. / 255.));
    for (int i = 0; i < number_of_bones + 1; i++)
        vC[i] = vT[i];

    //for (int i = 1; i < numberOfJoints + 1; i++)
        //viewer->data_list[i].SetTranslation(CT.row(2 * i - 1));
}
