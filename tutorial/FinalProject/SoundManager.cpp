//
// Created by Ofir Gilad on 07/02/2023.
//

#include "SoundManager.h"

SoundManager::SoundManager() {
	
}

void SoundManager::MusicHandler(string music_file) {
    // Calling a script to play the music
    const auto& playMusic = [&](std::string music_file) {
        std::string command = "python ../tutorial/scripts/play_music.py \"" + music_file + "\"";
        int result = system(command.c_str());
        if (result != 0) {
            std::cerr << "Error running command: " << command << std::endl;
        }
    };

    if (playing == true) {
        // Kill The Previous Music Player
        playing = false;
        std::string kill_command = "taskkill /f /im " + python_exe;
        int result = system(kill_command.c_str());

        // Play New Music
        playing = true;
        std::thread t1(playMusic, music_file);
        t1.detach();
    }
    else {
        // Play Music For The First Time
        playing = true;
        std::thread t1(playMusic, music_file);
        t1.detach();
    }
}