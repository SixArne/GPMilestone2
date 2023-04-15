#include "stdafx.h"
#include "W1_AssignmentScene.h"

#include "CubePosColorNorm.h"
#include "SpherePosColorNorm.h"
#include "PhysxManager.h"

constexpr auto ASSIGNMENT_ROWS = 10;
constexpr auto ASSIGNMENT_COLS = 10;

#define EPSILON 0.4
#define SPLIT_HALF(var) { var.x / 2.f, var.y / 2.f, var.z / 2.f}

void W1_AssignmentScene::Initialize()
{
	EnablePhysxDebugRendering(true);
	m_pPhysxScene->setVisualizationParameter(PxVisualizationParameter::eBODY_LIN_VELOCITY, 1.f);

	PxPhysics* pPhysX = PhysxManager::GetInstance()->GetPhysics();

	CreateCubes(pPhysX);
	CreateBall(pPhysX);
	CreatePlane(pPhysX);

	// Camera
	m_SceneContext.GetCamera()->SetPositionAndLookAt(XMFLOAT3{ 0,5,-20 }, XMFLOAT3{});

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
};

void W1_AssignmentScene::CreateCubes(PxPhysics* pPhysX)
{
	m_Cubes.reserve(ASSIGNMENT_COLS * ASSIGNMENT_ROWS);
	m_CubePhysicsActors.reserve(ASSIGNMENT_COLS * ASSIGNMENT_ROWS);

	const XMFLOAT3 cubeDimensions{ 1.f, 1.f, 1.f };

	// Create Cube physics material
	PxMaterial* pBoxMaterial = pPhysX->createMaterial(.5, .5, 0.05);

	// Make geometry with half the geo's extent
	PxBoxGeometry boxGeometry = PxBoxGeometry{ SPLIT_HALF(cubeDimensions) };
	
	for (size_t c{}; c < ASSIGNMENT_COLS; c++)
	{
		for (size_t r{}; r < ASSIGNMENT_ROWS; r++)
		{
			const size_t currentIndex = (c * ASSIGNMENT_COLS) + r;

			// We make a cube and give it to the scene to handle it
			m_Cubes.push_back(new CubePosColorNorm{ cubeDimensions.x,  cubeDimensions.y,  cubeDimensions.z });
			AddGameObject(m_Cubes[currentIndex]);


			// Needs to have physics interaction
			m_CubePhysicsActors.push_back(pPhysX->createRigidDynamic(PxTransform(PxIdentity)));
			m_CubePhysicsActors[currentIndex]->setMass(0.1);
	
			/*PxRigidBodyExt::setMassAndUpdateInertia(*m_CubePhysicsActors[currentIndex], PxReal{ 50 });*/
			PxRigidActorExt::createExclusiveShape(*m_CubePhysicsActors[currentIndex], boxGeometry, &pBoxMaterial, 1);
			m_Cubes[currentIndex]->AttachRigidActor(m_CubePhysicsActors[currentIndex]);
			m_Cubes[currentIndex]->Translate(r * 1.1, c * 1.1, 10);
		}
	}
}

void W1_AssignmentScene::CreateBall(PxPhysics* pPhysX)
{
	const float size = 1.f;

	m_Sphere = new SpherePosColorNorm{ size, 10, 10, XMFLOAT4{1, 0.5, 0, 1} };
	AddGameObject(m_Sphere);

	m_SpherePhysicsActor = pPhysX->createRigidDynamic(PxTransform(PxIdentity));
	m_SpherePhysicsActor->setMass(100);

	PxMaterial* pSphereMaterial = pPhysX->createMaterial(.8, .8, 0.05);
	PxSphereGeometry sphereGeometry = PxSphereGeometry{ PxReal{size} };
	PxRigidActorExt::createExclusiveShape(*m_SpherePhysicsActor, sphereGeometry, &pSphereMaterial, 1);

	m_Sphere->AttachRigidActor(m_SpherePhysicsActor);
	m_Sphere->Translate(0, 5, -10);
}

void W1_AssignmentScene::CreatePlane(PxPhysics* pPhysX)
{
	PxMaterial* pGroundMaterial = pPhysX->createMaterial(.5, .8, 0.1);
	PxRigidStatic* pGroundActor = pPhysX->createRigidStatic(PxTransform{ PxQuat{PxPiDivTwo, PxVec3{0,0,1}} });
	PxRigidActorExt::createExclusiveShape(*pGroundActor, PxPlaneGeometry{}, &pGroundMaterial, 1);
	m_pPhysxScene->addActor(*pGroundActor);
	m_GroundPlane = pGroundActor;
}

void W1_AssignmentScene::Update()
{
	bool bHasInputOnBall{ false };
	PxVec3 outTorque{};

	const float torqueValue{100000.f};

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
		static_cast<PxRigidDynamic*>(m_Sphere->GetRigidActor())->addTorque(outTorque);

	}
};

void W1_AssignmentScene::CalculateTorque(XMFLOAT3& force, PxVec3& outTorque) const
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

void W1_AssignmentScene::ResetScene()
{
	for (auto cube: m_Cubes)
	{
		RemoveGameObject(cube);
		SafeDelete(cube);
	}

	m_Cubes.clear();
	m_pPhysxScene->removeActors(reinterpret_cast<PxActor* const*>(m_CubePhysicsActors.data()), m_CubePhysicsActors.size());
	m_CubePhysicsActors.clear();

	RemoveGameObject(m_Sphere);
	SafeDelete(m_Sphere);

	m_pPhysxScene->removeActor(*m_SpherePhysicsActor);
	m_pPhysxScene->removeActor(*m_GroundPlane);

	PxPhysics* pPhysX = PhysxManager::GetInstance()->GetPhysics();

	CreateCubes(pPhysX);
	CreateBall(pPhysX);
	CreatePlane(pPhysX);
}

void W1_AssignmentScene::Draw() const
{

};

void W1_AssignmentScene::OnSceneActivated()
{
	
};

void W1_AssignmentScene::OnSceneDeactivated()
{

};