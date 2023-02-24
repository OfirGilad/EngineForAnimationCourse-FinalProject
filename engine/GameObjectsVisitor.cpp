//
// Created by Ofir Gilad on 21/02/2023.
//

#include "GameObjectsVisitor.h"
#include "Utility.h"

void GameObjectsVisitor::Run(cg3d::Scene* _scene, cg3d::Camera* camera) 
{
    Visitor::Run(_scene, camera);
}

void GameObjectsVisitor::Visit(Scene* _scene)
{
    if (_scene->GetAnimate()) {
        game_manager = ((BasicScene*)_scene)->game_manager;

        if (game_manager->stats->active_bonus != "None") {
            game_manager->active_bonus_timer.StartTimer();
            int bonuses_duration = game_manager->stats->bonuses_duration;
            int current_active_bonus_time = game_manager->active_bonus_timer.GetElapsedTime();

            if (current_active_bonus_time > bonuses_duration) {
                game_manager->active_bonus_timer.ResetTimer();
                game_manager->stats->active_bonus = "None";
            }
        }

        for (int i = 0; i < int(game_manager->alive_objects.size()); i++) {
            GameObject* current_game_object = game_manager->alive_objects[i];
            current_game_object->MoveObject();
        }
        for (int i = 0; i < int(game_manager->dead_objects.size()); i++) {
            GameObject* current_game_object = game_manager->dead_objects[i];
            float object_time = current_game_object->dead_timer.GetElapsedTime();

            // Handle Exit Object
            if (current_game_object->model->name == "ExitObject") {
                if (game_manager->stats->current_score >= game_manager->stats->objective_score) {
                    // Reset Timers
                    current_game_object->dead_timer.ResetTimer();
                    current_game_object->alive_timer.StartTimer();

                    // Move to Alive Objects
                    game_manager->dead_objects.erase(game_manager->dead_objects.begin() + i);
                    game_manager->alive_objects.push_back(current_game_object);

                    // Handle Object Event
                    current_game_object->SetAlive();
                    current_game_object->MoveObject();
                    cout << "Exit Appeared" << endl;

                    return;
                }
            }
            else if (object_time > 5.f) {
                // Reset Timers
                current_game_object->dead_timer.ResetTimer();
                current_game_object->alive_timer.StartTimer();

                // Move to Alive Objects
                game_manager->dead_objects.erase(game_manager->dead_objects.begin() + i);
                game_manager->alive_objects.push_back(current_game_object);

                // Handle Object Event
                current_game_object->SetAlive();

                return;
            }
        }
    }
}
