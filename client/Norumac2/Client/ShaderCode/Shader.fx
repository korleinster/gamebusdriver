
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer ConstantBuffer : register( b0 ) //컨스턴트 버퍼
{
	matrix gMatWorld;
	matrix gMatView;
	matrix gMatProjection;
}

#define MAX		128


cbuffer cbBoneWorldMatrix : register(b1)
{
	row_major matrix gMtxBone[MAX] : packoffset(c0); // row_major 행 우선 행렬 = 전치행렬X
};



struct VS_INPUT
{
	float4 Pos : POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD0;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD0;
};


VS_OUT VS(VS_INPUT input)
{
	VS_OUT output = (VS_OUT)0;
	output.Pos = mul(input.Pos, gMatWorld);
	output.Pos = mul(output.Pos, gMatView);
	output.Pos = mul(output.Pos, gMatProjection);
	output.Tex = input.Tex;

	return output;
}


///로고
VS_OUT VS_Logo(VS_INPUT input)
{
	VS_OUT output = (VS_OUT)0;
	output.Pos = input.Pos;
	output.Tex = input.Tex;
	return output;
}

/////애니메이션 셰이더 //////////////////////////////////////////////

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

struct VS_ANI_OUT
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 tex2dcoord : TEXCOORD0;
};

VS_OUT VS_ANI(VS_ANI_IN input)
{
	VS_OUT output = (VS_OUT)0;


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

	if (fWeight0 > 0) output.Pos += fWeight0 * mul(Pos, m0);
	if (fWeight1 > 0) output.Pos += fWeight1 * mul(Pos, m1);
	if (fWeight2 > 0) output.Pos += fWeight2 * mul(Pos, m2);
	if (fWeight3 > 0) output.Pos += fWeight3 * mul(Pos, m3);
	if (fWeight4 > 0) output.Pos += fWeight4 * mul(Pos, m4);
	if (fWeight5 > 0) output.Pos += fWeight5 * mul(Pos, m5);
	if (fWeight6 > 0) output.Pos += fWeight6 * mul(Pos, m6);
	if (fWeight7 > 0) output.Pos += fWeight7 * mul(Pos, m7);

	matrix matAll = mul(gMatWorld, gMatView);
	matAll = mul(matAll, gMatProjection);


	output.Normal = mul(input.normal, (float3x3)gMatWorld);
	output.Pos = mul(output.Pos, matAll);
	output.Tex = input.tex2dcoord;


	return output;
}



// 픽셀 셰이더 /////////////////////////////////////////////////////////////////////////////
float4 PS(VS_OUT input) : SV_Target
{
	return txDiffuse.Sample(samLinear, input.Tex);
}


