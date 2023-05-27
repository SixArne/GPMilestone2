#pragma once

class Mario;

class BanzaiBill : public GameObject
{
public:
	BanzaiBill(Mario* mario);
	~BanzaiBill() override = default;

	BanzaiBill(const BanzaiBill& other) = delete;
	BanzaiBill(BanzaiBill&& other) noexcept = delete;
	BanzaiBill& operator=(const BanzaiBill& other) = delete;
	BanzaiBill& operator=(BanzaiBill&& other) noexcept = delete;

	void DrawImGui();
	RigidBodyComponent* GetRigidBody();

protected:
	void Initialize(const SceneContext&) override;
	void PostInitialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:
	void InitializeSounds();
	XMFLOAT3 GetDirectionToMario();
	void RotateTowardsMario();
	void MoveTowardsMario(float dt);
	bool IsMarioClose();

	XMFLOAT3 ApplyRotationToVelocity(const XMFLOAT4& rotation, const XMFLOAT3& velocity);

	RigidBodyComponent* m_pRigidBody{};
	ParticleEmitterComponent* m_pEmitter{};

	GameObject* m_pParticles{};
	GameObject* m_pVisuals{};

	Mario* m_pMario{ };

	float m_Lifetime{ 3.f };
	float m_MoveSpeed{ 75.f };
	float m_MaxDetectRadius{ 300.f };
};

