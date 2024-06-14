#ifndef LOADINGSCENE_HPP
#define LOADINGSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>
#include "Engine/IScene.hpp"

class LoadingScene final : public Engine::IScene {
public:
    std::string nextScene = "";
    std::string prevScene = "";
    float duration = 0.f;
    float totalDur = 0.f;
    IObject * loadingProgressBar = nullptr;

	explicit LoadingScene() = default;
	void Initialize() override;
	void Terminate() override;
    void Update(float deltaTime) override;

    void InitLoadingScreen(std::string nextScene, float duration);
};

#endif // LOSESCENE_HPP
