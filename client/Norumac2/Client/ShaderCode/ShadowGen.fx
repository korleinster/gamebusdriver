///////////////////////////////////////////////////////////////////
// Spot shadow map generation
///////////////////////////////////////////////////////////////////
#define MAX		128

cbuffer cbPointShadowGenWorldVS : register(b0)
{
	float4x4 World : packoffset(c0);
}

cbuffer cbBoneWorldMatrix : register(b1)
{
	row_major matrix gMtxBone[MAX] : packoffset(c0); // row_major 행 우선 행렬 = 전치행렬X
};

cbuffer cbPointShadowGenViewProjVS : register(b2)
{
	float4x4 ShadowViewProj : packoffset(c0);
}

///////////////////////////////////////////////////////////////////
// Point shadow map generation
///////////////////////////////////////////////////////////////////

float4 PointShadowGenVS(float4 Pos : POSITION) : SV_Position
{
	return mul(mul(Pos, World), ShadowViewProj);
}

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

float4 PointShandowGenVS_ANI(VS_ANI_IN input) : SV_Position
{
	float4 OutPos = (float4)0;
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

	if (fWeight0 > 0) OutPos += fWeight0 * mul(Pos, m0);
	if (fWeight1 > 0) OutPos += fWeight1 * mul(Pos, m1);
	if (fWeight2 > 0) OutPos += fWeight2 * mul(Pos, m2);
	if (fWeight3 > 0) OutPos += fWeight3 * mul(Pos, m3);
	if (fWeight4 > 0) OutPos += fWeight4 * mul(Pos, m4);
	if (fWeight5 > 0) OutPos += fWeight5 * mul(Pos, m5);
	if (fWeight6 > 0) OutPos += fWeight6 * mul(Pos, m6);
	if (fWeight7 > 0) OutPos += fWeight7 * mul(Pos, m7);

	// Transform the position from object space to homogeneous projection space
	return mul(mul(OutPos, World), ShadowViewProj);
}

cbuffer cbuffercbShadowMapCubeGS : register( b0 )
{
	float4x4 CubeViewProj[6] : packoffset( c0 );
};

struct GS_OUTPUT
{
	float4 Pos		: SV_POSITION;
	uint RTIndex	: SV_RenderTargetArrayIndex;
};

[maxvertexcount(18)]
void PointShadowGenGS(triangle float4 InPos[3] : SV_Position, inout TriangleStream<GS_OUTPUT> OutStream)
{
	for(int iFace = 0; iFace < 6; iFace++ )
	{
		GS_OUTPUT output;

		output.RTIndex = iFace;

		for(int v = 0; v < 3; v++ )
		{
			output.Pos = mul(InPos[v], CubeViewProj[iFace]);
			OutStream.Append(output);
		}
		OutStream.RestartStrip();
	}
}

///////////////////////////////////////////////////////////////////
// Cascaded shadow maps generation
///////////////////////////////////////////////////////////////////

cbuffer cbuffercbShadowMapCubeGS : register( b0 )
{
	float4x4 CascadeViewProj[3] : packoffset( c0 );
};

[maxvertexcount(9)]
void CascadedShadowMapsGenGS(triangle float4 InPos[3] : SV_Position, inout TriangleStream<GS_OUTPUT> OutStream)
{
	for(int iFace = 0; iFace < 3; iFace++ )
	{
		GS_OUTPUT output;

		output.RTIndex = iFace;

		for(int v = 0; v < 3; v++ )
		{
			output.Pos = mul(InPos[v], CascadeViewProj[iFace]);
			OutStream.Append(output);
		}
		OutStream.RestartStrip();
	}
}