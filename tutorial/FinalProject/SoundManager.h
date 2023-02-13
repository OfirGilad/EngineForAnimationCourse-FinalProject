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

    
    // Global Indexed
    int playing_index = -1;
    int stage_index = 0;
    bool player_status = false;

private:
    // Make sure to set the correct python version
    string python_exe = "python3.8.exe";
    bool playing = false;
};