Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer cbLinearDepth : register(b0)
{
	float4 PerspectiveValues : packoffset(c0); // 퍼스펙티브
}

float g_fOffSetX = 1.f / 1024.f;
float g_fOffSetY = 1.f / 768.f;

float3x3 g_fx = 
{
-1,  0,  1,
-1,  0,  1,
-1,  0,  1 };

float3x3 g_fy =
{
-1, -1, -1,
0,  0,  0,
1,  1,  1 };

float3x3 g_fxy = 
{ 
-1, -1,  0,
-1,  0,  1,
0,  1,  1 };

float3x3 g_fyx = {
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
	float hereLinearDepth = (PerspectiveValues.z / (hereDepth + PerspectiveValues.w)) / 1000.f;

	float fAccDepthX = 0.0f;
	float fAccDepthY = 0.0f;
	float fAccDepthXY = 0.0f;
	float fAccDepthYX = 0.0f;

	for (int y = -1; y <= 1; ++y)
	{
		for (int x = -1; x <= 1; ++x)
		{
			float2 Offset = float2(x * g_fOffSetX, y * g_fOffSetY);

			float linearDepth = (PerspectiveValues.z / (txDiffuse.Sample(samLinear, In.UV + Offset).r + PerspectiveValues.w)) / 1000.f;
			//float linearDepth = txDiffuse.Sample(samLinear, In.UV + Offset).r;

			fAccDepthX += linearDepth * g_fx[y + 1][x + 1];
			fAccDepthY += linearDepth * g_fy[y + 1][x + 1];
			fAccDepthXY += linearDepth * g_fxy[y + 1][x + 1];
			fAccDepthYX += linearDepth * g_fyx[y + 1][x + 1];
		}
	}

	float fSobel = abs(fAccDepthX) + abs(fAccDepthY) + abs(fAccDepthXY) + abs(fAccDepthYX);

	if (0.999f < hereLinearDepth || fSobel <= PerspectiveValues.x)
	{
		return float4(1.f, 1.f, 1.f, 1.f);
	}
	
	return float4(0.f, 0.f, 0.f, 1.f);
}
