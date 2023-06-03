#pragma once
class Mario;
class BanzaiBill;

class BanzaiBillLauncher final : public GameObject
{
public:
	BanzaiBillLauncher(Mario* mario);
	~BanzaiBillLauncher() override = default;

	BanzaiBillLauncher(const BanzaiBillLauncher& other) = delete;
	BanzaiBillLauncher(BanzaiBillLauncher&& other) noexcept = delete;
	BanzaiBillLauncher& operator=(const BanzaiBillLauncher& other) = delete;
	BanzaiBillLauncher& operator=(BanzaiBillLauncher&& other) noexcept = delete;
protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:

	void InstantiateBulletBill();

	std::vector<BanzaiBill*> m_pBanzaiBills{};
	Mario* m_pMario{};

	const float m_MarioMinDistance{200.f};
	const float m_SpawnDistance{ 5.f };

	float m_CurrentCooldown{ 0.f };
	float m_FireCooldown{4.f};

	bool m_CanFire{true};
};

