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
    void VirtualDraw() const override;
    
private:
    IObject * TXT_Name;
    // * Profile List Components
    Engine::ImageButton * BTN_BackPrfl, * BTN_CreateNewProfile, * BTN_NextPage, *BTN_PrevPage;
    IObject * IMG_ProfileBG, * TXT_AddNew;
    std::vector<PlayerEntryComponents> playerEntryComps;
    void InitializeProfileListMenu();
    void DestroyProfileListMenu();

    int currPage = 1;
    int maxPage = 1;
    int totalEntry = 0;

    int currAvatarID = 0;
    std::string avatarPath[6] = {"avatar/avatar_1.png", "avatar/avatar_2.png", "avatar/avatar_3.png", "avatar/avatar_4.png", "avatar/avatar_5.png", "avatar/avatar_6.png"};
    int currDifficulty = 1; // 0 = easy, 1 = normal, 2 = hard

    vector<PlayerEntry> profiles;
    //Buttons Inits
    void InitializeProfileList(int page);
    void InitializeProfileListButtons();
    void CreateNewProfile();
    void PlayProfileBtn(std::vector<PlayerEntry> & entries, int id);

    void Profile_BackBtnClick();
    void Profile_DoneBtnClick();
    void RecordPlayerName();
    void ChangeAvatar(int mode);
    void ChangeDifficulty(int mode);
    void OnKeyDown(int keyCode) override;

    void ChangePageProfileList(int mode);
    void EmptyFunction() {}
};

#endif // LOSESCENE_HPP
