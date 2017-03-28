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
	//float4 Sobel : SV_TARGET3;
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
