#pragma once

#include <vector>

#include "GameScene.h"



class W1_AssignmentScene :public GameScene
{
public:
	enum InputIDs
	{
		MoveLeft,
		MoveRight,
		MoveForward,
		MoveBackward,
		Reset
	};


	W1_AssignmentScene() :GameScene(L"W1_AssignmentScene") {}
	~W1_AssignmentScene() override = default;
	W1_AssignmentScene(const W1_AssignmentScene& other) = delete;
	W1_AssignmentScene(W1_AssignmentScene&& other) noexcept = delete;
	W1_AssignmentScene& operator=(const W1_AssignmentScene other) = delete;
	W1_AssignmentScene& operator=(W1_AssignmentScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void OnSceneActivated() override;
	void OnSceneDeactivated() override;

private:
	void CreateCubes(PxPhysics* physics);
	void CreatePlane(PxPhysics* physics);
	void CreateBall(PxPhysics* physics);

	void CalculateTorque(XMFLOAT3& force, PxVec3& outTorque) const;
	void ResetScene();
	
private:

	// 1:1 relation
	std::vector<GameObject*> m_Cubes{};
	std::vector<PxRigidDynamic*> m_CubePhysicsActors{};

	GameObject* m_Sphere{};
	PxRigidDynamic* m_SpherePhysicsActor{};

	PxRigidStatic* m_GroundPlane{};
};
