#pragma once
class FontTestScene: public GameScene
{
public:
	FontTestScene() : GameScene(L"FontTestScene") {}
	~FontTestScene() override;
	FontTestScene(const FontTestScene& other) = delete;
	FontTestScene(FontTestScene&& other) noexcept = delete;
	FontTestScene& operator=(const FontTestScene& other) = delete;
	FontTestScene& operator=(FontTestScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void PostDraw() override;
	void OnGUI() override;

private:
	SpriteFont* m_pFont{};

	std::string m_Content{"My example string"};
	XMFLOAT2 m_ContentPosition{};
	XMFLOAT4 m_ContentColor{ 1.f, 1.f, 1.f, 1.f };
};

