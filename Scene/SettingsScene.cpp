#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <iostream>
using namespace std;

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "PlayScene.hpp"
#include "UI/Component/Slider.hpp"
#include "SettingsScene.hpp"
#include "StartScene.h"

void SettingsScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;
    // btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH / 2 - 50, 400,
    //                               100);
    // btn->SetOnClickCallback(std::bind(&SettingsScene::PlayOnClick, this, 1));
    // AddNewControlObject(btn);
    // AddNewObject(new Engine::Label("", "pirulen.ttf", 48, halfW, halfH / 2, 0, 0, 0, 255, 0.5, 0.5));
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH * 3 / 2 - 50,
                                  400, 100);
    btn->SetOnClickCallback(std::bind(&SettingsScene::BackMainMenu, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pixel-font.ttf", 48, halfW, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));

    Slider *sliderBGM, *sliderSFX;

    Engine::Image *slider_bg;
    slider_bg = new Engine::Image("stage-select/dirt.png", 40 + halfW - 95, halfH - 50 -2, 190, 10);
    // std::cout << "slider bg " << slider_bg << endl;
    

    sliderBGM = new Slider(40 + halfW - 95, halfH - 50 - 2, 190, 4);
    sliderBGM->SetOnValueChangedCallback(
            std::bind(&SettingsScene::BGMSlideOnValueChanged, this, std::placeholders::_1));
    AddNewControlObject(sliderBGM);
    AddNewObject(
            new Engine::Label("BGM: ", "pixel-font.ttf", 28, 40 + halfW - 60 - 95, halfH - 50, 255, 255, 255, 255, 0.5,
                              0.5));
    sliderSFX = new Slider(40 + halfW - 95, halfH + 50 - 2, 190, 4);
    sliderSFX->SetOnValueChangedCallback(
            std::bind(&SettingsScene::SFXSlideOnValueChanged, this, std::placeholders::_1));
    AddNewControlObject(sliderSFX);
    AddNewObject(
            new Engine::Label("SFX: ", "pixel-font.ttf", 28, 40 + halfW - 60 - 95, halfH + 50, 255, 255, 255, 255, 0.5,
                              0.5));
    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
    // bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
    sliderBGM->SetValue(AudioHelper::BGMVolume);
    sliderSFX->SetValue(AudioHelper::SFXVolume);
    // if (AudioHelper::isMainMenuMusicPlaying) cout << "DBG : Main Menu Music Playing, skipping \n";
}

void SettingsScene::Terminate() {
    // AudioHelper::StopSample(bgmInstance);
    // bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}

void SettingsScene::PlayOnClick(int stage) {
    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
    scene->MapId = stage;
    Engine::GameEngine::GetInstance().ChangeScene("play");
}

void SettingsScene::BGMSlideOnValueChanged(float value) {
    // cout << "value : " << value << endl;
    //if (value <= 0.01) value = 0.0f;
    if (!bgmInstance) {
        cout << "BGM Instance NOT VALID\n";
        return;
    }
    AudioHelper::ChangeSampleVolume(bgmInstance, value);
    AudioHelper::BGMVolume = value;
    // Engine::GameEngine::GetInstance().WriteSaveFile({"", AudioHelper::BGMVolume, AudioHelper::SFXVolume});
}

void SettingsScene::SFXSlideOnValueChanged(float value) {
    AudioHelper::SFXVolume = value;
    // StartScene::SaveSettings();
    // Engine::GameEngine::GetInstance().WriteSaveFile({"", AudioHelper::BGMVolume, AudioHelper::SFXVolume});
}

void SettingsScene::BackMainMenu(){
    Engine::GameEngine::GetInstance().ChangeScene("start-scene");
}

void SettingsScene::OnMouseUp(int button, int mx, int my) {
    IScene::OnMouseUp(button, mx, my);
    Engine::GameEngine::GetInstance().WriteSaveFile({"", AudioHelper::BGMVolume, AudioHelper::SFXVolume});
}