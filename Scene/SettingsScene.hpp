#ifndef SettingsScene_HPP
#define SettingsScene_HPP

#include <memory>
#include "Engine/IScene.hpp"

class SettingsScene final : public Engine::IScene {
private:
public:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    explicit SettingsScene() = default;

    void Initialize() override;

    void Terminate() override;

    void PlayOnClick(int stage);

    void BGMSlideOnValueChanged(float value);

    void SFXSlideOnValueChanged(float value);

    void BackMainMenu();

    void OnMouseUp(int button, int mx, int my) override;
};
#endif // SettingsScene_HPP
