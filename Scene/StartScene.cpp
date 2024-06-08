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
#include "Scene/StartScene.h"

// ! For save
#include <fstream>
using namespace std;

bool drawProfileMenu = false;
bool recordingPlayerName = false;

bool drawCreditMenu = false;

void StartScene::InitializeAudioEngine(){
    // TODO : Change Volume Based on Saved File
    if (AudioHelper::isMainMenuMusicPlaying && bgmInstance != nullptr) return;

    SaveFileData saveData = Engine::GameEngine::GetInstance().LoadSaveFile();
    AudioHelper::BGMVolume = saveData.bgmVolume;
    AudioHelper::SFXVolume = saveData.sfxVolume;
    Engine::GameEngine::playerName = saveData.playerName;
    oldName = Engine::GameEngine::playerName;

    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
    AudioHelper::SetMainMenuMusicPlaying(true);
}

void StartScene::Initialize() {
    InitializeAudioEngine();
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton* btn;

    AddNewObject(new Engine::Label("RPG Game", "pixel-font.ttf", 120, halfW, halfH / 3 + 50, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("V 1.0 I2P Demo", "pixel-font.ttf", 28, 5, h - 30, 255, 255, 255, 150, 0.0, 0.0));

    btn = new Engine::ImageButton("btn/btn_long.png", "btn/btn_long_hover.png", halfW - 200, halfH / 2 + 285 , 350, 88);
    btn->SetOnClickCallback(std::bind(&StartScene::PlayOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("PLAY", "pixel-font.ttf", 45, halfW - 25, halfH / 2 + 332, 255, 255, 255, 255, 0.5, 0.5));

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

    btn = new Engine::ImageButton("btn/profile_bg.png", "btn/profile_bg_hover.png", 10, 15, 298, 70);
    btn->SetOnClickCallback(bind(&StartScene::ProfileBtn, this));
    AddNewControlObject(btn);
    
    TXT_Name = new Engine::Label(Engine::GameEngine::playerName, "pixel-font.ttf", 30, 95, 35, 255, 255, 255, 255, 0, 0);
    AddNewObject(TXT_Name);
}


void StartScene::InitializeProfileMenu(){
    
    // TXT_Profile = new Engine::Label("Profile Menu", "pixel-font.ttf", 50, 95, 35, 255, 255, 255, 255, 0, 0);
    IMG_ProfileBG = new Engine::Image("bg/ProfileScreen_Game.png", 0, 0, 1600, 832, 0, 0);
    BTN_Back = new Engine::ImageButton("btn/back_normal.png", "btn/back_hover.png", 432, 223, 70, 70);
    BTN_Back->SetOnClickCallback(bind(&StartScene::Profile_BackBtnClick, this));
    AddNewControlObject(BTN_Back, true);
    BTN_Done = new Engine::ImageButton("btn/btn_long.png", "btn/btn_long_hover.png", 696, 517, 214, 55);
    BTN_Done->SetOnClickCallback(bind(&StartScene::Profile_DoneBtnClick, this));
    AddNewControlObject(BTN_Done, true);

    // * Draw Text Box
    NameTextBox = new Engine::ImageButton("btn/buttonLong_blue.png", "btn/buttonLong_blue_pressed.png", 658, 355, 284, 73);
    NameTextBox->SetOnClickCallback(bind(&StartScene::RecordPlayerName, this));
    AddNewControlObject(NameTextBox, true);
    NameText = new Engine::Label((!Engine::GameEngine::playerName.empty() ? Engine::GameEngine::playerName : "Player"), "pixel-font.ttf", 35, 673, 373,  255, 255, 255, 255, 0, 0);
    DoneText = new Engine::Label("Done", "pixel-font.ttf", 32, 760, 530,  255, 255, 255, 255, 0, 0);
}

void StartScene::Profile_BackBtnClick(){
    drawProfileMenu = false;
}
void StartScene::Profile_DoneBtnClick(){
    drawProfileMenu = false;
    Engine::GameEngine::GetInstance().WriteSaveFile({Engine::GameEngine::playerName, AudioHelper::BGMVolume, AudioHelper::SFXVolume});
    RemoveObject(TXT_Name->GetObjectIterator());
    TXT_Name = new Engine::Label(Engine::GameEngine::playerName, "pixel-font.ttf", 30, 95, 35, 255, 255, 255, 255, 0, 0);
    AddNewObject(TXT_Name);
}

void StartScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    // cout << keyCode << endl;

    if (drawProfileMenu == true && recordingPlayerName == true){
        if (keyCode == 59) { // * Escape
            recordingPlayerName = false;
            return;
        }
        if (keyCode == 63) { // * Backspace
            Engine::GameEngine::playerName.pop_back();
            NameText = new Engine::Label(Engine::GameEngine::playerName, "pixel-font.ttf", 35, 673, 373,  255, 255, 255, 255, 0, 0);
            return;
        }
        if (keyCode < 0 || keyCode > 36 || Engine::GameEngine::playerName.length() >= 12) return;
        for (int i = 0; i <= 36; i++){
            if (keyCode == i){
                Engine::GameEngine::playerName += '@' + i;
                NameText = new Engine::Label(Engine::GameEngine::playerName, "pixel-font.ttf", 35, 673, 373,  255, 255, 255, 255, 0, 0);
                break;
            }
        }
    }
    if (drawProfileMenu){
        if (keyCode == 59){
            drawProfileMenu = false;
        }
    }
}
void StartScene::OnMouseDown(int button, int mx, int my) {
    cout << " On Mouse Down " << endl;
    if (recordingPlayerName) {
        recordingPlayerName = false;
    }
    IScene::OnMouseDown(button, mx, my);
}

void StartScene::VirtualDraw() const {
    IScene::VirtualDraw();
    // DynamicBG->Draw();
    if (drawProfileMenu){
        // * Draw Profile Menu Here
        // cout << "Drawing Profile Menu" << endl;
        // TXT_Profile->Draw();
        if (IMG_ProfileBG) IMG_ProfileBG->Draw();
        if (BTN_Back) BTN_Back->Draw();
        if (BTN_Done) BTN_Done->Draw();
        if (NameTextBox) NameTextBox->Draw();
        if (NameText) NameText->Draw();
        if (DoneText) DoneText->Draw();
    }
}

void StartScene::Terminate() {
    // if (TXT_Profile) delete TXT_Profile;
    // if (IMG_ProfileBG) delete IMG_ProfileBG;
    // if (BTN_Back) delete BTN_Back;
    // if (BTN_Done) delete BTN_Done;
    // if (NameTextBox) delete NameTextBox;
    
    IScene::Terminate();
}
void StartScene::PlayOnClick(int stage) {
    if (drawProfileMenu) return;
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
    StageSelectScene *scene = dynamic_cast<StageSelectScene *>(Engine::GameEngine::GetInstance().GetScene("stage-select"));
    if (scene != nullptr) scene->bgmInstance = this->bgmInstance;
}
void StartScene::SettingsOnClick(int stage) {
    if (drawProfileMenu) return;
    Engine::GameEngine::GetInstance().ChangeScene("settings");
    SettingsScene *scene = dynamic_cast<SettingsScene *>(Engine::GameEngine::GetInstance().GetScene("settings"));
    if (scene != nullptr) scene->bgmInstance = this->bgmInstance;
}

void StartScene::OnExitGameBtn(){
    if (drawProfileMenu) return;
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
    
    // if (drawProfileMenu) return;
    // playerName = ok ? "Kontol" : "Melvin";
    // cout << playerName << endl;
    
    // // for (auto obj : GetObjects()){
    // //     if (obj == TXT_Name){
    // //         cout << "Found !" << endl;
    // //         RemoveObject(obj->GetObjectIterator());
    // //     }
    // // }

    // RemoveObject(TXT_Name->GetObjectIterator());

    // TXT_Name = new Engine::Label(playerName, "pixel-font.ttf", 30, 95, 35, 255, 255, 255, 255, 0, 0);
    // AddNewObject(TXT_Name);
    // ok = !ok;

    openBrowser("https://docs.google.com/document/d/1zvZ2EW1jFqdQMtwXJ28hzIIh0uHgJxsPPQYok0K_8JQ/edit?usp=sharing");
}

void StartScene::ScoreBoardBtn(){
    if (drawProfileMenu) return;
    Engine::GameEngine::GetInstance().ChangeScene("score-board");
    ScoreBoardScene *scene = dynamic_cast<ScoreBoardScene *>(Engine::GameEngine::GetInstance().GetScene("score-board"));
    if (scene != nullptr) scene->backMenuLevel = "start-scene";
}


void StartScene::ProfileBtn(){
    if (drawProfileMenu) return;
    if (!TXT_Profile) InitializeProfileMenu();

    drawProfileMenu = true;
}

void StartScene::RecordPlayerName(){
    if (drawProfileMenu) {
        cout << "Recording Player Name \n";
        recordingPlayerName = !recordingPlayerName;
    }
}
