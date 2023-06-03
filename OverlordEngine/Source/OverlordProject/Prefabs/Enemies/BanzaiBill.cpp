#include "stdafx.h"
#include "BanzaiBill.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Prefabs/Player/Mario.h"
#include "Prefabs/Player/Character.h"
#include "Scenes/EndProject/CapKingdom.h"

BanzaiBill::BanzaiBill(Mario* mario)
	:m_pMario{mario}
{
}

void BanzaiBill::Initialize(const SceneContext&)
{
	m_pVisuals = AddChild(new GameObject());
	m_pParticles = m_pVisuals->AddChild(new GameObject());
	auto bilModel = m_pVisuals->AddComponent(new ModelComponent(L"Meshes/Bill.ovm"));

	// Materials
	const auto pEyeMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pEyeMaterial->SetDiffuseTexture(L"Textures/Enemies/bill_eye_diffuse.png");

	const auto pBodyMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pBodyMaterial->SetDiffuseTexture(L"Textures/Enemies/bill_diffuse.png");

	bilModel->SetMaterial(pEyeMaterial, 0);
	bilModel->SetMaterial(pBodyMaterial, 0);

	// Particles
	ParticleEmitterSettings settings{};
	settings.velocity = { 0.f, 0.f, -100.f };
	settings.minSize = 1.f;
	settings.maxSize = 2.f;
	settings.minEnergy = 1.f;
	settings.maxEnergy = 2.f;
	settings.minScale = 3.5f;
	settings.maxScale = 5.5f;
	settings.minEmitterRadius = .2f;
	settings.maxEmitterRadius = .5f;
	settings.color = { 1.f,0.2f,0.2f, .6f };

	m_pEmitter = m_pParticles->AddComponent(new ParticleEmitterComponent(L"Textures/Enemies/Smoke.png", settings, 200));
	
	// Physics
	auto pDefaultMat = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.8f);
	m_pRigidBody = AddComponent(new RigidBodyComponent());
	auto colliderId = m_pRigidBody->AddCollider(PxCapsuleGeometry{ 5, 1 }, *pDefaultMat);
	auto colliderInfo = m_pRigidBody->GetCollider(colliderId);
	colliderInfo.SetTrigger(true);

	this->SetOnTriggerCallBack([=](GameObject* pTrigger, GameObject* pOther, PxTriggerAction action)
		{
			OnCollision(pTrigger, pOther, action);
		});

	m_pParticles->GetTransform()->Translate(0, 0, -10);
	GetTransform()->Rotate(0, 0, 0);

	auto scene = reinterpret_cast<CapKingdom*>(GetScene());
	scene->AddBulletBill(this);

	SetTag(L"bill");
}

void BanzaiBill::Update(const SceneContext& ctx)
{
	if (IsMarioClose())
	{
		MoveTowardsMario(ctx.pGameTime->GetElapsed());
		RotateTowardsMario();
	}

	if (m_MarkedForDestruction)
	{
		m_pRigidBody->PutToSleep();

		// Let scene clean up references
		auto scene = reinterpret_cast<CapKingdom*>(GetScene());

		scene->RemoveBulletBill(this);
		scene->RemoveChild(this, true);
	}
}

void BanzaiBill::OnCollision(GameObject* /*pTrigger*/, GameObject* pOther, PxTriggerAction action)
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
			
			if (character != nullptr)
			{
				auto mario = reinterpret_cast<Mario*>(character->GetOwningPrefab());

				if (mario != nullptr)
				{
					mario->TakeDamage();
				}
			}

		}

		// Mark for destruction
		m_MarkedForDestruction = true;
	}
}

XMFLOAT3 BanzaiBill::GetDirectionToMario()
{
	auto marioPosition = m_pMario->GetMarioLocation();
	auto billPosition = GetTransform()->GetWorldPosition();

	XMVECTOR marioPositionVector = XMLoadFloat3(&marioPosition);
	XMVECTOR billPositionVector = XMLoadFloat3(&billPosition);

	auto deltaVector = marioPositionVector - billPositionVector;

	XMFLOAT3 delta{ };
	XMStoreFloat3(&delta, XMVector3Normalize(deltaVector));

	return delta;
}

void BanzaiBill::RotateTowardsMario()
{
	auto direction = GetDirectionToMario();
	auto forward = GetTransform()->GetForward();

	auto direction2D = XMFLOAT2{ direction.x, direction.z };
	auto forward2D = XMFLOAT2{ forward.x, forward.z };

	auto direction2DVector = XMLoadFloat2(&direction2D);
	auto forward2DVector = XMLoadFloat2(&forward2D);

	// Determine side for rotation
	float side{};
	XMStoreFloat(&side, XMVector2Cross(direction2DVector, forward2DVector));
	
	// Get angle
	auto resultVector = XMVector2AngleBetweenVectors(XMLoadFloat2(&direction2D), XMLoadFloat2(&forward2D));
	
	XMFLOAT2 result{};
	XMStoreFloat2(&result, resultVector);
	float angle = XMConvertToDegrees(result.x);
	
	if (side >= 0)
	{
		m_pVisuals->GetTransform()->Rotate(0, angle, 0);
	}
	else
	{
		m_pVisuals->GetTransform()->Rotate(0, -angle, 0);

	}

	// Change emitter velocity based on rotation
	m_pEmitter->GetSettings().velocity = XMFLOAT3{-direction2D.x * 100, 0, -direction2D.y * 100};
}


void BanzaiBill::MoveTowardsMario(float dt)
{
	auto direction = GetDirectionToMario();
	auto movement = XMLoadFloat3(&direction) * m_MoveSpeed * dt;
	
	GetTransform()->Move(movement);
}

bool BanzaiBill::IsMarioClose()
{
	return true;
}

RigidBodyComponent* BanzaiBill::GetRigidBody()
{
	return m_pRigidBody;
}

void BanzaiBill::SetMarioRef(Mario* marioRef)
{
	m_pMario = marioRef;
}