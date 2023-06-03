#pragma once
class Coin final: public GameObject
{
public:
	Coin() = default;
	~Coin() override = default;
	Coin(const Coin& other) = delete;
	Coin(Coin&& other) noexcept = delete;
	Coin& operator=(const Coin& other) = delete;
	Coin& operator=(Coin&& other) noexcept = delete;

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;

private:
	void OnGrabCoin(GameObject*, GameObject* pOther, PxTriggerAction action);

	RigidBodyComponent* m_rb{};
	GameObject* m_pVisuals{};

	float m_Angle{};
	float m_Oscillator{};

	bool m_MarkedForDestruction{ false };
	bool m_HasCallbackTriggered{ false };
};

