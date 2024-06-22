#ifndef DIALOGSCREEN_HPP
#define DIALOGSCREEN_HPP
#include <allegro5/bitmap.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/IControl.hpp"
#include "Engine/IObject.hpp"
#include "UI/Component/MultiLineLabel.hpp"
#include "Engine/GameEngine.hpp"

namespace Engine {

	class DialogScreen: public Image, public IControl {
	protected:
		bool mouseIn = false;
		std::function<void()> OnClickCallback;
	public:
		// Whether the button can be pressed.
		int dialogID = -1;
		bool Enabled = true;
        void VirtualDraw() const override;
		explicit DialogScreen(std::string dialogText, std::string charName, float duration, IObject * playerSprite = nullptr, int dialogID = -1);
		void SetOnClickCallback(std::function<void(void)> onClickCallback);
		void OnMouseDown(int button, int mx, int my) override;
		void OnMouseMove(int mx, int my) override;

        ~DialogScreen();

		IObject * IMG_Background = nullptr;
		IObject * playerCharSprite = nullptr;
        
        mutable IObject * TXT_CharName = nullptr;
        mutable Engine::MultiLineLabel * TXT_Dialog = nullptr;
        mutable std::string displayedText = "";
        
        mutable double lastTime = 0.0;
        mutable float dialogDuration = 0.0;
        mutable std::string initialText = "";
        mutable float durationPerChar = 0.0; // * Total Characters in dialogText divided by dialog duration set by player
        mutable float tempDur = 0.f;
        mutable int idx = 0;
	};
}
#endif // DialogScreen
