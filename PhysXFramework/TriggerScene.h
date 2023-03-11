#pragma once
#include "GameScene.h"



class TriggerScene : public GameScene
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


	TriggerScene() :GameScene(L"TestScene") {}
	~TriggerScene() override = default;
	TriggerScene(const TriggerScene& other) = delete;
	TriggerScene(TriggerScene&& other) noexcept = delete;
	TriggerScene& operator=(const TriggerScene other) = delete;
	TriggerScene& operator=(TriggerScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void OnSceneActivated() override;
	void OnSceneDeactivated() override;

	void CreateBall(PxPhysics* ph);
	void CreatePlane(PxPhysics* ph);
	void CreateTriggers(PxPhysics* ph);
	void CalculateTorque(XMFLOAT3& force, PxVec3& outTorque) const;

	virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) override;

private:
	GameObject* m_Sphere{};
	PxRigidDynamic* m_SpherePhysicsActor{};

	PxRigidStatic* m_pTriggerLeft{};
	PxRigidStatic* m_pTriggerRight{};
};

