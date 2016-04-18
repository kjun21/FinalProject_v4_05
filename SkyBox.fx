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


struct VS_SKYBOX_CUBEMAP_INPUT
{
	float3 position : POSITION;
};
struct VS_SKYBOX_CUBEMAP_OUTPUT
{
	float3 position : POSITION;
	float4 positionH : SV_POSITION;
};

TextureCube gtxtCubeMapSkyBox : register(t0);
SamplerState gssSkyBox : register(s0);

VS_SKYBOX_CUBEMAP_OUTPUT VSSkyBox(VS_SKYBOX_CUBEMAP_INPUT input)
{
	VS_SKYBOX_CUBEMAP_OUTPUT output = (VS_SKYBOX_CUBEMAP_OUTPUT)0;
	output.positionH = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProjection);

	output.position = input.position;
	return output;
}

float4 PSSkyBox(VS_SKYBOX_CUBEMAP_OUTPUT input) : SV_Target
{
	float4 cColor = gtxtCubeMapSkyBox.Sample(gssSkyBox, input.position);
	return cColor;
}