#include "stdafx.h"
#include "Player.h"
#include "Materials/DiffuseMaterial.h"

Player::Player()
{
	// Setup texture main
	DiffuseMaterial* pMainDiffuseMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	const UINT mainMaterialId = pMainDiffuseMaterial->GetMaterialId();
	pMainDiffuseMaterial->SetDiffuseTexture(L"Textures/female_goblin.png");

	// Load model
	const auto pModel = new ModelComponent(L"Characters/protagonist.ovm");

	pModel->SetMaterial(mainMaterialId, 0); // set texture

	// Setup physics
	const auto pRigidBodyMesh = new RigidBodyComponent();
	//pRigidBodyMesh->SetKinematic(true);
	auto pDefaultMat = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.8f);
	//auto* pConvexGeo = ContentManager::Load<PxConvexMesh>(L"Characters/protagonist.ovpc");
	pRigidBodyMesh->SetConstraint(RigidBodyConstraint::RotZ | RigidBodyConstraint::RotX, false);

	PxBoxGeometry capsuleGeo = PxBoxGeometry{ 4.f, 10.f, 4.f };

	pRigidBodyMesh->AddCollider(capsuleGeo, *pDefaultMat, false, PxTransform{ PxVec3{0.f, 10.f, 0.f} });
	GetTransform()->Translate(0, 10, 0);

	AddComponent(pModel);
	AddComponent(pRigidBodyMesh);
	GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	GetTransform()->Rotate(0, 0, 0);



	m_pCamera = AddChild(new FreeCamera());
	m_pCamera->GetTransform()->Translate(-3709.9255f, 127.60736f, -2100.6003f);
	//m_pCamera->SetRotation(0, 180);
}

void Player::Initialize(const SceneContext&)
{
	

}

void Player::PostInitialize(const SceneContext&)
{
	// clips

	auto pAnimator = GetComponent<ModelComponent>()->GetAnimator();
	auto count = pAnimator->GetClipCount();

	std::cout << count << std::endl;
}

void Player::Draw(const SceneContext&)
{
	auto start = GetTransform()->GetPosition();
	DirectX::XMFLOAT3 end
	{
		start.x + GetTransform()->GetForward().x * 1000.f,
		start.y + GetTransform()->GetForward().y * 1000.f,
		start.z + GetTransform()->GetForward().z * 1000.f
	};



	DebugRenderer::DrawLine(start, end, XMFLOAT4{ 1,0,1,1 });
}

void Player::Update(const SceneContext& /*ctx*/)
{
	/*if (InputManager::IsKeyboardKey(InputState::down, 'W'))
	{
		XMVECTOR forward = XMLoadFloat3(&GetTransform()->GetForward());
		forward = XMVectorScale(forward, m_MoveSpeed * ctx.pGameTime->GetElapsed());

		XMFLOAT3 displacement{};
		XMStoreFloat3(&displacement, forward);
		GetTransform()->Move(displacement);
	}
	else if (InputManager::IsKeyboardKey(InputState::down, 'S'))
	{
		XMVECTOR forward = XMLoadFloat3(&GetTransform()->GetForward());
		forward = XMVectorScale(forward, -m_MoveSpeed * ctx.pGameTime->GetElapsed());

		XMFLOAT3 displacement{};
		XMStoreFloat3(&displacement, forward);
		GetTransform()->Move(displacement);
	}

	if (InputManager::IsKeyboardKey(InputState::down, 'A'))
	{
		XMVECTOR right = XMLoadFloat3(&GetTransform()->GetRight());
		right = XMVectorScale(right, -m_MoveSpeed * ctx.pGameTime->GetElapsed());
		XMFLOAT3 displacement{};
		XMStoreFloat3(&displacement, right);
		GetTransform()->Move(displacement);
	}
	else if (InputManager::IsKeyboardKey(InputState::down, 'D'))
	{
		XMVECTOR right = XMLoadFloat3(&GetTransform()->GetRight());
		right = XMVectorScale(right, m_MoveSpeed * ctx.pGameTime->GetElapsed());
		XMFLOAT3 displacement{};
		XMStoreFloat3(&displacement, right);
		GetTransform()->Move(displacement);
	}*/
}

void Player::OnImGui(const SceneContext&) 
{
	auto pos = m_pCamera->GetTransform()->GetPosition();

	ImGui::SliderFloat3("Camera pos", (float*)&pos, -1000.f, 1000.f);
	if (ImGui::Button("Copy", ImVec2{ 100, 50 }))
	{
		auto toSave = std::format("{}, {}, {}", pos.x, pos.y, pos.z);
		std::cout << toSave << std::endl;
	}

	m_pCamera->GetTransform()->Translate(pos);

	ImGui::SliderFloat("PlayerSpeed", &m_MoveSpeed, 1.f, 100.f);
}
