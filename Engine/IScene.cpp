#include <allegro5/allegro.h>
#include "IScene.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/Image.hpp"
using namespace Engine;
IObject * DynamicBG, * DynamicBG2;

float fadeDuration = 1.5f;
Enum_TransitionMode currentTransitionMode;

namespace Engine {
	void IScene::Terminate() {
		Clear();
	}
	void IScene::VirtualDraw() const {
		
	}; 
	float posX = 0, posY = 0;
	void IScene::Draw() const {
		al_clear_to_color(al_map_rgb(36, 40, 59));
		Group::Draw();
		VirtualDraw();
	}
}
