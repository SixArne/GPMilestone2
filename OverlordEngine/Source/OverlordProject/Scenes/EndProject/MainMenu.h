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
	enum MenuOptions
	{
		Play,
		Down,
		Up,
		Quit
	};

	GameObject* m_pBackground{};

	FMOD::Channel* m_pBackgroundMusic{};
	FMOD::Sound* m_pMenuIntro{};
	FMOD::Sound* m_pMenuLoop{};

	MenuItem* m_pPlayButton{};

	bool m_IsIntroPlaying{ true };
};
