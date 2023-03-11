#pragma once
#include "GameScene.h"

class MeshObject;

class W2_AssignmentScene :
    public GameScene
{
	enum InputIDs
	{
		MoveLeft,
		MoveRight,
		MoveForward,
		MoveBackward,
		Reset
	};

public:
	W2_AssignmentScene();
	~W2_AssignmentScene() override;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() const override;
	virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) override;

private:
	MeshObject* CreateTriangleMesh(const std::wstring& meshSource, const std::wstring& collisionSource);
	GameObject* CreateTrigger(const PxBoxGeometry& shape, const XMFLOAT4& color);
	
	GameObject* CreateBall();
	GameObject* CreateCube(const PxBoxGeometry& shape, const XMFLOAT4& color);
	GameObject* CreateHatch(const PxBoxGeometry& shape, const XMFLOAT4& color);

	void PlaySound(FMOD::Sound& sound);

	void SetupTriggerEnterSound();
	void CalculateTorque(XMFLOAT3& force, PxVec3& outTorque) const;

	void ResetScene();

private:
	GameObject* m_pBlueTrigger{};
	GameObject* m_pRedTrigger{};
	GameObject* m_pControllableBall{};

	GameObject* m_pBlueCube{};
	GameObject* m_pRedCube{};

	GameObject* m_pBlueHatch{};
	GameObject* m_pRedHatch{};

	PxRevoluteJoint* m_pBlueHatchJoint{};

	FMOD::Sound* m_pBellSound{};

	FMOD::Channel* m_pTriggerChannel2D{};
};

