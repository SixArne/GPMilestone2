#include "stdafx.h"
#include "TestScene.h"

#include "Logger.h"

#include "CubePosColorNorm.h"
#include "TorusPosColorNorm.h"
#include "CylinderPosColorNorm.h"

void TestScene::Initialize()
{
	m_pBox = new CubePosColorNorm(5, 5, 5);
	AddGameObject(m_pBox);

	m_pBox2 = new CubePosColorNorm(5, 5, 5);
	AddGameObject(m_pBox2);
	m_pBox2->Translate(0, 8, 0);
	m_pBox2->SetIsFlipped(true);

	m_pTorus = new TorusPosColorNorm(8.f, 50.f, 1.f, 10.f);
	AddGameObject(m_pTorus);

	m_pCylinder = new CylinderPosColorNorm(1.5f, 10.f, 4.0f);
	m_pTorus->AddChild(m_pCylinder);

	m_pCylinder->Translate(0, 8, 0);

	// Camera
	m_SceneContext.GetCamera()->SetPosition(XMFLOAT3{ 0, 10, -25 });
	m_SceneContext.GetCamera()->SetForward(XMFLOAT3{ 0, -.3, 1 });

	// Input
	//m_SceneContext.GetInput()->AddInputAction(
	//	InputAction{InputIDs::MoveLeft, InputTriggerState::down, VK_LEFT, -1, XINPUT_GAMEPAD_DPAD_LEFT}
	//);

	//m_SceneContext.GetInput()->AddInputAction(
	//	InputAction{ InputIDs::MoveRight, InputTriggerState::down, VK_RIGHT, -1, XINPUT_GAMEPAD_DPAD_RIGHT }
	//);
}

void TestScene::Update()
{
	for (auto obj : m_GameObjects)
	{
		obj->Update(m_SceneContext);
	}

	Logger::GetInstance()->Log(LogLevel::Info, L"Update");
}

void TestScene::Draw() const
{
	for (auto obj : m_GameObjects)
	{
		obj->Draw(m_SceneContext);
	}
}

void TestScene::OnSceneActivated()
{
	Logger::GetInstance()->LogFormat(LogLevel::Info, L"Scene Activated > \"%ls\"", GetName().c_str());
}

void TestScene::OnSceneDeactivated()
{
}
