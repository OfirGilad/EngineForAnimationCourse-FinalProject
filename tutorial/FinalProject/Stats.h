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

    bool save_data_available = false;

    // User data
    string user_name;
    int gold;

    // Snake stats
    int max_health;
    int max_movement_speed;
    int bonuses_duration;
    int gold_multiplier;

    // Statistics
    int total_points_earned;
    int total_gold_earned;
    int total_gold_spent;
    int total_boosts_collected;
    int total_deaths;

private:
    json save_data;
};
