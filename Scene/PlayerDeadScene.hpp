#ifndef DEADSCENE_HPP
#define DEADSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>
#include "Engine/IScene.hpp"

class DeadScene final : public Engine::IScene {
private:
	std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
public:
	explicit DeadScene() = default;
	void Initialize() override;
	void Terminate() override;

	void RestartBtn();
	void MainMenuBtn();
};

#endif // LOSESCENE_HPP
