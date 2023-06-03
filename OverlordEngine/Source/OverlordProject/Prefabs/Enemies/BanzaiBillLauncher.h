#pragma once
class Mario;
class BanzaiBill;

class BanzaiBillLauncher : public GameObject
{
public:
	BanzaiBillLauncher(Mario* mario);
	~BanzaiBillLauncher() override;

	BanzaiBillLauncher(const BanzaiBillLauncher& other) = delete;
	BanzaiBillLauncher(BanzaiBillLauncher&& other) noexcept = delete;
	BanzaiBillLauncher& operator=(const BanzaiBillLauncher& other) = delete;
	BanzaiBillLauncher& operator=(BanzaiBillLauncher&& other) noexcept = delete;

	void SetBillDeathCallback(std::function<void(BanzaiBill* bill)> callback) { m_BillDeathCallback = callback; }
	void SetBillCreateCallback(std::function<void(BanzaiBill* bill)> callback) { m_BillCreateCallback = callback; }

protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:
	std::function<void(BanzaiBill* bill)> m_BillDeathCallback{};
	std::function<void(BanzaiBill* bill)> m_BillCreateCallback{};

	std::vector<BanzaiBill*> m_pBanzaiBills{};
	Mario* m_pMario{};

	const float m_MarioMinDistance{200.f};
	const float m_SpawnDistance{ 10.f };

	float m_CurrentCooldown{ 0.f };

	bool m_CanFire{true};
	float m_FireCooldown{4.f};
};

