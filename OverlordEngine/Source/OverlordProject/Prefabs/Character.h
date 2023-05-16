#pragma once
#define BIT(x) (1 << x)

struct CharacterDesc
{
	CharacterDesc(
		PxMaterial* pMaterial,
		float radius = .5f,
		float height = 2.f)
	{
		controller.setToDefault();
		controller.radius = radius;
		controller.height = height;
		controller.material = pMaterial;
	}

	float maxMoveSpeed{ 80.f };
	float maxFallSpeed{ 100.f };

	float JumpSpeed{ 50.f };

	float moveAccelerationTime{ .1f };
	float fallAccelerationTime{ .8f };

	PxCapsuleControllerDesc controller{};

	float rotationSpeed{ 110.f };

	int actionId_MoveLeft{ -1 };
	int actionId_MoveRight{ -1 };
	int actionId_MoveForward{ -1 };
	int actionId_MoveBackward{ -1 };
	int actionId_Jump{ -1 };
};

enum StateBitfield
{
	HasStartedIdle		= BIT(0),
	Idle				= BIT(1),
	HasStartedMoving	= BIT(2),
	Moving				= BIT(3),
	HasStartedJump		= BIT(4),
	Jumping				= BIT(5),
	IsGrounded			= BIT(6)
};

class Character : public GameObject
{
public:
	Character(const CharacterDesc& characterDesc, GameObject* visuals);
	~Character() override = default;

	Character(const Character& other) = delete;
	Character(Character&& other) noexcept = delete;
	Character& operator=(const Character& other) = delete;
	Character& operator=(Character&& other) noexcept = delete;

	void DrawImGui();
	uint32_t GetState();

protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:
	CameraComponent* m_pCameraComponent{};
	ControllerComponent* m_pControllerComponent{};
	GameObject* m_pVisuals{};
	GameObject* m_pCameraBoom{};
	

	CharacterDesc m_CharacterDesc;
	float m_TotalPitch{}, m_TotalYaw{};				//Total camera Pitch(X) and Yaw(Y) rotation
	float m_MoveAcceleration{},						//Acceleration required to reach maxMoveVelocity after 1 second (maxMoveVelocity / moveAccelerationTime)
		m_FallAcceleration{},						//Acceleration required to reach maxFallVelocity after 1 second (maxFallVelocity / fallAccelerationTime)
		m_MoveSpeed{};								//MoveSpeed > Horizontal Velocity = MoveDirection * MoveVelocity (= TotalVelocity.xz)

	XMFLOAT3 m_TotalVelocity{};						//TotalVelocity with X/Z for Horizontal Movement AND Y for Vertical Movement (fall/jump)
	XMFLOAT3 m_CurrentDirection{};					//Current/Last Direction based on Camera forward/right (Stored for deacceleration)

	bool m_IsGrounded{ false };
	
	uint32_t m_State{};
};

