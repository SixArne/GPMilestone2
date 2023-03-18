#include "stdafx.h"
#include "PongScene.h"

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
	auto pbounyMaterial = physx.createMaterial(.5, .5, 0);

	GameSceneExt::CreatePhysXGroundPlane(*this, pbounyMaterial);

	m_pBotWall = new GameObject{};
	//auto collider = physx.createRigidStatic(PxTransform{ PhysxHelper::ToPxVec3(m_pBotWall->GetTransform()->GetPosition()) });
	auto pBotWallRigid = m_pBotWall->AddComponent(new RigidBodyComponent(false));
	pBotWallRigid->AddCollider(PxBoxGeometry{ 2, 1, .5 }, *pbounyMaterial);
	pBotWallRigid->SetCollisionGroup(CollisionGroup::Group0);
	pBotWallRigid->SetConstraint(RigidBodyConstraint::All, true);

	m_pBotWall->GetTransform()->Translate(XMFLOAT3{ 0, 2, 0 });

	m_pFixedCamera = new FixedCamera{};
	AddChild(m_pFixedCamera);
	SetActiveCamera(m_pFixedCamera->GetComponent<CameraComponent>()); //trigger a flag

	m_pFixedCamera->GetTransform()->Translate(0, 20, 0);
	m_pFixedCamera->GetTransform()->Rotate(90, 0, 0);

	AddChild(m_pBotWall);
}

void PongScene::Update()
{
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