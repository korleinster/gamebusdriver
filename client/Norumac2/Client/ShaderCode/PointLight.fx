#include "common.fx"

// constants
cbuffer cbPointLightDomain : register( b0 )
{
	float4x4 LightProjection : packoffset( c0 );
}

cbuffer cbPointLightPixel : register( b1 )
{
	float3 PointLightPos : packoffset( c0 );
	float PointLightRangeRcp : packoffset( c0.w );
	float3 PointColor : packoffset( c1 );
	float2 LightPerspectiveValues : packoffset( c2 );
}

// Vertex shader
// 입력을 받지 않고 위치 값 하나를 출력
float4 PointLightVS() : SV_Position
{
    return float4(0.0, 0.0, 0.0, 1.0); 
}

// Hull shader
struct HS_CONSTANT_DATA_OUTPUT
{
	float Edges[4] : SV_TessFactor;
	float Inside[2] : SV_InsideTessFactor;
};

// 입력을 받지 않고 패치에 적용하기 원하는 테셀레이션 값에 대한 구조체를 출력
HS_CONSTANT_DATA_OUTPUT PointLightConstantHS()
{
	HS_CONSTANT_DATA_OUTPUT Output;
	
	float tessFactor = 18.0;
	Output.Edges[0] = Output.Edges[1] = Output.Edges[2] = Output.Edges[3] = tessFactor;
	Output.Inside[0] = Output.Inside[1] = tessFactor;

	return Output;
}

struct HS_OUTPUT
{
	float3 HemiDir : POSITION;
};
// 버텍스 셰이더와 마찬가지로 헐 셰이더는 구의 양면을 오가기 위해서 위치 값을 출력해야한다.
static const float3 HemilDir[2] = {
	float3(1.0, 1.0,1.0),
	float3(-1.0, 1.0, -1.0)
};

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PointLightConstantHS")]
HS_OUTPUT PointLightHS(uint PatchID : SV_PrimitiveID)
{
	HS_OUTPUT Output;

	Output.HemiDir = HemilDir[PatchID];

	return Output;
}

// Domain Shader
struct DS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 cpPos	: TEXCOORD0;
};


// 테셀레이션 과정에서 생성된 UV좌표를 입력받아 버텍스의 투영 또는 클리핑 공간 위치를 반환한다
[domain("quad")]
DS_OUTPUT PointLightDS( HS_CONSTANT_DATA_OUTPUT input, float2 UV : SV_DomainLocation, const OutputPatch<HS_OUTPUT, 4> quad)
{
	// UV를 클리핑 공간으로 변환
	float2 posClipSpace = UV.xy * 2.0 - 1.0;

	// 중심에서 가장 먼 지점의 절댓값 거리 계산
	float2 posClipSpaceAbs = abs(posClipSpace.xy);
	float maxLen = max(posClipSpaceAbs.x, posClipSpaceAbs.y);

	// 클리핑 공간에서의 최종 위치 생성
	float3 normDir = normalize(float3(posClipSpace.xy, (maxLen - 1.0)) * quad[0].HemiDir);
	float4 posLS = float4(normDir.xyz, 1.0);
	
	// 프로젝션 공간에 대해 모두 변환한 후
	// UV 좌표 생성
	DS_OUTPUT Output;
	Output.Position = mul( posLS, LightProjection );

	// 클리핑 공간 위치 저장
	Output.cpPos = Output.Position.xy / Output.Position.w;

	return Output;
}

// Pixel shader
float3 CalcPoint(float3 position, Material material, bool bUseShadow)
{
   float3 ToLight = PointLightPos - position;
   float3 ToEye = EyePosition - position;
   float DistToLight = length(ToLight);
   
   // Phong diffuse
   ToLight /= DistToLight; // Normalize
   float NDotL = saturate(dot(ToLight, material.normal));
   float3 finalColor = material.diffuseColor.rgb * NDotL;
   // 툰
   finalColor = ceil(finalColor * 10) / 10.f;
   
   // Blinn specular
   ToEye = normalize(ToEye);
   float3 HalfWay = normalize(ToEye + ToLight);
   float NDotH = saturate(dot(HalfWay, material.normal));
   finalColor += pow(NDotH, material.specPow) * material.specIntensity;

   // Attenuation
   float DistToLightNorm = 1.0 - saturate(DistToLight * PointLightRangeRcp);
   float Attn = DistToLightNorm * DistToLightNorm;
   finalColor *= PointColor.rgb * Attn;
   
   return finalColor;
}

float4 PointLightCommonPS(DS_OUTPUT In, bool bUseShadow) : SV_TARGET
{
	// GBuffer 언패킹
	SURFACE_DATA gbd = UnpackGBuffer_Loc(In.Position.xy);
	
	// 재질 구조체로 데이터 변환
	Material mat;
	MaterialFromGBuffer(gbd, mat);

	// 월드 위치 복원
	float3 position = CalcWorldPos(In.cpPos, gbd.LinearDepth);

	// 조명 분포 계산
	float3 finalColor = CalcPoint(position, mat, bUseShadow);

	// return the final color
	return float4(finalColor, 1.0);
}

float4 PointLightPS(DS_OUTPUT In) : SV_TARGET
{
	return PointLightCommonPS(In, false);
}

float4 PointLightShadowPS(DS_OUTPUT In) : SV_TARGET
{
	return PointLightCommonPS(In, true);
}