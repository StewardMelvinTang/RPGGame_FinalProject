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
    Engine::GameEngine::playerName = saveData.playerName;
    oldName = Engine::GameEngine::playerName;

    bgmInstance = AudioHelper::PlaySample("NewMenu_Theme.ogg", true, AudioHelper::BGMVolume);
    AudioHelper::SetMainMenuMusicPlaying(true);
}

void StartScene::Initialize() {
    recordingPlayerName = false;
    drawProfileListMenu = false;
    drawCreatePlayerMenu = false;
    hasInitialized = false;
    profileListMenuInitialized = false;
    playerEntryComps.clear();

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
    AddNewObject(new Engine::Label("PLAY", "pixel-font.ttf", 45, halfW - 25, halfH / 2 + 323, 255, 255, 255, 255, 0.5, 0.5));

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
    btn->SetOnClickCallback(bind(&StartScene::AddNewPlayerBtn, this));
    AddNewControlObject(btn);
    
    TXT_Name = new Engine::Label(Engine::GameEngine::playerName, "pixel-font.ttf", 30, 95, 35, 255, 255, 255, 255, 0, 0);
    AddNewObject(TXT_Name);
}



void StartScene::InitializeCreatePlayerMenu(){
    // recordingPlayerName = false;
    IMG_AddNewPlayerBG = new Engine::Image("bg/createnewprofile_bg.png", 0, 0, 1600, 832, 0, 0);
    BTN_Back = new Engine::ImageButton("btn/back_normal.png", "btn/back_hover.png", 570, 105, 70, 70);
    if (!hasInitialized)BTN_Back->SetOnClickCallback(bind(&StartScene::Profile_BackBtnClick, this, 1));
    AddNewControlObject(BTN_Back, true);
    BTN_Done = new Engine::ImageButton("btn/btn_long.png", "btn/btn_long_hover.png", 696, 635, 214, 55);
    BTN_Done->SetOnClickCallback(bind(&StartScene::Profile_DoneBtnClick, this));
    AddNewControlObject(BTN_Done, true);

    // * Draw Text Box
    NameTextBox = new Engine::ImageButton("btn/textbox.png", "btn/textbox.png", 619, 422, 362, 49);
    NameTextBox->SetOnClickCallback(bind(&StartScene::RecordPlayerName, this));
    AddNewControlObject(NameTextBox, true);
    NameText = new Engine::Label((!Engine::GameEngine::playerName.empty() ? Engine::GameEngine::playerName : "Player"), "pixel-font.ttf", 24, 630, 437,  255, 255, 255, 255, 0, 0);
    DoneText = new Engine::Label("Done", "pixel-font.ttf", 32, 760, 650,  255, 255, 255, 255, 0, 0);

    // * Draw Avatar Selection
    IMG_Avatar = new Engine::Image("avatar/avatar_1.png", 720, 218, 160, 160);
    AvatarRight = new Engine::ImageButton("btn/arrowBeige_right.png", "btn/arrowBeige_right.png", 895, 281, 36, 34);
    AddNewControlObject(AvatarRight, true);
    AvatarLeft = new Engine::ImageButton("btn/arrowBeige_left.png", "btn/arrowBeige_left.png", 672, 281, 36, 34);
    AddNewControlObject(AvatarLeft, true);
    if (!hasInitialized)AvatarRight->SetOnClickCallback(std::bind(&StartScene::ChangeAvatar, this, 1));
    if (!hasInitialized)AvatarLeft->SetOnClickCallback(std::bind(&StartScene::ChangeAvatar, this, -1));

    // * Draw Difficulty Selection
    DiffText = new Engine::Label("Normal", "pixel-font.ttf", 30, 800, 530,  255, 255, 255, 255, 0.5, 0.5);
    DiffRight = new Engine::ImageButton("btn/arrowBeige_right.png", "btn/arrowBeige_right.png", 895, 510, 36, 34);
    AddNewControlObject(DiffRight, true);
    if (!hasInitialized) DiffRight->SetOnClickCallback(std::bind(&StartScene::ChangeDifficulty, this, 1));
    DiffLeft = new Engine::ImageButton("btn/arrowBeige_left.png", "btn/arrowBeige_left.png", 672, 510, 36, 34);
    AddNewControlObject(DiffLeft, true);
    if (!hasInitialized) DiffLeft->SetOnClickCallback(std::bind(&StartScene::ChangeDifficulty, this, -1));
    

    hasInitialized = true;
}

void StartScene::ChangeAvatar(int mode){
    if (!drawCreatePlayerMenu) return;
    if (IMG_Avatar) {
        if (mode == 1) {
            if (currAvatarID + 1 >= 6) return;
            currAvatarID++;
        } else if (mode == -1){
            if (currAvatarID - 1 < 0) return;
            currAvatarID--;
        }

        IMG_Avatar = new Engine::Image(avatarPath[currAvatarID], 720, 218, 160, 160);
    }
}

void StartScene::ChangeDifficulty(int mode){ //  1 for inc, -1 for dec
    if (!drawCreatePlayerMenu) return;
    if (mode == 1){
        if (currDifficulty + 1 > 2) return;
        currDifficulty++;
    } else if (mode == -1){
        if (currDifficulty - 1 < 0) return;
        currDifficulty--;
    }

    delete DiffText;
    DiffText = new Engine::Label((currDifficulty == 0 ? "Easy" : currDifficulty == 1 ? "Normal" : currDifficulty == 2 ? "Hard" : "Normal"), "pixel-font.ttf", 30, 800, 530,  255, 255, 255, 255, 0.5, 0.5);
}

void StartScene::Profile_BackBtnClick(int screen){
    if (screen == 1) {
        drawCreatePlayerMenu = false;
        recordingPlayerName = false;
    } else if (screen == 2){
        drawProfileListMenu = false;
    }
}
void StartScene::Profile_DoneBtnClick(){
    if (!drawCreatePlayerMenu) return;

    cout << drawCreatePlayerMenu << " | " << drawProfileListMenu << endl;
    drawCreatePlayerMenu = false;
    recordingPlayerName = false;
    Engine::GameEngine::GetInstance().WriteSaveFile({Engine::GameEngine::playerName, AudioHelper::BGMVolume, AudioHelper::SFXVolume});
    RemoveObject(TXT_Name->GetObjectIterator());
    TXT_Name = new Engine::Label(Engine::GameEngine::playerName, "pixel-font.ttf", 30, 95, 35, 255, 255, 255, 255, 0, 0);
    AddNewObject(TXT_Name); 

    LoadingScene* loadingScene = dynamic_cast<LoadingScene*>(Engine::GameEngine::GetInstance().GetScene("loading-scene"));
    loadingScene->InitLoadingScreen("gamescene_hall", 2.5f);
    Engine::GameEngine::GetInstance().ChangeScene("loading-scene");

    auto oldData = Engine::GameEngine::GetInstance().LoadProfileBasedSaving(); 
    PlayerEntry entry;
    entry.name = Engine::GameEngine::playerName;
    entry.difficulty = (currDifficulty == 0 ? "Easy" : currDifficulty == 1 ? "Normal" : currDifficulty == 2 ? "Hard" : "Normal");
    entry.avatarID = currAvatarID;
    Engine::GameEngine::GetInstance().WriteProfileBasedSaving(oldData, entry);
}

void StartScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    if (drawCreatePlayerMenu == true && recordingPlayerName == true){
        if (keyCode == 59) { // * Escape
            recordingPlayerName = false;
            return;
        }
        if (keyCode == 63 && !Engine::GameEngine::playerName.empty()) { // * Backspace
            Engine::GameEngine::playerName.pop_back();
            NameText = new Engine::Label(Engine::GameEngine::playerName, "pixel-font.ttf",  24, 630, 437,  255, 255, 255, 255, 0, 0);
            return;
        }
        if (keyCode < 0 || keyCode > 36 || Engine::GameEngine::playerName.length() >= 12) return;
        for (int i = 0; i <= 36; i++){
            if (keyCode == i){
                Engine::GameEngine::playerName += '@' + i;
                NameText = new Engine::Label(Engine::GameEngine::playerName, "pixel-font.ttf",  24, 630, 437,  255, 255, 255, 255, 0, 0);
                break;
            }
        }
    }
    if (drawCreatePlayerMenu){
        if (keyCode == 59){
            drawCreatePlayerMenu = false;
        }
    }


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

    if (drawProfileListMenu){
        if (IMG_ProfileBG) IMG_ProfileBG->Draw();
        if (BTN_BackPrfl) BTN_BackPrfl->Draw();
        if (BTN_CreateNewProfile) BTN_CreateNewProfile->Draw();
        if (TXT_AddNew) TXT_AddNew->Draw();

        // * Draw Entry Comps
        for (auto & comp : playerEntryComps){
            if (comp.PlayButton) comp.PlayButton->Draw();
            if (comp.IMG_Avatar) comp.IMG_Avatar->Draw();
            if (comp.PlayerDifficulty) comp.PlayerDifficulty->Draw();
            if (comp.PlayerMoney) comp.PlayerMoney->Draw();
            if (comp.PlayerName) comp.PlayerName->Draw();
        }

        
        if (drawCreatePlayerMenu){
            // * Draw Profile Menu Here
            if (IMG_AddNewPlayerBG) IMG_AddNewPlayerBG->Draw();
            if (BTN_Back) BTN_Back->Draw();
            if (BTN_Done && !Engine::GameEngine::playerName.empty()) BTN_Done->Draw();
            if (NameTextBox) NameTextBox->Draw();
            if (NameText && !Engine::GameEngine::playerName.empty()) NameText->Draw();
            if (DoneText) DoneText->Draw();
            if (IMG_Avatar) IMG_Avatar->Draw();
            if (AvatarLeft) AvatarLeft->Draw();
            if (AvatarRight) AvatarRight->Draw();
            if (DiffText) DiffText->Draw();
            if (DiffLeft) DiffLeft->Draw();
            if (DiffRight) DiffRight->Draw();
        } 
    }
}

void StartScene::Terminate() {
    IScene::Terminate();

    if (hasInitialized){
        if (IMG_AddNewPlayerBG )delete IMG_AddNewPlayerBG;
        if (NameText )delete NameText;
        if (DoneText )delete DoneText;
        if (IMG_Avatar )delete IMG_Avatar;
        if (DiffText )delete DiffText;
    }
}
void StartScene::PlayOnClick() {
    // if (drawCreatePlayerMenu) return;
    // // Engine::GameEngine::GetInstance().ChangeScene("stage-select"); gamescene_hall
    // LoadingScene* loadingScene = dynamic_cast<LoadingScene*>(Engine::GameEngine::GetInstance().GetScene("loading-scene"));
    // loadingScene->InitLoadingScreen("gamescene_hall", 2.5f);
    // Engine::GameEngine::GetInstance().ChangeScene("loading-scene");

    // GameSceneHall * gameHallScene = dynamic_cast<GameSceneHall *>(Engine::GameEngine::GetInstance().GetScene("gamescene_hall"));
    // if (bgmInstance){
    //     AudioHelper::StopSample(bgmInstance);
    //     bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    // }
    // StageSelectScene *scene = dynamic_cast<StageSelectScene *>(Engine::GameEngine::GetInstance().GetScene("stage-select"));
    // if (scene != nullptr) scene->bgmInstance = this->bgmInstance;

    if (drawCreatePlayerMenu || drawProfileListMenu) return;
    drawProfileListMenu = true;
    InitializeProfileListMenu();
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


void StartScene::AddNewPlayerBtn(){
    if (drawCreatePlayerMenu) return;
    if (!TXT_Profile) InitializeCreatePlayerMenu();

    drawCreatePlayerMenu = true;
}

void StartScene::RecordPlayerName(){
    if (drawCreatePlayerMenu) {
        // cout << "Recording Player Name " << recordingPlayerName << endl;
        recordingPlayerName = true;
        cout << "Recording Player Name " << recordingPlayerName << endl;
    }
}

void StartScene::InitializeProfileListMenu(){
    IMG_ProfileBG = new Engine::Image("bg/profilelist_bg.png", 0, 0, 1600, 832);

    BTN_BackPrfl = new Engine::ImageButton("btn/back_normal.png", "btn/back_hover.png", 570, 105, 70, 70);
    if (!profileListMenuInitialized) BTN_BackPrfl->SetOnClickCallback(bind(&StartScene::Profile_BackBtnClick, this, 2));
    AddNewControlObject(BTN_BackPrfl, true);
    BTN_CreateNewProfile = new Engine::ImageButton("btn/add_normal.png", "btn/add_hover.png", 950, 105, 70, 70);
    if (!profileListMenuInitialized) BTN_CreateNewProfile->SetOnClickCallback(bind(&StartScene::Profile_AddNewPlayerBtnClick, this));
    AddNewControlObject(BTN_CreateNewProfile, true);

    // * Load All Profile Lists
    vector<PlayerEntry> profiles = Engine::GameEngine::GetInstance().LoadProfileBasedSaving();
    int offsetY = 0;
    for (auto & profile : profiles){
        // * Construct a new EntryComponent Here
        PlayerEntryComponents newComponent;
        newComponent.IMG_Avatar = new Engine::Image(avatarPath[profile.avatarID], 637, 218 + offsetY, 64, 64);
        newComponent.PlayButton = new Engine::ImageButton("btn/profileentry_normal.png", "btn/profileentry_normal.png", 625, 208 + offsetY, 350, 83);
        newComponent.PlayerDifficulty = new Engine::Label(profile.difficulty, "pixel-font.ttf", 19, 707, 254 + offsetY, 255, 255, 255, 255);
        newComponent.PlayerName = new Engine::Label(profile.name, "pixel-font.ttf", profile.name.length() > 10 ? 25 : 35, 707, profile.name.length() > 10 ? 225 + offsetY : 219 + offsetY, 255, 255, 255, 255);
        newComponent.PlayerMoney = new Engine::Label(to_string(profile.money), "pixel-font.ttf", 19, 942, 241 + offsetY, 255, 255, 255, 255, 1.0);
        AddNewControlObject(newComponent.PlayButton, true);
        newComponent.PlayButton->SetOnClickCallback(bind(&StartScene::PlayProfileBtn, this, profiles, offsetY / 85));
        
        playerEntryComps.push_back(newComponent);


        offsetY += 85;
    }

    profileListMenuInitialized = true;
}

void StartScene::PlayProfileBtn(vector<PlayerEntry> & entries, int id){
    if (drawProfileListMenu == false) return;
    cout << "Loading Player " << entries[id].name << endl;
    LoadingScene* loadingScene = dynamic_cast<LoadingScene*>(Engine::GameEngine::GetInstance().GetScene("loading-scene"));
    loadingScene->InitLoadingScreen("gamescene_hall", 2.5f);
    Engine::GameEngine::GetInstance().ChangeScene("loading-scene");

    GameSceneHall * gameHallScene = dynamic_cast<GameSceneHall *>(Engine::GameEngine::GetInstance().GetScene("gamescene_hall"));
    if (bgmInstance){
        AudioHelper::StopSample(bgmInstance);
        bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    }

    if (gameHallScene){
        cout << "PPPPPP pass\n";
        gameHallScene->playerEntryData = entries[id];
    }

}

void StartScene::Profile_AddNewPlayerBtnClick(){
    if (drawCreatePlayerMenu) return;

    cout << "Add New player\n";
    drawCreatePlayerMenu = true;
    InitializeCreatePlayerMenu();
}