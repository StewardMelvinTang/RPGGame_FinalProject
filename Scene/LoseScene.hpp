#ifndef LOSESCENE_HPP
#define LOSESCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>
#include "Engine/IScene.hpp"

class LoseScene final : public Engine::IScene {
private:
	std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
public:
	explicit LoseScene() = default;
	void Initialize() override;
	void Terminate() override;
	void BackOnClick(int stage);

	void RestartBtn();
	void ScoreBoardBtn();

	int stage = 0;
	int score = 0, kill = 0, towerBuilt = 0;
};

#endif // LOSESCENE_HPP
