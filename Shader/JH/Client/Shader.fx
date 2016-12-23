
// float vs half 에대해서 자세히 알아보자 셰이더 최적화 단계에서 필요하게 될가능성이 있다
// float : 고도로 정밀한 부동 소수점. 일반적으로 32비트
// half : 중간 정도로 정밀한 부동 소수점. 일반적으로 16비트

Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer ConstantBuffer : register(b0) //컨스턴트 버퍼 넘기면 여기로 꽂힘
{
	matrix World;
	matrix View;
	matrix Projection;
}

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	half4 Pos : POSITION; // 정점 데이터에서 위치 정보를 가져와서 Pos에 대입
	half2 Tex : TEXCOORD0;
};

struct VS_OUT
{
	half4 Pos : SV_POSITION; // 투영공간으로 변환된 위치 정보
	half2 Tex : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

VS_OUT VS(VS_INPUT input) // 정점 쉐이더의 가장 중요한 임무는 로컬공간에 있는 정점의 위치를 투영공간까지 변환
{	
	VS_OUT output = (VS_OUT)0;
	output.Pos = mul(input.Pos, World); // 로컬공간에 월드행렬을 곱해서 월드공간의 위치를 대입 
	output.Pos = mul(output.Pos, View); // 같은 방식으로 뷰공간을 대입
	output.Pos = mul(output.Pos, Projection); // 같은 방식으로 투영공간으로 대입
	output.Tex = input.Tex;

	return output; // 투영공간까지 변환한 값을 반환
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUT input) : SV_Target
{
	return txDiffuse.Sample(samLinear, input.Tex);
	//return input.Color;
}



