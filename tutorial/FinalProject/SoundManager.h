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

    void MusicHandler(string music_file);
    void SoundHandler(string sound_file);
    void StopMusic();
    
    // Global Indexed
    int playing_index = -1;
    int stage_index = 0;
    bool player_enabled = true;

private:
    // Make sure to set the correct python version
    string python_version = "python3.10";
    bool playing = false;
};