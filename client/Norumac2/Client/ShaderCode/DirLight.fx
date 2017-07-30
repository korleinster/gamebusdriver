#include "common.fx"

Texture2D<float> CascadeShadowMapTexture : register(t5);

// shader input/output structure
cbuffer cbDirLight : register(b1)
{
	float3 AmbientDown	: packoffset(c0);
	float3 AmbientRange	: packoffset(c1);
	float3 DirToLight		: packoffset(c2);
	float3 DirLightColor	: packoffset(c3);
	float4x4 ToShadowSpace		: packoffset(c4);
	float4x4 ShadowSpaceProjInv		: packoffset(c8);
}

// 각 버텍스의 클리핑 공간 위치를 가늠하기 위해 버텍스 셰이더에 상수 배열 정의
static const float2 arrBasePos[4] = {
	float2(-1.0, 1.0),
	float2(1.0, 1.0),
	float2(-1.0, -1.0),
	float2(1.0, -1.0),
};

// Vertex shader
struct VS_OUTPUT
{
	float4 Position : SV_Position; // vertex position
	float2 cpPos	: TEXCOORD0;
};

// 버텍스 인덱스를 취해 버텍스 투영위치와 클리핑 공간 xy위치를 반환
VS_OUTPUT DirLightVS(uint VertexID : SV_VertexID)
{
	VS_OUTPUT Output;

	Output.Position = float4(arrBasePos[VertexID].xy, 0.0, 1.0);
	Output.cpPos = Output.Position.xy;

	return Output;
}

// Pixel shaders
// Ambient light calculation helper function
float3 CalcAmbient(float3 normal, float3 color)
{
	// Convert from [-1, 1] to [0, 1]
	float up = normal.y * 0.5 + 0.5;

	// Calculate the ambient value
	float3 ambient = AmbientDown + up * AmbientRange;

	// Apply the ambient value to the color
	return ambient * color;
}

// Cascaded shadow calculation
float GetShadowAtt(float3 position)
{
	// Transform the world position to shadow space
	float4 posShadowSpace = mul(float4(position, 1.0), ToShadowSpace);

	posShadowSpace.xy = posShadowSpace.xy / posShadowSpace.w;

	float2 vShadowMapUV = float2(posShadowSpace.x * 0.5f + 0.5f, posShadowSpace.y * -0.5f + 0.5f);

	/*if (vShadowMapUV.x < -0.001f || vShadowMapUV.x > 1.001f || vShadowMapUV.y < -0.001f || vShadowMapUV.y > 1.001f)
	{
		return 1.0f;
	}*/

	float shadowMapDepth = CascadeShadowMapTexture.Sample(PointSampler, vShadowMapUV).r;

	float4 posShadowView = mul(posShadowSpace, ShadowSpaceProjInv);

	if (posShadowView.z > (shadowMapDepth * 500.0f) + 0.2f)
	{
		return 0.0f;
	}

	return 1.0f;
}

// Directional light calculation helper function
float3 CalcDirectional(float3 position, Material material)
{
	// Phong diffuse
	float NDotL = dot(DirToLight, material.normal);
	float3 finalColor = DirLightColor.rgb * saturate(NDotL);
	// 툰
	finalColor = ceil(finalColor * 10) / 10.f;

	// Blinn specular
	float3 ToEye = EyePosition - position;
	ToEye = normalize(ToEye);
	float3 HalfWay = normalize(ToEye + DirToLight);
	float NDotH = saturate(dot(HalfWay, material.normal));
	finalColor += DirLightColor.rgb * pow(NDotH, material.specPow) * material.specIntensity;

	// Take shadows into consideration

	return finalColor * material.diffuseColor.rgb * GetShadowAtt(position);
}

// 버텍스 셰이더에서 반환한 값 중 픽셀 텍스처 위치와 클리핑 공간 위치를 취해 해당 셰이더의 색상 값을 반환
float4 DirLightPS(VS_OUTPUT In) : SV_TARGET
{
	// GBuffer 언패킹
	SURFACE_DATA gbd = UnpackGBuffer_Loc(In.Position.xy);

	//if (gbd.LinearDepth > 50.f)
	//	return float4(0.8f, 0.8f, 0.8f, 1.f);

	if (gbd.BorderLine < 0.1f)
	{
		return float4(0.f, 0.f, 0.f, 1.f);
	}

	// 데이터를 재질 구조체로 반환
	Material mat;
	MaterialFromGBuffer(gbd, mat);

	// 월드 위치 복원
	float3 position = CalcWorldPos(In.cpPos, gbd.LinearDepth);

	// 앰비언트 라이트 비중 계산
	float3 finalColor = CalcAmbient(mat.normal, mat.diffuseColor.rgb);

	// 디렉셔널 라이트 비중 계산
	finalColor += CalcDirectional(position, mat);

	// Return the final color
	return float4(finalColor, 1.0);
}

float4 CascadeShadowDebugPS(VS_OUTPUT In) : SV_TARGET
{
	//// Unpack the GBuffer
	//SURFACE_DATA gbd = UnpackGBuffer_Loc(In.Position.xy);

	//// Reconstruct the world position
	//float3 position = CalcWorldPos(In.cpPos, gbd.LinearDepth);

	//// Transform the world position to shadow space
	//float4 posShadowSpace = mul(float4(position, 1.0), ToShadowSpace);

	//// Transform the shadow space position into each cascade position
	//float4 posCascadeSpaceX = (ToCascadeOffsetX + posShadowSpace.xxxx) * ToCascadeScale;
	//float4 posCascadeSpaceY = (ToCascadeOffsetY + posShadowSpace.yyyy) * ToCascadeScale;

	//// Check which cascade we are in
	//float4 inCascadeX = abs(posCascadeSpaceX) <= 1.0;
	//float4 inCascadeY = abs(posCascadeSpaceY) <= 1.0;
	//float4 inCascade = inCascadeX * inCascadeY;

	//// Prepare a mask for the highest quality cascade the position is in
	//float4 bestCascadeMask = inCascade;
	//bestCascadeMask.yzw = (1.0 - bestCascadeMask.x) * bestCascadeMask.yzw;
	//bestCascadeMask.zw = (1.0 - bestCascadeMask.y) * bestCascadeMask.zw;
	//bestCascadeMask.w = (1.0 - bestCascadeMask.z) * bestCascadeMask.w;

	//return 0.5 * bestCascadeMask;

	return float4(1.f, 1.f, 1.f, 1.f);
}
