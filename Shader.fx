
#define MAX_MATRIX 57
cbuffer cbViewProjectionMatrix : register(b0)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
};

//월드 변환 행렬을 위한 쉐이더 변수를 선언한다(슬롯 1을 사용). 
cbuffer cbWorldMatrix : register(b1)
{
	matrix gmtxWorld : packoffset(c0);
};

cbuffer cbBoneWorldMatrix : register(b5)
{
	row_major matrix gmtxBone[MAX_MATRIX] : packoffset(c0);
};


Texture2D gtxtTexture : register(t0);
SamplerState gSamplerState : register(s0);

//cbuffer cbScaleMatrix : register(b6)

struct VS_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	float4 Weights1 : WEIGHTS;
	float4 Weights2 : WEIGHT;
	uint4 Bones1 : BONES;
	uint4 Bones2 : BONE;

};

struct VS_OUTPUT 
{ 
	float4 position : SV_POSITION ;
	float2 texCoord : TEXCOORD;
};


VS_OUTPUT VS(VS_INPUT input) 
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	
	float4 Pos = float4(input.position,1);

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


    matrix m0 = gmtxBone[iBone0];
	matrix m1 = gmtxBone[iBone1];
	matrix m2 = gmtxBone[iBone2];
	matrix m3 = gmtxBone[iBone3];

	matrix m4 = gmtxBone[iBone4];
	matrix m5 = gmtxBone[iBone5];
	matrix m6 = gmtxBone[iBone6];
	matrix m7 = gmtxBone[iBone7];

	//matrix m8 = gmtxBone[22];
	
	if(fWeight0 > 0 ) output.position += fWeight0 * mul(Pos,m0);
	if(fWeight1 > 0 ) output.position += fWeight1 * mul(Pos,m1);
	if(fWeight2 > 0 ) output.position += fWeight2 * mul(Pos,m2);
	if(fWeight3 > 0 ) output.position += fWeight3 * mul(Pos,m3);
	if(fWeight4 > 0 ) output.position += fWeight4 * mul(Pos,m4);
	if(fWeight5 > 0 ) output.position += fWeight5 * mul(Pos,m5);
	if(fWeight6 > 0 ) output.position += fWeight6 * mul(Pos,m6);
	if(fWeight7 > 0 ) output.position += fWeight7 * mul(Pos,m7);


	//output.position +=  mul(Pos, m8);
	
	//output.position = float4(input.position, 1);


	//float4x4 scale ={ { 0.3, 0, 0, 0 }, { 0, 0.3, 0, 0 }, { 0, 0, 0.3, 0 }, { 0, 0, 0, 1 } };

	float4x4 scale = { { 9.0, 0, 0, 0 }, { 0, 9.0, 0, 0 }, { 0, 0, 9.0, 0 }, { 0, 0, 0, 1 } };


	output.position = mul(output.position, scale);
	output.position = mul(output.position, gmtxWorld);
	output.position = mul( output.position , gmtxView );
	output.position = mul( output.position, gmtxProjection);
	
	//output.color = input.color;
	
	//output.color = input.Weights2;
	output.texCoord = input.texCoord;
	//output.normal = input.Weights1.xxxw;

    return output;
}


float4 PS( VS_OUTPUT input ) : SV_Target
{
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.texCoord);
      //return input.color;
	  return cColor;
}
