#pragma once

class GameObject;
class MenuItem;

class MainMenu :
    public GameScene
{
public:
	MainMenu() : GameScene(L"MainMenuScene") {}

	~MainMenu() override = default;
	MainMenu(const MainMenu& other) = delete;
	MainMenu(MainMenu&& other) noexcept = delete;
	MainMenu& operator=(const MainMenu& other) = delete;
	MainMenu& operator=(MainMenu&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnGUI() override;
	void PostDraw() override;

private:
	void PlayAudio(FMOD::Sound* sound, FMOD::Channel* ch, float volume);

	enum MenuOptions
	{
		Play,
		Down,
		Up,
		Quit
	};

	enum class MenuButtons
	{
		Play,
		Quit
	};

	GameObject* m_pBackground{};

	FMOD::Channel* m_pBackgroundMusic{};
	FMOD::Channel* m_pSFXChannel{};
	FMOD::Sound* m_pMenuIntro{};
	FMOD::Sound* m_pMenuLoop{};
	FMOD::Sound* m_pMenuSelect{};

	std::vector<MenuItem*> m_pButtons{};
	size_t m_CurrentButtonIdx{0};

	MenuItem* m_pPlayButton{};
	MenuItem* m_pQuitButton{};

	bool m_IsIntroPlaying{ true };
};
