//
// Created by Ofir Gilad on 05/02/2023.
//

#ifndef ENGINEFORANIMATIONCOURSE_FINALPROJECT_SNAKE_H
#define ENGINEFORANIMATIONCOURSE_FINALPROJECT_SNAKE_H

#endif //ENGINEFORANIMATIONCOURSE_FINALPROJECT_SNAKE_H

#pragma once
#include "Scene.h"
#include "IglMeshLoader.h"
#include "ObjLoader.h"

#include "igl/AABB.h"

#include <igl/directed_edge_orientations.h>
#include <igl/directed_edge_parents.h>
#include <igl/forward_kinematics.h>
#include <igl/PI.h>
#include <igl/lbs_matrix.h>
#include <igl/deform_skeleton.h>
#include <igl/dqs.h>
#include <igl/readDMAT.h>
#include <igl/readOBJ.h>
#include <igl/readTGF.h>
#include <igl/opengl/glfw/Viewer.h>

#include <Eigen/Geometry>
#include <Eigen/StdVector>
#include <vector>
#include <algorithm>
#include <iostream>

#include <memory>
#include <utility>

using namespace cg3d;
using namespace std;

class Snake
{
public:
    Snake() {};
    Snake(std::shared_ptr<Movable> root, vector<std::shared_ptr<Camera>> camera_list);
    void InitSnake(int num_of_bones);

    void MoveUp();
    void MoveDown();
    void MoveLeft();
    void MoveRight();
    void RollLeft();
    void RollRight();

    void SkinningInit();
    void Skinning();


    std::vector<std::shared_ptr<cg3d::Model>> GetBones() { return snake_bones; }
    Eigen::Vector3f GetBonePosition(int bone_id, int position);

private:
    void UpdateCameraView();
    
    //void InitBonesData();
    void RestartData();
    void CalculateWeight();

    


    vector<std::shared_ptr<Camera>> camera_list;
    std::shared_ptr<Movable> root;
    std::vector<std::shared_ptr<cg3d::Model>> snake_bones;
    std::shared_ptr <cg3d::Model> snake_body;
    float bone_size = 1.6;
    int number_of_bones = 0;
    int first_index = 0;
    int last_index = 0;
    int view_state = 0;
    float snake_length = 0;


    // Skinning Part
    typedef
        std::vector<Eigen::Quaterniond, Eigen::aligned_allocator<Eigen::Quaterniond>>
        RotationList;

    // W - weights matrix
    // BE - Edges between joints
    // C - joints positions
    // P - parents
    // M - weights per vertex per joint matrix
    // U - new vertices position after skinning
    Eigen::MatrixXd C, V, W, M, U;
    Eigen::MatrixXi F, BE;
    Eigen::MatrixXi UF;

    //vector<int> P;
    Eigen::VectorXi P;

    std::vector<RotationList > poses; // rotations of joints for animation
    double anim_t = 0.0;
    double anim_t_dir = 0.015;

    //igl::opengl::glfw::Viewer viewer;


    // Propagate relative rotations via FK to retrieve absolute transformations
    // vQ - rotations of joints
    // vT - translation of joints
    RotationList vQ;
    vector<Eigen::Vector3d> vT;
    vector<Eigen::Vector3d> vC;

    Eigen::MatrixXd VN, FN, T;
};