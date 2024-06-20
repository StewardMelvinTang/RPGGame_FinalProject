#ifndef PROFILESCENE_HPP
#define PROFILESCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>
#include "Engine/IScene.hpp"

class ProfileScene final : public Engine::IScene {
struct PlayerEntryComponents
{
    IObject * IMG_Avatar;
    IObject * PlayerName, * PlayerDifficulty, * PlayerMoney;
    Engine::ImageButton * PlayButton;
};
public:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    
    std::string nextScene = "";
    std::string prevScene = "";
    std::string backMenuLevel = "start-scene";
    float duration = 0.f;
    float totalDur = 0.f;
    IObject * loadingProgressBar = nullptr;

	explicit ProfileScene() = default;
	void Initialize() override;
	void Terminate() override;
    // void Update(float deltaTime) override;
    void InitLoadingScreen(std::string nextScene, float duration);
    // New Inits
    void BackOnClick();
    
private:
    // bools
    bool drawCreatePlayerMenu = false;
    bool recordingPlayerName = false;

    bool drawProfileListMenu = false;
    bool hasInitialized = false;
    bool profileListMenuInitialized = false;

    ALLEGRO_SAMPLE_ID bgmId;
    
    IObject * TXT_Name;
    // * Create New Player
    IObject * TXT_Profile;
    IObject * IMG_AddNewPlayerBG;
    IObject * NameText, * DoneText, *DiffText;
    IObject * IMG_Avatar;
    Engine::ImageButton * BTN_Back, * BTN_Done, * NameTextBox, * AvatarLeft, *AvatarRight, *DiffLeft, *DiffRight;
    
    // * Profile List Components
    Engine::ImageButton * BTN_BackPrfl, * BTN_CreateNewProfile;
    IObject * IMG_ProfileBG, * TXT_AddNew;
    std::vector<PlayerEntryComponents> playerEntryComps;
    void InitializeProfileListMenu();
    void DestroyProfileListMenu();

    int currAvatarID = 0;
    std::string avatarPath[6] = {"avatar/avatar_1.png", "avatar/avatar_2.png", "avatar/avatar_3.png", "avatar/avatar_4.png", "avatar/avatar_5.png", "avatar/avatar_6.png"};
    int currDifficulty = 1; // 0 = easy, 1 = normal, 2 = hard
    //Buttons Inits
    void InitializeProfileList();
    void InitializeProfileListButtons();
    void CreateNewProfile();
    void PlayProfileBtn(std::vector<PlayerEntry> & entries, int id);

    void Profile_BackBtnClick();
    void Profile_DoneBtnClick();
    void RecordPlayerName();
    void ChangeAvatar(int mode);
    void ChangeDifficulty(int mode);
    void OnKeyDown(int keyCode) override;
};

#endif // LOSESCENE_HPP