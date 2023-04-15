#include "stdafx.h"
#include "PongScene.h"
#include "Prefabs/SpherePrefab.h"
#include "Prefabs/CubePrefab.h"

PongScene::PongScene()
	: GameScene(L"PongScene")
{
}

void PongScene::Initialize()
{
	//m_SceneContext.settings.showInfoOverlay = true;
	m_SceneContext.settings.drawPhysXDebug = true;
	m_SceneContext.settings.drawGrid = true;
	m_SceneContext.settings.enableOnGUI = true;

	auto& physx = PxGetPhysics();
	auto pBounyMaterial = physx.createMaterial(0, 0, 1);

	// Floor
	GameSceneExt::CreatePhysXGroundPlane(*this, pBounyMaterial);

	// Ball
	m_pBall = new SpherePrefab(1.f, 20, { 1.f, 1.f, 1.f, 1.f });
	m_pBall->GetTransform()->Translate(0.f, 1.f, 0.f);
	{
		m_pBallRigidBody = m_pBall->AddComponent(new RigidBodyComponent());
		m_pBallRigidBody->SetConstraint(RigidBodyConstraint::TransY, true);
		m_pBallRigidBody->SetDensity(0.f);
		m_pBallRigidBody->AddCollider(PxSphereGeometry(0.5f), *pBounyMaterial);
	}
	AddChild(m_pBall);
	ResetBall();

	// Create walls
	auto topWall = CreateWall(PxVec3(30.f, 2.f, 2.f), PxVec3(0.f, 0.f, -10.f), pBounyMaterial);
	AddChild(topWall);

	auto bottomWall = CreateWall(PxVec3(30.f, 2.f, 2.f), PxVec3(0.f, 0.f, 10.f), pBounyMaterial);
	AddChild(bottomWall);


	// Setup players
	const PxVec3 playerDim = PxVec3(2.f, 2.f, 5.f);
	const PxBoxGeometry playerGeom(playerDim / 2);

	// Create player 1
	m_pPlayer1 = CreatePlayer(playerDim, playerGeom, PxVec3{ -15.f, 1.f, 0.f }, pBounyMaterial);
	AddChild(m_pPlayer1);

	// Create player 2
	m_pPlayer2 = CreatePlayer(playerDim, playerGeom, PxVec3{ 15.f, 1.f, 0.f }, pBounyMaterial);
	AddChild(m_pPlayer2);

	// Setup Camera
	FixedCamera* pCamera = new FixedCamera();
	pCamera->GetTransform()->Translate(0.f, 30.f, 0.f);
	pCamera->GetTransform()->Rotate(90.f, 0.f, 0.f);
	AddChild(pCamera);
	SetActiveCamera(pCamera->GetComponent<CameraComponent>());

	// input
	m_SceneContext.pInput->AddInputAction(InputAction(0, InputState::down, 'W'));
	m_SceneContext.pInput->AddInputAction(InputAction(1, InputState::down, 'S'));
	m_SceneContext.pInput->AddInputAction(InputAction(2, InputState::down, VK_UP));
	m_SceneContext.pInput->AddInputAction(InputAction(3, InputState::down, VK_DOWN));
}

CubePrefab* PongScene::CreateWall(const PxVec3& wallDim, const PxVec3& wallOffset, auto pBounyMaterial)
{
	const PxBoxGeometry wallGeo(wallDim / 2);

	auto topWall = new CubePrefab(wallDim.x, wallDim.y, wallDim.z, { 1.f, 1.f, 1.f, 1.f });
	topWall->GetTransform()->Translate(wallOffset.x, wallOffset.y, wallOffset.z);
	{
		RigidBodyComponent* pPlayer1Rigid = topWall->AddComponent(new RigidBodyComponent());
		pPlayer1Rigid->SetKinematic(true);
		pPlayer1Rigid->AddCollider(wallGeo, *pBounyMaterial);
	}

	return topWall;
}

CubePrefab* PongScene::CreatePlayer(const PxVec3& playerDim, const PxBoxGeometry playerGeom, const PxVec3& playerOffset, auto pBounyMaterial)
{
	// player 1
	auto player = new CubePrefab(playerDim.x, playerDim.y, playerDim.z, { 0.f, 0.f, 1.f, 1.f });
	player->GetTransform()->Translate(playerOffset.x, playerOffset.y, playerOffset.z);
	{
		RigidBodyComponent* pPlayer1Rigid = player->AddComponent(new RigidBodyComponent());
		pPlayer1Rigid->SetKinematic(true);
		pPlayer1Rigid->AddCollider(playerGeom, *pBounyMaterial);
	}

	return player;
}

void PongScene::Update()
{
	TransformComponent* pTransform = m_pPlayer1->GetTransform();

	if (m_SceneContext.pInput->IsActionTriggered(0))
	{
		MovePlayer(pTransform, m_MoveSpeed);
	}
	if (m_SceneContext.pInput->IsActionTriggered(1))
	{
		MovePlayer(pTransform, -m_MoveSpeed);
	}

	pTransform = m_pPlayer2->GetTransform();

	if (m_SceneContext.pInput->IsActionTriggered(2))
	{
		MovePlayer(pTransform, m_MoveSpeed);
	}
	if (m_SceneContext.pInput->IsActionTriggered(3))
	{
		MovePlayer(pTransform, -m_MoveSpeed);
	}

	// Reset ball if it goes out of bounds
	if (m_pBallRigidBody->GetTransform()->GetPosition().x > m_DistanceToGoal ||
		m_pBallRigidBody->GetTransform()->GetPosition().x < -m_DistanceToGoal)
	{
		ResetBall();
	}
}

void PongScene::Draw()
{
}

void PongScene::OnGUI()
{
	ImGui::Text("This only activates if\n SceneSettings.enableOnGUI is True.\n\n");
	ImGui::Text("Use ImGui to add custom\n controllable scene parameters!");
	ImGui::ColorEdit3("Demo ClearColor", &m_SceneContext.settings.clearColor.x, ImGuiColorEditFlags_NoInputs);
	//ImGui::ColorEdit3("Bottom collider location", &m_pBotWall->GetTransform()->GetPosition().x, ImGuiColorEditFlags_NoInputs)
}

void PongScene::MovePlayer(TransformComponent* pTransform, float speed) const
{
	XMFLOAT3 position = pTransform->GetPosition();
	position.z += speed;
	position.z = PxClamp(position.z, -m_Constraint, m_Constraint);

	pTransform->Translate(position);
}

void PongScene::ResetBall() const
{
	m_pBallRigidBody->GetTransform()->Translate(0.f, 0.f, 0.f);
	m_pBallRigidBody->AddForce(
		{
		MathHelper::randF(-1.f, 1.f) * MathHelper::randF(m_MaxSpeed, m_MaxSpeed * 2),
		0.f,
		MathHelper::randF(-m_MaxSpeed, m_MaxSpeed)
		},
		PxForceMode::eIMPULSE
	);
}