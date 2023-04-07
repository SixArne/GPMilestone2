#pragma once

class SpikyMaterial;

class SpikyScene : public GameScene
{
public:
	SpikyScene() : GameScene(L"SpikyScene") {}
	~SpikyScene() override = default;
	SpikyScene(const SpikyScene&) = delete;
	SpikyScene(SpikyScene&&) noexcept = delete;
	SpikyScene& operator=(const SpikyScene&) = delete;
	SpikyScene& operator=(SpikyScene&&) noexcept = delete;

protected:
	virtual void Initialize() override;
	virtual void OnGUI() override;


	void Update() override;

private:
	SpikyMaterial* m_pMaterial{};
	GameObject* m_pSphere{};
};

