#include "common.fx"

// Constant Buffers
cbuffer cbPerObjectVS : register( b0 ) // Model vertex shader constants
{
    float4x4 WorldViewProjection	: packoffset( c0 );
	float4x4 World					: packoffset( c4 );
}

cbuffer cbPerObjectPS : register( b0 ) // Model pixel shader constants
{
	float specExp		: packoffset( c0 );
	float specIntensity	: packoffset( c0.y );
}

// Diffuse texture and linear sampler
Texture2D    DiffuseTexture	: register( t0 );
SamplerState LinearSampler	: register( s0 );

// shader input/output structure
struct VS_INPUT
{
    float4 Position	: POSITION;		// vertex position 
    float3 Normal	: NORMAL;		// vertex normal
    float2 UV		: TEXCOORD0;	// vertex texture coords 
};

struct VS_OUTPUT
{
    float4 Position	: SV_POSITION;	// vertex position 
    float2 UV		: TEXCOORD0;	// vertex texture coords
	float3 Normal	: TEXCOORD1;	// vertex normal
};

// Vertex shader
VS_OUTPUT RenderSceneVS( VS_INPUT input )
{
    VS_OUTPUT Output;
    float3 vNormalWorldSpace;
	
    // Transform the position from object space to homogeneous projection space
    Output.Position = mul( input.Position, WorldViewProjection );

    // Just copy the texture coordinate through
    Output.UV = input.UV; 

	// Transform the normal to world space
	Output.Normal = mul(input.Normal, (float3x3)World);
    
    return Output;    
}

// Pixel shader
struct PS_GBUFFER_OUT // GBuffer가 렌더링하는 순서대로 배열해 놓음
{
	float4 ColorSpecInt : SV_TARGET0;
	float4 Normal : SV_TARGET1;
	float4 SpecPow : SV_TARGET2;
};

// 알베도, 노멀, 스펙큘러 세기, 스펙큘러 정도를 입력받아 이를 포함한 GBuffer형식을 반환
PS_GBUFFER_OUT PackGBuffer(float3 BaseColor, float3 Normal, float SpecIntensity, float SpecPower)
{
	PS_GBUFFER_OUT Out;

	// 스펙큘러 파워 정규화
	float SpecPowerNorm = max(0.0001, (SpecPower - g_SpecPowerRange.x) / g_SpecPowerRange.y);

	// GBuffer 구조체에 데이터 패킹
	Out.ColorSpecInt = float4(BaseColor.rgb, SpecIntensity);
	Out.Normal = float4(Normal * 0.5 + 0.5, 0.0);
	Out.SpecPow = float4(SpecPowerNorm, 0.0, 0.0, 0.0);

	return Out;
}

PS_GBUFFER_OUT RenderScenePS( VS_OUTPUT In )
{ 
    // Lookup mesh texture and modulate it with diffuse
    float3 DiffuseColor = DiffuseTexture.Sample( LinearSampler, In.UV );
	DiffuseColor *= DiffuseColor;

	return PackGBuffer(DiffuseColor, normalize(In.Normal), specIntensity, specExp);
}

///////// 애니메이션 셰이더 //////////////////////////////////////////////

#define MAX		128

struct VS_ANI_IN
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 tex2dcoord : TEXCOORD0;
	uint4 Bones1 : BONES0;
	uint4 Bones2 : BONES1;
	float4 Weights1 : WEIGHTS0;
	float4 Weights2 : WEIGHTS1;
};

cbuffer cbBoneWorldMatrix : register(b1)
{
	row_major matrix gMtxBone[MAX] : packoffset(c0); // row_major 행 우선 행렬 = 전치행렬X
};

VS_OUTPUT RenderSceneVS_ANI(VS_ANI_IN input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	float4 Pos = float4(input.position, 1);

	uint iBone0 = input.Bones1.r;
	uint iBone1 = input.Bones1.g;
	uint iBone2 = input.Bones1.b;
	uint iBone3 = input.Bones1.a;
	uint iBone4 = input.Bones2.r;
	uint iBone5 = input.Bones2.g;
	uint iBone6 = input.Bones2.b;
	uint iBone7 = input.Bones2.a;

	float fWeight0 = input.Weights1.r;
	float fWeight1 = input.Weights1.g;
	float fWeight2 = input.Weights1.b;
	float fWeight3 = input.Weights1.a;
	float fWeight4 = input.Weights2.r;
	float fWeight5 = input.Weights2.g;
	float fWeight6 = input.Weights2.b;
	float fWeight7 = input.Weights2.a;

	matrix m0 = gMtxBone[iBone0];
	matrix m1 = gMtxBone[iBone1];
	matrix m2 = gMtxBone[iBone2];
	matrix m3 = gMtxBone[iBone3];
	matrix m4 = gMtxBone[iBone4];
	matrix m5 = gMtxBone[iBone5];
	matrix m6 = gMtxBone[iBone6];
	matrix m7 = gMtxBone[iBone7];

	if (fWeight0 > 0) output.Position += fWeight0 * mul(Pos, m0);
	if (fWeight1 > 0) output.Position += fWeight1 * mul(Pos, m1);
	if (fWeight2 > 0) output.Position += fWeight2 * mul(Pos, m2);
	if (fWeight3 > 0) output.Position += fWeight3 * mul(Pos, m3);
	if (fWeight4 > 0) output.Position += fWeight4 * mul(Pos, m4);
	if (fWeight5 > 0) output.Position += fWeight5 * mul(Pos, m5);
	if (fWeight6 > 0) output.Position += fWeight6 * mul(Pos, m6);
	if (fWeight7 > 0) output.Position += fWeight7 * mul(Pos, m7);

	// Transform the position from object space to homogeneous projection space
	output.Position = mul(output.Position, WorldViewProjection);

	// Just copy the texture coordinate through
	output.UV = input.tex2dcoord;

	// Transform the normal to world space

	float3 Normal = input.normal;

	if (fWeight0 > 0) output.Normal += fWeight0 * mul(Normal, (float3x3)m0);
	if (fWeight1 > 0) output.Normal += fWeight1 * mul(Normal, (float3x3)m1);
	if (fWeight2 > 0) output.Normal += fWeight2 * mul(Normal, (float3x3)m2);
	if (fWeight3 > 0) output.Normal += fWeight3 * mul(Normal, (float3x3)m3);
	if (fWeight4 > 0) output.Normal += fWeight4 * mul(Normal, (float3x3)m4);
	if (fWeight5 > 0) output.Normal += fWeight5 * mul(Normal, (float3x3)m5);
	if (fWeight6 > 0) output.Normal += fWeight6 * mul(Normal, (float3x3)m6);
	if (fWeight7 > 0) output.Normal += fWeight7 * mul(Normal, (float3x3)m7);

	output.Normal = mul(output.Normal, (float3x3)World);

	return output;
}
