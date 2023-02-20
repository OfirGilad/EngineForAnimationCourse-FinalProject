//
// Created by Ofir Gilad on 20/02/2023.
//

#ifndef ENGINEFORANIMATIONCOURSE_FINALPROJECT_GAMETIMER_H
#define ENGINEFORANIMATIONCOURSE_FINALPROJECT_GAMETIMER_H

#endif //ENGINEFORANIMATIONCOURSE_FINALPROJECT_GAMETIMER_H

#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

class GameTimer
{
public:
    GameTimer();
	
    void StartTimer();
    void StopTimer();
    void ResetTimer();
	
    double GetElapsedTime();
	
private:
    time_point<system_clock> start_time;
    bool running_status;
    double elapsed_time;
};


