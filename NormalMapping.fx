//--------------------------------------------------------------------------------------
// File: Tutorial02.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
//ī�޶� ��ȯ ��İ� ���� ��ȯ ����� ���� ���̴� ������ �����Ѵ�(���� 0�� ���).
#include "Light.fx"

cbuffer cbViewProjectionMatrix : register(b0)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
};

//���� ��ȯ ����� ���� ���̴� ������ �����Ѵ�(���� 1�� ���). 
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





//�ν��Ͻ�, �ؽ��Ŀ� ������ ���� ����ϴ� ��� ���� ���̴��� �Է��� ���� ����ü�̴�.
struct VS_INSTANCED_TEXTURED_LIGHTING_COLOR_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float2 texCoord : TEXCOORD0;
	//float4x4 mtxTransform : INSTANCEPOS;
};

//�ν��Ͻ�, �ؽ��Ŀ� ������ ���� ����ϴ� ��� ���� ���̴��� ����� ���� ����ü�̴�.
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



//�ν��Ͻ�, �ؽ��Ŀ� ������ ���� ����ϴ� ��� ���� ���̴��� �Է��� ���� ����ü�̴�.
struct VS_TEXTURED_LIGHTING_COLOR_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD0;
	//float4x4 mtxTransform : INSTANCEPOS;
};

//�ν��Ͻ�, �ؽ��Ŀ� ������ ���� ����ϴ� ��� ���� ���̴��� ����� ���� ����ü�̴�.
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
//=====================�׼����̼�=======================
//===================================================

// �� ���̴� �Է� �� ��� �������� ����

struct VS_INPUT
{
	float3 vPosition :	POSITION;
	float3 vUV : TEXCOORD;
	//�������� ���� ����
	float3 vTangent : TANGENT;
};

struct VS_OUTPUT
{
	float3 vPosition : POSITION;
	float3 vUV : TEXCOORD;
	//�������� ���� ����
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


// ��� ��ġ ��� �����͸� ����
//  ��� �� ���̴��� �Է� ��ġ���� ����
// �׼����̼� ���ڸ� ���
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

	//��� ������ 3����Ѵ�.
	output.fTessEdges[0] = tess; //���� ����
	output.fTessEdges[1] = tess; //���� ����
	output.fTessEdges[2] = tess; //������ ����
	output.fTessEdges[3] = tess;// �Ʒ��� ����

	output.fTessInsides[0] = tess; //u-��(���� ����)
	output.fTessInsides[1] = tess; //v-��(���� ����)
	//......
	return output;
}

struct HS_OUTPUT
{
	float3 position : POSITION;
};

// �� ���̴� �Լ��� ��ġ�� ��� �������� �Է� ����.
// �� ���̴� �Լ��� �Ź� �ϳ��� ���ο� ������(����)�� �����
// �Է� �������� ���� ���̴��� ���
// ����� �������� �ε����� SV_OutputControlPointID �ø�ƽ�� �Է�
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
//===============================���� ����=======================
//============================================================

