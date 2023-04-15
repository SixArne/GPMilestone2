#include "stdafx.h"
#include "TriggerScene.h"
#include "SpherePosColorNorm.h"

#include "Logger.h"

void TriggerScene::Initialize()
{

	EnablePhysxDebugRendering(true);


	PxPhysics* pPhysX = PhysxManager::GetInstance()->GetPhysics();

	CreateBall(pPhysX);
	CreatePlane(pPhysX);
	CreateTriggers(pPhysX);


	// Input
	m_SceneContext.GetInput()->AddInputAction(
		InputAction{ InputIDs::MoveLeft, InputTriggerState::down, VK_LEFT, -1, XINPUT_GAMEPAD_DPAD_LEFT }
	);

	m_SceneContext.GetInput()->AddInputAction(
		InputAction{ InputIDs::MoveRight, InputTriggerState::down, VK_RIGHT, -1, XINPUT_GAMEPAD_DPAD_RIGHT }
	);

	m_SceneContext.GetInput()->AddInputAction(
		InputAction{ InputIDs::MoveForward, InputTriggerState::down, VK_UP, -1, XINPUT_GAMEPAD_DPAD_UP }
	);

	m_SceneContext.GetInput()->AddInputAction(
		InputAction{ InputIDs::MoveBackward, InputTriggerState::down, VK_DOWN, -1, XINPUT_GAMEPAD_DPAD_DOWN }
	);

	m_SceneContext.GetInput()->AddInputAction(
		InputAction{ InputIDs::Reset, InputTriggerState::down, 0x52, -1, XINPUT_GAMEPAD_B }
	);
}

void TriggerScene::Update()
{
	bool bHasInputOnBall{ false };
	PxVec3 outTorque{};

	const float torqueValue{ 100000.f };

	if (m_SceneContext.GetInput()->IsActionTriggered(InputIDs::MoveForward))
	{
		XMFLOAT3 force{ torqueValue, 0, 0 };
		CalculateTorque(force, outTorque);

		bHasInputOnBall = true;
	}
	else if (m_SceneContext.GetInput()->IsActionTriggered(InputIDs::MoveBackward))
	{
		XMFLOAT3 force{ -torqueValue, 0, 0 };
		CalculateTorque(force, outTorque);

		bHasInputOnBall = true;
	}

	if (m_SceneContext.GetInput()->IsActionTriggered(InputIDs::MoveRight))
	{
		XMFLOAT3 force{ 0, 0, torqueValue };
		CalculateTorque(force, outTorque);

		bHasInputOnBall = true;
	}
	else if (m_SceneContext.GetInput()->IsActionTriggered(InputIDs::MoveLeft))
	{
		XMFLOAT3 force{ 0, 0, -torqueValue };
		CalculateTorque(force, outTorque);

		bHasInputOnBall = true;
	}

	if (bHasInputOnBall)
	{
		static_cast<PxRigidDynamic*>(m_Sphere->GetRigidActor())->addTorque(outTorque);

	}
}

void TriggerScene::Draw() const
{
}

void TriggerScene::OnSceneActivated()
{
}

void TriggerScene::OnSceneDeactivated()
{
}

void TriggerScene::CreateBall(PxPhysics* ph)
{
	// sphere
	const float size = 1.f;

	m_Sphere = new SpherePosColorNorm{ size, 10, 10, XMFLOAT4{1, 0.5, 0, 1} };
	AddGameObject(m_Sphere);

	m_SpherePhysicsActor = ph->createRigidDynamic(PxTransform(PxIdentity));
	m_SpherePhysicsActor->setMass(100);

	PxMaterial* pSphereMaterial = ph->createMaterial(0.5, 1, 0.05);
	PxSphereGeometry sphereGeometry = PxSphereGeometry{ PxReal{size} };
	PxRigidActorExt::createExclusiveShape(*m_SpherePhysicsActor, sphereGeometry, &pSphereMaterial, 1);

	m_Sphere->AttachRigidActor(m_SpherePhysicsActor);
	m_Sphere->Translate(0, 5, -10);
}

void TriggerScene::CreatePlane(PxPhysics* ph)
{

	PxMaterial* pGroundMaterial = ph->createMaterial(0.5, 1, 0.1);
	PxRigidStatic* pGroundActor = ph->createRigidStatic(PxTransform{ PxQuat{PxPiDivTwo, PxVec3{0,0,1}} });
	PxRigidActorExt::createExclusiveShape(*pGroundActor, PxPlaneGeometry{}, &pGroundMaterial, 1);
	m_pPhysxScene->addActor(*pGroundActor);
}

void TriggerScene::CreateTriggers(PxPhysics* ph)
{
	PxMaterial* pTriggerMaterial = ph->createMaterial(1, 1, 0.05);

	m_pTriggerLeft = ph->createRigidStatic(PxTransform{ -8, 0.5, 0 });
	auto pShape = PxRigidActorExt::createExclusiveShape(*m_pTriggerLeft, PxBoxGeometry{ 0.5, 0.5, 0.5 }, *pTriggerMaterial);

	pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

	m_pPhysxScene->addActor(*m_pTriggerLeft);



	m_pTriggerRight = ph->createRigidStatic(PxTransform{ 8, 0.5, 0 });
	pShape = PxRigidActorExt::createExclusiveShape(*m_pTriggerRight, PxBoxGeometry{ 0.5, 0.5, 0.5 }, *pTriggerMaterial);

	pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

	m_pPhysxScene->addActor(*m_pTriggerRight);
}

void TriggerScene::CalculateTorque(XMFLOAT3& force, PxVec3& outTorque) const
{
	// Get the camera forward, right, and up directions
	const XMFLOAT3 cameraForward = m_SceneContext.GetCamera()->GetForward();
	const XMFLOAT3 cameraRight = m_SceneContext.GetCamera()->GetRight();
	const XMFLOAT3 cameraUp = m_SceneContext.GetCamera()->GetUp();

	// Calculate the global torque in PhysX coordinates
	const XMVECTOR directionTorque = XMVectorSet(force.x, 0.0f, force.z, 0.0f);
	//const XMMATRIX cameraToWorld = XMMatrixSet(
	//	cameraRight.x, cameraRight.y, cameraRight.z, 0.0f,
	//	cameraUp.x, cameraUp.y, cameraUp.z, 0.0f,
	//	-cameraForward.x, -cameraForward.y, -cameraForward.z, 0.0f,
	//	0.0f, 0.0f, 0.0f, 1.0f);

	// Transform the local torque to the camera world direction.
	const XMVECTOR torqueGlobal = XMVector3TransformNormal(directionTorque, XMLoadFloat4x4(&GetSceneContext().GetCamera()->GetView()));

	XMFLOAT3 globalTorque{};
	XMStoreFloat3(&globalTorque, torqueGlobal);

	// Apply the global torque to the sphere's rigid body
	outTorque = { globalTorque.x, 0, -globalTorque.z };
}

void TriggerScene::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	for (auto i{0}; i < count; ++i)
	{
		// ignore removed actors
		if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_OTHER | PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER))
			continue;

		const PxTriggerPair& pair = pairs[i];
		
		if (pair.triggerActor == m_pTriggerLeft)
		{
			if (pair.status == PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				Logger::GetInstance()->LogInfo(L"Left trigger enter");
			}
			else if (pair.status == PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				Logger::GetInstance()->LogInfo(L"Left trigger exit");

			}
		}
	}
}
