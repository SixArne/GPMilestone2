#include "stdafx.h"
#include "PauseMenu.h"

void PauseMenu::Toggle()
{
	SetActive(!IsActive());
	m_pPlayButton->Toggle();
	m_pQuitButton->Toggle();
	m_pMainMenu->Toggle();
}

void PauseMenu::Initialize(const SceneContext& sceneContext)
{
	// SHOULD BE INACTIVE BY DEFAULT
	SetActive(false);

	auto bg = AddChild(new GameObject());

	bg->AddComponent(new SpriteComponent(L"Textures/Hud/pause_screen.png", { 0.5f,0.5f }, { 1.f,1.f,1.f,.5f }));
	bg->GetTransform()->Translate(sceneContext.windowWidth / 2.f, sceneContext.windowHeight / 2.f, .9f);
	bg->GetTransform()->Scale(1.f, 1.f, 1.f);

	if (!m_pMenuSelect)
	{
		SoundManager::Get()->GetSystem()->createStream("Resources/Sound/menu_select_short.mp3", FMOD_DEFAULT, nullptr, &m_pMenuSelect);
		m_pMenuSelect->setMode(FMOD_LOOP_OFF);
		m_pMenuSelect->set3DMinMaxDistance(0.f, 100.f);
	}

	auto inputAction = InputAction(PauseMenuOptions::Accept, InputState::pressed, VK_SPACE, -1, XINPUT_GAMEPAD_A);
	sceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(PauseMenuOptions::Down, InputState::pressed, VK_DOWN, -1, XINPUT_GAMEPAD_DPAD_DOWN);
	sceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(PauseMenuOptions::Up, InputState::pressed, VK_UP, -1, XINPUT_GAMEPAD_DPAD_UP);
	sceneContext.pInput->AddInputAction(inputAction);

	//////////////////////////////////////////////////////////////
	// Menu items
	//////////////////////////////////////////////////////////////

	m_pPlayButton = AddChild(new MenuItem("Resume"));
	m_pPlayButton->SetPosition(XMFLOAT2{ 250,260 });

	m_pMainMenu = AddChild(new MenuItem("Main menu"));
	m_pMainMenu->SetPosition(XMFLOAT2{ 250,400 });

	m_pQuitButton = AddChild(new MenuItem("Quit"));
	m_pQuitButton->SetPosition(XMFLOAT2{ 250,560 });

	m_pButtons.push_back(m_pPlayButton);
	m_pButtons.push_back(m_pMainMenu);
	m_pButtons.push_back(m_pQuitButton);

	// DISABLE BUTTONS ON START
	m_pPlayButton->Toggle();
	m_pQuitButton->Toggle();
	m_pMainMenu->Toggle();
}

void PauseMenu::Update(const SceneContext& sceneContext)
{
	if (!IsActive()) return;

	for (size_t i{}; i < m_pButtons.size(); i++)
	{
		if (i == m_CurrentButtonIdx)
		{
			m_pButtons[i]->SetIsSelected(true);
		}
		else
		{
			m_pButtons[i]->SetIsSelected(false);
		}
	}

	if (sceneContext.pInput->IsActionTriggered(PauseMenuOptions::Accept))
	{
		switch (m_CurrentButtonIdx)
		{
		case (int)PauseMenuButtons::Restart:
		{
			SceneManager::Get()->NextScene();
			//m_pBackgroundMusic->setPaused(true);
			break;
		}
		case (int)PauseMenuButtons::Menu:
		{
			SceneManager::Get()->NextScene();
			//m_pBackgroundMusic->setPaused(true);
			break;
		}
		case (int)PauseMenuButtons::Quit:
		{
			Logger::LogDebug(L"Quiting game");
			break;
		}
		}
	}
	else if (sceneContext.pInput->IsActionTriggered(PauseMenuOptions::Down))
	{
		m_CurrentButtonIdx = (m_CurrentButtonIdx + 1) % m_pButtons.size();
		PlayAudio(m_pMenuSelect, 1.f);
	}
	else if (sceneContext.pInput->IsActionTriggered(PauseMenuOptions::Up))
	{
		m_CurrentButtonIdx = (m_CurrentButtonIdx - 1) % m_pButtons.size();
		PlayAudio(m_pMenuSelect, 1.f);
	}
}

void PauseMenu::PlayAudio(FMOD::Sound* sound, float volume)
{
	FMOD::Channel* ch = nullptr;
	SoundManager::Get()->GetSystem()->playSound(sound, nullptr, false, &ch);
	ch->setVolume(volume);
}
