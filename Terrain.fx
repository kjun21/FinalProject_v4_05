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



cbuffer cbCameraPosition : register(b2)
{
	float4 gvCameraPosition2 : packoffset(c0);
};


Texture2D gtxtTexture : register(t0);


Texture2D gtxtDetailTexture : register(t1);
SamplerState gDetailSamplerState : register(s1);

Texture2DArray gtxtLayerMapArray : register(t3);

Texture2D gtxtBlendMap : register(t0);
SamplerState gSamplerState : register(s0);


Texture2D gtxtNormal : register(t2);
SamplerState gssNormal : register(s2);

Texture2D gtxtHeightMap : register(t5);
SamplerState gssHeightMap : register(s5);

//디테일 텍스쳐와 조명을 같이 사용하는 경우 정점 쉐이더의 입력을 위한 구조체이다.
struct VS_DETAIL_TEXTURED_LIGHTING_COLOR_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texCoordBase : TEXCOORD0;
	//float2 texCoordDetail : TEXCOORD1;
};

//디테일 텍스쳐와 조명을 같이 사용하는 경우 정점 쉐이더의 출력을 위한 구조체이다.
struct VS_DETAIL_TEXTURED_LIGHTING_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 texCoordBase : TEXCOORD0;
	float2 texCoordDetail : TEXCOORD1;
};


VS_DETAIL_TEXTURED_LIGHTING_COLOR_OUTPUT VSDetailTexturedLightingColor(VS_DETAIL_TEXTURED_LIGHTING_COLOR_INPUT input)
{
	VS_DETAIL_TEXTURED_LIGHTING_COLOR_OUTPUT output = (VS_DETAIL_TEXTURED_LIGHTING_COLOR_OUTPUT)0;
	output.normalW = mul(input.normal, (float3x3)gmtxWorld);
	output.positionW = mul(float4(input.position, 1.0f), gmtxWorld).xyz;
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);

	output.texCoordDetail = input.texCoordBase;
	output.texCoordBase = input.texCoordBase *50.0;




	return(output);
}

float4 PSDetailTexturedLightingColor(VS_DETAIL_TEXTURED_LIGHTING_COLOR_OUTPUT input) : SV_Target
{
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);
	//float4 cBaseTexColor = gtxtTexture.Sample(gSamplerState, input.texCoordBase);
		//float4 cDetailTexColor = gtxtDetailTexture.Sample(gDetailSamplerState, input.texCoordDetail);


		//
		// Texturing
		//

		// Sample layers in texture array.
		// 베이스 텍스쳐 샘플러 스테이트를 사용하기 때문에 반드시 고치거나, 수정할것.
		float4 c0 = gtxtLayerMapArray.Sample(gSamplerState, float3(input.texCoordBase, 0.0f));
		float4 c1 = gtxtLayerMapArray.Sample(gSamplerState, float3(input.texCoordBase, 1.0f));
		float4 c2 = gtxtLayerMapArray.Sample(gSamplerState, float3(input.texCoordBase, 2.0f));

		//  blend Map을 샘플링한다.
		float4 t = gtxtBlendMap.Sample(gSamplerState, input.texCoordDetail);

		// Blend the layers on top of each other.
		float4 texColor = c0;
		//b는 길
		texColor = lerp(texColor, c1, t.b);
	    texColor = lerp(texColor, c2, t.a);
  	  //  texColor = lerp(texColor, c3, t.b);

		return   texColor * cIllumination;




		//float4 cColor = cBaseTexColor;
			//float4 cColor = saturate((cBaseTexColor * 0.5f) + (cDetailTexColor * 0.5f));
		//return cColor;
		//return(cColor*cIllumination);
}

//============================================================================
//============================================================================
//============================================================================


struct VS_INPUT
{
	float3 posL : POSITION;
	float2 texCoord : TEXCOORD0;
};
struct VS_OUTPUT
{
	float3 posW : POSITION;
	float2 texCoord : TEXCOORD0;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;
	output.posW = input.posL;
	output.posW.y = gtxtHeightMap.SampleLevel(gssHeightMap, input.texCoord, 0).r;
	output.texCoord = input.texCoord;

	return output;
}

struct HS_CONSTANT_OUTPUT
{
	float fTessEdges[4] : SV_TessFactor;
	float fTessInsides[2] : SV_InsideTessFactor;


};
float CalculateTessFactor(float3 p)
{
	float fMinDist = 20.0f;
	float fMaxDist = 500.0f;

	float fMinTess = 5.0f;
	float fMaxTess = 6.0f;
	float fDistToCamera = distance(p, gvCameraPosition2.xyz);
	float s = saturate((fDistToCamera - fMinDist) / (fMaxDist - fMinDist));
	return pow(2, (lerp(fMaxTess, fMinTess, s)));
}


HS_CONSTANT_OUTPUT HSConstant(InputPatch<VS_OUTPUT, 4> input, uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_OUTPUT output;

	float3 e0 = 0.5f*(input[0].posW + input[2].posW);
	float3 e1 = 0.5f*(input[0].posW + input[1].posW);
	float3 e2 = 0.5f*(input[1].posW + input[3].posW);
	float3 e3 = 0.5f*(input[2].posW + input[3].posW);
	float3  c = 0.25f*(input[0].posW + input[1].posW + input[2].posW + input[3].posW);
		float3 fCameraPosition = gvCameraPosition2.xyz;



		output.fTessEdges[0] = CalculateTessFactor(e0);
		output.fTessEdges[1] = CalculateTessFactor(e1);
		output.fTessEdges[2] = CalculateTessFactor(e2);
		output.fTessEdges[3] = CalculateTessFactor(e3);

		output.fTessInsides[0] = CalculateTessFactor(c);
		output.fTessInsides[1] = output.fTessInsides[0];

	//float fTess = 1.0f;

	/*output.fTessEdges[0] = fTess;
	output.fTessEdges[1] = fTess;
	output.fTessEdges[2] = fTess;
	output.fTessEdges[3] = fTess;

	output.fTessInsides[0] = fTess;
	output.fTessInsides[1] = fTess;
*/
	
	return output;
}
struct HS_OUTPUT
{
	float3 positionW : POSITION;
//	float3 normalW : NORMAL;
	float2 texCoord : TEXCOORD;
};


[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("HSConstant")]
[maxtessfactor(64.0f)]
HS_OUTPUT HS(InputPatch<VS_OUTPUT, 4> input,
	uint i : SV_OutputControlPointID,
	uint patchID : SV_PrimitiveID)
{
	HS_OUTPUT output;
	output.positionW = input[i].posW;
	//output.normalW = input[i].normalW;
	output.texCoord = input[i].texCoord;

	return output;
}

struct DS_OUTPUT
{
	float4 posH : SV_POSITION;
	float3 posW : POSITION;
	//float3 normalW : NORMAL;
	float2 texCoord : TEXCOORD0;
	float2 texDetail : TEXCOORD1;
};

[domain("quad")]
DS_OUTPUT DS(HS_CONSTANT_OUTPUT input, float2 uv : SV_DomainLocation,
	OutputPatch<HS_OUTPUT, 4> quad)
{
	DS_OUTPUT output = (DS_OUTPUT)0;


	// Bilinear interpolation.
	output.posW = lerp(
		                     lerp(quad[0].positionW, quad[1].positionW, uv.x),
		                     lerp(quad[2].positionW, quad[3].positionW, uv.x),
		uv.y);

	output.texCoord = lerp(
		lerp(quad[0].texCoord, quad[1].texCoord, uv.x),
		lerp(quad[2].texCoord, quad[3].texCoord, uv.x),
		uv.y);

	

	float gvTextureScale = 50.0f;
	output.texDetail = output.texCoord*gvTextureScale;
	output.posW.y = gtxtHeightMap.SampleLevel(gssHeightMap, output.texCoord, 0).r - 130.0f;

	output.posH= mul(mul(float4(output.posW, 1.0f), gmtxView), gmtxProjection);

	return output;
}

float4 PS(DS_OUTPUT input) : SV_Target
{

	float gTexelCellSpaceU = 1.0f / 257.0f;
	float gTexelCellSpaceV = 1.0f / 257.0f;
	float gWorldCellSpace = 8.0f / 2.0f;

	float2 leftTex = input.texCoord + float2(-gTexelCellSpaceU, 0.0f);
	float2 rightTex = input.texCoord + float2(gTexelCellSpaceU, 0.0f);
	float2 bottomTex = input.texCoord + float2(0.0f, gTexelCellSpaceV);
	float2 topTex = input.texCoord + float2(0.0f, -gTexelCellSpaceV);

	float leftY = gtxtHeightMap.SampleLevel(gssHeightMap, leftTex, 0).r;
	float rightY = gtxtHeightMap.SampleLevel(gssHeightMap, rightTex, 0).r;
	float bottomY = gtxtHeightMap.SampleLevel(gssHeightMap, bottomTex, 0).r;
	float topY = gtxtHeightMap.SampleLevel(gssHeightMap, topTex, 0).r;

	float3 tangent = normalize(float3(2.0f*gWorldCellSpace, rightY - leftY, 0.0f));
	float3 bitan = normalize(float3(0.0f, bottomY - topY, -2.0f*gWorldCellSpace));
	float3 normalW = cross(tangent, bitan);


	float4 cIllumination = Lighting(input.posW, normalW);

	float4 cBaseTexColor = gtxtTexture.Sample(gSamplerState, input.texCoord);
	float4 cDetailTexColor = gtxtDetailTexture.Sample(gDetailSamplerState, input.texDetail);
	float4 cColor = saturate((cBaseTexColor * 0.5f) + (cDetailTexColor * 0.5f));

	return cColor * cIllumination;

	//return float4(1.0, 0.0f, 0.0f, 1.0f);

}
