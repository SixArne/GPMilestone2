#include "stdafx.h"
#include "W2_AssignmentScene.h"
#include "MeshObject.h"
#include "ContentManager.h"
#include "CubePosColorNorm.h"
#include "SpherePosColorNorm.h"
#include "SoundManager.h"

W2_AssignmentScene::W2_AssignmentScene()
	: GameScene(L"W2_AssignmentScene")
{
}

W2_AssignmentScene::~W2_AssignmentScene()
{
}

void W2_AssignmentScene::Initialize()
{
	EnablePhysxDebugRendering(true);

	auto Physx = PhysxManager::GetInstance()->GetPhysics();

	auto pSceneMesh = CreateTriangleMesh(L"Resources/Collision/Level.ovm", L"Resources/Collision/Level.ovpt");
	
	
	auto triggerShape = PxBoxGeometry{ 1, 1, 2 };
	auto cubeShape = PxBoxGeometry{ 1,1,1 };
	auto hatchShape = PxBoxGeometry{ 1, 0.25, 3 };

	auto blueTriggerColor = XMFLOAT4{ 0,0,1,1 };
	auto redTriggerColor = XMFLOAT4{ 1,0,0,1 };

	auto blueSideBall = CreateBall();
	auto redSideBall = CreateBall();


	
	m_pBlueTrigger = CreateTrigger(triggerShape, blueTriggerColor);
	m_pRedTrigger = CreateTrigger(triggerShape, redTriggerColor);

	m_pBlueCube = CreateCube(cubeShape, blueTriggerColor);
	m_pRedCube = CreateCube(cubeShape, redTriggerColor);

	m_pBlueHatch = CreateHatch(hatchShape, blueTriggerColor);
	m_pRedHatch = CreateHatch(hatchShape, redTriggerColor);

	m_pControllableBall = CreateBall();
	m_pControllableBall->Translate(0, 5, 0);

	m_pBlueTrigger->Translate(-7.5, 2, 0);
	m_pRedTrigger->Translate(7, 2, 0);

	int hatchHeight = 17;
	int hatchXOffset = 9;
	m_pBlueHatch->Translate(-hatchXOffset, hatchHeight, 0);
	m_pRedHatch->Translate(hatchXOffset, hatchHeight, 0);

	blueSideBall->Translate(-hatchXOffset, hatchHeight + 2, 0);
	redSideBall->Translate(hatchXOffset, hatchHeight + 2, 0);

	m_pBlueCube->Translate(-5, 5, 0);
	m_pRedCube->Translate(5, 5, 0);

	AddGameObject(m_pControllableBall);
	
	AddGameObject(m_pBlueTrigger);
	AddGameObject(m_pBlueCube);

	AddGameObject(m_pBlueHatch);
	AddGameObject(m_pRedHatch);

	AddGameObject(m_pRedTrigger);
	AddGameObject(m_pRedCube);
	
	AddGameObject(blueSideBall);
	AddGameObject(redSideBall);

	AddGameObject(pSceneMesh);

	m_pBlueHatchJoint = PxRevoluteJointCreate(*Physx, 
		m_pBlueHatch->GetRigidActor(), PxTransform{1,0,0}, 
		pSceneMesh->GetRigidActor(), PxTransform{1,1,1}
	);
	m_pBlueHatchJoint->setDriveVelocity(10.0f);
	m_pBlueHatchJoint->setRevoluteJointFlag(PxRevoluteJointFlag::eDRIVE_ENABLED, true);

	// Setup input

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

	SetupTriggerEnterSound();
}

MeshObject* W2_AssignmentScene::CreateTriangleMesh(const std::wstring& meshSource, const std::wstring& collisionSource)
{
	auto Physx = PhysxManager::GetInstance()->GetPhysics();

	auto mesh = new MeshObject(meshSource);
	auto pTriangleMesh = ContentManager::GetInstance()->Load<PxTriangleMesh>(collisionSource);

	auto pMeshPhysicsActor = Physx->createRigidDynamic(PxTransform{ PxIdentity });
	auto pMeshPhysicsMaterial = Physx->createMaterial(0.5, 0.5, 0);
	
	pMeshPhysicsActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
	PxRigidActorExt::createExclusiveShape(*pMeshPhysicsActor, PxTriangleMeshGeometry{ pTriangleMesh }, &pMeshPhysicsMaterial, 1);

	mesh->AttachRigidActor(pMeshPhysicsActor);

	return mesh;
}

GameObject* W2_AssignmentScene::CreateTrigger(const PxBoxGeometry& shape, const XMFLOAT4& color)
{
	auto Physx = PhysxManager::GetInstance()->GetPhysics();

	auto gameObject = new CubePosColorNorm{ 
		shape.halfExtents.x * 2, 
		shape.halfExtents.y * 2, 
		shape.halfExtents.z * 2,
		color
	};

	PxMaterial* pTriggerMaterial = Physx->createMaterial(0.5, 0.5, 0);

	auto pTriggerPhysxActor = Physx->createRigidStatic(PxTransform{ PxIdentity});
	auto pShape = PxRigidActorExt::createExclusiveShape(*pTriggerPhysxActor, shape, *pTriggerMaterial);

	pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

	gameObject->AttachRigidActor(pTriggerPhysxActor);

	return gameObject;
}

GameObject* W2_AssignmentScene::CreateBall()
{
	// sphere
	const float size = 1.f;
	auto Physx = PhysxManager::GetInstance()->GetPhysics();

	auto pSphere = new SpherePosColorNorm{ size, 10, 10, XMFLOAT4{1, 0.5, 0, 1} };

	auto pSpherePhysicsActor = Physx->createRigidDynamic(PxTransform(PxIdentity));
	pSpherePhysicsActor->setMass(100);

	PxMaterial* pSphereMaterial = Physx->createMaterial(0.5, 1, 0.05);
	PxSphereGeometry sphereGeometry = PxSphereGeometry{ PxReal{size} };
	PxRigidActorExt::createExclusiveShape(*pSpherePhysicsActor, sphereGeometry, &pSphereMaterial, 1);

	pSphere->AttachRigidActor(pSpherePhysicsActor);
	
	return pSphere;
}

GameObject* W2_AssignmentScene::CreateCube(const PxBoxGeometry& shape, const XMFLOAT4& color)
{
	auto Physx = PhysxManager::GetInstance()->GetPhysics();

	auto gameObject = new CubePosColorNorm{
		shape.halfExtents.x * 2,
		shape.halfExtents.y * 2,
		shape.halfExtents.z * 2,
		color
	};

	PxMaterial* pCubeMaterial = Physx->createMaterial(0.5, 0.5, 0);

	auto pCubePhysxActor = Physx->createRigidDynamic(PxTransform{ PxIdentity });
	auto pShape = PxRigidActorExt::createExclusiveShape(*pCubePhysxActor, shape, *pCubeMaterial);
	
	auto centerOfMass = PxVec3{ 0,0,0 };
	PxRigidBodyExt::setMassAndUpdateInertia(*pCubePhysxActor, 10.f, &centerOfMass);

	gameObject->AttachRigidActor(pCubePhysxActor);

	return gameObject;
}

void W2_AssignmentScene::Update()
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

	if (m_SceneContext.GetInput()->IsActionTriggered(InputIDs::Reset))
	{
		ResetScene();
	}

	if (bHasInputOnBall)
	{
		static_cast<PxRigidDynamic*>(m_pControllableBall->GetRigidActor())->addTorque(outTorque);
	}
}

void W2_AssignmentScene::ResetScene()
{
	for (auto obj : m_GameObjects)
	{
		if (obj->GetRigidActor() != nullptr)
		{
			auto val = obj->GetRigidActor();
			m_pPhysxScene->removeActor(*val);
		}

		SafeDelete(obj);
	}

	m_GameObjects.clear();

	Initialize();
}

void W2_AssignmentScene::CalculateTorque(XMFLOAT3& force, PxVec3& outTorque) const
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

void W2_AssignmentScene::SetupTriggerEnterSound()
{
	auto pFmod = SoundManager::GetInstance()->GetSystem();

	FMOD_RESULT result = pFmod->createStream("Resources/Sounds/bell.mp3", FMOD_DEFAULT, nullptr, &m_pBellSound);
	SoundManager::GetInstance()->ErrorCheck(result);
}

void W2_AssignmentScene::PlaySound(FMOD::Sound& sound)
{
	auto pFmod = SoundManager::GetInstance()->GetSystem();

	FMOD_RESULT result = pFmod->playSound(m_pBellSound, nullptr, false, &m_pTriggerChannel2D);
	SoundManager::GetInstance()->ErrorCheck(result);
}

GameObject* W2_AssignmentScene::CreateHatch(const PxBoxGeometry& shape, const XMFLOAT4& color)
{
	auto Physx = PhysxManager::GetInstance()->GetPhysics();

	auto gameObject = new CubePosColorNorm{
		shape.halfExtents.x * 2,
		shape.halfExtents.y * 2,
		shape.halfExtents.z * 2,
		color
	};

	PxMaterial* pCubeMaterial = Physx->createMaterial(0.5, 0.5, 0);

	auto pCubePhysxActor = Physx->createRigidDynamic(PxTransform{ PxIdentity });
	auto pShape = PxRigidActorExt::createExclusiveShape(*pCubePhysxActor, shape, *pCubeMaterial);
	pCubePhysxActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	auto centerOfMass = PxVec3{ 0,0,0 };
	PxRigidBodyExt::setMassAndUpdateInertia(*pCubePhysxActor, 10.f, &centerOfMass);

	gameObject->AttachRigidActor(pCubePhysxActor);

	return gameObject;
}


void W2_AssignmentScene::Draw() const
{

}

void W2_AssignmentScene::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	for (auto i{ 0 }; i < count; i++)
	{
		// ignore removed actors
		if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_OTHER | PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER))
			continue;

		const PxTriggerPair& pair = pairs[i];

		if (pair.triggerActor == m_pBlueTrigger->GetRigidActor() && pair.otherActor == m_pBlueCube->GetRigidActor())
		{
			if (pair.status == PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				Logger::GetInstance()->LogInfo(L"Blue trigger entered");

				m_pBlueHatch->RotateDegrees(0, 0, 90);
				m_pBlueHatch->Translate(-1, 0, 0);

				PlaySound(*m_pBellSound);
			}
			else if (pair.status == PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				Logger::GetInstance()->LogInfo(L"Blue trigger exit");
			}
		}

		if (pair.triggerActor == m_pRedTrigger->GetRigidActor() && pair.otherActor == m_pRedCube->GetRigidActor())
		{
			if (pair.status == PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				Logger::GetInstance()->LogInfo(L"Red trigger entered");

				m_pRedHatch->RotateDegrees(0, 0, 90);
				m_pRedHatch->Translate(1, 0, 0);

				PlaySound(*m_pBellSound);
			}
			else if (pair.status == PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				Logger::GetInstance()->LogInfo(L"Red trigger exit");
			}
		}
	}
}
