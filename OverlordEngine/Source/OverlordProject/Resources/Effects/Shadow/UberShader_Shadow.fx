float4x4 gWorldViewProj : WORLDVIEWPROJECTION;
float4x4 gMatrixViewInverse : VIEWINVERSE;
float4x4 gWorld : WORLD;

float4x4 gWorldViewProj_Light;
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);
float gShadowMapBias = 0.01f;
Texture2D gShadowMap;

SamplerComparisonState cmpSampler
{
	// sampler state
	Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
	AddressU = MIRROR;
	AddressV = MIRROR;

	// sampler comparison state
	ComparisonFunc = LESS_EQUAL;
};

RasterizerState Solid
{
	FillMode = SOLID;
	CullMode = FRONT;
};

BlendState gBS_EnableBlending
{
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
};

SamplerState gTextureSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
 	AddressU = WRAP;
	AddressV = WRAP;
	AddressW = WRAP;
};


// Diffuse properties
bool gUseTextureDiffuse; // Indicates should render diffuse
float4 gColorDiffuse; // Backup
Texture2D gTextureDiffuse; // Diffuse texture

// Specular properties
bool gUseTextureSpecular;
float4 gColorSpecular;
Texture2D gTextureSpecular;
int gShininess;

// Ambient properties
float4 gColorAmbient;
float gAmbientIntensity;

// Normal properties
bool gUseTextureNormal;
Texture2D gTextureNormal;
float gNormalStrength;

// Fresnel properties
bool gUseFresnelFalloff;
float4 gColorFresnel;
float gFresnelPower;
float gFresnelMultiplier;
float gFresnelHardness;

// Opacity properties
float gOpacityIntensity;
bool gTextureOpacityIntensity;
Texture2D gTextureOpacity;

// Specular properties
bool gUseSpecularBlinn;
bool gUseSpecularPhong;

TextureCube gCubeEnvironment;
bool gUseEnvironmentMapping;
float gReflectionStrength;
float gRefractionStrength;
float gRefractionIndex;

// Input and Output
struct VS_Input
{
	float3 Position: POSITION;
	float3 Normal: NORMAL;
	float3 Tangent: TANGENT;
	float2 TexCoord: TEXCOORD0;
};

struct VS_Output
{
	float4 Position: SV_POSITION;
	float4 WorldPosition: COLOR0;
	float3 Normal: NORMAL;
	float3 Tangent: TANGENT;
	float2 TexCoord: TEXCOORD0;
	float4 lPos : TEXCOORD1;
};

float3 CalculateSpecularBlinn(float3 viewDirection, float3 normal)
{
	float3 specularColor = gColorSpecular;

	const float3 halfVector = normalize(viewDirection + gLightDirection);
	const float specularStrength = saturate(dot(halfVector, -normal));
	const float specularValue = pow(specularStrength, gShininess);

	return specularColor * specularValue;
}

float3 CalculateSpecularPhong(float3 viewDirection, float3 normal)
{
	float3 specularColor = gColorSpecular;

	const float specularStrength = saturate(dot(reflect(-gLightDirection, normal), viewDirection));
	const float specularValue = pow(specularStrength, gShininess);

	return specularColor * specularValue;
}

float3 CalculateSpecular(float3 viewDirection, float3 normal, float2 texCoord)
{
	float3 specColor = float3(0,0,0);

	if(gUseSpecularBlinn)
	{
		specColor += saturate(CalculateSpecularBlinn(viewDirection, normal));
	}

	if(gUseSpecularPhong)
	{
		specColor += saturate(CalculateSpecularPhong(viewDirection, normal));
	}

	if (!gUseTextureSpecular)
	{
		return specColor;
	}

	return specColor * gTextureSpecular.Sample(gTextureSampler, texCoord).r;
}

float3 CalculateNormal(float3 tangent, float3 normal, float2 texCoord)
{
	if (!gUseTextureNormal)
	{
		return normal;
	}

	float3 binormal = normalize(cross(tangent, normal));
	float3x3 localAxis = float3x3(tangent, binormal, normal);

	float3 sampledNormal = gTextureNormal.Sample( gTextureSampler, texCoord);
	sampledNormal = sampledNormal * 2.f - float3(1.f, 1.f, 1.f);	// move from 0_1 range into -1_1 range

	// Apply the normal strength
    float3 transformedNormal = mul(sampledNormal, localAxis);

    // Blend the original normal with the transformed normal based on the normalStrength
    float3 blendedNormal = lerp(normal, transformedNormal, gNormalStrength);

    return normalize(blendedNormal);
}

float3 CalculateDiffuse(float3 normal, float2 texCoord)
{
	float3 diffuseColor = gColorDiffuse;

	if (gUseTextureDiffuse)
	{
		float diffuseStrength = dot(normal, -gLightDirection);
		diffuseColor = gColorDiffuse * saturate(diffuseStrength);
		diffuseColor *= gTextureDiffuse.Sample(gTextureSampler, texCoord);
	}

	return diffuseColor;
}

float CalculateOpacity(float2 texCoord)
{
	return 1 - gTextureOpacity.Sample(gTextureSampler, texCoord).r * gOpacityIntensity * gTextureOpacityIntensity;
}

float2 texOffset(int u, int v)
{
	//TODO: return offseted value (our shadow map has the following dimensions: 1280 * 720)
	return float2(float(u) / 1280.f, float(v) / 720.f);
}

float EvaluateShadowMap(float4 lpos)
{
	//TODO: complete
	//re-homogenize position after interpolation
    lpos.xyz /= lpos.w;
	
	//if position is not visible to the light - dont illuminate it
    //results in hard light frustum
    if (lpos.x < -1.0f || lpos.x > 1.0f ||
        lpos.y < -1.0f || lpos.y > 1.0f ||
        lpos.z <  0.0f || lpos.z > 1.0f)
        return 1.f;
	
	//transform clip space coords to texture space coords (-1:1 to 0:1)
    lpos.x = lpos.x / 2 + 0.5;
    lpos.y = lpos.y / -2 + 0.5;
	
	//apply shadow map bias
    lpos.z -= gShadowMapBias;
	
	
	    //if clip space z value greater than shadow map value then pixel is in shadow
	float sum = 0;
	for (float y = -1.5f; y <= 1.5f; y += 1.f)
	{
		for (float x = -1.5f; x <= 1.5f; x += 1.f)
		{
			sum += gShadowMap.SampleCmpLevelZero(cmpSampler, lpos.xy + texOffset(x, y), lpos.z);
		}
	}

	return (sum / 16.0f) * 0.5 + 0.5;
}


VS_Output MainVS(VS_Input input)
{
	VS_Output output = (VS_Output)0;

	output.Position = mul(float4(input.Position, 1.0), gWorldViewProj);
	output.WorldPosition = mul(float4(input.Position,1.0), gWorld);
	output.Normal = mul(input.Normal, (float3x3)gWorld);
	output.Tangent = mul(input.Tangent, (float3x3)gWorld);
	output.TexCoord = input.TexCoord;
    output.lPos = mul(float4(input.Position, 1.f),  gWorldViewProj_Light);

	return output;
}

float3 CalculateFresnelFalloff(float3 normal, float3 viewDirection, float3 environmentColor)
{
	if (!gUseFresnelFalloff)
	{
		return environmentColor;
	}
	if (!gUseEnvironmentMapping)
	{
		environmentColor = gColorFresnel;
	}

	float fresnelFalloff = dot(normal, viewDirection);
	fresnelFalloff = saturate(abs(fresnelFalloff));
	fresnelFalloff = 1 - fresnelFalloff;
	fresnelFalloff = pow(fresnelFalloff, gFresnelPower);
	fresnelFalloff = saturate(fresnelFalloff);
	fresnelFalloff *= gFresnelMultiplier;

	float fresnelMask = dot(float3(0, -1, 0), normal);
	fresnelMask = saturate(fresnelMask);
	fresnelMask = 1 - fresnelMask;
	fresnelMask = pow(fresnelMask, gFresnelHardness);

	fresnelFalloff = fresnelFalloff * fresnelMask;
	return environmentColor * fresnelFalloff;
}

float3 CalculateEnvironment(float3 viewDirection, float3 normal)
{
	if (!gUseEnvironmentMapping)
	{
		return float3(0, 0, 0);
	}

	float3 reflected = reflect(viewDirection, normal);
	float3 refracted = refract(viewDirection, normal, gRefractionIndex);

	reflected = gCubeEnvironment.Sample(gTextureSampler, reflected) * gReflectionStrength;
	refracted = gCubeEnvironment.Sample(gTextureSampler, refracted) * gRefractionStrength;

	return reflected + refracted;
}

float4 MainPS(VS_Output input) : SV_TARGET
{
	float shadowValue = EvaluateShadowMap(input.lPos);

	// NORMALIZE
	input.Normal = normalize(input.Normal);
	input.Tangent = normalize(input.Tangent);

	float3 viewDirection = normalize(input.WorldPosition.xyz - gMatrixViewInverse[3].xyz);

	//NORMAL
	float3 newNormal = CalculateNormal(input.Tangent, input.Normal, input.TexCoord);

	//SPECULAR
	float3 specColor = CalculateSpecular(viewDirection, newNormal, input.TexCoord);

	//DIFFUSE
	float3 diffColor = CalculateDiffuse(newNormal, input.TexCoord);
	
	//AMBIENT
	float3 ambientColor = gColorAmbient * gAmbientIntensity;

	//ENVIRONMENT MAPPING
	float3 environmentColor = CalculateEnvironment(viewDirection, newNormal);

	//FRESNEL FALLOFF
	environmentColor = CalculateFresnelFalloff(newNormal, viewDirection, environmentColor);

	//FINAL COLOR CALCULATION
	float3 finalColor = diffColor + specColor + environmentColor + ambientColor;

	//OPACITY4
	float opacity = CalculateOpacity(input.TexCoord);

	return float4(finalColor * shadowValue,opacity);
}

technique10 WithAlphaBlending
{
	pass p0
	{
		SetRasterizerState(Solid);
		SetBlendState(gBS_EnableBlending,float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader( NULL );
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
	}
}

technique10 WithoutAlphaBlending
{
	pass p0
	{
		SetRasterizerState(Solid);
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader( NULL );
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
	}
}
