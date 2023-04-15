#include "stdafx.h"
#include "BoneObject.h"

BoneObject::BoneObject(BaseMaterial* pMaterial, float length)
	: m_pMaterial{ pMaterial }
	, m_Offset{ length }
{
}

void BoneObject::AddBone(BoneObject* pBone)
{
	pBone->GetTransform()->Translate(m_Offset, 0.f, 0.f);
	AddChild(pBone);
}

void BoneObject::CalculateBindPose()
{
	XMStoreFloat4x4(&m_BindPose, XMMatrixInverse(nullptr, XMLoadFloat4x4(&GetTransform()->GetWorld())));

	for (BoneObject* pChildBone : GetChildren<BoneObject>())
	{
		pChildBone->CalculateBindPose();
	}
}

void BoneObject::Initialize(const SceneContext&)
{
	auto pRoot = new GameObject();
	AddChild(pRoot);

	ModelComponent* pModel{ new ModelComponent{ L"Meshes/Bone.ovm" } };
	pModel->SetMaterial(m_pMaterial);

	pRoot->GetTransform()->Rotate(0, -90, 0);
	pRoot->GetTransform()->Scale(m_Offset, m_Offset, m_Offset);
	pRoot->AddComponent(pModel);
}