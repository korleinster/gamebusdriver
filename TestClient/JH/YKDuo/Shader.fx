
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

<<<<<<< HEAD
cbuffer ConstantBuffer : register(b0) //컨스턴트 버퍼 넘기면 여기로 꽂힘
=======
cbuffer ConstantBuffer : register( b0 ) //컨스턴트 버퍼 넘기면 여기로 꽂힘
>>>>>>> origin/master
{
	matrix World;
	matrix View;
	matrix Projection;
}

//--------------------------------------------------------------------------------------
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

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

VS_OUT VS(VS_INPUT input)
{
	VS_OUT output = (VS_OUT)0;
	output.Pos = mul(input.Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	output.Tex = input.Tex;

	return output;
}




//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
<<<<<<< HEAD
float4 PS(VS_OUT input) : SV_Target
{
	return txDiffuse.Sample(samLinear, input.Tex);
//return input.Color;
// return float4(1.0f, 1.0f, 0.0f, 1.0f);
=======
float4 PS(VS_OUT input ) : SV_Target
{
	return txDiffuse.Sample(samLinear, input.Tex);
	//return input.Color;
   // return float4(1.0f, 1.0f, 0.0f, 1.0f);
>>>>>>> origin/master
}



