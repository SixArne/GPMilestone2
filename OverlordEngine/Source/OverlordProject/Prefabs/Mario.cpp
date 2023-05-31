#include "stdafx.h"
#include "Mario.h"
#include "Materials/DiffuseMaterial_Skinned.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow_Skinned.h"
#include "Prefabs/Character.h"


void Mario::TakeDamage()
{
	if (m_IsDead) return;

	std::cout << "Mario Lives left: " << m_Lives << std::endl;

	--m_Lives;
	m_Lives = std::clamp(m_Lives, 0, 4);

	if (m_Lives >= 1)
	{
		m_IsHurt = true;
	}

	if (m_Lives > 1)
	{
		FMOD::Channel* channel = NULL;
		SoundManager::Get()->GetSystem()->playSound(m_pDamageSoundEffect, nullptr, false, &channel);
		if (channel != nullptr)
		{
			channel->setVolume(0.5f);
		}

	}
	else if (m_Lives == 1)
	{
		FMOD::Channel* channel = NULL;
		SoundManager::Get()->GetSystem()->playSound(m_pLastLifeSoundEffect, nullptr, false, &channel);
		if (channel != nullptr)
		{
			channel->setVolume(0.5f);
		}

	}
	else if (m_Lives == 0)
	{
		FMOD::Channel* channel = NULL;
		SoundManager::Get()->GetSystem()->playSound(m_pDeathSoundEffect, nullptr, false, &channel);

		if (channel != nullptr)
		{
			channel->setVolume(0.5f);
		}


		m_IsDead = true;
		m_HasRecentlyDied = true;
		m_OnDieCallback();
	}

	std::cout << "Mario took damage! Lives left: " << m_Lives << std::endl;
}

void Mario::TakeMoon()
{
	m_Moons++;
}

void Mario::TakeCoin()
{
	m_Coins++;

	FMOD::Channel* channel = NULL;
	SoundManager::Get()->GetSystem()->playSound(m_pCoinSoundEffect, nullptr, false, &channel);
	if (channel)
	{
		channel->setVolume(0.1f);
	}
}

void Mario::TakeSpecialCoin()
{
	m_SpecialCoins++;
}

void Mario::SetStartPosition(XMFLOAT3 position)
{
	m_pCharacterController->GetTransform()->Translate(position);
}

XMFLOAT3 Mario::GetMarioLocation()
{
	return m_pCharacterController->GetTransform()->GetWorldPosition();
}

Character* Mario::GetCharacterController()
{
	return m_pCharacterController;
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
	m_pCharacterController->SetVisuals(m_pVisuals);
	m_pCharacterController->SetOwningPrefab(this);
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

Mario::Mario()
{
	std::cout << "constructing mario" << std::endl;
}

void Mario::DrawImGui()
{
	 //reinterpret_cast<Character*>(GetParent())->DrawImGui();
}

int Mario::GetLives()
{
	return m_Lives;
}

int Mario::GetCoins()
{
	return m_Coins;
}

int Mario::GetSpecialCoins()
{
	return m_SpecialCoins;
}

int Mario::GetMoons()
{
	return m_Moons;
}


void Mario::Update(const SceneContext& sceneContext)
{
	if (m_IsDead && !m_HasRecentlyDied) return;

	////////////////////////////////////////////////////////////
	//						Visual rotation
	////////////////////////////////////////////////////////////
	// Rotate the visuals only when moving
	XMFLOAT3 direction = m_pCharacterController->GetCurrentDirection();
	auto directionInDegrees = XMConvertToDegrees(atan2(direction.x, direction.z));
	m_pVisuals->GetTransform()->Rotate(0, directionInDegrees + 180.f, 0);


	uint32_t state = reinterpret_cast<Character*>(m_pCharacterController)->GetState();

	if (m_HasInvincibilityFrames)
	{
		m_InvincibilityTimer += sceneContext.pGameTime->GetElapsed();

		if (m_InvincibilityTimer >= m_InvincibilityTime)
		{
			m_HasInvincibilityFrames = false;
			m_WasHurt = true;
			m_InvincibilityTimer = 0.f;
		}
	}

	if (m_IsHurt)
	{
		pAnimator->SetAnimation(L"Hurt");
		pAnimator->Play();

		m_IsHurt = false;
		m_HasInvincibilityFrames = true;
	}
	else if (m_Lives == 0 && m_HasRecentlyDied)
	{
		pAnimator->SetAnimation(L"Death");
		pAnimator->Play();

		m_HasRecentlyDied = false;
	}
	else if (state & StateBitfield::HasStartedIdle || m_WasHurt)
	{
		pAnimator->SetAnimation(L"Idle");
		pAnimator->Play();

		m_WasHurt = false;
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

void Mario::OnCollision(GameObject* /*pTriggerObject*/, GameObject* /*pOtherObject*/, PxTriggerAction /*action*/)
{
	/*if (action == PxTriggerAction::ENTER)
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
	}*/
}

void Mario::InitializeSounds()
{
	if (!m_pJumpSoundEffect)
	{

		SoundManager::Get()->GetSystem()->createStream("Resources/Sound/jump.wav", FMOD_DEFAULT, nullptr, &m_pJumpSoundEffect);
		m_pJumpSoundEffect->setMode(FMOD_LOOP_OFF);

	}

	if (!m_pDamageSoundEffect)
	{

		SoundManager::Get()->GetSystem()->createStream("Resources/Sound/damage.wav", FMOD_DEFAULT, nullptr, &m_pDamageSoundEffect);
		m_pDamageSoundEffect->setMode(FMOD_LOOP_OFF);
	}

	if (!m_pLastLifeSoundEffect)
	{

		SoundManager::Get()->GetSystem()->createStream("Resources/Sound/last_life.wav", FMOD_DEFAULT, nullptr, &m_pLastLifeSoundEffect);
		m_pLastLifeSoundEffect->setMode(FMOD_LOOP_OFF);
	}

	if (!m_pDeathSoundEffect)
	{

		SoundManager::Get()->GetSystem()->createStream("Resources/Sound/die.wav", FMOD_DEFAULT, nullptr, &m_pDeathSoundEffect);
		m_pDeathSoundEffect->setMode(FMOD_LOOP_OFF);


	}

	if (!m_pCoinSoundEffect)
	{

		SoundManager::Get()->GetSystem()->createStream("Resources/Sound/coin.mp3", FMOD_DEFAULT, nullptr, &m_pCoinSoundEffect);
		m_pCoinSoundEffect->setMode(FMOD_LOOP_OFF);

	}


	SoundManager::Get()->GetSystem()->playSound(m_pJumpSoundEffect, nullptr, true, &m_pJumpSound);
	m_pJumpSound->setVolume(0.1f);
}
