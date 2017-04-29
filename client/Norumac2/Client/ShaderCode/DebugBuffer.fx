Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer ConstantBuffer : register(b0) //컨스턴트 버퍼
{
	matrix gMatWorld;
}

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

VS_OUTPUT DebugBufferVS(VS_INPUT Input/*uint VertexID : SV_VertexID*/)
{
	VS_OUTPUT Output;
	
	Output.Pos = float4(mul(Input.Pos, gMatWorld).xy, 0.0f, 1.0f);
	Output.UV = Input.UV;

	return Output;
}

float4 DebugBufferPS(VS_OUTPUT In) : SV_TARGET
{
	return txDiffuse.Sample(samLinear, In.UV);
}

float4 DebugBufferPS_Red(VS_OUTPUT In) : SV_TARGET
{
	return float4(0.0f, 0.0f, txDiffuse.Sample(samLinear, In.UV).r, 1.0f);
}
