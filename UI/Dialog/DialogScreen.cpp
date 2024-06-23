#include <functional>
#include <memory>

#include "Engine/Collider.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/Image.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/MultiLineLabel.hpp"
#include "Engine/Group.hpp"
#include <iostream>
#include <string>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro.h>

#include "DialogScreen.hpp"
using namespace std;

const std::string dialogBGpath = "bg/Dialog_background_full.png";

namespace Engine {
    DialogScreen::DialogScreen(std::string dialogText, std::string charName, float duration, IObject* playerSprite, int dialogID) 
        : Image(dialogBGpath, 0, 0, 0, 0, 0, 0, true), playerCharSprite(playerSprite) {
        this->dialogID = dialogID;
        Point mouse = GameEngine::GetInstance().GetMousePosition();
        IMG_Background = new Engine::Image(dialogBGpath, 0, 0, 1600, 832, 0, 0, false);

        lastTime = al_get_time();
        initialText = dialogText;

        TXT_CharName = new Engine::Label(charName, "pixel-font.ttf", 40, 190, 510, 255, 255, 255, 255);
        TXT_Dialog = new Engine::MultiLineLabel(displayedText, "pixel-font.ttf", 30, 190, 565, 255, 255, 255, 255);

        durationPerChar = (duration / dialogText.length()) / 1.5;
        cout << "Duration Per Char : " << durationPerChar << endl;
        cout << "Dialog Duration : " << duration << endl;
        tempDur = durationPerChar;
    }

    void DialogScreen::SetOnClickCallback(std::function<void(void)> onClickCallback) {
        OnClickCallback = onClickCallback;
    }

    void DialogScreen::OnMouseDown(int button, int mx, int my) {
        if (Enabled && idx >= initialText.length() - 1) {
            cout << "Click Callback Called\n";
            if (OnClickCallback)
                OnClickCallback();
        } else if (Enabled && idx < initialText.length() - 1) {
            // * Complete Dialog
            idx = initialText.length() - 1;
            displayedText = initialText;
            TXT_Dialog->SetText(displayedText);
        }
    }

    void DialogScreen::OnMouseMove(int mx, int my) {
        // mouseIn = Collider::IsPointInBitmap(Point((mx - Position.x) * GetBitmapWidth() / Size.x + Anchor.x * GetBitmapWidth(), (my - Position.y) * GetBitmapHeight() / Size.y + Anchor.y * GetBitmapHeight()), bmp);
        // if (!mouseIn || !Enabled) bmp = imgOut;
        // else bmp = imgIn;
    }

    DialogScreen::~DialogScreen() {
        cout << "Deinitialize\n";
        Enabled = false;
        delete TXT_CharName;
        delete TXT_Dialog;
        delete IMG_Background;
    }

    void DialogScreen::VirtualDraw() const {
        // * Get Delta Time
        double currentTime = al_get_time();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // cout << "delta time : " << deltaTime << endl;
        if (playerCharSprite) playerCharSprite->Draw();
        if (IMG_Background) IMG_Background->Draw();
        if (TXT_CharName) TXT_CharName->Draw();
        if (TXT_Dialog) TXT_Dialog->Draw();

        if (idx == initialText.length()) return; // ? text animation finished playing noted with idx == initialText.length()
        if (tempDur >= -.1 && tempDur <= 0.0) {
            displayedText += initialText[idx++];
            TXT_Dialog->SetText(displayedText);
            tempDur = durationPerChar;
        } else {
            tempDur -= 1 * deltaTime;
        }
    }
}
