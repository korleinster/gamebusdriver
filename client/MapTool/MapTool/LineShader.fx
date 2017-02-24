cbuffer MatrixBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
};

struct VS_LINE_INPUT
{
	float4 pos : POSITION;
	float4 color : COLOUR;
};

struct VS_LINE_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOUR;
};

VS_LINE_OUTPUT VS_LINE(VS_LINE_INPUT input)
{
	VS_LINE_OUTPUT output = (VS_LINE_OUTPUT)0;

	matrix matAll = mul(World, View);
	matAll = mul(matAll, Projection);

	output.pos = mul(input.pos, matAll);

	output.color = input.color;

	return output;
}

float4 PS_LINE(VS_LINE_OUTPUT input) : SV_Target
{
	return input.color;
}