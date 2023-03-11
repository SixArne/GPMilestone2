#include "stdafx.h"
#include "PhysicsTestScene.h"
#include "ContentManager.h"
#include "MeshObject.h"

#include "CubePosColorNorm.h"

namespace VECTOR3 {
#define OP(var, op) { var op }
#define OP_DIMENENSION(var, op) { OP(var.x, op), OP(var.y, op), OP(var.z, op)  }
}

void PhysicsTestScene::Initialize()
{
	EnablePhysxDebugRendering(true);


	PxPhysics* pPhysX = PhysxManager::GetInstance()->GetPhysics();
	PxMaterial* pDefaultMaterial = pPhysX->createMaterial(.5f, .5f, .2f);

	// Ground plane
	PxRigidStatic* pGroundActor = pPhysX->createRigidStatic(PxTransform{ PxQuat{PxPiDivTwo, PxVec3{0,0,1}} });
	PxRigidActorExt::createExclusiveShape(*pGroundActor, PxPlaneGeometry{}, &pDefaultMaterial, 1);
	m_pPhysxScene->addActor(*pGroundActor);

	// Chair
	m_pConvexChair = new MeshObject(L"Resources/Collision/chair.ovm");
	AddGameObject(m_pConvexChair);

	auto pConvexMesh = ContentManager::GetInstance()->Load<PxConvexMesh>(L"Resources/Collision/chair.ovpc");
	auto pConvexActor = pPhysX->createRigidDynamic(PxTransform(PxIdentity));
	PxRigidActorExt::createExclusiveShape(*pConvexActor, PxConvexMeshGeometry{ pConvexMesh }, *pDefaultMaterial);
	auto centerOfMass = PxVec3{ 0,5,0 };
	PxRigidBodyExt::updateMassAndInertia(*pConvexActor, 10.f, &centerOfMass);

	m_pConvexChair->AttachRigidActor(pConvexActor);
	m_pConvexChair->Translate(0, 12, 0);
	//m_pConvexChair->RotateDegrees(30, 0, 0);

	// triangle chair
	m_pTriangleChair = new MeshObject(L"Resources/Collision/chair.ovm");
	AddGameObject(m_pTriangleChair);
	
	const auto pTriangleMesh = ContentManager::GetInstance()->Load<PxTriangleMesh>(L"Resources/Collision/chair.ovpt");

	//const auto pTriangleStatic = pPhysX->createRigidStatic(PxTransform{PxIdentity});
	//PxRigidActorExt::createExclusiveShape(*pTriangleStatic, PxTriangleMeshGeometry{ pTriangleMesh }, *pDefaultMaterial);
	//m_pTriangleChair->AttachRigidActor(pTriangleStatic);

	m_pTriangleActor = pPhysX->createRigidDynamic(PxTransform{ PxIdentity });
	m_pTriangleActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
	auto pShape = PxRigidActorExt::createExclusiveShape(*m_pTriangleActor, PxTriangleMeshGeometry{ pTriangleMesh }, *pDefaultMaterial);



	m_pTriangleChair->AttachRigidActor(m_pTriangleActor);
	m_pTriangleChair->Translate(10, 0, 0);

	m_KinematicPosition = {10,0,0};
}

void PhysicsTestScene::Update()
{
	if (m_SceneContext.GetInput()->IsKeyboardKey(InputTriggerState::down, VK_LEFT))
	{
		m_KinematicPosition.x -= 5.f * m_SceneContext.GetGameTime()->GetElapsed();
		m_pTriangleActor->setKinematicTarget(PxTransform{ m_KinematicPosition });
	}
}

void PhysicsTestScene::Draw() const
{
}

void PhysicsTestScene::OnSceneActivated()
{
}

void PhysicsTestScene::OnSceneDeactivated()
{
}
