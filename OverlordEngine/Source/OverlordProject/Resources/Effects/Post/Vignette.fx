//=============================================================================
//// Shader uses position and texture
//=============================================================================
SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};

Texture2D gTexture;
float gRadius = 0.8;
float gFeather = 0.5;
float gTime = 0.0;
float3 gColor = float3(1,0,0);

/// Create Depth Stencil State (ENABLE DEPTH WRITING)
/// Create Rasterizer State (Backface culling) 
DepthStencilState depthStencilState
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
};

/// Create Rasterizer State (Backface culling) 
RasterizerState rasterizerState
{
    FillMode = SOLID;
    CullMode = BACK;
};

//IN/OUT STRUCTS
//--------------
struct VS_INPUT
{
    float3 Position : POSITION;
	float2 TexCoord : TEXCOORD0;

};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD1;
};


//VERTEX SHADER
//-------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	// Set the Position
	// Set the TexCoord
	output.Position = float4(input.Position, 1);
    output.TexCoord = input.TexCoord;

	return output;
}


//PIXEL SHADER
//------------
float4 PS(PS_INPUT input): SV_Target
{
	float textureWidth = 0;
    float textureHeight = 0;

    // Sample color
    gTexture.GetDimensions(textureWidth, textureHeight);
    float4 color = gTexture.Sample(samPoint, input.TexCoord);

    // remap UV
	float2 newUV = input.TexCoord * 2 - 1; 
    float circle = length(newUV);
    
    // Create mask and inverted mask
    float mask = 1 - smoothstep(gRadius, gRadius + (gFeather * sin(gTime)), circle);
    float invertMask = 1 - mask;

    float3 displayColor = color.rgb * mask;
    float3 vingColor = color.rgb * invertMask * gColor;

    return float4(vingColor + displayColor, 1);
}


//TECHNIQUE
//---------
technique11 Grayscale
{
    pass P0
    {          
        // Set states...
        SetRasterizerState(rasterizerState);
        SetDepthStencilState(depthStencilState, 0);
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

