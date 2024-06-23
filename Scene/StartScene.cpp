#include "StartScene.h"
#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "PlayScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "UI/Component/Slider.hpp"
#include "SettingsScene.hpp"
#include "StageSelectScene.hpp"
#include "ScoreBoardScene.hpp"
#include "GameScene_Hall.hpp"
#include "Scene/StartScene.h"

#include "Scene/Loading/LoadingScene.hpp"
#include "Scene/Profile/ProfileScene.hpp"
// ! For save
#include <fstream>
using namespace std;

bool drawCreatePlayerMenu = false;
bool recordingPlayerName = false;

bool drawProfileListMenu = false;
bool hasInitialized = false;
bool profileListMenuInitialized = false;

void StartScene::InitializeAudioEngine(){
    // TODO : Change Volume Based on Saved File
    if (AudioHelper::isMainMenuMusicPlaying && bgmInstance != nullptr) return;

    SaveFileData saveData = Engine::GameEngine::GetInstance().LoadSaveFile();
    AudioHelper::BGMVolume = saveData.bgmVolume;
    AudioHelper::SFXVolume = saveData.sfxVolume;
    // Engine::GameEngine::currentActivePlayerName = saveData.playerName;
    // oldName = Engine::GameEngine::playerName;

    bgmInstance = AudioHelper::PlaySample("NewMenu_Theme.ogg", true, AudioHelper::BGMVolume);
    AudioHelper::SetMainMenuMusicPlaying(true);
}

void StartScene::Initialize() {
    recordingPlayerName = false;
    drawProfileListMenu = false;
    drawCreatePlayerMenu = false;
    hasInitialized = false;
    profileListMenuInitialized = false;
    // playerEntryComps.clear();

    InitializeAudioEngine();
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton* btn;

    AddNewObject(new Engine::Label("RPG Game", "pixel-font.ttf", 120, halfW, halfH / 3 + 50, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("V 1.0 I2P Demo", "pixel-font.ttf", 28, 5, h - 30, 255, 255, 255, 150, 0.0, 0.0));

    btn = new Engine::ImageButton("btn/btn_long.png", "btn/btn_long_hover.png", halfW - 200, halfH / 2 + 285 , 350, 88);
    btn->SetOnClickCallback(std::bind(&StartScene::PlayOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("PLAY", "pixel-font.ttf", 45, halfW - 25, halfH / 2 + 333, 255, 255, 255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("btn/settings_normal.png", "btn/settings_hover.png", w - 260, 15, 70, 70);
    btn->SetOnClickCallback(std::bind(&StartScene::SettingsOnClick, this, 2));
    AddNewControlObject(btn);
    // AddNewObject(new Engine::Label("Settings", "pirulen.ttf", 48, halfW, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("btn/exit_normal.png", "btn/exit_hover.png", w - 90, 15, 70, 70);
    btn->SetOnClickCallback(bind(&StartScene::OnExitGameBtn, this));
    AddNewControlObject(btn);

    btn = new Engine::ImageButton("btn/readme_normal.png", "btn/readme_hover.png", w - 175, 15, 70, 0);
    btn->SetOnClickCallback(bind(&StartScene::CreditBtn, this));
    AddNewControlObject(btn);

    btn = new Engine::ImageButton("btn/scoreboard_normal.png", "btn/scoreboard_hover.png", 315, 15, 70, 70);
    btn->SetOnClickCallback(bind(&StartScene::ScoreBoardBtn, this));
    AddNewControlObject(btn);

    // btn = new Engine::ImageButton("btn/profile_bg.png", "btn/profile_bg_hover.png", 10, 15, 298, 70);
    // btn->SetOnClickCallback(bind(&StartScene::AddNewPlayerBtn, this));
    // AddNewControlObject(btn);
    
    // TXT_Name = new Engine::Label(NAMEPLAYER, "pixel-font.ttf", 30, 95, 35, 255, 255, 255, 255, 0, 0);
    // AddNewObject(TXT_Name);
}
    
void StartScene::AddNewPlayerBtn(){
    Engine::GameEngine::GetInstance().ChangeScene("profile-scene");
}

void StartScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);

    // * DEBUG PROFILE BASED WRITING
    if (keyCode == 28){
        cout << "Attempt to saving...\n";
        auto oldData =Engine::GameEngine::GetInstance().LoadProfileBasedSaving(); 
        PlayerEntry entry;
        entry.name = "WICAKSONO"; entry.money = 15000; entry.difficulty = "Normal";
        Engine::GameEngine::GetInstance().WriteProfileBasedSaving(oldData, entry);
    }
}
void StartScene::OnMouseDown(int button, int mx, int my) {
    if (recordingPlayerName) {
        recordingPlayerName = false;
    }
    IScene::OnMouseDown(button, mx, my);
}


void StartScene::VirtualDraw() const {
    IScene::VirtualDraw();
    
}

void StartScene::Terminate() {
    IScene::Terminate();

}
void StartScene::PlayOnClick() {
    if(Id_Entries == -1){
        Engine::GameEngine::GetInstance().ChangeScene("profile-scene");
        ProfileScene *scene = dynamic_cast<ProfileScene *>(Engine::GameEngine::GetInstance().GetScene("profile-scene"));
        if(scene != nullptr) scene->bgmInstance = this->bgmInstance;
        return;
    }
    std::cout << "Loading Player " << entries[Id_Entries].name << endl;
    LoadingScene* loadingScene = dynamic_cast<LoadingScene*>(Engine::GameEngine::GetInstance().GetScene("loading-scene"));
    loadingScene->InitLoadingScreen("gamescene_hall", 2.5f);
    Engine::GameEngine::GetInstance().ChangeScene("loading-scene");

    if (bgmInstance){
        AudioHelper::StopSample(bgmInstance);
        bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    }
    ProfileScene *scene = dynamic_cast<ProfileScene *>(Engine::GameEngine::GetInstance().GetScene("profile-scene"));
    if(scene != nullptr) scene->bgmInstance = this->bgmInstance;

}
void StartScene::SettingsOnClick(int stage) {
    if (drawCreatePlayerMenu || drawProfileListMenu) return;
    Engine::GameEngine::GetInstance().ChangeScene("settings");
    SettingsScene *scene = dynamic_cast<SettingsScene *>(Engine::GameEngine::GetInstance().GetScene("settings"));
    if (scene != nullptr) scene->bgmInstance = this->bgmInstance;
}

void StartScene::OnExitGameBtn(){
    if (drawCreatePlayerMenu || drawProfileListMenu) return;
    exit(1);
}

bool ok = 0;

void openBrowser(const std::string& url) {
    std::string command;
#if defined(_WIN32) || defined(_WIN64)
    command = "start " + url;
#elif defined(__APPLE__) || defined(__MACH__)
    command = "open " + url;
#elif defined(__linux__)
    command = "xdg-open " + url;
#else
    std::cerr << "Unsupported platform!" << std::endl;
    return;
#endif
    system(command.c_str());
}


void StartScene::CreditBtn(){
    

    //openBrowser("https://docs.google.com/document/d/1zvZ2EW1jFqdQMtwXJ28hzIIh0uHgJxsPPQYok0K_8JQ/edit?usp=sharing");
}

void StartScene::ScoreBoardBtn(){
    if (drawCreatePlayerMenu) return;
    Engine::GameEngine::GetInstance().ChangeScene("score-board");
    ScoreBoardScene *scene = dynamic_cast<ScoreBoardScene *>(Engine::GameEngine::GetInstance().GetScene("score-board"));
    if (scene != nullptr) scene->backMenuLevel = "start-scene";
}


void StartScene::RecordPlayerName(){
    if (drawCreatePlayerMenu) {
        // cout << "Recording Player Name " << recordingPlayerName << endl;
        recordingPlayerName = true;
        cout << "Recording Player Name " << recordingPlayerName << endl;
    }
}
