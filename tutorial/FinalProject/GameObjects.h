//
// Created by Ofir Gilad on 13/02/2023.
//

#ifndef ENGINEFORANIMATIONCOURSE_FINALPROJECT_GAMEOBJECTS_H
#define ENGINEFORANIMATIONCOURSE_FINALPROJECT_GAMEOBJECTS_H

#endif //ENGINEFORANIMATIONCOURSE_FINALPROJECT_GAMEOBJECTS_H

#include <string>
#include "BasicScene.h"


class GameObject
{
public:
    virtual void InitObject(BasicScene* scene) = 0;
    virtual void CollisionWithObject() = 0;
    virtual ~GameObject() {}

    BasicScene* scene;
    int stage_number;
};

class HealthObject : public GameObject
{
public:
    void InitObject(BasicScene *scene) override;
    void CollisionWithObject() override;

private:
    int health_value;
};

class ScoreObject : public GameObject
{
public:
    void InitObject(BasicScene *scene) override;
    void CollisionWithObject() override;

private:
    int score_value;
};

class GoldObject : public GameObject
{
public:
    void InitObject(BasicScene *scene) override;
    void CollisionWithObject() override;

private:
    int gold_value;
};

class BonusObject : public GameObject
{
public:
    void InitObject(BasicScene *scene) override;
    void CollisionWithObject() override;

private:
    int bonus_duration;
};

class ObstacleObject : public GameObject
{
public:
    void InitObject(BasicScene *scene) override;
    void CollisionWithObject() override;

private:
    int damage_value;
};


class ObjectHandler
{
public:
    ObjectHandler() {};
    void InitObjectHandler(string object_name, BasicScene* scene);
    void HandleCollision();

    string object_name;
    BasicScene* scene;
    GameObject* game_object;
};
