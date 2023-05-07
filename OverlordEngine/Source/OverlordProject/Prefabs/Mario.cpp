#include "stdafx.h"
#include "Mario.h"
#include "Materials/DiffuseMaterial_Skinned.h"
#include "Prefabs/Character.h"


void Mario::Initialize(const SceneContext& sceneContext)
{
	

	//////////////////////////////////////////////////////////////////////////
	//							Character Controller.
	//////////////////////////////////////////////////////////////////////////
	auto marioObject = new GameObject();
	auto marioMesh = marioObject->AddComponent(new ModelComponent(L"Meshes/mario.ovm"));

	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	//Character
	CharacterDesc characterDesc{ pDefaultMaterial, 1.f, 5.f };
	characterDesc.actionId_MoveForward = CharacterMoveForward;
	characterDesc.actionId_MoveBackward = CharacterMoveBackward;
	characterDesc.actionId_MoveLeft = CharacterMoveLeft;
	characterDesc.actionId_MoveRight = CharacterMoveRight;
	characterDesc.actionId_Jump = CharacterJump;

	m_pCharacter = AddChild(new Character(characterDesc, marioObject));
	m_pCharacter->GetTransform()->Translate(0.f, 20.f, 0.f);

	//Input
	auto inputAction = InputAction(CharacterMoveLeft, InputState::down, 'A');
	sceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveRight, InputState::down, 'D');
	sceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveForward, InputState::down, 'W');
	sceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveBackward, InputState::down, 'S');
	sceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterJump, InputState::pressed, VK_SPACE, -1, XINPUT_GAMEPAD_A);
	sceneContext.pInput->AddInputAction(inputAction);

	//////////////////////////////////////////////////////////////////////////
	//							Mario model and textures.
	//////////////////////////////////////////////////////////////////////////

	const auto pBodyMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
	pBodyMaterial->SetDiffuseTexture(L"Textures/mariobodyfix_alb.png");

	const auto pFaceMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
	pFaceMaterial->SetDiffuseTexture(L"Textures/marioface_alb.png");

	const auto pHandsMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
	pHandsMaterial->SetDiffuseTexture(L"Textures/mariohandnew_alb.png");

	const auto pShoeMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
	pShoeMaterial->SetDiffuseTexture(L"Textures/marioshoes_alb.png");

	const auto pEyeMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
	pEyeMaterial->SetDiffuseTexture(L"Textures/marioeye_alb.png");



	marioMesh->SetMaterial(pBodyMaterial, 0);
	marioMesh->SetMaterial(pBodyMaterial, 1);
	marioMesh->SetMaterial(pBodyMaterial, 2);
	marioMesh->SetMaterial(pFaceMaterial, 3);
	marioMesh->SetMaterial(pHandsMaterial, 4);
	marioMesh->SetMaterial(pBodyMaterial, 5);
	marioMesh->SetMaterial(pFaceMaterial, 6);
	marioMesh->SetMaterial(pHandsMaterial, 7);
	marioMesh->SetMaterial(pShoeMaterial, 8);
	marioMesh->SetMaterial(pEyeMaterial, 9);

	// get half height from character capsule
	marioObject->GetTransform()->Translate(0, -(characterDesc.controller.height / 2), 0);

	//////////////////////////////////////////////////////////////////////////
	//							Mario animations
	//////////////////////////////////////////////////////////////////////////
	pAnimator = marioMesh->GetAnimator();
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
	m_pCharacter->DrawImGui();
}

void Mario::Update(const SceneContext&)
{
	uint32_t state = m_pCharacter->GetState();

	if (state & StateBitfield::HasStartedIdle)
	{
		pAnimator->SetAnimation(L"Idle");
		pAnimator->Play();
	}
	else if (state & StateBitfield::HasStartedJump)
	{
		Logger::LogDebug(L"HasStartedJump");
		pAnimator->SetAnimation(L"Jump");
		pAnimator->Play();

	}
	else if (state & StateBitfield::HasStartedMoving)
	{
		pAnimator->SetAnimation(L"Run");
		pAnimator->Play();
	}
}
