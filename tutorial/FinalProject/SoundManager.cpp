//
// Created by Ofir Gilad on 07/02/2023.
//

#include "SoundManager.h"

SoundManager::SoundManager() {
	
}

void SoundManager::MusicHandler(string music_file) {
    // Calling a script to play the music
    const auto& PlayMusic = [&](std::string music_file) {
        std::string command = "python ../tutorial/scripts/play_music.py \"" + music_file + "\"";
        system(command.c_str());
    };

    if (player_enabled) {
        if (playing) {
            // Kill The Previous Music Player
            playing = false;
            std::string kill_command = "taskkill /f /im " + python_exe;
            system(kill_command.c_str());

            // Play New Music
            playing = true;
            std::thread t1(PlayMusic, music_file);
            t1.detach();
        }
        else {
            // Play Music For The First Time
            playing = true;
            std::thread t1(PlayMusic, music_file);
            t1.detach();
        }
    }
}

void SoundManager::SoundHandler(string sound_file) {
    // Calling a script to play the music
    const auto& PlaySound = [&](std::string sound_file) {
        std::string command = "python ../tutorial/scripts/play_sound.py \"" + sound_file + "\"";
        system(command.c_str());
    };
    if (player_enabled) {
        std::thread t2(PlaySound, sound_file);
        t2.detach();
    }
}

void SoundManager::StopMusic() {
    if (playing) {
        // Kill The Previous Music Player
        playing = false;
        std::string kill_command = "taskkill /f /im " + python_exe;
        system(kill_command.c_str());
    }
}