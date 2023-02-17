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
    this->current_health = game_manager->stats.current_health;
    this->max_health = game_manager->stats.max_health;
}

void HealthObject::CollisionWithObject() {
    health_value = stage_number * 10;
    if (current_health + health_value > max_health) {
        current_health = max_health;
    }
    else {
        current_health += health_value;
    }
    game_manager->stats.current_health = current_health;
    cout << "HealthObject" << endl;
}

/////////////////
// ScoreObject //
/////////////////

void ScoreObject::InitObject(GameManager* game_manager) {
    this->game_manager = game_manager;
    this->stage_number = game_manager->selected_stage;
    this->current_score = game_manager->stats.current_score;
}

void ScoreObject::CollisionWithObject() {
    score_value = 10 * stage_number;
    current_score = current_score + score_value;
    game_manager->stats.current_score = current_score;
    game_manager->stats.total_points_earned += score_value;
    cout << "ScoreObject" << endl;
}

////////////////
// GoldObject //
////////////////

void GoldObject::InitObject(GameManager* game_manager) {
    this->game_manager = game_manager;
    this->stage_number = game_manager->selected_stage;
    this->gold = game_manager->stats.gold;
    this->gold_multiplier = game_manager->stats.gold_multiplier;
}

void GoldObject::CollisionWithObject() {
    gold_value = 10 * stage_number * gold_multiplier;
    gold = gold + gold_value;
    game_manager->stats.gold = gold;
    game_manager->stats.total_gold_earned += gold_value;
    // add to statistics
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
    cout << "BonusObject" << endl;
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
