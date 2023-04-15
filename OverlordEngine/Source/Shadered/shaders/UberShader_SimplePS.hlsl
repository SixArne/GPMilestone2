//Texture2D gTextureDiffuse;
float3 gLightDirection: DIRECTION = float3(1,0,0);


// Diffuse
bool gUseDiffuseTexture = false;
float4 gColorDiffuse = float4(0.2, 0.4, 0.7, 1);
Texture2D gDiffuseTexture : register(t0);

// Specular
bool gUseSpecularTexture = false;
bool gUseSpecularBlin = false;
bool gUseSpecularPhong = false;
float4 gColorSpecular = float4(1,1,1,1);
Texture2D gSpecularIntensity : register(t1);
int gShininess = 15;

// Ambient
float4 gColorAmbient: COLOR = float4(0,0,0,1);
float gAmbientIntensity = float(1);

// Normal mapping
bool gUseNormalTexture = false;
bool gFlipGreenChannel = false;
Texture2D gNormalTexture: register(t2);

// Fresnel
bool gUseFresnelFalloff = false;
float4 gColorFresnel = float4(1,1,1,1);
float gFresnelPower = 1.0f;
float gFresnelMultiplier = 1.0f;
float gFresnelHardness = 0.f;

// Opacity
bool gUseOpacityTexture = false;
float gOpacityIntensity = 1.0f;
Texture2D gOpacityTexture: register(t3);

/*
SamplerState gSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};
*/

SamplerState gSampler : register(s0);


struct PSInput
{
	float4 Position : SV_POSITION;
	float4 WorldPos: COLOR0;
	float3 Normal : NORMAL;
	float3 Tangent: TANGENT;
	float2 TexCoord: TEXCOORD0;
};

float4 main(PSInput pin) : SV_TARGET
{
	pin.Normal = normalize(pin.Normal);
	pin.Tangent = normalize(pin.Tangent);
	
	


	float diffuseStrength = dot(pin.Normal, -gLightDirection);
	float4 diffuse = gColorDiffuse * saturate(diffuseStrength);
	diffuse *= gDiffuseTexture.Sample(gSampler, pin.TexCoord);

	float4 ambient = gColorAmbient * gAmbientIntensity;

	return diffuse + ambient;

}
