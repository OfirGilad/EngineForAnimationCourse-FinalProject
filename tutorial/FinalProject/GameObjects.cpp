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

////////////////
// GameObject //
////////////////

void GameObject::InitObject(GameManager* game_manager) {
    this->game_manager = game_manager;
}

//////////////////
// HealthObject //
//////////////////

void HealthObject::CollisionWithObject() {
    // Get required parameters
    stage_number = game_manager->selected_stage;
    current_health = game_manager->stats.current_health;
    max_health = game_manager->stats.max_health;

    // Handle event
    health_value = stage_number * 10;
    if (current_health + health_value > max_health) {
        health_value = max_health - current_health;
    }
    current_health += health_value;

    // Update stats
    game_manager->stats.current_health = current_health;
    game_manager->stats.total_health_points_healed += health_value;

    // Handle sound
    game_manager->sound_manager.SoundHandler("health_object.mp3");
    cout << "HealthObject" << endl;
}

/////////////////
// ScoreObject //
/////////////////

void ScoreObject::CollisionWithObject() {
    // Get required parameters
    stage_number = game_manager->selected_stage;
    current_score = game_manager->stats.current_score;

    // Handle event
    score_value = 10 * stage_number;
    current_score = current_score + score_value;

    // Update stats
    game_manager->stats.current_score = current_score;
    game_manager->stats.total_score_points_earned += score_value;

    // Handle sound
    game_manager->sound_manager.SoundHandler("score_object.mp3");
    cout << "ScoreObject" << endl;
}

////////////////
// GoldObject //
////////////////

void GoldObject::CollisionWithObject() {
    // Get required parameters
    stage_number = game_manager->selected_stage;
    gold = game_manager->stats.gold;
    gold_multiplier = game_manager->stats.gold_multiplier;

    // Handle event
    gold_value = 10 * stage_number * gold_multiplier;
    gold = gold + gold_value;

    // Update stats
    game_manager->stats.gold = gold;
    game_manager->stats.total_gold_earned += gold_value;

    // Handle sound
    game_manager->sound_manager.SoundHandler("gold_object.mp3");
    cout << "GoldObject" << endl;
}

/////////////////
// BonusObject //
/////////////////

void BonusObject::CollisionWithObject() {
    // Get required parameters
    stage_number = game_manager->selected_stage;
    bonus_duration = game_manager->stats.bonuses_duration;

    // Handle event

    // Update stats
    game_manager->stats.total_bonuses_collected += 1;

    // Handle sound
    game_manager->sound_manager.SoundHandler("bonus_object.mp3");
    cout << "BonusObject" << endl;
}

////////////////////
// ObstacleObject //
////////////////////

void ObstacleObject::CollisionWithObject() {
    // Get required parameters
    stage_number = game_manager->selected_stage;
    current_health = game_manager->stats.current_health;

    // Handle event
    damage_value = 10 * stage_number;
    if (current_health - damage_value < 0) {
        damage_value = current_health;
    }
    current_health -= damage_value;

    // Update stats
    game_manager->stats.current_health = current_health;
    game_manager->stats.total_health_points_lost += damage_value;

    // Handle sound
    game_manager->sound_manager.SoundHandler("obstacle_object.mp3");
    cout << "ObstacleObject" << endl;
}
