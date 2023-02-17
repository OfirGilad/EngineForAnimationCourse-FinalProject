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
    virtual void InitObject(GameManager* game_manager) = 0;
    virtual void CollisionWithObject() = 0;
    virtual ~GameObject() {}

    GameManager* game_manager;
    int stage_number;
};

class HealthObject : public GameObject
{
public:
    void InitObject(GameManager* game_manager) override;
    void CollisionWithObject() override;

private:
    int health_value, current_health, max_health;
};

class ScoreObject : public GameObject
{
public:
    void InitObject(GameManager* game_manager) override;
    void CollisionWithObject() override;

private:
    int score_value;
};

class GoldObject : public GameObject
{
public:
    void InitObject(GameManager* game_manager) override;
    void CollisionWithObject() override;

private:
    int gold_value;
};

class BonusObject : public GameObject
{
public:
    void InitObject(GameManager* game_manager) override;
    void CollisionWithObject() override;

private:
    int bonus_duration;
};

class ObstacleObject : public GameObject
{
public:
    void InitObject(GameManager* game_manager) override;
    void CollisionWithObject() override;

private:
    int damage_value;
};


class ObjectHandler
{
public:
    ObjectHandler() {};
    void InitObjectHandler(string object_name, GameManager* game_manager);
    void HandleCollision();

    string object_name;
    GameManager* game_manager;
    GameObject* game_object;
};
