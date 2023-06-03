#include "stdafx.h"
#include "Coin.h"
#include "Materials/UberMaterial.h"
#include "Prefabs/Player/Mario.h"
#include "Prefabs/Player/Character.h"

void Coin::Initialize(const SceneContext&)
{
	auto visuals = AddComponent(new ModelComponent(L"Meshes/Collectibles/coin.ovm"));
	GetTransform()->Scale(2, 2, 2);

	const auto coinMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	coinMaterial->SetDiffuseTexture(L"Textures/Collectibles/coin_alb.png");
	coinMaterial->SetNormalTexture(L"Textures/Collectibles/coin_nrm.png");
	coinMaterial->SetSpecularTexture(L"Textures/Collectibles/coin.png");

	visuals->SetMaterial(coinMaterial, 0);
	
	auto pDefaultMat = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.8f);
	
	m_rb = AddComponent(new RigidBodyComponent());
	m_rb->SetKinematic(true);

	auto colliderID = m_rb->AddCollider(PxCapsuleGeometry{3,3}, *pDefaultMat);
	auto collider = m_rb->GetCollider(colliderID);
	collider.SetTrigger(true);

	this->SetOnTriggerCallBack([=](GameObject* self , GameObject* pOther, PxTriggerAction action)
		{
			OnGrabCoin(self, pOther, action);
		});
}

void Coin::Update(const SceneContext& sceneContext)
{
	m_Angle += sceneContext.pGameTime->GetElapsed() * 100;
	m_Oscillator += sceneContext.pGameTime->GetElapsed();

	GetTransform()->Move(0, sin(m_Oscillator) * .01f, 0);
	GetTransform()->Rotate(0, m_Angle, 0);

	if (m_MarkedForDestruction)
	{
		GetScene()->RemoveChild(this, true);
	}
}

void Coin::OnGrabCoin(GameObject*, GameObject* pOther, PxTriggerAction action)
{
	// Ignore first call
	if (!m_HasCallbackTriggered)
	{
		m_HasCallbackTriggered = true;
		return;
	}

	if (action == PxTriggerAction::ENTER)
	{
		if (pOther->GetTag() == L"mario")
		{
			auto character = reinterpret_cast<Character*>(pOther);
			auto mario = reinterpret_cast<Mario*>(character->GetOwningPrefab());
			mario->TakeCoin();

			// We can't delete it here so we delay until the next frame
			m_MarkedForDestruction = true;
		}
	}
}
