// GBuffer textures and Samplers
Texture2D<float> DepthTexture         : register(t0);
Texture2D<float4> ColorSpecIntTexture : register(t1);
Texture2D<float3> NormalTexture       : register(t2);
Texture2D<float4> SpecPowTexture      : register(t3);
Texture2D<float4> BorderLineTexture   : register(t4);

SamplerState PointSampler             : register(s0); // 샘플러(슬롯 번호 0번과 연결)
SamplerComparisonState PCFSampler     : register(s2);

													  // constants
cbuffer cbGBufferUnpack : register(b0)
{
	float4 PerspectiveValues : packoffset(c0); // 퍼스펙티브
	float4x4 ViewInv         : packoffset(c1); // 퍼스펙티브
}

cbuffer cbFog : register(b2)
{
	float3 FogColor          : packoffset(c0);
	float FogStartDepth : packoffset(c0.w);
	float3 FogHighlightColor : packoffset(c1);
	float FogGlobalDensity : packoffset(c1.w);
	float3 FogSunDir	     : packoffset(c2);
	float FogStartHeight : packoffset(c2.w);
}

// 픽셀에서 카메라를 향하는 벡터(뷰 역행렬에 포함)
#define EyePosition (ViewInv[3].xyz)

static const float2 g_SpecPowerRange = { 10.0, 250.0 }; // 스펙큘러 정도 정규화

float3 DecodeNormal(float2 encodedNormal)
{
	float4 decodedNormal = encodedNormal.xyyy * float4(2, 2, 0, 0) + float4(-1, -1, 1, -1);
	decodedNormal.z = dot(decodedNormal.xyz, -decodedNormal.xyw);
	decodedNormal.xy *= sqrt(decodedNormal.z);
	return decodedNormal.xyz * 2.0 + float3(0.0, 0.0, -1.0);
}

// 샘플링된 깊이 값을 입력받아 선형 깊이 값을 반환
float ConvertZToLinearDepth(float depth)
{
	float linearDepth = PerspectiveValues.z / (depth + PerspectiveValues.w);
	return linearDepth;
}

// 클리핑 공간 위치와 선형 깊이 값을 취해 해당 깊이에 대한 본래의 월드 위치를 반환
float3 CalcWorldPos(float2 csPos, float depth)
{
	float4 position;

	position.xy = csPos.xy * PerspectiveValues.xy * depth;
	position.z = depth;
	position.w = 1.0;

	return mul(position, ViewInv).xyz;
}

// GBuffer값을 언팩한 후에 저장할 구조체
struct SURFACE_DATA
{
	float LinearDepth;
	float3 Color;
	float3 Normal;
	float SpecPow;
	float SpecIntensity;
	float BorderLine;
};

SURFACE_DATA UnpackGBuffer(float2 UV)
{
	SURFACE_DATA Out = (SURFACE_DATA)0;

	// sample 텍스쳐를 셈플링
	float depth = DepthTexture.Sample(PointSampler, UV.xy).x;
	Out.LinearDepth = ConvertZToLinearDepth(depth);

	float4 baseColorSpecInt = ColorSpecIntTexture.Sample(PointSampler, UV.xy);
	Out.Color = baseColorSpecInt.xyz;
	Out.SpecIntensity = baseColorSpecInt.w;

	Out.Normal = NormalTexture.Sample(PointSampler, UV.xy).xyz;
	Out.Normal = normalize(Out.Normal * 2.0 - 1.0);

	Out.SpecPow = SpecPowTexture.Sample(PointSampler, UV.xy).x;

	Out.BorderLine = BorderLineTexture.Sample(PointSampler, UV.xy).x;

	return Out;
}

// 메인 언패킹 함수
// 언패킹하는 픽셀에 대한 좌표 값을 입력받아 이에 대한 표면 데이터 구조체를 반환
SURFACE_DATA UnpackGBuffer_Loc(int2 location)
{
	SURFACE_DATA Out;

	// 로드 함수를 위한 3 컴포넌트 선언
	int3 location3 = int3(location, 0);

	// 깊이 값 추출 및 선형 깊이 값으로 변환
	float depth = DepthTexture.Load(location3).x;
	Out.LinearDepth = ConvertZToLinearDepth(depth);

	// 알베도 색상과 스펙큘러 세기 값 추출
	float4 baseColorSpecInt = ColorSpecIntTexture.Load(location3);
	Out.Color = baseColorSpecInt.xyz;
	Out.SpecIntensity = baseColorSpecInt.w;

	// 노멀 샘플링 후 전체 범위 변환 및 정규화
	Out.Normal = NormalTexture.Load(location3).xyz;
	Out.Normal = normalize(Out.Normal * 2.0 - 1.0);

	// 원래 범위 값에 대해 스펙큘러 파워 스케일 조정
	Out.SpecPow = SpecPowTexture.Load(location3).x;

	Out.BorderLine = BorderLineTexture.Load(location3).x;

	return Out;
}

// 재질 구조체
struct Material
{
	float3 normal;
	float4 diffuseColor;
	float specPow;
	float specIntensity;
};

void MaterialFromGBuffer(SURFACE_DATA gbd, inout Material mat)
{
	mat.normal = gbd.Normal;
	mat.diffuseColor.xyz = gbd.Color;
	mat.diffuseColor.w = 1.0; // 완전 불투명
	mat.specPow = g_SpecPowerRange.x + g_SpecPowerRange.y * gbd.SpecPow;
	mat.specIntensity = gbd.SpecIntensity;
}

float4 DebugLightPS() : SV_TARGET
{
	return float4(1.0, 1.0, 1.0, 1.0);
}