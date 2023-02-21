//
// Created by Ofir Gilad on 07/02/2023.
//

#ifndef ENGINEFORANIMATIONCOURSE_FINALPROJECT_SOUNDMANAGER_H
#define ENGINEFORANIMATIONCOURSE_FINALPROJECT_SOUNDMANAGER_H

#endif //ENGINEFORANIMATIONCOURSE_FINALPROJECT_SOUNDMANAGER_H

#include <string>
#include <thread>
#include <iostream>

using namespace std;

class SoundManager
{
public:
    SoundManager();

    void HandleMusic(string music_file);
    void HandleSound(string sound_file);
    
    void SetMusicVolume(double volume);
    void SetSoundVolume(double volume);

    void SetMusicPlayerStatus(bool enabled);
    void SetSoundPlayerStatus(bool enabled);

    void StopMusic();
    ~SoundManager();

    // Global Indexed
    int playing_index = -1;
    int stage_index = 0;

private:
    FILE* pipe;

    // Make sure to set the correct python version
    string python_version = "python3.10";

    bool playing = false;
    bool music_player_enabled = true;
    bool sound_player_enabled = true;

    double music_volume = 0.5;
    double sound_volume = 0.7;
};