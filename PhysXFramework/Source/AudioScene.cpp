
#include "stdafx.h"
#include "AudioScene.h"
#include "PhysxManager.h"
#include "SpherePosColorNorm.h"
#include "SoundManager.h"

void AudioScene::Initialize()
{
	EnablePhysxDebugRendering(true);
	m_pPhysxScene->setVisualizationParameter(PxVisualizationParameter::eBODY_LIN_VELOCITY, 1.f);

	PxPhysics* pPhysX = PhysxManager::GetInstance()->GetPhysics();
	CreatePlane(pPhysX);
	CreateBall(pPhysX);

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
		InputAction{ InputIDs::Play2D, InputTriggerState::pressed, VK_HOME, -1, XINPUT_GAMEPAD_B }
	);

	m_SceneContext.GetInput()->AddInputAction(
		InputAction{ InputIDs::Play3D, InputTriggerState::pressed, VK_END, -1, XINPUT_GAMEPAD_B }
	);

	m_SceneContext.GetInput()->AddInputAction(
		InputAction{ InputIDs::IncreaseVolume, InputTriggerState::down, VK_INSERT, -1, XINPUT_GAMEPAD_B }
	);

	m_SceneContext.GetInput()->AddInputAction(
		InputAction{ InputIDs::DecreaseVolume, InputTriggerState::down, VK_PAUSE, -1, XINPUT_GAMEPAD_B }
	);

	auto pFmod = SoundManager::GetInstance()->GetSystem();
	
	FMOD::Sound* pSound{};
	FMOD_RESULT result = pFmod->createStream("Resources/Sounds/wave.mp3", FMOD_DEFAULT | FMOD_LOOP_NORMAL, nullptr, &pSound);
	SoundManager::GetInstance()->ErrorCheck(result);

	result = pFmod->playSound(pSound, nullptr, true, &m_pChannel2D);
	SoundManager::GetInstance()->ErrorCheck(result);

	//
	FMOD::Sound* pSound2{};
	result = pFmod->createStream("Resources/Sounds/Thrones.mp3", FMOD_3D | FMOD_3D_LINEARROLLOFF, 0, &pSound2);
	SoundManager::GetInstance()->ErrorCheck(result);

	result = pFmod->playSound(pSound2, nullptr, true, &m_pChannel3D);
	SoundManager::GetInstance()->ErrorCheck(result);

	m_pChannel3D->setMode(FMOD_LOOP_NORMAL);
	m_pChannel3D->set3DMinMaxDistance(0.f, 100.f);
}

inline FMOD_VECTOR ToFMod(XMFLOAT3 v)
{
	return FMOD_VECTOR{ v.x, v.y, v.z };
}

inline FMOD_VECTOR ToFMod(PxVec3 v)
{
	return FMOD_VECTOR{ v.x, v.y, v.z };
}

void AudioScene::Update()
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

	if (m_SceneContext.GetInput()->IsActionTriggered(InputIDs::Play2D))
	{
		bool bPaused{};
		m_pChannel2D->getPaused(&bPaused);
		m_pChannel2D->setPaused(!bPaused);
	}

	if (m_SceneContext.GetInput()->IsActionTriggered(InputIDs::Play3D))
	{
		bool bPaused{};
		m_pChannel3D->getPaused(&bPaused);
		m_pChannel3D->setPaused(!bPaused);
	}

	if (m_SceneContext.GetInput()->IsActionTriggered(InputIDs::IncreaseVolume))
	{
		float bVolume{};
		m_pChannel2D->getVolume(&bVolume);

		bVolume += 0.1f;
		bVolume = std::clamp(bVolume, 0.f, 1.f);

		m_pChannel2D->setVolume(bVolume);
	}

	if (m_SceneContext.GetInput()->IsActionTriggered(InputIDs::DecreaseVolume))
	{
		float bVolume{};
		m_pChannel2D->getVolume(&bVolume);

		bVolume -= 0.1f;
		bVolume = std::clamp(bVolume, 0.f, 1.f);

		m_pChannel2D->setVolume(bVolume);
	}

	if (bHasInputOnBall)
	{
		static_cast<PxRigidDynamic*>(m_Sphere->GetRigidActor())->addTorque(outTorque);
	}

	// listener
	auto pCam = m_SceneContext.GetCamera();
	auto pos = ToFMod(pCam->GetPosition());
	auto forward = ToFMod(pCam->GetForward());
	auto up = ToFMod(pCam->GetUp());

	FMOD_VECTOR vel{};
	auto dt = m_SceneContext.GetGameTime()->GetElapsed();
	vel.x = (pos.x - m_PrevListenerPosition.x) / dt;
	vel.y = (pos.y - m_PrevListenerPosition.y) / dt;
	vel.z = (pos.z - m_PrevListenerPosition.z) / dt;
	m_PrevListenerPosition = pos;

	SoundManager::GetInstance()->GetSystem()->set3DListenerAttributes(0, &pos, &vel, &forward, &up);

	// source
	auto spherePos = ToFMod(m_Sphere->GetPosition());
	auto sphereVelocity = ToFMod(m_Sphere->GetRigidActor()->is<PxRigidDynamic>()->getLinearVelocity());

	m_pChannel3D->set3DAttributes(&spherePos, &sphereVelocity);
}

void AudioScene::Draw() const
{
}

void AudioScene::OnSceneActivated()
{
}

void AudioScene::OnSceneDeactivated()
{
}

void AudioScene::CreatePlane(PxPhysics* pPhysX)
{
	PxMaterial* pGroundMaterial = pPhysX->createMaterial(.5, .8, 0.1);
	PxRigidStatic* pGroundActor = pPhysX->createRigidStatic(PxTransform{ PxQuat{PxPiDivTwo, PxVec3{0,0,1}} });
	PxRigidActorExt::createExclusiveShape(*pGroundActor, PxPlaneGeometry{}, &pGroundMaterial, 1);
	m_pPhysxScene->addActor(*pGroundActor);
	m_GroundPlane = pGroundActor;
}

void AudioScene::CreateBall(PxPhysics* pPhysX)
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

void AudioScene::CalculateTorque(XMFLOAT3& force, PxVec3& outTorque) const
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
