#pragma once
class Moon : public GameObject
{
public:
	Moon() = default;
	~Moon() override = default;
	Moon(const Moon& other) = delete;
	Moon(Moon&& other) noexcept = delete;
	Moon& operator=(const Moon& other) = delete;
	Moon& operator=(Moon&& other) noexcept = delete;

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;

private:
	void OnGrabMoon(GameObject*, GameObject* pOther, PxTriggerAction action);

	RigidBodyComponent* m_rb{};
	GameObject* m_pVisuals{};

	float m_Angle{};
	float m_Oscelator{};

	bool m_MarkedForDestruction{ false };
	bool m_HasCallbackTriggered{ false };
};

