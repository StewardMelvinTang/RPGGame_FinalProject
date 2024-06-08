#include <allegro5/allegro.h>
#include "IScene.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/Image.hpp"
using namespace Engine;
IObject * DynamicBG, * DynamicBG2;
namespace Engine {
	void IScene::Terminate() {
		Clear();
	}
	void IScene::VirtualDraw() const {
		
	}; 
	float posX = 0, posY = 0;
	void IScene::Draw() const {
		
		// if (DynamicBG == nullptr){
		// 	 DynamicBG = new Engine::Image("bg/DynamicBG.png", 0, 0, 1600, 832, 0, 0);
		// 	 DynamicBG2 = new Engine::Image("bg/DynamicBG.png", 0, -832, 1600, 832, 0, 0);
		// }
		
		al_clear_to_color(al_map_rgb(136, 93, 55));
		// DynamicBG->Draw();
		// DynamicBG2->Draw();
		// // DynamicBG->Position.x += posX;
		// DynamicBG->Position.y += posY;
		// DynamicBG2->Position.y += posY;
		// posY += 0.001;

		// if (posY/832 == 1){
		// 	// posY = 0;
		// 	DynamicBG->Position.y = 0;
		// 	DynamicBG->Position.y = -832;
		// }
		Group::Draw();
		VirtualDraw();
	}
	
}
