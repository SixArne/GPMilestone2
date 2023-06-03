#include "stdafx.h"
#include "BanzaiBillLauncher.h"
#include "Materials/UberMaterial.h"
#include "Prefabs/Player/Mario.h"
#include "Prefabs/Player/Character.h"
#include "Prefabs/Enemies/BanzaiBill.h"
#include "Utils/TimerManager.h"

BanzaiBillLauncher::BanzaiBillLauncher(Mario* mario)
	: m_pMario(mario)
{
}

void BanzaiBillLauncher::Initialize(const SceneContext&)
{
	auto launcherModel = AddComponent(new ModelComponent(L"Meshes/Enemies/banzai_bill_launcher.ovm"));

	// Load materials
	const auto pMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	pMaterial->SetDiffuseTexture(L"Textures/Enemies/KillerLauncherBody_alb.png");
	pMaterial->SetNormalTexture(L"Textures/Enemies/KillerLauncherBody_nrm.png");
	pMaterial->SetSpecularTexture(L"Textures/Enemies/KillerLauncherBody_rgh.png");

	launcherModel->SetMaterial(pMaterial, 0);
}

void BanzaiBillLauncher::Update(const SceneContext& sceneContext)
{
	// Fire cooldown
	if (!m_CanFire)
	{
		m_CurrentCooldown += sceneContext.pGameTime->GetElapsed();
		if (m_CurrentCooldown >= m_FireCooldown)
		{
			m_CurrentCooldown = 0.f;
			m_CanFire = true;
		}
	}

	if (m_CanFire  && !m_pMario->IsDead())
	{
		// Get distance launcher -> mario
		XMVECTOR currentPos = XMLoadFloat3(&GetTransform()->GetWorldPosition());
		XMVECTOR marioPos = XMLoadFloat3(&m_pMario->GetCharacterController()->GetTransform()->GetWorldPosition());

		XMVECTOR distance = XMVector3Length(marioPos - currentPos);

		XMFLOAT3 distanceFloat3{};
		XMStoreFloat3(&distanceFloat3, distance);

		if (distanceFloat3.x <= m_MarioMinDistance)
		{
			m_CanFire = false;

			InstantiateBulletBill();
		}
	}
}

void BanzaiBillLauncher::InstantiateBulletBill()
{
	auto bill = GetScene()->AddChild(new BanzaiBill(m_pMario));
	

	XMFLOAT3 forward = bill->GetTransform()->GetForward();

	auto spawnerPos = GetTransform()->GetWorldPosition();

	bill->GetTransform()->Translate(
		spawnerPos.x + forward.x * m_SpawnDistance,
		spawnerPos.y + forward.y * m_SpawnDistance,
		spawnerPos.z + forward.z * m_SpawnDistance
	);
}
