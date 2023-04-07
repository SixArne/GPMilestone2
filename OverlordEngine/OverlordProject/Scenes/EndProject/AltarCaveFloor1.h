#pragma once
class AltarCaveFloor1 :
    public GameScene
{
public:
	AltarCaveFloor1() : GameScene(L"ModelTestScene") {}

	~AltarCaveFloor1() override = default;
	AltarCaveFloor1(const AltarCaveFloor1& other) = delete;
	AltarCaveFloor1(AltarCaveFloor1&& other) noexcept = delete;
	AltarCaveFloor1& operator=(const AltarCaveFloor1& other) = delete;
	AltarCaveFloor1& operator=(AltarCaveFloor1&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnGUI() override;

private:
	GameObject* m_pModelGameObject{};
	FMOD::Channel* m_pBackgroundMusic = nullptr;
	FreeCamera* m_pCamera{};
	GameObject* m_pProtagonist{};

	class UberMaterial* m_DebugMaterial{};

	void CreateMap();
};
