//--------------------------------------------------------------------------------------
// File: Tutorial02.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
//카메라 변환 행렬과 투영 변환 행렬을 위한 쉐이더 변수를 선언한다(슬롯 0을 사용).
#include "Light.fx"

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


cbuffer cbCameraPosition : register(b0)
{
	float4 gvCameraPosition2 : packoffset(c0);
};


Texture2D gtxtTexture : register(t0);
SamplerState gSamplerState : register(s0);


Texture2D gtxtNormal : register(t1);
SamplerState gssNormal : register(s1);





//인스턴싱, 텍스쳐와 조명을 같이 사용하는 경우 정점 쉐이더의 입력을 위한 구조체이다.
struct VS_INSTANCED_TEXTURED_LIGHTING_COLOR_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float2 texCoord : TEXCOORD0;
	//float4x4 mtxTransform : INSTANCEPOS;
};

//인스턴싱, 텍스쳐와 조명을 같이 사용하는 경우 정점 쉐이더의 출력을 위한 구조체이다.
struct VS_INSTANCED_TEXTURED_LIGHTING_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float3 tangentW :TANGENT;
	float2 texCoord : TEXCOORD0;
};

//=-----------------------------------------


VS_INSTANCED_TEXTURED_LIGHTING_COLOR_OUTPUT VSNormalMapping(VS_INSTANCED_TEXTURED_LIGHTING_COLOR_INPUT input)
{
	VS_INSTANCED_TEXTURED_LIGHTING_COLOR_OUTPUT output = (VS_INSTANCED_TEXTURED_LIGHTING_COLOR_OUTPUT)0;
	output.normalW = mul(input.normal, (float3x3)gmtxWorld);
	output.tangentW = mul(input.tangent, (float3x3)gmtxWorld);

	output.positionW = mul(float4(input.position, 1.0f), gmtxWorld).xyz;
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.texCoord = input.texCoord;
	
	return(output);
}

float4 PSNormalMapping(VS_INSTANCED_TEXTURED_LIGHTING_COLOR_OUTPUT input) : SV_Target
{

	float3 N = normalize(input.normalW);
	float3 T = normalize(input.tangentW - dot(input.tangentW, N) * N);
	float3 B = cross(N, T);
	float3x3 TBN = float3x3(T, B, N);
	float3 normal = gtxtNormal.Sample(gssNormal, input.texCoord).rgb;
	normal = 2.0f * normal - 1.0f;
	float3 normalW = mul(normal, TBN);



	//input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, normalW);
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.texCoord) * cIllumination;
	//return float4(1.0f, 1.0f, 0.0f, 1.0f);
	return(cColor);
}



//인스턴싱, 텍스쳐와 조명을 같이 사용하는 경우 정점 쉐이더의 입력을 위한 구조체이다.
struct VS_TEXTURED_LIGHTING_COLOR_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD0;
	//float4x4 mtxTransform : INSTANCEPOS;
};

//인스턴싱, 텍스쳐와 조명을 같이 사용하는 경우 정점 쉐이더의 출력을 위한 구조체이다.
struct VS_TEXTURED_LIGHTING_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 texCoord : TEXCOORD0;
};


VS_TEXTURED_LIGHTING_COLOR_OUTPUT VS_N(VS_TEXTURED_LIGHTING_COLOR_INPUT input)
{
	VS_TEXTURED_LIGHTING_COLOR_OUTPUT output = (VS_TEXTURED_LIGHTING_COLOR_OUTPUT)0;
	output.normalW = mul(input.normal, (float3x3)gmtxWorld);

	output.positionW = mul(float4(input.position, 1.0f), gmtxWorld).xyz;
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.texCoord = input.texCoord;

	return(output);
}

float4 PS_N(VS_TEXTURED_LIGHTING_COLOR_OUTPUT  input) : SV_Target
{
		input.normalW = normalize(input.normalW);
		float4 cIllumination = Lighting(input.positionW, input.normalW);
		float4 cColor = gtxtTexture.Sample(gSamplerState, input.texCoord) * cIllumination;
		return(cColor);
		//return float4(1.0, 0.0f, 0.0f, 1.0f);
}


//===================================================
//=====================테셀레이션=======================
//===================================================

// 헐 셰이더 입력 및 출력 제어점을 정의

struct VS_INPUT
{
	float3 vPosition :	POSITION;
	float3 vUV : TEXCOORD;
	//제어점의 법선 벡터
	float3 vTangent : TANGENT;
};

struct VS_OUTPUT
{
	float3 vPosition : POSITION;
	float3 vUV : TEXCOORD;
	//제어점의 법선 벡터
	float3 vTangent : TANGENT;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.vPosition = input.vPosition;
	output.vUV = input.vUV;
	output.vTangent = input.vTangent;
	return output;
}


// 출력 패치 상수 데이터를 정의
//  상수 헐 셰이더는 입력 패치마다 실행
// 테셀레이션 인자를 출력
struct HS_CONSTANT_OUTPUT
{
	float fTessEdges[4] : SV_TessFactor;
	float fTessInsides[2] : SV_InsideTessFactor;

	/*float3 vTangent[4]: TANGENT;
	float2 vUV[4] : TEXCOORD;
	float3 vTanUCorner[4] : TANUCORNER;
	float3 vTanVCorner[4] : TANVCORNER;
	float4 vCWts : TANWEIGHTS;*/
};

HS_CONSTANT_OUTPUT HSConstant(InputPatch<VS_OUTPUT, 4> input, uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_OUTPUT output;


	float3 centerL = 0.25f*(input[0].vPosition + input[1].vPosition + input[2].vPosition + input[3].vPosition);
	float3 centerW = mul(float4(centerL, 1.0f), gmtxWorld).xyz;

	float d = distance(centerW, gvCameraPosition2.xyz);

	const float d0 = 10.0f;
	const float d1 = 200.0f;
	float tess = 64.0f*saturate((d1 - d) / (d1 - d0));
	//float tess = 1.0f;

	//모든 에지를 3등분한다.
	output.fTessEdges[0] = tess; //왼쪽 에지
	output.fTessEdges[1] = tess; //위쪽 에지
	output.fTessEdges[2] = tess; //오른쪽 에지
	output.fTessEdges[3] = tess;// 아래쪽 에지

	output.fTessInsides[0] = tess; //u-축(열의 개수)
	output.fTessInsides[1] = tess; //v-축(열의 개수)
	//......
	return output;
}

struct HS_OUTPUT
{
	float3 position : POSITION;
};

// 헐 셰이더 함수는 패치의 모든 제어점을 입력 받음.
// 헐 셰이더 함수는 매번 하나의 새로운 제어점(정점)을 출력함
// 입력 제어점은 정점 셰이더의 출력
// 출력할 제어점의 인덱스는 SV_OutputControlPointID 시멘틱을 입력
[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("HSConstant")]
[maxtessfactor(64.0f)]
HS_OUTPUT HS(InputPatch<VS_OUTPUT, 4> input,
	uint i : SV_OutputControlPointID,
	uint patchID : SV_PrimitiveID)
{
	HS_OUTPUT output;
	output.position = input[i].vPosition;
	return output;


	//// Pass through shader.
	//hout.PosW = p[i].PosW;
	//hout.NormalW = p[i].NormalW;
	//hout.TangentW = p[i].TangentW;
	//hout.Tex = p[i].Tex;

	//return hout;
}

struct DS_OUTPUT
{
	float4 position : SV_POSITION;
};

[domain("quad")]
DS_OUTPUT DS(HS_CONSTANT_OUTPUT input, float2 uv : SV_DomainLocation,
	                           OutputPatch<HS_OUTPUT, 4> patch)
{
	DS_OUTPUT output = (DS_OUTPUT)0;

	float3 v1 = lerp(patch[0].position, patch[1].position, uv.x);
	float3 v2 = lerp(patch[2].position, patch[3].position, uv.x);
	float3 position = lerp(v1, v2, uv.y);


	output.position = mul(mul(mul(float4(position, 1.0f), gmtxWorld), gmtxView), gmtxProjection);

	return output;
}
float4 PS(DS_OUTPUT output) : SV_Target
{
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}
//============================================================
//===============================변위 매핑=======================
//============================================================

