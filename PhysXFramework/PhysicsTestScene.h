#pragma once
#include "GameScene.h"

class PhysicsTestScene: public GameScene
{
public:
	PhysicsTestScene() :GameScene(L"TestScene") {}
	~PhysicsTestScene() override = default;
	PhysicsTestScene(const PhysicsTestScene& other) = delete;
	PhysicsTestScene(PhysicsTestScene&& other) noexcept = delete;
	PhysicsTestScene& operator=(const PhysicsTestScene other) = delete;
	PhysicsTestScene& operator=(PhysicsTestScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void OnSceneActivated() override;
	void OnSceneDeactivated() override;

private:
	GameObject* m_pConvexChair{ nullptr };
	GameObject* m_pTriangleChair{ nullptr };
	PxRigidDynamic* m_pTriangleActor{ nullptr };
	PxVec3 m_KinematicPosition{};
};

