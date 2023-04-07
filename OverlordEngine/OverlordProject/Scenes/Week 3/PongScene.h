#pragma once

class CubePrefab;
class SpherePrefab;

class PongScene : public GameScene
{
public:
	PongScene();
	~PongScene() override = default;

	PongScene(const PongScene& other) = delete;
	PongScene(PongScene&& other) noexcept = delete;
	PongScene& operator=(const PongScene& other) = delete;
	PongScene& operator=(PongScene&& other) noexcept = delete;

protected:
	void Initialize() override;


	

	void Update() override;
	void Draw() override;
	void OnGUI() override;

private:
	// ball
	SpherePrefab* m_pBall = nullptr;
	RigidBodyComponent* m_pBallRigidBody = nullptr;

	// players
	CubePrefab* m_pPlayer1 = nullptr;
	CubePrefab* m_pPlayer2 = nullptr;

	float m_MoveSpeed = 0.15f;
	float m_Constraint = 6.f;

	float m_DistanceToGoal = 30.f;
	float m_MaxSpeed = 20.f;
	float m_MinSpeed = 10.f;

	void MovePlayer(TransformComponent* pTransform, float speed) const;
	CubePrefab* CreatePlayer(const PxVec3& playerDim, const PxBoxGeometry playerGeom, const PxVec3& playerOffset, auto pBounyMaterial);
	CubePrefab* CreateWall(const PxVec3& wallDim, const PxVec3& wallOffset, auto pBounyMaterial);

	void ResetBall() const;
};

