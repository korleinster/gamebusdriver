Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer cbLinearDepth : register(b0)
{
	float4 PerspectiveValues : packoffset(c0); // 퍼스펙티브
}

static const float2 g_fOffSet = float2(1 / 1024.f, 1 / 768.f);

static const float3x3 g_fx = 
{
-1,  0,  1,
-1,  0,  1,
-1,  0,  1 };

static const float3x3 g_fy =
{
-1, -1, -1,
0,  0,  0,
1,  1,  1 };

static const float3x3 g_fxy =
{ 
-1, -1,  0,
-1,  0,  1,
0,  1,  1 };

static const float3x3 g_fyx = {
0,  1,  1,
-1,  0,  1,
-1, -1,  0 };

struct VS_INPUT
{
	float4 Pos : POSITION;
	float2 UV : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 Pos	: SV_Position; // 버텍스 포지션 
	float2 UV	: TEXCOORD0;   // 버텍스 texture coords
};

VS_OUTPUT BorderLineVS(VS_INPUT Input/*uint VertexID : SV_VertexID*/)
{
	VS_OUTPUT Output;
	
	Output.Pos = float4(Input.Pos.xy, 0.0f, 1.0f);
	Output.UV = Input.UV;

	return Output;
}

float4 BorderLinePS(VS_OUTPUT In) : SV_TARGET
{
	float hereDepth = txDiffuse.Sample(samLinear, In.UV).r;
	float hereLinearDepth = (PerspectiveValues.z / (hereDepth + PerspectiveValues.w));


	//return float4(g_fx[0][2], g_fx[0][1], g_fx[0][0], 1.f);
	//return float4 (0.0000976f, 0.0013021f, txDiffuse.Sample(samLinear, In.UV).r, 1.f);

	/*
	float fFar = (PerspectiveValues.z / (txDiffuse.Sample(samLinear, float2(0.f, 0.f)).r + PerspectiveValues.w)) * 0.01f;
	float fNear = (PerspectiveValues.z / (txDiffuse.Sample(samLinear, float2(1.f, 1.f)).r + PerspectiveValues.w)) * 0.01f;
	return float4(fFar, fNear, 0.f, 1.f);
	*/

	float fAccDepthX = 0.0f;
	float fAccDepthY = 0.0f;
	float fAccDepthXY = 0.0f;
	float fAccDepthYX = 0.0f;

	for (int y = -1; y <= 1; ++y)
	{
		for (int x = -1; x <= 1; ++x)
		{
			float2 Offset = float2(x * g_fOffSet.x, y * g_fOffSet.y);

			float linearDepth = ((PerspectiveValues.z / (txDiffuse.Sample(samLinear, float2(In.UV + Offset)).r + PerspectiveValues.w)) - 10.f) * 0.025f;
			//return float4 (g_fOffSet.x, g_fOffSet.y, txDiffuse.Sample(samLinear, In.UV).r, 1.f);
			//return float4(linearDepth, 0.f, 0.f, 1.f);

			fAccDepthX += linearDepth * g_fx[y + 1][x + 1];
			fAccDepthY += linearDepth * g_fy[y + 1][x + 1];
			fAccDepthXY += linearDepth * g_fxy[y + 1][x + 1];
			fAccDepthYX += linearDepth * g_fyx[y + 1][x + 1];
		}
	}

	//return float4((hereLinearDepth - 10.f) * 0.025f, 0.f, 0.f, 1.f);

	float fSobel = abs(fAccDepthX) + abs(fAccDepthY) + abs(fAccDepthXY) + abs(fAccDepthYX);

	//return float4(fSobel, 0.f, 0.f, 1.f);

	
	if (fSobel >= PerspectiveValues.x)
	{
		return float4(0.f, 0.f, 0.f, 1.f);
	}

	return float4(1.f, 1.f, 1.f, 1.f);	
	
}
