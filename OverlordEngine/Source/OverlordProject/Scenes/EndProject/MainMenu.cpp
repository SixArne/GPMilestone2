#include "stdafx.h"
#include "MainMenu.h"
#include "Prefabs/Menu/MenuItem.h"

void MainMenu::Initialize()
{
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.drawPhysXDebug = false;
	m_SceneContext.settings.enableOnGUI = true;

	m_pBackground = new GameObject();
	m_pBackground->AddComponent(new SpriteComponent(L"Textures/main_menu.png", { 0.5f,0.5f }, { 1.f,1.f,1.f,.5f }));
	AddChild(m_pBackground);

	m_pBackground->GetTransform()->Translate(m_SceneContext.windowWidth / 2.f, m_SceneContext.windowHeight / 2.f, .9f);
	m_pBackground->GetTransform()->Scale(1.f, 1.f, 1.f);

	auto inputAction = InputAction(MenuOptions::Play, InputState::pressed, VK_SPACE, -1, XINPUT_GAMEPAD_A);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(MenuOptions::Quit, InputState::pressed, VK_SPACE, -1, XINPUT_GAMEPAD_B);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(MenuOptions::Down, InputState::pressed, VK_SPACE, -1, XINPUT_GAMEPAD_DPAD_DOWN);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(MenuOptions::Up, InputState::pressed, VK_SPACE, -1, XINPUT_GAMEPAD_DPAD_UP);
	m_SceneContext.pInput->AddInputAction(inputAction);

	////////////////////////////////////////////////////////////
	// Music
	////////////////////////////////////////////////////////////
	if (!m_pMenuIntro)
	{
		SoundManager::Get()->GetSystem()->createStream("Resources/Sound/main_menu_intro.wav", FMOD_DEFAULT, nullptr, &m_pMenuIntro);
		m_pMenuIntro->setMode(FMOD_LOOP_OFF);
		m_pMenuIntro->set3DMinMaxDistance(0.f, 100.f);
	}

	if (!m_pMenuLoop)
	{
		SoundManager::Get()->GetSystem()->createStream("Resources/Sound/main_menu.mp3", FMOD_DEFAULT, nullptr, &m_pMenuLoop);
		m_pMenuLoop->setMode(FMOD_LOOP_NORMAL);
		m_pMenuLoop->set3DMinMaxDistance(0.f, 100.f);
	}

	SoundManager::Get()->GetSystem()->playSound(m_pMenuIntro, nullptr, false, &m_pBackgroundMusic);
	m_pBackgroundMusic->setVolume(0.1f);

	//////////////////////////////////////////////////////////////
	// Menu items
	//////////////////////////////////////////////////////////////

	m_pPlayButton = AddChild(new MenuItem("Play"));
	m_pPlayButton->GetTransform()->Translate(0, 0, 0);
}

void MainMenu::Update()
{
	if (m_IsIntroPlaying)
	{
		unsigned int currentPosition{};
		unsigned int introLength{};
		m_pBackgroundMusic->getPosition(&currentPosition, FMOD_TIMEUNIT_MS);
		m_pMenuIntro->getLength(&introLength, FMOD_TIMEUNIT_MS);

		if (introLength - currentPosition <= 1000)
		{
			SoundManager::Get()->GetSystem()->playSound(m_pMenuLoop, nullptr, false, &m_pBackgroundMusic);
			m_pBackgroundMusic->setVolume(0.1f);

			m_IsIntroPlaying = false;
		}
	}

	if (m_SceneContext.pInput->IsActionTriggered(MenuOptions::Play))
	{
		SceneManager::Get()->NextScene();
	}
	else if (m_SceneContext.pInput->IsActionTriggered(MenuOptions::Quit))
	{
		//exit(0);
	}
	else if (m_SceneContext.pInput->IsActionTriggered(MenuOptions::Down))
	{
	}
	else if (m_SceneContext.pInput->IsActionTriggered(MenuOptions::Up))
	{
	}
}

void MainMenu::Draw()
{
}

void MainMenu::OnGUI()
{
	m_pPlayButton->OnImGui();
}

void MainMenu::PostDraw()
{
}
