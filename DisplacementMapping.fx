
#include "Light.fx"



cbuffer cbViewProjectionMatrix : register(b0)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
};

cbuffer cbWorldMatrix : register(b1)
{
	matrix gmtxWorld : packoffset(c0);
};



//테셀레이션 팩터를 주기 위한 카메라 좌표를 정점 셰이더에 연결한다.
cbuffer cbCameraPositionForVS : register(b3)
{
	float4 gvCameraPositionForVS : packoffset(c0);
};

//텍스쳐(픽셀 셰이더에 연결)
Texture2D gtxtTexture : register(t0);
SamplerState gSamplerState : register(s0);

//노말맵(도메인 셰이더와 픽셀 셰이더에 연결)
Texture2D gtxtNormal : register(t1);
SamplerState gssNormal : register(s1);



// 헐 셰이더 입력 및 출력 제어점을 정의
struct VS_INPUT
{
	float3 vPosition :	POSITION;
	float3 normal : NORMAL;
	float3 vTangent : TANGENT;
	float2 vUV : TEXCOORD;
};	


struct VS_OUTPUT
{
	float3 vPositionW :	POSITION;
	float3 normalW : NORMAL;
	float3 vTangentW : TANGENT;
	float2 vUV : TEXCOORD;
	float fTessFactor : TESSFACTOR;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.vPositionW = mul(float4(input.vPosition, 1), gmtxWorld).xyz;
	output.vTangentW = mul(input.vTangent, (float3x3)gmtxWorld);
	output.normalW = mul(input.normal, (float3x3)gmtxWorld);
	output.vUV = input.vUV;


	float gfMaxDistance = 80.0f;
	float gfMinDistance = 100.0f;

	float gfMinTessFactor = 2.0f;
	float gfMaxTessFactor = 5.0f;

	float fDistToCamera = distance(output.vPositionW, gvCameraPositionForVS.xyz) + 0.0f;
	float fTessFactor = saturate((gfMinDistance - fDistToCamera) / (gfMinDistance - gfMaxDistance));
	output.fTessFactor = gfMinTessFactor + fTessFactor * (gfMaxTessFactor - gfMinTessFactor);

	return output;
}


// 출력 패치 상수 데이터를 정의
//  상수 헐 셰이더는 입력 패치마다 실행
// 테셀레이션 인자를 출력
struct HS_CONSTANT_OUTPUT
{
	float fTessEdges[3] : SV_TessFactor;
	float fTessInsides : SV_InsideTessFactor;

};

HS_CONSTANT_OUTPUT HSConstant(InputPatch<VS_OUTPUT, 3> input, uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_OUTPUT output;

	output.fTessEdges[0] = 0.5f * (input[1].fTessFactor + input[2].fTessFactor);
	output.fTessEdges[1] = 0.5f * (input[2].fTessFactor + input[0].fTessFactor);
	output.fTessEdges[2] = 0.5f * (input[0].fTessFactor + input[1].fTessFactor);

	output.fTessInsides = output.fTessEdges[0];
	
	return output;
}
struct HS_OUTPUT
{
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float3 tangentW : TANGENT;
	float2 texCoord : TEXCOORD;
};

// 헐 셰이더 함수는 패치의 모든 제어점을 입력 받음.
// 헐 셰이더 함수는 매번 하나의 새로운 제어점(정점)을 출력함
// 입력 제어점은 정점 셰이더의 출력
// 출력할 제어점의 인덱스는 SV_OutputControlPointID 시멘틱을 입력
[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("HSConstant")]
[maxtessfactor(64.0f)]
HS_OUTPUT HS(InputPatch<VS_OUTPUT, 3> input,
	uint i : SV_OutputControlPointID,
	uint patchID : SV_PrimitiveID)
{
	HS_OUTPUT output;

	
	output.positionW = input[i].vPositionW;
	output.normalW = input[i].normalW;
	output.tangentW = input[i].vTangentW;
	output.texCoord = input[i].vUV;
	
	return output;
}

struct DS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float3 tangentW : TANGENT;
	float2 texCoord : TEXCOORD;
};

[domain("tri")]
DS_OUTPUT DS(HS_CONSTANT_OUTPUT input, float3 uv : SV_DomainLocation,
	                          const  OutputPatch<HS_OUTPUT, 3> patch)
{
	DS_OUTPUT output = (DS_OUTPUT)0;

	output.positionW = uv.x * patch[0].positionW + uv.y * patch[1].positionW + uv.z * patch[2].positionW;
	output.normalW =   uv.x * patch[0].normalW + uv.y * patch[1].normalW + uv.z * patch[2].normalW;
	output.tangentW =  uv.x * patch[0].tangentW + uv.y * patch[1].tangentW + uv.z * patch[2].tangentW;
	output.texCoord =  uv.x * patch[0].texCoord + uv.y * patch[1].texCoord + uv.z * patch[2].texCoord;
	output.normalW = normalize(output.normalW);


	float fHeight = gtxtNormal.SampleLevel(gssNormal, output.texCoord, 0.0).a;
	float gfHeightScalse = 6.0f;

	output.positionW += (gfHeightScalse * (fHeight - 1.0f)) * output.normalW;
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);

	return output;
}

float4 PS(DS_OUTPUT input) : SV_Target
{
	float3 N = normalize(input.normalW);
	float3 T = normalize(input.tangentW - dot(input.tangentW, N) * N);
	float3 B = cross(N, T);
	float3x3 TBN = float3x3(T, B, N);
	float3 normal = gtxtNormal.Sample(gssNormal, input.texCoord).rgb;
	normal = 2.0f * normal - 1.0f;
	float3 normalW = mul(normal, TBN);


	float4 cIllumination = Lighting(input.positionW, normalW);
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.texCoord) * cIllumination;
	return(cColor);

}
