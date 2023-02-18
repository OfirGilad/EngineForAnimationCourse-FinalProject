//
// Created by Ofir Gilad on 13/02/2023.
//

#ifndef ENGINEFORANIMATIONCOURSE_FINALPROJECT_STATS_H
#define ENGINEFORANIMATIONCOURSE_FINALPROJECT_STATS_H

#endif //ENGINEFORANIMATIONCOURSE_FINALPROJECT_STATS_H

#include <string>
#include "../json/json.hpp"
#include <fstream>
#include <iostream>

using namespace std;
using namespace nlohmann;

class Stats
{
public:
    Stats();

    void InitStats();
    void NewGame(string name);
    void SaveGame();

    bool save_data_available = false;

    // User data
    string user_name;
    int gold;

    // Snake stats
    int max_health;
    int score_multiplier;
    int gold_multiplier;
    int bonuses_duration;
    int max_movement_speed;

    // Statistics
    int total_health_points_healed;
    int total_health_points_lost;
    int total_score_points_earned;
    int total_gold_earned;
    int total_gold_spent;
    int total_bonuses_collected;
    int total_deaths;


    // Stage stats
    int current_health = 0;
    int current_score = 0;
    int min_movement_speed = 1; // Always
    int current_movement_speed = 1;
    int objective_score = 0;

private:
    json save_data;
};
