#pragma once
#include "GameScene.h"



class AudioScene : public GameScene
{
public:
	enum InputIDs
	{
		MoveLeft,
		MoveRight,
		MoveForward,
		MoveBackward,
		Reset,
		Play2D,
		Play3D,
		IncreaseVolume,
		DecreaseVolume,
	};


	AudioScene() :GameScene(L"TestScene") {}
	~AudioScene() override = default;
	AudioScene(const AudioScene& other) = delete;
	AudioScene(AudioScene&& other) noexcept = delete;
	AudioScene& operator=(const AudioScene other) = delete;
	AudioScene& operator=(AudioScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void OnSceneActivated() override;
	void OnSceneDeactivated() override;

	void CreatePlane(PxPhysics* physics);
	void CreateBall(PxPhysics* physics);
	void CalculateTorque(XMFLOAT3& force, PxVec3& outTorque) const;

private:
	GameObject* m_Sphere{};
	PxRigidDynamic* m_SpherePhysicsActor{};
	PxRigidStatic* m_GroundPlane{};

	FMOD::Channel* m_pChannel2D{};
	FMOD::Channel* m_pChannel3D{};
	FMOD_VECTOR m_PrevListenerPosition{};
};

