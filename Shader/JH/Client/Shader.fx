
// float vs half 에대해서 자세히 알아보자 셰이더 최적화 단계에서 필요하게 될가능성이 있다
// float : 고도로 정밀한 부동 소수점. 일반적으로 32비트
// half : 중간 정도로 정밀한 부동 소수점. 일반적으로 16비트

// 자동적으로 글로벌 변수가 된다
cbuffer ConstantBuffer : register(b0) // register(b#) (#은 0~13)을 지정해주어서 할당할 슬롯번호를 지정할 수 있다
									  // 참고로 b는 buffer에 약자이며 그밖에 t(텍스쳐), c(버퍼 오프셋), s(셈플러) 등이 있다
{
	matrix World;
	matrix View;
	matrix Projection;
}

// 글로벌 변수
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	half4 Pos : POSITION; // 정점 데이터에서 위치 정보를 가져와서 Pos에 대입
	half2 Tex : TEXCOORD0;
};

struct VS_OUT
{
	half4 Pos : SV_POSITION; // 투영공간으로 변환된 위치 정보, SV_POSITION은 GPU에게 변수의 사용을 전달하는 의미
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
float4 PS(VS_OUT input) : SV_Target // SV_는 System Value에 약자로 렌더링 파이프라인에서 특별한 의미를 가진 시멘틱스명으로 사용
{
	//half4 albedo = tex2D(txDiffuse.Sample, input.Tex);
	//return albedo.rgba;

	half4 textureColor;
	textureColor = txDiffuse.Sample(samLinear, input.Tex);

	// 스위즐링
	return textureColor.rbga;

	//return txDiffuse.Sample(samLinear, input.Tex);

	//return input.Color;
}



