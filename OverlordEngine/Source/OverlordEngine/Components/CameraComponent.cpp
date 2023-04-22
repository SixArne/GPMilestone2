#include "stdafx.h"
#include "CameraComponent.h"

CameraComponent::CameraComponent() :
	m_FarPlane(2500.0f),
	m_NearPlane(0.1f),
	m_FOV(XM_PIDIV4),
	m_Size(25.0f),
	m_PerspectiveProjection(true)
{
	XMStoreFloat4x4(&m_Projection, XMMatrixIdentity());
	XMStoreFloat4x4(&m_View, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewInverse, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjection, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjectionInverse, XMMatrixIdentity());
}

void CameraComponent::Update(const SceneContext& sceneContext)
{
	// see https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
	using namespace DirectX;

	XMMATRIX projection{};

	if (m_PerspectiveProjection)
	{
		projection = XMMatrixPerspectiveFovLH(m_FOV, sceneContext.aspectRatio, m_NearPlane, m_FarPlane);
	}
	else
	{
		const float viewWidth = (m_Size > 0) ? m_Size * sceneContext.aspectRatio : sceneContext.windowWidth;
		const float viewHeight = (m_Size > 0) ? m_Size : sceneContext.windowHeight;
		projection = XMMatrixOrthographicLH(viewWidth, viewHeight, m_NearPlane, m_FarPlane);
	}

	const XMVECTOR worldPosition = XMLoadFloat3(&GetTransform()->GetWorldPosition());
	const XMVECTOR lookAt = XMLoadFloat3(&GetTransform()->GetForward());
	const XMVECTOR upVec = XMLoadFloat3(&GetTransform()->GetUp());

	const XMMATRIX view = XMMatrixLookAtLH(worldPosition, worldPosition + lookAt, upVec);
	const XMMATRIX viewInv = XMMatrixInverse(nullptr, view);
	const XMMATRIX viewProjectionInv = XMMatrixInverse(nullptr, view * projection);

	XMStoreFloat4x4(&m_Projection, projection);
	XMStoreFloat4x4(&m_View, view);
	XMStoreFloat4x4(&m_ViewInverse, viewInv);
	XMStoreFloat4x4(&m_ViewProjection, view * projection);
	XMStoreFloat4x4(&m_ViewProjectionInverse, viewProjectionInv);
}

void CameraComponent::SetActive(bool active)
{
	if (m_IsActive == active) return;

	const auto pGameObject = GetGameObject();
	ASSERT_IF(!pGameObject, L"Failed to set active camera. Parent game object is null");

	if (!pGameObject) return; //help the compiler... (C6011)
	const auto pScene = pGameObject->GetScene();
	ASSERT_IF(!pScene, L"Failed to set active camera. Parent game scene is null");

	m_IsActive = active;
	pScene->SetActiveCamera(active?this:nullptr); //Switch to default camera if active==false
}

GameObject* CameraComponent::Pick(CollisionGroup ignoreGroups) const
{
	SceneContext sceneContext = m_pScene->GetSceneContext();
	POINT position = sceneContext.pInput->GetMousePosition();

	D3D11_VIEWPORT viewport{};
	UINT numViewports{ 1 };
	sceneContext.d3dContext.pDeviceContext->RSGetViewports(&numViewports, &viewport);

	float halfWidth{ viewport.Width / 2.f };
	float halfHeight{ viewport.Height / 2.f };

	// from screen to 0-1
	float x{ (float(position.x) - halfWidth) / halfWidth };
	float y{ (halfHeight - float(position.y)) / halfHeight };

	DirectX::XMFLOAT4 nearPlane{ x, y, 0.f, 0.f };
	DirectX::XMFLOAT4 farPlane{ x, y, 1.f, 0.f };
	DirectX::XMVECTOR nearP{ DirectX::XMLoadFloat4(&nearPlane) };
	DirectX::XMVECTOR farP{ DirectX::XMLoadFloat4(&farPlane) };

	// View projection inverse
	DirectX::XMMATRIX vpi{ DirectX::XMLoadFloat4x4(&m_ViewProjectionInverse) };

	DirectX::XMFLOAT4 nPoint{}, fPoint{};
	DirectX::XMVECTOR nearPoint{ DirectX::XMVector3TransformCoord(nearP, vpi) };
	DirectX::XMVECTOR farPoint{ DirectX::XMVector3TransformCoord(farP, vpi) };
	DirectX::XMStoreFloat4(&nPoint, nearPoint);
	DirectX::XMStoreFloat4(&fPoint, farPoint);

	auto start{ physx::PxVec3{ nPoint.x, nPoint.y, nPoint.z } };
	auto end{ physx::PxVec3{ fPoint.x, fPoint.y, fPoint.z } };

	physx::PxQueryFilterData filterData{};
	filterData.data.word0 = ~UINT(ignoreGroups);

	physx::PxRaycastBuffer hit{};
	auto activeScene = SceneManager::Get()->GetActiveScene();
	auto raycast = activeScene->GetPhysxProxy()->Raycast(
		start,
		(end - start).getNormalized(),
		(end - start).magnitude(),
		hit,
		physx::PxHitFlag::eDEFAULT,
		filterData);

	if (raycast)
	{
		auto const comp = hit.block.actor->userData;
		if (comp == nullptr)
		{
			Logger::LogError(L"component is a nullpointer");
			return nullptr;
		}
		auto gameObject = reinterpret_cast<BaseComponent*>(comp)->GetGameObject();
		if (gameObject == nullptr)
		{
			Logger::LogError(L"GameObject is a nullptr");
			return nullptr;
		}
		return gameObject;
	}


	return nullptr;
}