#pragma once

class Mario;

class BanzaiBill final: public GameObject
{
public:
	BanzaiBill(Mario* mario);
	~BanzaiBill() override = default;

	BanzaiBill(const BanzaiBill& other) = delete;
	BanzaiBill(BanzaiBill&& other) noexcept = delete;
	BanzaiBill& operator=(const BanzaiBill& other) = delete;
	BanzaiBill& operator=(BanzaiBill&& other) noexcept = delete;

	RigidBodyComponent* GetRigidBody();

	void SetMarioRef(Mario* marioRef);
	bool IsMarkedForDestruction() { return m_MarkedForDestruction; }

protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:
	void OnCollision(GameObject* pTrigger, GameObject* pOther, PxTriggerAction action);

	XMFLOAT3 GetDirectionToMario();
	void RotateTowardsMario();
	void MoveTowardsMario(float dt);
	bool IsMarioClose();

	RigidBodyComponent* m_pRigidBody{};
	ParticleEmitterComponent* m_pEmitter{};

	GameObject* m_pParticles{};
	GameObject* m_pVisuals{};

	Mario* m_pMario{ };

	float m_MoveSpeed{ 110.f };
	
	bool m_MarkedForDestruction{ false };
	bool m_HasCallbackTriggered{ false };
};

