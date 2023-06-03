#include "stdafx.h"
#include "BanzaiBillLauncher.h"
#include "Materials/UberMaterial.h"
#include "Prefabs/Mario.h"
#include "Prefabs/Character.h"
#include "Prefabs/Enemies/BanzaiBill.h"
#include "Utils/TimerManager.h"

BanzaiBillLauncher::BanzaiBillLauncher(Mario* mario)
	: m_pMario(mario)
{
}

BanzaiBillLauncher::~BanzaiBillLauncher()
{
}

void BanzaiBillLauncher::Initialize(const SceneContext&)
{
	auto launcherModel = AddComponent(new ModelComponent(L"Meshes/Enemies/banzai_bill_launcher.ovm"));

	const auto pMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	pMaterial->SetDiffuseTexture(L"Textures/Enemies/KillerLauncherBody_alb.png");
	pMaterial->SetNormalTexture(L"Textures/Enemies/KillerLauncherBody_nrm.png");
	pMaterial->SetSpecularTexture(L"Textures/Enemies/KillerLauncherBody_rgh.png");

	launcherModel->SetMaterial(pMaterial, 0);
}

void BanzaiBillLauncher::Update(const SceneContext& sceneContext)
{
	if (!m_CanFire)
	{
		m_CurrentCooldown += sceneContext.pGameTime->GetElapsedReal();
		if (m_CurrentCooldown >= m_FireCooldown)
		{
			m_CurrentCooldown = 0.f;
			m_CanFire = true;
		}
	}


	XMVECTOR currentPos = XMLoadFloat3(&GetTransform()->GetWorldPosition());
	XMVECTOR marioPos = XMLoadFloat3(&m_pMario->GetCharacterController()->GetTransform()->GetWorldPosition());

	XMVECTOR distance  = XMVector3Length(marioPos - currentPos);

	XMFLOAT3 distanceFloat3{};
	XMStoreFloat3(&distanceFloat3, distance);

	if (m_CanFire && distanceFloat3.x <= m_MarioMinDistance)
	{
		m_CanFire = false;

		// spawn Banzai will just before launcher with forward
		auto bill = new BanzaiBill(m_pMario);

		std::cout << "creating bill" << std::endl;

		// Let itself know when it dies
		bill->SetOnDeathCallback([this, bill]() {
			m_BillDeathCallback(bill);
			});

		// Add to external list
		m_BillCreateCallback(bill);
	}

	// draw forward
	XMFLOAT3 forward = GetTransform()->GetForward();

	XMFLOAT3 begin = GetTransform()->GetWorldPosition();
	XMFLOAT3 end{
		 begin.x + forward.x * 10.f,
		 begin.y + forward.y * 10.f,
		 begin.z + forward.z * 10.f

	};

	DebugRenderer::DrawLine(begin, end, XMFLOAT4{ 0,1,0,1 });
}
