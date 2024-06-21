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
    playerEntryComps.clear(); profiles.clear();
	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int halfW = w / 2;
	int halfH = h / 2;
    AddNewObject(new Engine::Image("bg/ProfileScene_bg.png", 0 ,0 , 1600, 832, 0.0, 0.0));

    profiles = Engine::GameEngine::GetInstance().LoadProfileBasedSaving();
    totalEntry = profiles.size();
    
    InitializeProfileList(0);
    InitializeProfileListButtons();

    cout << bgmInstance << endl;

}
void ProfileScene::InitializeProfileListButtons(){
    BTN_BackPrfl = new Engine::ImageButton("btn/back_normal.png", "btn/back_hover.png", 570, 105, 70, 70);
    BTN_BackPrfl->SetOnClickCallback(bind(&ProfileScene::BackOnClick, this));
    AddNewControlObject(BTN_BackPrfl);
    // AddNewObject(BTN_BackPrfl);
    BTN_CreateNewProfile = new Engine::ImageButton("btn/add_normal.png", "btn/add_hover.png", 950, 105, 70, 70);
    BTN_CreateNewProfile->SetOnClickCallback(bind(&ProfileScene::CreateNewProfile, this));
    AddNewControlObject(BTN_CreateNewProfile);

    BTN_NextPage = new Engine::ImageButton("btn/nextBtn_normal.png", "btn/nextBtn_hover.png", 920, 635, 60, 60);
    AddNewControlObject(BTN_NextPage, true);
    BTN_PrevPage = new Engine::ImageButton("btn/prevBtn_normal.png", "btn/prevBtn_hover.png", 620, 635, 60, 60);
    AddNewControlObject(BTN_PrevPage, true);
    BTN_NextPage->SetOnClickCallback(bind(&ProfileScene::ChangePageProfileList, this, 1));
    BTN_PrevPage->SetOnClickCallback(bind(&ProfileScene::ChangePageProfileList, this, -1));
    
    // AddNewObject(BTN_CreateNewProfile);
}

void ProfileScene::ChangePageProfileList(int mode){ // 1 = Next, -1 Prev
    if (mode == 1){
        if (currPage + 1 > maxPage) return;
        currPage++;
    } else if (mode == -1){
        if (currPage - 1 < 1) return;
        currPage--;
    }

    InitializeProfileList(currPage);
}
void ProfileScene::BackOnClick(){
    Engine::GameEngine::GetInstance().ChangeScene("start-scene");
}
void ProfileScene::CreateNewProfile(){
    NewProfileScene *scene = dynamic_cast<NewProfileScene*>(Engine::GameEngine::GetInstance().GetScene("newprofile-scene"));
    if(scene != nullptr) scene->bgmInstance = this->bgmInstance;
    Engine::GameEngine::GetInstance().ChangeScene("newprofile-scene");
}

void ProfileScene::InitializeProfileList(int page){

    cout << "Player Hve " << playerEntryComps.size() << endl;
    if (playerEntryComps.size() > 0){
        for (auto & comp : playerEntryComps){
            RemoveObject(comp.PlayerName->GetObjectIterator());
            RemoveObject(comp.IMG_Avatar->GetObjectIterator());
            RemoveObject(comp.PlayerMoney->GetObjectIterator());
            RemoveObject(comp.PlayerDifficulty->GetObjectIterator());           
            comp.PlayButton->SetOnClickCallback(bind(&ProfileScene::EmptyFunction, this));
            RemoveObject(comp.PlayButton->GetObjectIterator());
        }
    }
    
    playerEntryComps.clear();

    int offsetY = 0;

    
    maxPage = floor(totalEntry / 5) + 1;

    int startIndex = (currPage - 1 ) * 5;
    for (int i = startIndex; i < startIndex + 5 && i < profiles.size(); i++){
        // * Construct a new EntryComponent Here
        auto profile = profiles[i];
        PlayerEntryComponents newComponent;
        newComponent.IMG_Avatar = new Engine::Image(avatarPath[profile.avatarID], 637, 218 + offsetY, 64, 64);
        newComponent.PlayButton = new Engine::ImageButton("btn/profileentry_normal.png", "btn/profileentry_hover.png", 625, 208 + offsetY, 350, 83);
        newComponent.PlayerDifficulty = new Engine::Label(profile.difficulty, "pixel-font.ttf", 19, 707, 254 + offsetY, 255, 255, 255, 255);
        newComponent.PlayerName = new Engine::Label(profile.name, "pixel-font.ttf", profile.name.length() > 10 ? 25 : 35, 707, profile.name.length() > 10 ? 225 + offsetY : 219 + offsetY, 255, 255, 255, 255);
        newComponent.PlayerMoney = new Engine::Label(to_string(profile.money), "pixel-font.ttf", 19, 942, 241 + offsetY, 255, 255, 255, 255, 1.0);
        newComponent.PlayButton->SetOnClickCallback(bind(&ProfileScene::PlayProfileBtn, this, profiles, offsetY / 85));
        AddNewControlObject(newComponent.PlayButton);
        AddNewObject(newComponent.IMG_Avatar);
        AddNewObject(newComponent.PlayerDifficulty);
        AddNewObject(newComponent.PlayerName);
        AddNewObject(newComponent.PlayerMoney);
        
        playerEntryComps.push_back(newComponent);
        cout << "Created player" << profile.name << endl;

        offsetY += 85;
    }
}
void ProfileScene::PlayProfileBtn(vector<PlayerEntry> & entries, int id){
    // cout << "Loading Player " << entries[id].name << endl;
    // LoadingScene* loadingScene = dynamic_cast<LoadingScene*>(Engine::GameEngine::GetInstance().GetScene("loading-scene"));
    // loadingScene->InitLoadingScreen("gamescene_hall", 2.5f);

    Engine::GameEngine::GetInstance().SetCurrentActivePlayer(entries[id].name, entries[id]);

    Engine::GameEngine::GetInstance().ChangeScene("gamescene_hall");

    GameSceneHall * gameHallScene = dynamic_cast<GameSceneHall *>(Engine::GameEngine::GetInstance().GetScene("gamescene_hall"));
    
    if (bgmInstance){
        AudioHelper::StopSample(bgmInstance);
        bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    }
    // StartScene * Start = dynamic_cast<StartScene *>(Engine::GameEngine::GetInstance().GetScene("start-scene"));
    // Start->entries = entries;
    // Start->Id_Entries = id;
    // Start->NAMEPLAYER = entries[id].name;
    // Engine::GameEngine::GetInstance().ChangeScene("start-scene");
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

void ProfileScene::VirtualDraw() const {
     IScene::VirtualDraw();
     if (BTN_PrevPage && currPage > 1) BTN_PrevPage->Draw();
     if (BTN_NextPage && currPage < maxPage) BTN_NextPage->Draw();
}