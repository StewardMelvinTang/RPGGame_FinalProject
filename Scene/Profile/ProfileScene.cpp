#include <functional>
#include <string>
#include <iostream>
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/Label.hpp"
#include "Engine/Point.hpp"
#include "UI/Component/ImageButton.hpp"
// NEW
#include "Scene/GameScene_Hall.hpp"
#include "Scene/Loading/LoadingScene.hpp"
#include "Scene/Profile/NewProfileScene.hpp"
using namespace std;
#include "ProfileScene.hpp"
#include "Scene/StartScene.h"


void ProfileScene::Initialize() {
    playerEntryComps.clear();
	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int halfW = w / 2;
	int halfH = h / 2;
    AddNewObject(new Engine::Image("bg/ProfileScene_bg.png", 0 ,0 , 1600, 832, 0.0, 0.0));
    
    InitializeProfileList();
    InitializeProfileListButtons();
}
void ProfileScene::InitializeProfileListButtons(){
    BTN_BackPrfl = new Engine::ImageButton("btn/back_normal.png", "btn/back_hover.png", 570, 105, 70, 70);
    BTN_BackPrfl->SetOnClickCallback(bind(&ProfileScene::BackOnClick, this));
    AddNewControlObject(BTN_BackPrfl);
    // AddNewObject(BTN_BackPrfl);
    BTN_CreateNewProfile = new Engine::ImageButton("btn/add_normal.png", "btn/add_hover.png", 950, 105, 70, 70);
    BTN_CreateNewProfile->SetOnClickCallback(bind(&ProfileScene::CreateNewProfile, this));
    AddNewControlObject(BTN_CreateNewProfile);
    // AddNewObject(BTN_CreateNewProfile);
}
void ProfileScene::BackOnClick(){
    Engine::GameEngine::GetInstance().ChangeScene("start-scene");
}
void ProfileScene::CreateNewProfile(){
    NewProfileScene *scene = dynamic_cast<NewProfileScene*>(Engine::GameEngine::GetInstance().GetScene("newprofile-scene"));
    if(scene != nullptr) scene->bgmInstance = this->bgmInstance;
    Engine::GameEngine::GetInstance().ChangeScene("newprofile-scene");
}

void ProfileScene::InitializeProfileList(){
    vector<PlayerEntry> profiles = Engine::GameEngine::GetInstance().LoadProfileBasedSaving();
    int offsetY = 0;
    for (auto & profile : profiles){
        // * Construct a new EntryComponent Here
        PlayerEntryComponents newComponent;
        newComponent.IMG_Avatar = new Engine::Image(avatarPath[profile.avatarID], 637, 218 + offsetY, 64, 64);
        newComponent.PlayButton = new Engine::ImageButton("btn/profileentry_normal.png", "btn/profileentry_hover.png", 625, 208 + offsetY, 350, 83);
        newComponent.PlayerDifficulty = new Engine::Label(profile.difficulty, "pixel-font.ttf", 19, 707, 254 + offsetY, 255, 255, 255, 255);
        newComponent.PlayerName = new Engine::Label(profile.name, "pixel-font.ttf", profile.name.length() > 10 ? 25 : 35, 707, profile.name.length() > 10 ? 225 + offsetY : 219 + offsetY, 255, 255, 255, 255);
        newComponent.PlayerMoney = new Engine::Label(to_string(profile.money), "pixel-font.ttf", 19, 942, 241 + offsetY, 255, 255, 255, 255, 1.0);
        AddNewControlObject(newComponent.PlayButton);
        AddNewObject(newComponent.IMG_Avatar);
        AddNewObject(newComponent.PlayerDifficulty);
        AddNewObject(newComponent.PlayerName);
        AddNewObject(newComponent.PlayerMoney);
        newComponent.PlayButton->SetOnClickCallback(bind(&ProfileScene::PlayProfileBtn, this, profiles, offsetY / 85));
        
        playerEntryComps.push_back(newComponent);


        offsetY += 85;
    }

    profileListMenuInitialized = true;
}
void ProfileScene::PlayProfileBtn(vector<PlayerEntry> & entries, int id){
    // cout << "Loading Player " << entries[id].name << endl;
    // LoadingScene* loadingScene = dynamic_cast<LoadingScene*>(Engine::GameEngine::GetInstance().GetScene("loading-scene"));
    // loadingScene->InitLoadingScreen("gamescene_hall", 2.5f);
    // Engine::GameEngine::GetInstance().ChangeScene("loading-scene");

    // GameSceneHall * gameHallScene = dynamic_cast<GameSceneHall *>(Engine::GameEngine::GetInstance().GetScene("gamescene_hall"));
    
    // if (gameHallScene){
    //     cout << "PPPPPP pass\n";
    //     gameHallScene->playerEntryData = entries[id];
    // }
    // if (bgmInstance){
    //     AudioHelper::StopSample(bgmInstance);
    //     bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    // }
    StartScene * Start = dynamic_cast<StartScene *>(Engine::GameEngine::GetInstance().GetScene("start-scene"));
    Start->entries = entries;
    Start->Id_Entries = id;
    Start->NAMEPLAYER = entries[id].name;
    Engine::GameEngine::GetInstance().ChangeScene("start-scene");
}
void ProfileScene::Terminate() {
    // AudioHelper::StopBGM(bgmId);
	// AudioHelper::StopSample(deathBGMInstance);
	// deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
	cout << "Terminated_ProfileScene!\n";
	IScene::Terminate();
}
void ProfileScene::OnKeyDown(int keyCode){
    IScene::OnKeyDown(keyCode);
    if(keyCode == ALLEGRO_KEY_ESCAPE){
        Engine::GameEngine::GetInstance().ChangeScene("start-scene");
    }
}

