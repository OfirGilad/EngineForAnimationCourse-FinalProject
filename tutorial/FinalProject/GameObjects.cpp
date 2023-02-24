//
// Created by Ofir Gilad on 13/02/2023.
//

#include "GameObjects.h"

///////////////////
// ObjectHandler //
///////////////////

void ObjectsBuilder::InitObjectsBuilder(Stats* stats, SoundManager* sound_manager) {
    this->stats = stats;
    this->sound_manager = sound_manager;
}

GameObject* ObjectsBuilder::BuildGameObject(std::shared_ptr<Model> model, std::shared_ptr<Movable> root) {
    string model_name = model->name;
    GameObject* game_object;

    if (model_name == "HealthObject") {
        game_object = new HealthObject();
        game_object->InitObject(this->stats, this->sound_manager, model, root);
    }
    if (model_name == "ScoreObject") {
        game_object = new ScoreObject();
        game_object->InitObject(this->stats, this->sound_manager, model, root);
    }
    if (model_name == "GoldObject") {
        game_object = new GoldObject();
        game_object->InitObject(this->stats, this->sound_manager, model, root);
    }
    if (model_name == "BonusObject") {
        game_object = new BonusObject();
        game_object->InitObject(this->stats, this->sound_manager, model, root);
    }
    if (model_name == "ObstacleObject") {
        game_object = new ObstacleObject();
        game_object->InitObject(this->stats, this->sound_manager, model, root);
    }
    if (model_name == "ExitObject") {
        game_object = new ExitObject();
        game_object->InitObject(this->stats, this->sound_manager, model, root);
    }

    return game_object;
}

////////////////
// GameObject //
////////////////

void GameObject::InitObject(Stats* stats, SoundManager* sound_manager, std::shared_ptr<Model> model, std::shared_ptr<Movable> root) {
    this->stats = stats;
    this->model = model;
    this->root = root;
    this->sound_manager = sound_manager;

    alive_timer = GameTimer();
    dead_timer = GameTimer();
    bezier_logic = GameLogics();
}

Eigen::Vector3f GameObject::GenerateRandomPosition() {
    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_real_distribution<float> distribution(-stats->stage_size + 10, stats->stage_size - 10);

    return Eigen::Vector3f(distribution(generator), distribution(generator), distribution(generator));
}

//////////////////
// HealthObject //
//////////////////

void HealthObject::CollisionWithObject() {
    // Get required parameters
    int stage_number = stats->selected_stage;
    current_health = stats->current_health;
    max_health = stats->max_health;

    // Handle event
    health_value = stage_number * 10;
    if (current_health + health_value > max_health) {
        health_value = max_health - current_health;
    }
    current_health += health_value;

    // Update stats
    stats->current_health = current_health;
    stats->total_health_points_healed += health_value;

    // Handle sound
    sound_manager->HandleSound("health_object.mp3");
    cout << "HealthObject" << endl;
}

void HealthObject::MoveObject() {
    model->RotateByDegree(1.f, Eigen::Vector3f(0.f, 1.f, 0.f));

    if (stats->active_bonus == "Magnet") {
        Eigen::Vector3f object_translation = stats->snake_head->GetTranslation() - model->GetTranslation();
        object_translation.normalize();
        model->Translate(object_translation);
    }
}

void HealthObject::SetAlive() {
    Eigen::Vector3f original_translation = model->GetTranslation();
    model->Translate(-original_translation);
    model->Translate(GenerateRandomPosition());
    root->AddChild(model);

}

void HealthObject::SetDead() {
    root->RemoveChild(model);
}

/////////////////
// ScoreObject //
/////////////////

void ScoreObject::CollisionWithObject() {
    // Get required parameters
    int stage_number = stats->selected_stage;
    current_score = stats->current_score;
    score_multiplier = stats->score_multiplier;

    // Handle event
    score_value = 10 * stage_number * score_multiplier;
    current_score = current_score + score_value;

    // Update stats
    stats->current_score = current_score;
    stats->total_score_points_earned += score_value;

    // Handle sound
    sound_manager->HandleSound("score_object.mp3");
    cout << "ScoreObject" << endl;
}

void ScoreObject::MoveObject() {
    model->RotateByDegree(1.f, Eigen::Vector3f(0.f, 1.f, 0.f));

    if (stats->active_bonus == "Magnet") {
        Eigen::Vector3f object_translation = stats->snake_head->GetTranslation() - model->GetTranslation();
        object_translation.normalize();
        model->Translate(object_translation);
    }
}

void ScoreObject::SetAlive() {
    Eigen::Vector3f original_translation = model->GetTranslation();
    model->Translate(-original_translation);
    model->Translate(GenerateRandomPosition());
    root->AddChild(model);
}

void ScoreObject::SetDead() {
    root->RemoveChild(model);
}

////////////////
// GoldObject //
////////////////

void GoldObject::CollisionWithObject() {
    // Get required parameters
    int stage_number = stats->selected_stage;
    gold = stats->gold;
    gold_multiplier = stats->gold_multiplier;

    // Handle event
    gold_value = 10 * stage_number * gold_multiplier;
    gold = gold + gold_value;

    // Update stats
    stats->gold = gold;
    stats->total_gold_earned += gold_value;

    // Handle sound
    sound_manager->HandleSound("gold_object.mp3");
    cout << "GoldObject" << endl;
}

void GoldObject::MoveObject() {
    model->RotateByDegree(1.f, Eigen::Vector3f(0.f, 1.f, 0.f));

    if (stats->active_bonus == "Magnet") {
        Eigen::Vector3f object_translation = stats->snake_head->GetTranslation() - model->GetTranslation();
        object_translation.normalize();
        model->Translate(object_translation);
    }
}

void GoldObject::SetAlive() {
    Eigen::Vector3f original_translation = model->GetTranslation();
    model->Translate(-original_translation);
    model->Translate(GenerateRandomPosition());
    root->AddChild(model);
}

void GoldObject::SetDead() {
    root->RemoveChild(model);
}

/////////////////
// BonusObject //
/////////////////

void BonusObject::CollisionWithObject() {
    // Get required parameters
    int stage_number = stats->selected_stage;
    bonus_duration = stats->bonuses_duration;

    // Handle event
    stats->active_bonus = "Magnet";
    //stats->active_bonus = "Freeze Time";
    //stats->active_bonus = "Shield";
    //stats->active_bonus = "Stats Booster";

    // Update stats
    stats->total_bonuses_collected += 1;

    // Handle sound
    sound_manager->HandleSound("bonus_object.mp3");
    cout << "BonusObject" << endl;
}

void BonusObject::MoveObject() {
    model->RotateByDegree(1.f, Eigen::Vector3f(0.f, 1.f, 0.f));
    bezier_logic.MoveOnCurve();
}

void BonusObject::SetAlive() {
    Eigen::Vector3f original_translation = model->GetTranslation();
    model->Translate(-original_translation);
    root->AddChild(model);

    bezier_logic.InitBezierCurve(model, stats->stage_size);
    bezier_logic.GenerateBezierCurve();
    root->AddChild(bezier_logic.GetBezierCurveModel());
}

void BonusObject::SetDead() {
    root->RemoveChild(model);
    root->RemoveChild(bezier_logic.GetBezierCurveModel());
}

////////////////////
// ObstacleObject //
////////////////////

void ObstacleObject::CollisionWithObject() {
    // Get required parameters
    int stage_number = stats->selected_stage;
    current_health = stats->current_health;

    // Handle event
    damage_value = 10 * stage_number;
    if (current_health - damage_value < 0) {
        damage_value = current_health;
    }
    current_health -= damage_value;

    // Update stats
    stats->current_health = current_health;
    stats->total_health_points_lost += damage_value;

    // Handle sound
    sound_manager->HandleSound("obstacle_object.mp3");
    cout << "ObstacleObject" << endl;
}

void ObstacleObject::MoveObject() {
    model->RotateByDegree(1.f, Eigen::Vector3f(0.f, 1.f, 0.f));
    bezier_logic.MoveOnCurve();
}

void ObstacleObject::SetAlive() {
    Eigen::Vector3f original_translation = model->GetTranslation();
    model->Translate(-original_translation);
    root->AddChild(model);

    bezier_logic.InitBezierCurve(model, stats->stage_size);
    bezier_logic.GenerateBezierCurve();
    root->AddChild(bezier_logic.GetBezierCurveModel());
}

void ObstacleObject::SetDead() {
    root->RemoveChild(model);
    root->RemoveChild(bezier_logic.GetBezierCurveModel());
}

////////////////
// ExitObject //
////////////////

void ExitObject::CollisionWithObject() {
    // Get required parameters

    // Handle event

    // Update stats
    stats->stage_completed = true;

    // Handle sound
    cout << "ExitObject" << endl;
}

void ExitObject::MoveObject() {
    Eigen::Vector3f original_translation = model->GetTranslation();
    model->Translate(-original_translation);

    Eigen::Vector3f new_translation = Eigen::Vector3f(0.f, 0.f, -stats->stage_size);
    model->Translate(new_translation);
}

void ExitObject::SetAlive() {
    root->AddChild(model);
}

void ExitObject::SetDead() {
    root->RemoveChild(model);
}
