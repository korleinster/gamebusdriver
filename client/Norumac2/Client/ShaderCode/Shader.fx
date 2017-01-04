#define MAX 128

Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer ConstantBuffer : register( b0 ) //컨스턴트 버퍼
{
	matrix World;
	matrix View;
	matrix Projection;
}

cbuffer cbBoneWorldMatrix : register( b1 )
{
	row_major matrix Bone[MAX];
};

struct VS_INPUT
{
	float4 Pos : POSITION;
	float3 Norm : NORMAL;
	float2 Tex : TEXCOORD0;
	uint4 Bones1 : BONES0;
	uint4 Bones2 : BONES1;
	float4 Weights1 : WEIGHTS0;
	float4 Weights2 : WEIGHTS1;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
};


VS_OUT VS(VS_INPUT input)
{
	VS_OUT output = (VS_OUT)0;
	output.Pos = mul(input.Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	output.Tex = input.Tex;

	return output;
}

VS_OUT VS_Logo(VS_INPUT input)
{
	VS_OUT output = (VS_OUT)0;
	output.Pos = input.Pos;
	output.Tex = input.Tex;
	return output;
}

VS_OUT VS_ANI(VS_INPUT input)
{
	VS_OUT output = (VS_OUT)0;

	float4 Pos = input.Pos + float4(0.f,0.f,0.f,1.f);

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

	matrix m0 = Bone[iBone0];
	matrix m1 = Bone[iBone1];
	matrix m2 = Bone[iBone2];
	matrix m3 = Bone[iBone3];
	matrix m4 = Bone[iBone4];
	matrix m5 = Bone[iBone5];
	matrix m6 = Bone[iBone6];
	matrix m7 = Bone[iBone7];

	if (fWeight0 > 0) output.Pos += fWeight0 * mul(Pos, m0);
	if (fWeight1 > 0) output.Pos += fWeight1 * mul(Pos, m1);
	if (fWeight2 > 0) output.Pos += fWeight2 * mul(Pos, m2);
	if (fWeight3 > 0) output.Pos += fWeight3 * mul(Pos, m3);
	if (fWeight4 > 0) output.Pos += fWeight4 * mul(Pos, m4);
	if (fWeight5 > 0) output.Pos += fWeight5 * mul(Pos, m5);
	if (fWeight6 > 0) output.Pos += fWeight6 * mul(Pos, m6);
	if (fWeight7 > 0) output.Pos += fWeight7 * mul(Pos, m7);

	/*matrix matAll = mul(World, View);
	matAll = mul(matAll, Projection);


	output.Pos = mul(output.Pos, matAll);
	output.Tex = input.Tex;*/


	output.Pos = mul(output.Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	output.Tex = input.Tex;

	return output;
}


float4 PS(VS_OUT input) : SV_Target
{
	return txDiffuse.Sample(samLinear, input.Tex);
}







////--------------------------------------------------------------------------------------
//// File: Tutorial02.fx
////
//// Copyright (c) Microsoft Corporation. All rights reserved.
////--------------------------------------------------------------------------------------
//
////--------------------------------------------------------------------------------------
//// Vertex Shader
////--------------------------------------------------------------------------------------
//float4 VS(float4 Pos : POSITION) : SV_POSITION
//{
//	return Pos;
//}
//
//
////--------------------------------------------------------------------------------------
//// Pixel Shader
////--------------------------------------------------------------------------------------
//float4 PS(float4 Pos : SV_POSITION) : SV_Target
//{
//	return float4(1.0f, 1.0f, 0.0f, 1.0f);    // Yellow, with Alpha = 1
//}


//--------------------------------------------------------------------------------------
// File: Tutorial04.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
//cbuffer ConstantBuffer : register( b0 ) //컨스턴트 버퍼
//{
//	matrix World;
//	matrix View;
//	matrix Projection;
//}
//
////--------------------------------------------------------------------------------------
//struct VS_OUTPUT
//{
//    float4 Pos : SV_POSITION;
//    float4 Color : COLOR0;
//};
//
//struct VS_INPUT
//{
//	float3 Pos : POSITION; // 시멘틱 라벨(레이블) 
//	float4 Color : COLOR;
//};
//
////--------------------------------------------------------------------------------------
//// Vertex Shader
////--------------------------------------------------------------------------------------
//VS_OUTPUT VS(VS_INPUT In)
//{
//
//    VS_OUTPUT output = (VS_OUTPUT)0;
//    output.Pos = mul( float4(In.Pos, 1), World );
//    output.Pos = mul( output.Pos, View );
//    output.Pos = mul( output.Pos, Projection );
//    output.Color = In.Color;
//    return output;
//}
//
//
////--------------------------------------------------------------------------------------
//// Pixel Shader
////--------------------------------------------------------------------------------------
//float4 PS( VS_OUTPUT input ) : SV_Target // 나도 교수님한테 질문하러가야함
//{
//	return input.Color;
//   // return float4(1.0f, 1.0f, 0.0f, 1.0f);
//}

//--------------------------------------------------------------------------------------
// File: Tutorial07.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
