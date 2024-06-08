#ifndef ScoreBoardScene_HPP
#define ScoreBoardScene_HPP
#include <allegro5/allegro_audio.h>
#include <memory>
#include <vector>
#include <string>
#include "Engine/IScene.hpp"
using namespace std;
using namespace Engine;

class ScoreBoardScene final : public Engine::IScene {
private:
	// std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;

	const string scoreBoardfileName = "scoreboard.txt";

	int currPage = 1;
	int maxPage = 3;


	std::vector<scoreBoardData> entriesData;
	// vector<scoreBoardData> entriesData;
	vector<Engine::Label *> entriesLabel;
	int maxEntryPerPage = 8;
	ImageButton * BTN_Next, * BTN_Prev;
public:

	std::string backMenuLevel = "start-scene";
	explicit ScoreBoardScene() = default;
	void Initialize() override;
	void VirtualDraw() const override;
	void Terminate() override;

	void BackOnClick();

	void NextPage();
	void PrevPage();

	void RefreshEntries();

    void OnKeyDown(int keyCode) override;
};

#endif // ScoreBoardScene_HPP
