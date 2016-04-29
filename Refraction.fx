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


Texture2D gtxtTexture : register(t0);
SamplerState gSamplerState : register(s0);



// We add the clip plane buffer to the light shader to make it a refraction shader. 
// �츮�� Ŭ�� ���� ��鸦 �̰��� ���� ���̴��� ����� ���� �� ���̴��� ���Ѵ�.

cbuffer ClipPlaneBuffer : register(b2)
{
	float4 clipPlane : packoffset(c0);
};



//�ؽ��Ŀ� ������ ���� ����ϴ� ��� ���� ���̴��� �Է��� ���� ����ü�̴�.
struct VS_TEXTURED_LIGHTING_COLOR_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD0;
};

//�ؽ��Ŀ� ������ ���� ����ϴ� ��� ���� ���̴��� ����� ���� ����ü�̴�.
struct VS_TEXTURED_LIGHTING_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 texCoord : TEXCOORD0;
};

//������ �ؽ��Ŀ� ������ ���� ����ϴ� ��� ���� ���̴��� �Է��� ���� ����ü�̴�.
struct VS_DETAIL_TEXTURED_LIGHTING_COLOR_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texCoordBase : TEXCOORD0;
	float2 texCoordDetail : TEXCOORD1;

};

//������ �ؽ��Ŀ� ������ ���� ����ϴ� ��� ���� ���̴��� ����� ���� ����ü�̴�.
struct VS_DETAIL_TEXTURED_LIGHTING_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 texCoordBase : TEXCOORD0;
	float2 texCoordDetail : TEXCOORD1;

};



VS_TEXTURED_LIGHTING_COLOR_OUTPUT VS(VS_TEXTURED_LIGHTING_COLOR_INPUT input)
{
	VS_TEXTURED_LIGHTING_COLOR_OUTPUT output = (VS_TEXTURED_LIGHTING_COLOR_OUTPUT)0;
	output.normalW = mul(input.normal, (float3x3)gmtxWorld);

	output.positionW = mul(float4(input.position, 1.0f), gmtxWorld).xyz;
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.texCoord = input.texCoord;

	return(output);
}

float4 PS(VS_TEXTURED_LIGHTING_COLOR_OUTPUT  input) : SV_Target
{
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);
			float4 cColor = gtxtTexture.Sample(gSamplerState, input.texCoord) * cIllumination;
		//float4 cColor = gtxtTexture.Sample(gSamplerState, input.texCoord);

		
		clip(cColor.a - 0.1f);
		return(cColor);
	//return float4(1.0, 0.0f, 0.0f, 1.0f);
}

// The clip distance variable is added to the pixel input to achieve the clipping effect needed for the refraction shader.
// Ŭ�� �Ÿ� ������ ���� ���̴��� �䱸�� Ŭ���� ȿ���� ��� ���� �ȼ� input�� ��������.

//�ν��Ͻ�, �ؽ��Ŀ� ������ ���� ����ϴ� ��� ���� ���̴��� �Է��� ���� ����ü�̴�.
struct VS_INSTANCED_TEXTURED_LIGHTING_COLOR_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD0;
	float4x4 mtxTransform : INSTANCEPOS;


};

//�ν��Ͻ�, �ؽ��Ŀ� ������ ���� ����ϴ� ��� ���� ���̴��� ����� ���� ����ü�̴�.
struct VS_INSTANCED_TEXTURED_LIGHTING_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 texCoord : TEXCOORD0;

	float clip : SV_ClipDistance0;
};


VS_INSTANCED_TEXTURED_LIGHTING_COLOR_OUTPUT VSInstancedTexturedLightingColor(VS_INSTANCED_TEXTURED_LIGHTING_COLOR_INPUT input)
{
	VS_INSTANCED_TEXTURED_LIGHTING_COLOR_OUTPUT output = (VS_INSTANCED_TEXTURED_LIGHTING_COLOR_OUTPUT)0;
	output.normalW = mul(input.normal, (float3x3)input.mtxTransform);
	output.positionW = mul(float4(input.position, 1.0f), input.mtxTransform).xyz;
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.texCoord = input.texCoord;


	//���� �츮�� Ŭ�� ����� ��ġ�Ѵ�.
	// Set the clipping plane.
	output.clip = dot(mul(float4(input.position, 1), input.mtxTransform), clipPlane);
	// output.clip = 0;

	return(output);
}



float4 PSInstancedTexturedLightingColor(VS_INSTANCED_TEXTURED_LIGHTING_COLOR_OUTPUT input) : SV_Target
{
	input.normalW = normalize(input.normalW);

	float4 cIllumination = Lighting(input.positionW, input.normalW);
		//float4 cColor = gtxtTexture.Sample(gSamplerState, input.texCoord) * cIllumination;

	float4 cColor = gtxtTexture.Sample(gSamplerState, input.texCoord) * cIllumination;
			clip(cColor.a - 0.1f);
		return(cColor);
}