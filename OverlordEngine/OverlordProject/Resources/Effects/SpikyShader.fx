//************
// VARIABLES *
//************
float4x4 m_MatrixWorldViewProj : WORLDVIEWPROJECTION;
float4x4 m_MatrixWorld : WORLD;
float3 m_LightDirection : DIRECTION
<
	string Object="TargetLight";
> = float3(0.577f, -0.577f, 0.577f);

float3 gColorDiffuse : COLOR = float3(1,1,1);

float gSpikeLength
<
	string UIWidget="Slider";
	float UIMin=0.0f;
	float UIMax=0.5f;
	float UIStep=0.0001f;
> = 0.2f;

RasterizerState FrontCulling 
{ 
	CullMode = FRONT; 
};

//**********
// STRUCTS *
//**********
struct VS_DATA
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
};

struct GS_DATA
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL;
};

//****************
// VERTEX SHADER *
//****************
VS_DATA MainVS(VS_DATA vsData)
{
    return vsData;
}

//******************
// GEOMETRY SHADER *
//******************
void CreateVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float3 normal)
{
	//Step 1. Create a GS_DATA object
    GS_DATA output = (GS_DATA)0;

	//Step 2. Transform the position using the WVP Matrix and assign it to (GS_DATA object).Position (Keep in mind: float3 -> float4)
	output.Position = mul(float4(pos,1),m_MatrixWorldViewProj);
    
    //Step 3. Transform the normal using the World Matrix and assign it to (GS_DATA object).Normal (Only Rotation, No translation!)
	output.Normal = mul(normal,(float3x3)m_MatrixWorld);
    
    //Step 4. Append (GS_DATA object) to the TriangleStream parameter (TriangleStream::Append(...))
    triStream.Append(output);
}

[maxvertexcount(9)]
void SpikeGenerator(triangle VS_DATA vertices[3], inout TriangleStream<GS_DATA> triStream)
{
	//Use these variable names
    float3 basePoint, top, left, right, spikeNormal;

	//Step 1. Calculate CENTER_POINT
    const float3 v1 = vertices[0].Position;
    const float3 v2 = vertices[1].Position;
    const float3 v3 = vertices[2].Position;

    basePoint = (v1 + v2 + v3) / 3;

	//Step 2. Calculate Face Normal (Original Triangle)
    const float3 a = v1 - v2;
    const float3 b = v3 - v2;
    spikeNormal = normalize(cross(a, b));

	//Step 3. Offset CENTER_POINT (use gSpikeLength)
    basePoint = basePoint + (spikeNormal * gSpikeLength);
    
	//Step 4 + 5. Calculate Individual Face Normals (Cross Product of Face Edges) & Create Vertices for every face

        //FACE 1
    float3 fa = basePoint - v2;
    float3 fb = v3 - v2;
    spikeNormal = normalize(cross(fa, fb));

    CreateVertex(triStream, v2, spikeNormal);
    CreateVertex(triStream, v3, spikeNormal);
    CreateVertex(triStream, basePoint, spikeNormal);

        //Restart Strip! >> We want to start a new triangle (= interrupt trianglestrip)
    triStream.RestartStrip();

        //FACE 2
    fa = basePoint - v3;
    fb = v1 - v3;
    spikeNormal = normalize(cross(fa, fb));
    CreateVertex(triStream, v3, spikeNormal);
    CreateVertex(triStream, v1, spikeNormal);
    CreateVertex(triStream, basePoint, spikeNormal);

        //...
    triStream.RestartStrip();

        //Face 3
    fa = basePoint - v1;
    fb = v2 - v1;
    spikeNormal = normalize(cross(fa, fb));
    CreateVertex(triStream, v1, spikeNormal);
    CreateVertex(triStream, v2, spikeNormal);
    CreateVertex(triStream, basePoint, spikeNormal);
        //faceNormal1 = ...
        //CreateVertex(triStream, ...)
        //CreateVertex(triStream, ...)
        //CreateVertex(triStream, ...)

        //Restart Strip! >> We want to start a new triangle (= interrupt trianglestrip)
        //(TriangleStream::RestartStrip())

        //FACE 2
        //...

        //...

        //Face 3
        //...

    //Step 6. Complete code in CreateVertex(...)
    //Step 7. Bind this Geometry Shader function to the effect pass (See Technique Struct)
}

//***************
// PIXEL SHADER *
//***************
float4 MainPS(GS_DATA input) : SV_TARGET
{
    input.Normal=normalize(input.Normal);
	float diffuseStrength = max(dot(normalize(m_LightDirection),-input.Normal),0.2f); 
	return float4(gColorDiffuse*diffuseStrength,1.0f);
}

//*************
// TECHNIQUES *
//*************
technique11 Default //FXComposer >> Rename to "technique10 Default"
{
    pass p0
    {
        SetRasterizerState(FrontCulling);
        SetVertexShader(CompileShader(vs_4_0, MainVS()));
        SetGeometryShader(CompileShader(gs_4_0, SpikeGenerator()));
        SetPixelShader(CompileShader(ps_4_0, MainPS()));
    }
}