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
    game_manager = ((BasicScene*)_scene)->game_manager;

    game_manager->stats->game_time = int(game_manager->game_timer.GetElapsedTime());

    float seconds = game_manager->game_timer.GetElapsedTime();
    game_manager->stats->time_played = game_manager->game_timer.SecondsToGameTime(seconds);

    if (_scene->GetAnimate()) {
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
                    game_manager->root->AddChild(current_game_object->model);
                    current_game_object->MoveObject();

                    // Move to Alive Objects
                    game_manager->dead_objects.erase(game_manager->dead_objects.begin() + i);
                    game_manager->alive_objects.push_back(current_game_object);

                    // Handle Timers
                    current_game_object->dead_timer.StopTimer();

                    current_game_object->alive_timer.ResetTimer();
                    current_game_object->alive_timer.StartTimer();

                    cout << "Exit Appeared" << endl;

                    return;
                }
            }
            else if (object_time > 5.f) {
                // Handle Object Event
                Eigen::Vector3f original_translation = current_game_object->model->GetTranslation();
                current_game_object->model->Translate(-original_translation);
                game_manager->root->AddChild(current_game_object->model);
                current_game_object->model->Translate(game_manager->GenerateRandomPosition());

                current_game_object->SetAlive();

                // Move to Alive Objects
                game_manager->dead_objects.erase(game_manager->dead_objects.begin() + i);
                game_manager->alive_objects.push_back(current_game_object);

                // Handle Timers
                current_game_object->dead_timer.StopTimer();

                current_game_object->alive_timer.ResetTimer();
                current_game_object->alive_timer.StartTimer();

                return;
            }
        }
    }
}
