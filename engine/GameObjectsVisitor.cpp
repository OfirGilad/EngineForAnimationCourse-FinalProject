//
// Created by Ofir Gilad on 21/02/2023.
//

#include "GameObjectsVisitor.h"
#include "Utility.h"

void GameObjectsVisitor::Run(cg3d::Scene* _scene, cg3d::Camera* camera) {
    //if(!is_visitor_inited){
    //    //don't need to init it anymore
    //    is_visitor_inited = true;
    //    program = std::make_shared<Program>("../tutorial/shaders/phongShader");
    //    //material1
    //    material =  std::make_shared<Material>("material", program); // empty material
    //    material->AddTexture(0, "textures/box0.bmp", 2);


    //    generateObjectBezier(BRICKS_MATERIAL,SPHERE, std::string(BEZIER_OBJECT_NAME) + " cube", 3.0f);
    //    auto truck = generateObjectBezier(PHONG_MATERIAL,TRUCK, std::string(BEZIER_OBJECT_NAME) + " cube", 3.0f);
    //    generateObjectBezier(PHONG_MATERIAL,SPHERE, std::string(BEZIER_OBJECT_NAME) + " cube", 3.0f);
    //    generateObjectBezier(PHONG_MATERIAL,CUBE, std::string(BEZIER_OBJECT_NAME) + " cube", 3.0f);
    //}

    Visitor::Run(_scene, camera);
}

void GameObjectsVisitor::Visit(Scene* _scene) {
    game_manager = ((BasicScene*)_scene)->game_manager;
    game_manager->stats->game_time = int(game_manager->game_timer.GetElapsedTime());

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

    //if(basicScene->animate && model->name.substr(0,strlen(BEZIER_OBJECT_NAME)) == std::string(BEZIER_OBJECT_NAME)) {
    //    if (model->t <= 1 && !model->moveBackwards) {
    //        model->t += 0.04*model->bezier_speed;
    //        moveAccordingToBeizerCurve(model);
    //    } else {
    //        if (!model->moveBackwards)
    //            model->moveBackwards = true;
    //        if (model->moveBackwards) {
    //            model->t -= 0.04*model->bezier_speed;
    //            moveAccordingToBeizerCurve(model);
    //            if (model->t <= 0)
    //                model->moveBackwards = false;
    //        }
    //    }
    //}
}

//std::shared_ptr<Model> GameObjectsVisitor::generateObjectBezier(int material_id ,int model_id, std::string name, float scale){
    //std::shared_ptr<Model> cube = ModelsFactory::getInstance()->CreateModel(material_id,model_id,name);
    //basicScene->GetRoot()->AddChild(cube);
    //cube->showWireframe = true;
    //Eigen::Vector3f location = Eigen::Vector3f (generate_random_number(minx,maxx),generate_random_number(miny,maxy),generate_random_number(minz,maxz));
    //cube->Translate(location);
    //cube->Scale(scale,Movable::Axis::XYZ);
    //setModelBezier(location,cube);
    //cube->stopper.start();
    //cube->GetTreeWithCube();
    //return cube;
//}

void GameObjectsVisitor::setModelBezier(Eigen::Vector3f vectors,std::shared_ptr<Model> model){
    //Calculates::getInstance()->generateRandomBeizierCurve(std::move(vectors),model->MG_Result);
    //moveAccordingToBeizerCurve(model.get());
    //drawTheBeizerCurve(model);
}

//float GameObjectsVisitor::generate_random_number(float min, float max) {
//    //return min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max-min)));
//}

void GameObjectsVisitor::moveAccordingToBeizerCurve(Model *model) {
    //Eigen::RowVector4f Ti;
    //Ti[0] = powf(model->t, 3);
    //Ti[1] = powf(model->t, 2);
    //Ti[2] = model->t;
    //Ti[3] = 1;

    //Eigen::Vector3f currentPosition;
    //currentPosition = (Ti * model->MG_Result);

    //Eigen::Vector3f oldPositionOfObject = model->GetPosition();
    //model->Translate(currentPosition - oldPositionOfObject);
}

void GameObjectsVisitor::drawTheBeizerCurve(std::shared_ptr<Model> model) {
    //int number_of_points_in_bezier = 100;
    //std::vector<double> line_space = Calculates::getInstance()->linspace(0,1,number_of_points_in_bezier);
    //Eigen::Vector3d drawingColor = Eigen::RowVector3d(0, 0, 2);

    //Eigen::MatrixXf vertices(number_of_points_in_bezier,3);
    //for (int i = 0; i < number_of_points_in_bezier; ++i) {
    //    double ti = line_space[i];
    //    Eigen::RowVector4f Ti;
    //    Ti[0] = powf(ti, 3);
    //    Ti[1] = powf(ti, 2);
    //    Ti[2] = ti;
    //    Ti[3] = 1;
    //    vertices.row(i) = Ti * model->MG_Result;
    //}

    //Eigen::MatrixXi faces(number_of_points_in_bezier-1,2);
    //for (int i = 0; i < number_of_points_in_bezier-1; ++i) {

    //    faces.row(i) = Eigen::Vector2i(i,i+1);
    //}

    //Eigen::MatrixXd vertexNormals = Eigen::MatrixXd::Ones(number_of_points_in_bezier,3);
    //Eigen::MatrixXd textureCoords = Eigen::MatrixXd::Ones(number_of_points_in_bezier,2);// empty material

    //std::shared_ptr<Mesh> coordsys = std::make_shared<Mesh>("coordsys",vertices.cast<double>(),faces,vertexNormals,textureCoords);
    //std::shared_ptr<Model> bezier = Model::Create("bez",coordsys,ModelsFactory::getInstance()->materials[BASIC_MATERIAL]);
    //bezier->mode = 1;
    //model->bezier = bezier;
    //basicScene->GetRoot()->AddChild(bezier);
}






