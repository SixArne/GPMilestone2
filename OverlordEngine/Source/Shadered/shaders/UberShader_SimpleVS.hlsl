float4x4 gMatrixWorldViewProj: WORLDVIEWPROJECTION;
float4x4 gMatrixWorld: WORLD;

float4 gColorDiffuse: COLOR = float4(1.0, 0.0, 0.0, 1.0);


cbuffer cbPerFrame : register(b0)
{
	float4x4 matVP;
	float4x4 matGeo;
	float4x4 matW;
};

struct VSInput
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float3 Tangent: TANGENT;
	float2 TexCoord: TEXCOORD0;
};

struct VSOutput
{
	float4 Position : SV_POSITION;
	float4 WorldPos: COLOR0;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float2 TexCoord: TEXCOORD0;
};

VSOutput main(VSInput vin)
{
	VSOutput vout = (VSOutput)0;

	vout.Position = mul(float4(vin.Position, 1.0f), gMatrixWorldViewProj);
	vout.WorldPos = mul(float4(vin.Position, 1.0f), gMatrixWorld);
	vout.Normal = mul(vin.Normal, (float3x3)gMatrixWorld);
	vout.Tangent = mul(vin.Tangent, (float3x3)gMatrixWorld);
	vout.TexCoord = vin.TexCoord;


	return vout;
}
