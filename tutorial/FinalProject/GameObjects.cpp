//
// Created by Ofir Gilad on 13/02/2023.
//

#include "GameObjects.h"

///////////////////
// ObjectHandler //
///////////////////

void ObjectHandler::InitObjectHandler(string object_name, GameManager* game_manager) {
    this->object_name = object_name;
    this->game_manager = game_manager;

    if (this->object_name == "HealthObject") {
        this->game_object = new HealthObject();
        this->game_object->InitObject(this->game_manager);
    }
    if (this->object_name == "ScoreObject") {
        this->game_object = new ScoreObject();
        this->game_object->InitObject(this->game_manager);
    }
    if (this->object_name == "GoldObject") {
        this->game_object = new GoldObject();
        this->game_object->InitObject(this->game_manager);
    }
    if (this->object_name == "BonusObject") {
        this->game_object = new BonusObject();
        this->game_object->InitObject(this->game_manager);
    }
    if (this->object_name == "ObstacleObject") {
        this->game_object = new ObstacleObject();
        this->game_object->InitObject(this->game_manager);
    }
}

void ObjectHandler::HandleCollision() {
    this->game_object->CollisionWithObject();
}


//////////////////
// HealthObject //
//////////////////

void HealthObject::InitObject(GameManager* game_manager) {
    this->game_manager = game_manager;
    this->stage_number = game_manager->selected_stage;
}

void HealthObject::CollisionWithObject() {
    cout << "HealthObject" << endl;
}

/////////////////
// ScoreObject //
/////////////////

void ScoreObject::InitObject(GameManager* game_manager) {
    this->game_manager = game_manager;
    this->stage_number = game_manager->selected_stage;
}

void ScoreObject::CollisionWithObject() {
    cout << "ScoreObject" << endl;
}

////////////////
// GoldObject //
////////////////

void GoldObject::InitObject(GameManager* game_manager) {
    this->game_manager = game_manager;
    this->stage_number = game_manager->selected_stage;
}

void GoldObject::CollisionWithObject() {
    cout << "GoldObject" << endl;
}

/////////////////
// BonusObject //
/////////////////

void BonusObject::InitObject(GameManager* game_manager) {
    this->game_manager = game_manager;
    this->stage_number = game_manager->selected_stage;
}

void BonusObject::CollisionWithObject() {
    cout << "HealthObject" << endl;
}

////////////////////
// ObstacleObject //
////////////////////

void ObstacleObject::InitObject(GameManager* game_manager) {
    this->game_manager = game_manager;
    this->stage_number = game_manager->selected_stage;
}

void ObstacleObject::CollisionWithObject() {
    cout << "ObstacleObject" << endl;
}
