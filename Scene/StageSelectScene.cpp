#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "PlayScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "UI/Component/Slider.hpp"
#include "StageSelectScene.hpp"

void StageSelectScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    AddNewObject(new Engine::Image("bg/LevelSelect_Game.png", 0, 0, w, h, 0.f, 0.f));
    
    Engine::ImageButton* btn;
    btn = new Engine::ImageButton("btn/back_normal.png", "btn/back_hover.png", 391, 175, 70, 70);
    btn->SetOnClickCallback(std::bind(&StageSelectScene::BackOnClick, this, 1));
    AddNewControlObject(btn);

    btn = new Engine::ImageButton("btn/stage1_normal.png", "btn/stage1_hover.png", 466, 279, 219, 196);
    btn->SetOnClickCallback(std::bind(&StageSelectScene::PlayOnClick, this, 1));
    AddNewControlObject(btn);

    btn = new Engine::ImageButton("btn/stage2_normal.png", "btn/stage2_hover.png", 692, 279, 219, 196);
    btn->SetOnClickCallback(std::bind(&StageSelectScene::PlayOnClick, this, 2));
    AddNewControlObject(btn);

    btn = new Engine::ImageButton("btn/stage3_normal.png", "btn/stage3_hover.png", 918, 279, 219, 196);
    btn->SetOnClickCallback(std::bind(&StageSelectScene::PlayOnClick, this, 3));
    AddNewControlObject(btn);

    // btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH * 3 / 2 - 50, 400, 100);
    // btn->SetOnClickCallback(std::bind(&StageSelectScene::BackOnClick, this, 1));
    // AddNewControlObject(btn);
    // AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));

    // btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH / 2 - 50, 400, 100);
    // btn->SetOnClickCallback(std::bind(&StageSelectScene::PlayOnClick, this, 1));
    // AddNewControlObject(btn);
    // AddNewObject(new Engine::Label("Stage 1", "pirulen.ttf", 48, halfW, halfH / 2, 0, 0, 0, 255, 0.5, 0.5));
    // btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH /2 + 100, 400, 100);
    // btn->SetOnClickCallback(std::bind(&StageSelectScene::PlayOnClick, this, 2));
    // AddNewControlObject(btn);
    // AddNewObject(new Engine::Label("Stage 2", "pirulen.ttf", 48, halfW, halfH / 2 +150, 0, 0, 0, 255, 0.5, 0.5));

    // btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH / 2 + 250, 400, 100);
    // btn->SetOnClickCallback(std::bind(&StageSelectScene::ScoreboardOnClick, this));
    // AddNewControlObject(btn);
    // AddNewObject(new Engine::Label("Scoreboard", "pirulen.ttf", 36, halfW, halfH / 2 + 300, 0, 0, 0, 255, 0.5, 0.5));

    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
	// bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}
void StageSelectScene::Terminate() {
	// AudioHelper::StopSample(bgmInstance);
	// bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
	IScene::Terminate();
}
void StageSelectScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("start-scene");
}
void StageSelectScene::PlayOnClick(int stage) {
    if (bgmInstance){
        AudioHelper::StopSample(bgmInstance);
        bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    }
    PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play"));
    scene->MapId = stage;
    Engine::GameEngine::GetInstance().ChangeScene("play");
}
void StageSelectScene::ScoreboardOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("score-board");
}