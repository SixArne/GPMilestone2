#include "stdafx.h"
#include "MenuItem.h"

#include <array>

MenuItem::MenuItem(const std::string& name)
	: m_Content{name}
{
}

void MenuItem::SetIsSelected(bool value)
{
	m_IsSelected = value;
}

void MenuItem::SetPosition(XMFLOAT2 position)
{
	m_ContentPosition = position;
}

void MenuItem::SetColor(XMFLOAT4& color)
{
	m_ContentColor = color;
}

void MenuItem::SetText(const std::string& text)
{
	m_Content = text;
}

const std::string& MenuItem::GetText()
{
	return m_Content;
}

void MenuItem::Initialize(const SceneContext& /*sceneContext*/)
{
	m_pFont = ContentManager::Load<SpriteFont>(L"SpriteFonts/Consolas_32.fnt");
}

void MenuItem::Update(const SceneContext& /*sceneContext*/)
{
	if (m_IsSelected)
	{
		TextRenderer::Get()->DrawText(m_pFont, StringUtil::utf8_decode(m_Content), m_ContentPosition, XMFLOAT4{1,1,0,1});
	}
	else
	{
		TextRenderer::Get()->DrawText(m_pFont, StringUtil::utf8_decode(m_Content), m_ContentPosition, m_ContentColor);
	}
}

void MenuItem::OnImGui()
{
	std::array<char, 256> buffer{};
	m_Content.copy(buffer.data(), 256);

	if (ImGui::InputText("Text", buffer.data(), 256))
	{
		m_Content = std::string(buffer.data());
	}

	ImGui::SliderFloat2("Position", &m_ContentPosition.x, 0, 1000);
	ImGui::ColorEdit4("Color", &m_ContentColor.x, ImGuiColorEditFlags_NoInputs);
}
