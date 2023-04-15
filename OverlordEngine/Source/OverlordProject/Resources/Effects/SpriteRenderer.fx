float4x4 gTransform : WorldViewProjection;
Texture2D gSpriteTexture;
float2 gTextureSize;

SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};

BlendState EnableBlending
{
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
};

DepthStencilState NoDepth
{
    DepthEnable = FALSE;
};

RasterizerState BackCulling
{
    CullMode = BACK;
};

struct VS_DATA
{
    uint TextureId : TEXCOORD0; //Can be ignored
    float4 TransformData : POSITION; //PosX, PosY, Depth (PosZ), Rotation
    float4 TransformData2 : POSITION1; //PivotX, PivotY, ScaleX, ScaleY
    float4 Color : COLOR;
};

struct GS_DATA
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD0;
};

VS_DATA MainVS(VS_DATA input)
{
    return input;
}

void CreateVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float4 col, float2 texCoord, float rotation, float2 rotCosSin, float2 offset, float2 pivotOffset)
{
    if (rotation != 0)
    {
		//Step 3.
		
		
        //Transform to origin
        pos.x = pos.x - pivotOffset.x - offset.x;
		pos.y = pos.y - pivotOffset.y - offset.y;

		float x = pos.x;
		float y = pos.y;

        //Do rotation calculations
		pos.x = x * rotCosSin.x - y * rotCosSin.y;
		pos.y = y * rotCosSin.x + x * rotCosSin.y;

        // Offset
		pos.x = pos.x + offset.x;
		pos.y = pos.y + offset.y;
    }
    else
    {
		//Step 2.
		//No rotation calculations (no need to do the rotation calculations if there is no rotation applied > redundant operations)
		//Just apply the pivot offset
        pos.xy -= pivotOffset.xy;
    }

	//Geometry Vertex Output
    GS_DATA geomData = (GS_DATA) 0;
    geomData.Position = mul(float4(pos, 1.0f), gTransform);
    geomData.Color = col;
    geomData.TexCoord = texCoord;
    triStream.Append(geomData);
}

[maxvertexcount(4)]
void MainGS(point VS_DATA vertex[1], inout TriangleStream<GS_DATA> triStream)
{
	//Given Data (Vertex Data)
    float3 position = vertex[0].TransformData.xyz; //Extract the position data from the VS_DATA vertex struct
    float2 offset = vertex[0].TransformData.xy; //Extract the offset data from the VS_DATA vertex struct (initial X and Y position)
    float rotation = vertex[0].TransformData.w; //Extract the rotation data from the VS_DATA vertex struct
    float2 pivot = vertex[0].TransformData2.xy; //Extract the pivot data from the VS_DATA vertex struct
    float2 scale = vertex[0].TransformData2.zw; //Extract the scale data from the VS_DATA vertex struct
    float2 texCoord = float2(0, 0); //Initial Texture Coordinate
	
	//...

	// LT----------RT //TringleStrip (LT > RT > LB, LB > RB > RT)
	// |          / |
	// |       /    |
	// |    /       |
	// | /          |
	// LB----------RB
    const float2 scaledSize = float2(gTextureSize.x * scale.x, gTextureSize.y * scale.y);
    float2 pivotOffset = pivot * scaledSize;

    float2 rotCosSin = float2(0, 0);
    if (rotation != 0)
    {
        rotCosSin.x = cos(rotation);
        rotCosSin.y = sin(rotation);
    }


	//VERTEX 1 [LT]
    CreateVertex(triStream, position, float4(1, 1, 1, 1), texCoord, rotation, rotCosSin, offset, pivotOffset); //Change the color data too!

	//VERTEX 2 [RT]
    float3 pos1 = position;
    pos1.x += scaledSize.x;
    texCoord = float2(1.f, 0.f);
    CreateVertex(triStream, pos1, float4(1, 1, 1, 1), texCoord, rotation, rotCosSin, offset, pivotOffset); //Change the color data too!

	//VERTEX 3 [LB]
    float3 pos2 = position;
    pos2.y += scaledSize.y;
    texCoord = float2(0.f, 1.f);
    CreateVertex(triStream, pos2, float4(1, 1, 1, 1), texCoord, rotation, rotCosSin, offset, pivotOffset); //Change the color data too!

	//VERTEX 4 [RB]   
    float3 pos3 = position;
    pos3.x += scaledSize.x;
    pos3.y += scaledSize.y;
    texCoord = float2(1.f, 1.f);
    CreateVertex(triStream, pos3, float4(1, 1, 1, 1), texCoord, rotation, rotCosSin, offset, pivotOffset); //Change the color data too!
}

float4 MainPS(GS_DATA input) : SV_TARGET
{
    return gSpriteTexture.Sample(samPoint, input.TexCoord) * input.Color;
}

technique11 Default
{
    pass p0
    {
        SetRasterizerState(BackCulling);
        SetBlendState(EnableBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_4_0, MainVS()));
        SetGeometryShader(CompileShader(gs_4_0, MainGS()));
        SetPixelShader(CompileShader(ps_4_0, MainPS()));
    }
}
