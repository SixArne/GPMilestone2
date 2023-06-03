#pragma once
class MenuItem final: public GameObject
{
public:
	MenuItem(const std::string& name);
	~MenuItem() override = default;
	MenuItem(const MenuItem& other) = delete;
	MenuItem(MenuItem&& other) noexcept = delete;
	MenuItem& operator=(const MenuItem& other) = delete;
	MenuItem& operator=(MenuItem&& other) noexcept = delete;

	void SetIsSelected(bool value);
	void SetPosition(XMFLOAT2 position);
	void SetColor(XMFLOAT4& color);
	void SetText(const std::string& text);
	const std::string& GetText();

	void OnImGui();
	void Toggle();
	void SetMenuItemActive(bool value) { m_IsActive = value;}

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;


private:
	SpriteFont* m_pFont{};
	bool m_IsActive{true};

	std::string m_Content{ "My example string" };
	std::string m_ContentBuffer{};
	XMFLOAT2 m_ContentPosition{};
	XMFLOAT4 m_ContentColor{ 1.f, 1.f, 1.f, 1.f };

	bool m_IsSelected{ false };
};

