//
// Created by Ofir Gilad on 20/02/2023.
//

#include "GameTimer.h"

GameTimer::GameTimer() {
    running_status = false;
    elapsed_time = 0;
}

void GameTimer::StartTimer() {
    if (!running_status) {
        running_status = true;
        start_time = system_clock::now();
    }
}

void GameTimer::StopTimer() {
    if (running_status) {
        running_status = false;
        elapsed_time += duration<double>(system_clock::now() - start_time).count();
    }
}

void GameTimer::ResetTimer() {
    running_status = false;
    elapsed_time = 0;
}

double GameTimer::GetElapsedTime() {
    if (running_status) {
        return elapsed_time + duration<double>(system_clock::now() - start_time).count();
    } else {
        return elapsed_time;
    }
}