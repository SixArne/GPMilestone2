#include "stdafx.h"
#include "Mario.h"
#include "Materials/DiffuseMaterial_Skinned.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow_Skinned.h"
#include "Prefabs/Character.h"


void Mario::Initialize(const SceneContext&)
{
	

	//////////////////////////////////////////////////////////////////////////
	//							Character Controller.
	//////////////////////////////////////////////////////////////////////////
	auto marioObject = AddChild(new GameObject());
	m_pMarioModel = marioObject->AddComponent(new ModelComponent(L"Meshes/mario.ovm"));

	marioObject->GetTransform()->Translate(0.f, -1.5f, 0.f);

	//const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	//////////////////////////////////////////////////////////////////////////
	//							Mario model and textures.
	//////////////////////////////////////////////////////////////////////////

	const auto pBodyMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow_Skinned>();
	pBodyMaterial->SetDiffuseTexture(L"Textures/mariobodyfix_alb.png");

	const auto pFaceMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow_Skinned>();
	pFaceMaterial->SetDiffuseTexture(L"Textures/marioface_alb.png");

	const auto pHandsMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow_Skinned>();
	pHandsMaterial->SetDiffuseTexture(L"Textures/mariohandnew_alb.png");

	const auto pShoeMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow_Skinned>();
	pShoeMaterial->SetDiffuseTexture(L"Textures/marioshoes_alb.png");

	const auto pEyeMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow_Skinned>();
	pEyeMaterial->SetDiffuseTexture(L"Textures/marioeye_alb.png");



	m_pMarioModel->SetMaterial(pBodyMaterial, 0);
	m_pMarioModel->SetMaterial(pBodyMaterial, 1);
	m_pMarioModel->SetMaterial(pBodyMaterial, 2);
	m_pMarioModel->SetMaterial(pFaceMaterial, 3);
	m_pMarioModel->SetMaterial(pHandsMaterial, 4);
	m_pMarioModel->SetMaterial(pBodyMaterial, 5);
	m_pMarioModel->SetMaterial(pFaceMaterial, 6);
	m_pMarioModel->SetMaterial(pHandsMaterial, 7);
	m_pMarioModel->SetMaterial(pShoeMaterial, 8);
	m_pMarioModel->SetMaterial(pEyeMaterial, 9);

	// get half height from character capsule
	//-characterDesc.controller.height

	//////////////////////////////////////////////////////////////////////////
	//							Mario animations
	//////////////////////////////////////////////////////////////////////////
	
	InitializeSounds();
}

void Mario::PostInitialize(const SceneContext&)
{
	pAnimator = m_pMarioModel->GetAnimator();
	pAnimator->SetAnimation(m_AnimationClipId);
	pAnimator->SetAnimationSpeed(m_AnimationSpeed);

	//Gather Clip Names
	m_ClipCount = pAnimator->GetClipCount();
	std::vector<std::wstring> clipNames{};
	for (UINT i{ 0 }; i < m_ClipCount; ++i)
	{
		clipNames.emplace_back(pAnimator->GetClip(static_cast<int>(i)).name);
	}

	pAnimator->Play();
}

void Mario::DrawImGui()
{
	 reinterpret_cast<Character*>(GetParent())->DrawImGui();
}


void Mario::Update(const SceneContext&)
{
	if (InputManager::IsKeyboardKey(InputState::released, VK_RIGHT))
	{
		m_Lives = 1;
	}

	uint32_t state = reinterpret_cast<Character*>(GetParent())->GetState();

	if (state & StateBitfield::HasStartedIdle)
	{
		pAnimator->SetAnimation(L"Idle");
		pAnimator->Play();
	}
	else if (state & StateBitfield::HasStartedJump)
	{
		Logger::LogDebug(L"HasStartedJump");
		pAnimator->SetAnimation(L"Jump");
		SoundManager::Get()->GetSystem()->playSound(m_pJumpSoundEffect, nullptr, false, &m_pJumpSound);
		pAnimator->Play();

	}
	else if (state & StateBitfield::HasStartedMoving)
	{
		pAnimator->SetAnimation(L"Run");
		pAnimator->Play();
	}
}

void Mario::InitializeSounds()
{
	if (!m_pJumpSoundEffect)
	{

		SoundManager::Get()->GetSystem()->createStream("Resources/Sound/jump.mp3", FMOD_DEFAULT, nullptr, &m_pJumpSoundEffect);
		m_pJumpSoundEffect->setMode(FMOD_LOOP_OFF);
		m_pJumpSoundEffect->set3DMinMaxDistance(0.f, 100.f);
	}

	SoundManager::Get()->GetSystem()->playSound(m_pJumpSoundEffect, nullptr, true, &m_pJumpSound);
	m_pJumpSound->setVolume(0.1f);
}
