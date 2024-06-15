#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cctype>
// * Read File
#include <vector>
#include <fstream>

// * Date Time
#include <chrono>
#include <iomanip>

using namespace std;

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
// #include "PlayScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "ScoreBoardScene.hpp"

std::string toUpperCase(const std::string& str) {
    std::string result = str;
    for (size_t i = 0; i < result.length(); ++i) {
        result[i] = std::toupper(result[i]);
    }
    return result;
}

bool compareByScore(const scoreBoardData& a, const scoreBoardData& b) {
    return a.highestScore > b.highestScore; // Return true if 'a' should come before 'b'
}

string ProcessTimeAddZero(int input){ // * round to 00 if < 10
    string result = to_string(input);
    if (input < 10){
        result = "0" + result;
    }

    return result;
}

void ScoreBoardScene::RefreshEntries(){
    if (!entriesLabel.empty()){
        for (auto entry : entriesLabel){
            RemoveObject(entry->GetObjectIterator());
        }

        entriesLabel.clear();
    }
    int paddingY = 0;
    int startIndex = (currPage - 1) * maxEntryPerPage;
    for (int i = startIndex; i < startIndex + maxEntryPerPage && i < entriesData.size(); i++){
        if (entriesData[i].playerName == "") break;
        string playerNameStr = (toUpperCase(entriesData[i].playerName) == toUpperCase(Engine::GameEngine::playerName) ? toUpperCase(entriesData[i].playerName) + " (You)" : toUpperCase(entriesData[i].playerName));
        string dayMonth = std::to_string(entriesData[i].saveTime.day) + "/" + std::to_string(entriesData[i].saveTime.month);
        string timeStr =  ProcessTimeAddZero(entriesData[i].saveTime.hour) + ":" + ProcessTimeAddZero(entriesData[i].saveTime.minute) + ":" + ProcessTimeAddZero(entriesData[i].saveTime.second );

        entriesLabel.push_back(new Engine::Label(dayMonth, "pixel-font.ttf", 32, 540, 151 + paddingY, 255, 255, 255, 255, 0.5, 0.0 ));
        entriesLabel.push_back(new Engine::Label(timeStr, "pixel-font.ttf", 18, 540, 181 + paddingY, 255, 255, 255, 255, 0.5, 0.0 ));
        
        entriesLabel.push_back(new Engine::Label(playerNameStr, "pixel-font.ttf", 43, 610, 154 + paddingY, 255, 255, 255, 255));
        entriesLabel.push_back(new Engine::Label(std::to_string(entriesData[i].highestScore), "pixel-font.ttf", 43, 1060, 154 + paddingY, 255, 255, 255, 255, 1.0f));
        paddingY += 65;
    }

    for (auto entry : entriesLabel){
        AddNewObject(entry);
    }
}

void ScoreBoardScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    IObject * background = new Engine::Image("bg/Leaderboard_Game.png", 0, 0, 1600, 832, 0, 0);
    AddNewObject(background);
    
    Engine::ImageButton* btn;
    btn = new Engine::ImageButton("btn/back_normal.png", "btn/back_hover.png", 456, 42, 70, 70);
    btn->SetOnClickCallback(bind(&ScoreBoardScene::BackOnClick, this));
    AddNewControlObject(btn);
    
    BTN_Next = new Engine::ImageButton("btn/nextBtn_normal.png", "btn/nextBtn_hover.png", 1033, 677, 70, 70);
    BTN_Next->SetOnClickCallback(bind(&ScoreBoardScene::NextPage, this));
    AddNewControlObject(BTN_Next, true);

    BTN_Prev = new Engine::ImageButton("btn/prevBtn_normal.png", "btn/prevBtn_hover.png", 504, 677, 70, 70);
    BTN_Prev->SetOnClickCallback(bind(&ScoreBoardScene::PrevPage, this));
    AddNewControlObject(BTN_Prev, true);

    currPage = 1;   
    entriesData = Engine::GameEngine::GetInstance().LoadScoreBoard();
    sort(entriesData.begin(), entriesData.end(), compareByScore);
    maxPage = floor( entriesData.size() / maxEntryPerPage) + 1;

    cout << entriesData.size() << endl;

    RefreshEntries();

    // AddNewObject(new Engine::Label("Scoreboard", "pixel-font.ttf", 45, 95, 30, 255, 255, 255, 255, 0, 0));
}
void ScoreBoardScene::VirtualDraw() const {
    IScene::VirtualDraw();

    if (BTN_Prev && currPage > 1) BTN_Prev->Draw();
    if (BTN_Next && currPage < maxPage) BTN_Next->Draw();
    
}
void ScoreBoardScene::Terminate() {
    entriesData.clear();
    entriesLabel.clear();
    IScene::Terminate();
}
void ScoreBoardScene::BackOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene(backMenuLevel);
}
void ScoreBoardScene::NextPage(){
    if (currPage >= maxPage) return;
    currPage++;
    RefreshEntries();
}
void ScoreBoardScene::PrevPage(){
    if (currPage <= 1) return;
    currPage--;
    RefreshEntries();
}


void ScoreBoardScene::OnKeyDown(int keyCode) {
    if (keyCode == ALLEGRO_KEY_C){
        cout << "Adding Entry " << Engine::GameEngine::playerName << endl;

        bool contains = false;
        for (auto entry : entriesData){
            if (entry.playerName == Engine::GameEngine::playerName){
                contains = true;
            }
        }

        if (contains == false){
            scoreBoardData newEntry;
            newEntry.highestScore = 15000;
            newEntry.playerName = Engine::GameEngine::playerName;
            newEntry.saveTime = Engine::GameEngine::GetInstance().GetCurrentDateTime();
            entriesData.push_back(newEntry);
            cout << "Creating new Entry of name " << newEntry.playerName << endl;
        }
        for (auto entry : entriesData){
            // cout << "DBG : " << entry.playerName << endl;
            if (entry.playerName == Engine::GameEngine::playerName){
                // * found player. changing score
                entry.highestScore = 16000;
                entry.saveTime = Engine::GameEngine::GetInstance().GetCurrentDateTime();
                Engine::GameEngine::GetInstance().WriteScoreBoard(entriesData, entry);
                break;
            }

            RefreshEntries();
        }

    }


}