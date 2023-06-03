#include "stdafx.h"
#include "Moon.h"
#include "Materials/UberMaterial.h"
#include "Prefabs/Player/Mario.h"
#include "Prefabs/Player/Character.h"

void Moon::Initialize(const SceneContext&)
{
	auto visuals = AddComponent(new ModelComponent(L"Meshes/Collectibles/moon.ovm"));
	GetTransform()->Scale(4, 4, 4);

	const auto moonMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();

	moonMaterial->SetDiffuseTexture(L"Textures/Collectibles/moon_alb.png");
	moonMaterial->SetNormalTexture(L"Textures/Collectibles/moon_nrm.png");
	moonMaterial->SetSpecularTexture(L"Textures/Collectibles/moon_rgh.png");

	visuals->SetMaterial(moonMaterial, 0);

	auto pDefaultMat = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.8f);

	m_rb = AddComponent(new RigidBodyComponent());
	m_rb->SetKinematic(true);

	auto colliderID = m_rb->AddCollider(PxCapsuleGeometry{ 3,3 }, *pDefaultMat);
	auto collider = m_rb->GetCollider(colliderID);
	collider.SetTrigger(true);

	this->SetOnTriggerCallBack([=](GameObject* self, GameObject* pOther, PxTriggerAction action)
		{
			OnGrabMoon(self, pOther, action);
		});
}

void Moon::Update(const SceneContext& sceneContext)
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

void Moon::OnGrabMoon(GameObject*, GameObject* pOther, PxTriggerAction action)
{
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
			mario->TakeMoon();

			// Delay destruction until next frame
			m_MarkedForDestruction = true;
		}
	}
}
