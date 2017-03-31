#include "common.fx"

//// 버텍스 위치들을 컨트롤
//static float2 arrOffsets[4] = {
//	float2(-1, 3),
//	float2(-0.0, 0.0),//float2(-0.68, 0.89),
//	float2(-1, 3),
//	float2(-1, 3),
//};
//// 버텍스 위치
//static const float2 arrBasePos[4] = {
//	float2(10.0, 10.0),
//	float2(10.0, -10.0),
//	float2(-10.0, 10.0),
//	float2(-10.0, -10.0),
//};

// 버텍스 위치들을 컨트롤(평상시)
static float2 arrOffsets[4] = {
	float2(-0.89, 0.89),
	float2(-0.68, 0.89),
	float2(-0.47, 0.89),
	float2(-0.26, 0.89),
};

// 버텍스 위치(평상시)
static const float2 arrBasePos[4] = {
	float2(1.0, 1.0),
	float2(1.0, -1.0),
	float2(-1.0, 1.0),
	float2(-1.0, -1.0),
};



// uv
static const float2 arrUV[4] = {
	float2(1.0, 0.0),
	float2(1.0, 1.0),
	float2(0.0, 0.0),
	float2(0.0, 1.0),
};

static const float4 arrMask[4] = {
	float4(1.0, 0.0, 0.0, 0.0),
	float4(0.0, 1.0, 0.0, 0.0),
	float4(0.0, 0.0, 1.0, 0.0),
	float4(0.0, 0.0, 0.0, 1.0),
};

struct VS_OUTPUT
{
	float4 Position	: SV_Position; // 버텍스 포지션 
	float2 UV		: TEXCOORD0;   // 버텍스 texture coords
	float4 sampMask	: TEXCOORD1;
};

VS_OUTPUT GBufferVisVS(uint VertexID : SV_VertexID)
{
	VS_OUTPUT Output;

	// arrBasePos에다가 * 0.1을 함으로써 화면크기에 0.1크기만큼으로 만듬
	// [n % 4] - 0, 1, 2, 3 을 계속 반복
	// [n / 4] - 기본적으로 내림이기 때문에 결과가 4번씩 같은값이 나오면서 계속 1씩 증가 ex) 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2 ....
	Output.Position = float4(arrBasePos[VertexID % 4].xy * 0.1 + arrOffsets[VertexID / 4], 0.0, 1.0);

	// UV
	Output.UV = arrUV[VertexID % 4].xy; 
	
	// ??????
	Output.sampMask = arrMask[VertexID / 4].xyzw;

	return Output;
}

float4 GBufferVisPS(VS_OUTPUT In) : SV_TARGET // SV_Target이라는 의미소는 이 함수의 반환값 형식이 렌더 대상 형식과 일치해야함을 뜻한다
{
	SURFACE_DATA gbd = UnpackGBuffer(In.UV.xy);
	float4 finalColor = float4(0.0, 0.0, 0.0, 1.0);

	// saturate : 0보다 작으면 0, 1보다 크면 1, 그외는 그값
	finalColor += float4(1.0 - saturate(gbd.LinearDepth / 75.0), 1.0 - saturate(gbd.LinearDepth / 125.0), 1.0 - saturate(gbd.LinearDepth / 200.0), 0.0) * In.sampMask.xxxx;
	finalColor += float4(gbd.Color.xyz, 0.0) * In.sampMask.yyyy;
	finalColor += float4(gbd.Normal.xyz * 0.5 + 0.5, 0.0) * In.sampMask.zzzz;
	finalColor += float4(gbd.SpecIntensity, gbd.SpecPow, 0.0, 0.0) * In.sampMask.wwww;

	return finalColor;
}

VS_OUTPUT TextureVisVS(uint VertexID : SV_VertexID)
{
	VS_OUTPUT Output;

	Output.Position = float4(arrBasePos[VertexID].xy, 0.0, 1.0);
	Output.UV = arrUV[VertexID].xy;
	Output.sampMask = 0;

	return Output;
}

float4 TextureVisPS(VS_OUTPUT In) : SV_TARGET
{
	return float4(DepthTexture.Sample(PointSampler, In.UV.xy).x, 0.0, 0.0, 1.0);
}
