#include "stdafx.h"
#include "PostVignette.h"

PostVignette::PostVignette() :
	PostProcessingMaterial(L"Effects/Post/Vignette.fx")
{
}

void PostVignette::SetTime(float time)
{
	m_pTimeVariable->SetFloat(time);
}

void PostVignette::SetRadius(float radius)
{
	m_pRadiusVariable->SetFloat(radius);
}

void PostVignette::SetColor(DirectX::XMFLOAT3 color)
{
	m_pColorVariable->SetFloatVector(reinterpret_cast<float*>(&color));
}

void PostVignette::Initialize(const GameContext&)
{
	m_pTimeVariable = m_pBaseEffect->GetVariableByName("gTime")->AsScalar();
	m_pRadiusVariable = m_pBaseEffect->GetVariableByName("gRadius")->AsScalar();
	m_pColorVariable = m_pBaseEffect->GetVariableByName("gColor")->AsVector();
}
