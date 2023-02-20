//
// Created by Ofir Gilad on 13/02/2023.
//

#ifndef ENGINEFORANIMATIONCOURSE_FINALPROJECT_GAMEOBJECTS_H
#define ENGINEFORANIMATIONCOURSE_FINALPROJECT_GAMEOBJECTS_H

#endif //ENGINEFORANIMATIONCOURSE_FINALPROJECT_GAMEOBJECTS_H

#include <string>
#include "Scene.h"
#include "SoundManager.h"
#include "Stats.h"
#include "GameTimer.h"

using namespace cg3d;

class GameObject
{
public:
    virtual void InitObject(Stats* stats, SoundManager* sound_manager, std::shared_ptr<Model> model);
    virtual void CollisionWithObject() = 0;
    //virtual void MoveObject();

    //virtual void SetAlive();
    //virtual void SetDead();

    virtual ~GameObject() {}

    Stats* stats;
    SoundManager* sound_manager;
    std::shared_ptr<Model> model;
    int stage_number;
    GameTimer game_timer;
};

class HealthObject : public GameObject
{
public:
    void CollisionWithObject() override;

private:
    int health_value, current_health, max_health;
};

class ScoreObject : public GameObject
{
public:
    void CollisionWithObject() override;

private:
    int score_value, current_score, score_multiplier;
};

class GoldObject : public GameObject
{
public:
    void CollisionWithObject() override;

private:
    int gold_value, gold, gold_multiplier;
};

class BonusObject : public GameObject
{
public:
    void CollisionWithObject() override;

private:
    int bonus_duration;
};

class ObstacleObject : public GameObject
{
public:
    void CollisionWithObject() override;

private:
    int damage_value, current_health;
};


class ObjectsBuilder
{
public:
    ObjectsBuilder() {};
    void InitObjectsBuilder(Stats* stats, SoundManager* sound_manager);
    GameObject* BuildGameObject(std::shared_ptr<Model> model);

    Stats* stats;
    SoundManager* sound_manager;
};
