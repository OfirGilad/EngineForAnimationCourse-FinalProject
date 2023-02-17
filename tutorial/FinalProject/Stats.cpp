//
// Created by Ofir Gilad on 13/02/2023.
//

#include "Stats.h"


Stats::Stats() {

}

void Stats::InitStats() {
    std::ifstream file("../tutorial/json/savedata.json");

    if (file.good()) {
        std::cout << "Savedata exists! \nReading data" << std::endl;
        save_data_available = true;
        // Read savedata
        file >> save_data;

        // User data
        user_name = save_data["user_data"]["username"];
        gold = save_data["user_data"]["gold"];

        // Snake stats
        max_health = save_data["snake_stats"]["max_health"];
        score_multiplier = save_data["snake_stats"]["score_multiplier"];
        gold_multiplier = save_data["snake_stats"]["gold_multiplier"];
        bonuses_duration = save_data["snake_stats"]["bonuses_duration"];
        max_movement_speed = save_data["snake_stats"]["max_movement_speed"];

        // Statistics
        total_health_points_healed = save_data["statistics"]["total_health_points_healed"];
        total_health_points_lost = save_data["statistics"]["total_health_points_lost"];
        total_score_points_earned = save_data["statistics"]["total_score_points_earned"];
        total_gold_earned = save_data["statistics"]["total_gold_earned"];
        total_gold_spent = save_data["statistics"]["total_gold_spent"];
        total_bonuses_collected = save_data["statistics"]["total_bonuses_collected"];
        total_deaths = save_data["statistics"]["total_deaths"];
    }
    else {
        std::cout << "Savedata does not exist!" << std::endl;
        save_data_available = false;
    }
}

void Stats::NewGame(string name) {
    if (save_data_available) {
        save_data = json();
    }

    // User data
    user_name = name;
    gold = 0;

    save_data["user_data"]["username"] = user_name;
    save_data["user_data"]["gold"] = 0;

    // Snake stats
    max_health = 100;
    score_multiplier = 1;
    gold_multiplier = 1;
    bonuses_duration = 5;
    max_movement_speed = 1;

    save_data["snake_stats"]["max_health"] = max_health;
    save_data["snake_stats"]["score_multiplier"] = score_multiplier;
    save_data["snake_stats"]["gold_multiplier"] = gold_multiplier;
    save_data["snake_stats"]["bonuses_duration"] = bonuses_duration;
    save_data["snake_stats"]["max_movement_speed"] = max_movement_speed;
    
    
    // Statistics
    total_health_points_healed = 0;
    total_health_points_lost = 0;
    total_score_points_earned = 0;
    total_gold_earned = 0;
    total_gold_spent = 0;
    total_bonuses_collected = 0;
    total_deaths = 0;

    save_data["statistics"]["total_health_points_healed"] = total_health_points_healed;
    save_data["statistics"]["total_health_points_lost"] = total_health_points_lost;
    save_data["statistics"]["total_score_points_earned"] = total_score_points_earned;
    save_data["statistics"]["total_gold_earned"] = total_gold_earned;
    save_data["statistics"]["total_gold_spent"] = total_gold_spent;
    save_data["statistics"]["total_bonuses_collected"] = total_bonuses_collected;
    save_data["statistics"]["total_deaths"] = total_deaths;


    std::ofstream outfile("../tutorial/json/savedata.json");
    outfile << save_data.dump(4) << std::endl;
}
