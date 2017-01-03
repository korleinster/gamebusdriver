
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer ConstantBuffer : register( b0 ) //컨스턴트 버퍼
{
	matrix World;
	matrix View;
	matrix Projection;
}

struct VS_INPUT
{
	float4 Pos : POSITION;
	float2 Tex : TEXCOORD0;
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
