#pragma once
#include "Prefabs/Menu/MenuItem.h"

class PauseMenu : public GameObject
{
public:
	PauseMenu() = default;
	~PauseMenu() override = default;
	PauseMenu(const PauseMenu& other) = delete;
	PauseMenu(PauseMenu&& other) noexcept = delete;
	PauseMenu& operator=(const PauseMenu& other) = delete;
	PauseMenu& operator=(PauseMenu&& other) noexcept = delete;

	void Toggle();

	enum PauseMenuOptions
	{
		Open = 100,
		Down,
		Up,
		Accept,
	};

	enum class PauseMenuButtons
	{
		Restart,
		Menu,
		Quit
	};

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;

private:
	void PlayAudio(FMOD::Sound* sound, float volume);

	FMOD::Sound* m_pMenuSelect{};

	std::vector<MenuItem*> m_pButtons{};
	size_t m_CurrentButtonIdx{ 0 };


	MenuItem* m_pPlayButton{};
	MenuItem* m_pQuitButton{};
	MenuItem* m_pMainMenu{};
};

