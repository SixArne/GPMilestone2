#include "stdafx.h"
#include "HardwareSkinningScene.h"

#include "Materials/DiffuseMaterial_Skinned.h"
#include <Windows.h>

std::string WideStringToUTF8(const std::wstring& wide_string) 
{
	int utf8_length = WideCharToMultiByte(CP_UTF8, 0, wide_string.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string utf8_string(utf8_length, 0);
	WideCharToMultiByte(CP_UTF8, 0, wide_string.c_str(), -1, &utf8_string[0], utf8_length, nullptr, nullptr);
	return utf8_string;
}

std::vector<std::string> ConvertAnimationNames(const std::vector<std::wstring>& wide_strings) 
{
	// Convert std::vector<std::wstring> to std::vector<std::string>
	std::vector<std::string> utf8_strings;
	utf8_strings.reserve(wide_strings.size());
	for (const auto& wide_string : wide_strings) {
		utf8_strings.push_back(WideStringToUTF8(wide_string));
	}

	return utf8_strings;
}

HardwareSkinningScene::~HardwareSkinningScene()
{
}

void HardwareSkinningScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	const auto pSkinnedMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
	pSkinnedMaterial->SetDiffuseTexture(L"Textures/PeasantGirl_Diffuse.png");

	const auto pObject = AddChild(new GameObject);
	const auto pModel = pObject->AddComponent(new ModelComponent(L"Meshes/PeasantGirl.ovm"));
	pModel->SetMaterial(pSkinnedMaterial);

	pObject->GetTransform()->Scale(0.15f);

	pAnimator = pModel->GetAnimator();
	pAnimator->SetAnimation(m_AnimationClipId);
	pAnimator->SetAnimationSpeed(m_AnimationSpeed);

	//Gather Clip Names
	m_ClipCount = pAnimator->GetClipCount();
	std::vector<std::wstring> clipNames{};
	for (UINT i{ 0 }; i < m_ClipCount; ++i)
	{
		clipNames.emplace_back(pAnimator->GetClip(static_cast<int>(i)).name);
	}

	m_ClipNames = ConvertAnimationNames(clipNames);

}

void HardwareSkinningScene::OnGUI()
{
	if (ImGui::Button(pAnimator->IsPlaying() ? "PAUSE" : "PLAY"))
	{
		if (pAnimator->IsPlaying())pAnimator->Pause();
		else pAnimator->Play();
	}

	if (ImGui::Button("RESET"))
	{
		pAnimator->Reset();
	}

	ImGui::Dummy({ 0,5 });

	bool reversed = pAnimator->IsReversed();
	if (ImGui::Checkbox("Play Reversed", &reversed))
	{
		pAnimator->SetPlayReversed(reversed);
	}

	std::vector<const char*> c_string_list;
	c_string_list.reserve(m_ClipNames.size());
	for (const auto& utf8_string : m_ClipNames) {
		c_string_list.push_back(utf8_string.c_str());
	}
	if (ImGui::ListBox("Animation Clip", &m_AnimationClipId, c_string_list.data(), static_cast<int>(m_ClipCount)))
	{
		pAnimator->SetAnimation(m_AnimationClipId);
	}

	if (ImGui::SliderFloat("Animation Speed", &m_AnimationSpeed, 0.f, 4.f))
	{
		pAnimator->SetAnimationSpeed(m_AnimationSpeed);
	}
}