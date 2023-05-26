#include "stdafx.h"
#include "DeferredRenderer.h"

#include "DeferredLightRenderer.h"

DeferredRenderer::~DeferredRenderer()
{
	//Release Resources
	for (int i{ 1 }; i < RT_COUNT; ++i)
	{
		SafeDelete(m_GBuffer[i]);
	}

	SafeDelete(m_pLightPassRenderer);
}

void DeferredRenderer::Initialize()
{

	const auto w = m_GameContext.windowWidth;
	const auto h = m_GameContext.windowHeight;

	const auto pDefaultRenderTarget = m_GameContext.pGame->GetRenderTarget(); //Main RenderTarget
	m_pDefaultRenderTargetView = pDefaultRenderTarget->GetRenderTargetView(); //Main RenderTargetView
	m_pDefaultDepthStencilView = pDefaultRenderTarget->GetDepthStencilView(); //Main DepthStencilView

	m_GBuffer[int(eGBufferId::LightAccumulation)] = pDefaultRenderTarget;
	m_GBuffer[int(eGBufferId::Diffuse)] = CreateRenderTarget(w, h, DXGI_FORMAT_R8G8B8A8_UNORM);
	m_GBuffer[int(eGBufferId::Specular)] = CreateRenderTarget(w, h, DXGI_FORMAT_R8G8B8A8_UNORM);
	m_GBuffer[int(eGBufferId::Normal)] = CreateRenderTarget(w, h, DXGI_FORMAT_R32G32B32A32_FLOAT);

	//Collect References of RTVs
	for (int i{}; i < RT_COUNT; i++)
	{
		m_RenderTargetViews[i] = m_GBuffer[i]->GetRenderTargetView();
	}

	//Collect SRVs (Ambient/Diffuse/Specular/Normal) + DepthSRV
	for (int i{}; i < SRV_COUNT -1; i++)
	{
		m_ShaderResourceViews[i] = m_GBuffer[i]->GetColorShaderResourceView();
	}

	m_ShaderResourceViews[int(eGBufferId::Depth)] = pDefaultRenderTarget->GetDepthShaderResourceView();

	//LightPassRenderer Init
	m_pLightPassRenderer = new DeferredLightRenderer();
	m_pLightPassRenderer->Initialize(m_GameContext.d3dContext);

	const auto descDefaultRT = pDefaultRenderTarget->GetDesc();
	m_pLightPassRenderer->CreateReadOnlyDSV(m_GameContext.d3dContext, descDefaultRT.pDepth, descDefaultRT.depthFormat);
}

RenderTarget* DeferredRenderer::CreateRenderTarget(UINT width, UINT height, DXGI_FORMAT format) const
{
	RENDERTARGET_DESC rtDesc{};

	rtDesc.width = width;
	rtDesc.height = height;
	rtDesc.colorFormat = format;

	rtDesc.enableColorBuffer = true;
	rtDesc.enableColorSRV = true;

	const auto pRenderTarget = new RenderTarget(m_GameContext.d3dContext);
	HANDLE_ERROR(pRenderTarget->Create(rtDesc))

		return pRenderTarget;
}

void DeferredRenderer::Begin(const SceneContext& sceneContext) const
{
	//Ignore this function if DeferredRendering is not 'active'
	if (!sceneContext.useDeferredRendering) return;

	const auto pDeviceContext = sceneContext.d3dContext.pDeviceContext;

	//1. Clear the G-Buffer RTVs
	const auto clearColor = XMFLOAT4{Colors::Black};

	for (int i{}; i < RT_COUNT; ++i)
	{
		pDeviceContext->ClearRenderTargetView(m_RenderTargetViews[i], &clearColor.x);
	}

	pDeviceContext->ClearDepthStencilView(m_pDefaultDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//2. Bind GBuffer RTVs + DSV
	pDeviceContext->OMSetRenderTargets(RT_COUNT, m_RenderTargetViews, m_pDefaultDepthStencilView);

	//3. DRAW GEOMETRY
	//... (Render Scene)
}

void DeferredRenderer::End(const SceneContext& sceneContext) const
{
	//Ignore this function if DeferredRendering is not 'active'
	if (!sceneContext.useDeferredRendering) return;

	//1. Geometry Pass is finished > Unbind GBuffer RTVs
	const auto pDeviceContext = sceneContext.d3dContext.pDeviceContext;
	ID3D11RenderTargetView* rTargets[RT_COUNT]{nullptr};
	pDeviceContext->OMSetRenderTargets(RT_COUNT, rTargets, nullptr);
	pDeviceContext->OMSetRenderTargets(1, &m_pDefaultRenderTargetView,nullptr);

	//2. Restore Main RenderTarget WITHOUT DSV (DepthBuffer SRV is used in pipeline)
	//...

	//3. Directional Light Pass
	m_pLightPassRenderer->DirectionalLightPass(sceneContext, m_ShaderResourceViews);

	//4. Volumetric Light Pass
	m_pLightPassRenderer->VolumetricLightPass(sceneContext, m_ShaderResourceViews, m_pDefaultRenderTargetView);

	//5. Unbind G-Buffer SRVs (Diffuse, Specular, Normal & Depth)
	ID3D11ShaderResourceView* pSRV[SRV_COUNT - 1]{nullptr};
	pDeviceContext->PSGetShaderResources(0, SRV_COUNT - 1, pSRV);

	//6. Reset Game RenderTarget (back to normal)
	m_GameContext.pGame->SetRenderTarget(nullptr);


	//DEBUG >> Visualize GBUFFER
	//Draw ImGui (to default RT)
	if(m_DrawImGui)
	{
		Debug_DrawGBuffer();
	}
}

void DeferredRenderer::DrawImGui()
{
	ImGui::Checkbox("Draw GBuffer", &m_DrawImGui);

	if(m_DrawImGui)
		ImGui::SliderInt("RTV ID", &m_VizRTVid, 0, RT_COUNT - 1);
}

void DeferredRenderer::Debug_DrawGBuffer() const
{
	//FULL SCREEN
	if(m_VizRTVid >= 1)
		QuadRenderer::Get()->Draw(m_ShaderResourceViews[m_VizRTVid]);

	const auto aspectRatio = m_GameContext.windowWidth / float(m_GameContext.windowHeight);
	const auto width = static_cast<int>(m_GameContext.windowWidth / 3.f);
	const auto height = static_cast<int>(width / aspectRatio);

	Quad renderQuad{
		0,
		int(m_GameContext.windowHeight) - height,
		width,
		height
	};

	//GBUFFER SRVs (only Diffuse/Specular/Normal) - skip Ambient & Depth
	for (auto i{ 1 }; i < SRV_COUNT-1; ++i)
	{
		QuadRenderer::Get()->Draw(m_ShaderResourceViews[i], renderQuad);
		renderQuad.x += width;
	}
}
