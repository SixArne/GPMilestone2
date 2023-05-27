#include "stdafx.h"
#include "Mario.h"
#include "Materials/DiffuseMaterial_Skinned.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow_Skinned.h"
#include "Prefabs/Character.h"


void Mario::SetStartPosition(XMFLOAT3 position)
{
	m_pCharacterController->GetTransform()->Translate(position);
}

XMFLOAT3 Mario::GetMarioLocation()
{
	return m_pCharacterController->GetTransform()->GetWorldPosition();
}

void Mario::Initialize(const SceneContext& /*sceneContext*/)
{


	//////////////////////////////////////////////////////////////////////////
	//							Character Controller.
	//////////////////////////////////////////////////////////////////////////
	m_pVisuals = new GameObject();
	m_pMarioModel = m_pVisuals->AddComponent(new ModelComponent(L"Meshes/mario.ovm"));
	m_pVisuals->GetTransform()->Scale(4, 4, 4);
	m_pVisuals->GetTransform()->Translate(0, -8, 0);

	//Character
	
	auto pDefaultMat = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.8f);
	

	CharacterDesc characterDesc{ pDefaultMat };
	characterDesc.actionId_MoveForward = CharacterMoveForward;
	characterDesc.actionId_MoveBackward = CharacterMoveBackward;
	characterDesc.actionId_MoveLeft = CharacterMoveLeft;
	characterDesc.actionId_MoveRight = CharacterMoveRight;
	characterDesc.actionId_Jump = CharacterJump;
	characterDesc.controller.height = 8.5f;
	characterDesc.controller.radius = 3.f;
	characterDesc.JumpSpeed = 80.f;
	characterDesc.maxMoveSpeed = 200.f;
	characterDesc.maxFallSpeed = 120.f;
	characterDesc.fallAccelerationTime = 0.8f;
	m_pCharacterController = AddChild(new Character(characterDesc));
	m_pCharacterController->AddChild(m_pVisuals);

	m_pCharacterController->SetTag(L"mario");
	m_pCharacterController->SetOnTriggerCallBack([=](GameObject* /*pTrigger*/, GameObject* /*pOther*/, PxTriggerAction /*action*/)
		{
			std::cout << "Triggered mario2" << std::endl;
		});

	/////////////////////////////////////////////////////////////////////////
	//							Collision detection
	//////////////////////////////////////////////////////////////////////////
	/*auto rigidBody = m_pVisuals->AddComponent(new RigidBodyComponent());
	auto pDefaultMat = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.8f);
	auto colliderId = rigidBody->AddCollider(PxCapsuleGeometry{ 4.f, 8.f }, *pDefaultMat);
	rigidBody->SetKinematic(true);

	auto colliderInfo = rigidBody->GetCollider(colliderId);
	colliderInfo.SetTrigger(true);

	this->SetOnTriggerCallBack([=](GameObject* pTrigger, GameObject* pOther, PxTriggerAction action)
		{
			OnCollision(pTrigger, pOther, action);
		});*/
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
	 //reinterpret_cast<Character*>(GetParent())->DrawImGui();
}


void Mario::Update(const SceneContext&)
{
	////////////////////////////////////////////////////////////
	//						Visual rotation
	////////////////////////////////////////////////////////////
	// Rotate the visuals only when moving
	XMFLOAT3 direction = m_pCharacterController->GetCurrentDirection();
	auto directionInDegrees = XMConvertToDegrees(atan2(direction.x, direction.z));
	m_pVisuals->GetTransform()->Rotate(0, directionInDegrees + 180.f, 0);


	uint32_t state = reinterpret_cast<Character*>(m_pCharacterController)->GetState();

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

void Mario::OnCollision(GameObject* /*pTriggerObject*/, GameObject* pOtherObject, PxTriggerAction action)
{
	if (action == PxTriggerAction::ENTER)
	{
		if (pOtherObject->GetTag() == L"bill")
		{
			m_Lives--;
			std::cout << "hit" << std::endl;
			std::cout << "lives left: " << m_Lives << std::endl;
		}
		else if (pOtherObject->GetTag() == L"coin")
		{
			std::cout << "got coin" << std::endl;
		}
		else if (pOtherObject->GetTag() == L"specialCoin")
		{
			std::cout << "got special coin" << std::endl;
		}
		else if (pOtherObject->GetTag() == L"moon")
		{
			std::cout << "got moon" << std::endl;
		}
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
