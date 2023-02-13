//
// Created by Ofir Gilad on 13/02/2023.
//

#include "Leaderboard.h"

Leaderboard::Leaderboard() {

}

void Leaderboard::InitLeaderboard() {
    std::ifstream file("../tutorial/json/leaderboard.json");

    if (file.good()) {
        std::cout << "Leaderboard exists! \nReading data" << std::endl;
        leaderboard_available = true;
        // Read savedata
        file >> leaderboard_data;

        // Leaderboard data
        leaderboard_list.clear();

        for (int i = 0; i < 10; i++) {
            string index = to_string(i);
            string name = leaderboard_data[index]["name"];
            int score = leaderboard_data[index]["score"];
            leaderboard_list.push_back({ name, score });
        }
    }
    else {
        std::cout << "Leaderboard does not exist!" << std::endl;
        leaderboard_available = false;
    }
}

void Leaderboard::ResetLeaderboard() {
    if (leaderboard_available) {
        leaderboard_data = json();
    }

    // Leaderboard data
    for (int i = 0; i < 10; i++) {
        string index = to_string(i);
        leaderboard_data[index] = {};

        string name = "AAA";
        for (int j = 0; j < 3; j++) {
            name[j] += i;
        }

        int score = (10 - i) * 100;
        leaderboard_list.push_back({ name, score });
        leaderboard_data[index]["name"] = name;
        leaderboard_data[index]["score"] = score;
    }

    std::ofstream outfile("../tutorial/json/leaderboard.json");
    outfile << leaderboard_data.dump(4) << std::endl;
}