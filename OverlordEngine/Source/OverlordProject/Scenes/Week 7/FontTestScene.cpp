#include "stdafx.h"
#include "FontTestScene.h"

#include <array>

FontTestScene::~FontTestScene()
{}

void FontTestScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.clearColor = XMFLOAT4{ 0.f,0.f,0.f,0.f };

	m_pFont = ContentManager::Load<SpriteFont>(L"SpriteFonts/Consolas_32.fnt");

	auto sprite = new GameObject();
	sprite->AddComponent(new SpriteComponent(L"Textures/TestSprite.jpg", { 0.5f,0.5f }, { 1.f,1.f,1.f,.5f }));
	AddChild(sprite);
}

void FontTestScene::Update()
{
	TextRenderer::Get()->DrawText(m_pFont, StringUtil::utf8_decode(m_Content), m_ContentPosition, m_ContentColor);

}

void FontTestScene::OnGUI()
{
	std::array<char, 256> buffer{};
	m_Content.copy(buffer.data(), 256);
	
	if (ImGui::InputText("Text", buffer.data(), 256))
	{
		m_Content = std::string(buffer.data());
	}

	ImGui::SliderFloat2("Position", &m_ContentPosition.x, 0, m_SceneContext.windowWidth);
	ImGui::ColorEdit4("Color", &m_ContentColor.x, ImGuiColorEditFlags_NoInputs);
}