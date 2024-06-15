#ifndef DEATHSCENE_HPP
#define DEATHSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>
#include "Engine/IScene.hpp"

class DeathScene final : public Engine::IScene {
public:
    std::string nextScene = "";
    std::string prevScene = "";
    std::string backMenuLevel = "start-scene";
    float duration = 0.f;
    float totalDur = 0.f;
    IObject * loadingProgressBar = nullptr;

	explicit DeathScene() = default;
	void Initialize() override;
	void Terminate() override;
    void Update(float deltaTime) override;
    void InitLoadingScreen(std::string nextScene, float duration);
    // New Inits
    void BackOnClick();
private:
    ALLEGRO_SAMPLE_ID bgmId;
};

#endif // LOSESCENE_HPP
