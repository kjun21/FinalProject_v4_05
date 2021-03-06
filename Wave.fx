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

cbuffer cbCameraPosition : register(b3)
{
	float4 gvCameraPosition2 : packoffset(c0);
};

cbuffer cbReflectionMatrix : register(b4)
{
	float4 gmtxReflection : packoffset(c0);
};



// 파도 변위 행렬들
cbuffer cbWaveMatrices : register(b6)
{
	matrix gmtxWavesDispTexTransform1 : packoffset(c0);
	matrix gmtxWavesDispTexTransform2 : packoffset(c4);
	matrix gmtxWavesNormalTexTransform1 : packoffset(c8);
	matrix gmtxWavesNormalTexTransform2 : packoffset(c12);
};


// 일반 이미지.
Texture2D gtxtTexture : register(t0);
SamplerState gSamplerState : register(s0);


Texture2D gtxtNormal1 : register(t1);
SamplerState gssNormal1 : register(s1);

Texture2D gtxtNormal2 : register(t2);
//SamplerState gssNormal2 : register(s2);

Texture2D gtxtRefraction : register(t3);
Texture2D gtxtReflection : register(t4);


TextureCube gtxtCubeMapSkyBox : register(t7);
SamplerState gssSkyBox : register(s7);


//===================================================
//=====================테셀레이션=======================
//===================================================

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
	float3 vPositionW : POSITION;
	float3 normalW : NORMAL;
	float3 vTangentW : TANGENT;
	float2 vUV : TEXCOORD0;

	float2 waveDispTex0   : TEXCOORD1;
	float2 waveDispTex1   : TEXCOORD2;
	float2 waveNormalTex0 : TEXCOORD3;
	float2 waveNormalTex1 : TEXCOORD4;


	float fTessFactor : TESSFACTOR;
	//float3 fCameraPosition : TEXCOORD5;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	float4x4 mtxTexTransform = { { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }, { 0, 0, 0, 1 } };

	output.vPositionW = mul(float4(input.vPosition, 1), gmtxWorld).xyz;
	output.vTangentW = mul(input.vTangent, (float3x3)gmtxWorld);
	output.normalW = mul(input.normal, (float3x3)gmtxWorld);
	output.vUV = input.vUV;

	// Output vertex attributes for interpolation across triangle.
	output.vUV = mul(float4(input.vUV, 0.0f, 1.0f), mtxTexTransform).xy;
	output.waveDispTex0 = mul(float4(input.vUV, 0.0f, 1.0f), gmtxWavesDispTexTransform1).xy;
	output.waveDispTex1 = mul(float4(input.vUV, 0.0f, 1.0f), gmtxWavesDispTexTransform2).xy;
	output.waveNormalTex0 = mul(float4(input.vUV, 0.0f, 1.0f), gmtxWavesNormalTexTransform1).xy;
	output.waveNormalTex1 = mul(float4(input.vUV, 0.0f, 1.0f), gmtxWavesNormalTexTransform2).xy;




	float gfMaxDistance = 80.0f;
	float gfMinDistance = 100.0f;

	float gfMinTessFactor = 2.0f;
	float gfMaxTessFactor = 5.0f;

	float fDistToCamera = distance(output.vPositionW, gvCameraPosition2.xyz) + 0.0f;
	float fTessFactor = saturate((gfMinDistance - fDistToCamera) / (gfMinDistance - gfMaxDistance));
	output.fTessFactor = gfMinTessFactor + fTessFactor * (gfMaxTessFactor - gfMinTessFactor);
	//outpu.fCameraPosition = gvCameraPosition2.xyz;
	return output;
}


// 출력 패치 상수 데이터를 정의
//  상수 헐 셰이더는 입력 패치마다 실행
// 테셀레이션 인자를 출력
struct HS_CONSTANT_OUTPUT
{
	float fTessEdges[3] : SV_TessFactor;
	float fTessInsides : SV_InsideTessFactor;

	/*float3 vTangent[4]: TANGENT;
	float2 vUV[4] : TEXCOORD;
	float3 vTanUCorner[4] : TANUCORNER;
	float3 vTanVCorner[4] : TANVCORNER;
	float4 vCWts : TANWEIGHTS;*/
};

HS_CONSTANT_OUTPUT HSConstant(InputPatch<VS_OUTPUT, 3> input, uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_OUTPUT output;


	output.fTessEdges[0] = 0.5f * (input[1].fTessFactor + input[2].fTessFactor);
	output.fTessEdges[1] = 0.5f * (input[2].fTessFactor + input[0].fTessFactor);
	output.fTessEdges[2] = 0.5f * (input[0].fTessFactor + input[1].fTessFactor);

	output.fTessInsides = output.fTessEdges[0];
	//......
	return output;
}

struct HS_OUTPUT
{
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float3 tangentW : TANGENT;
	float2 texCoord : TEXCOORD;

	float2 waveDispTex0   : TEXCOORD1;
	float2 waveDispTex1   : TEXCOORD2;
	float2 waveNormalTex0 : TEXCOORD3;
	float2 waveNormalTex1 : TEXCOORD4;
	//float3 fCameraPosition :  TEXCOORD5;
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
	output.texCoord = input[i].vUV;
	output.tangentW = input[i].vTangentW;
	output.normalW = input[i].normalW;

	output.waveDispTex0 = input[i].waveDispTex0;
	output.waveDispTex1 = input[i].waveDispTex1;
	output.waveNormalTex0 = input[i].waveNormalTex0;
	output.waveNormalTex1 = input[i].waveNormalTex1;
	//output.fCameraPosition = input[i].fCameraPosition;

	return output;

}

struct DS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float3 tangentW : TANGENT;
	float2 texCoord : TEXCOORD;

	float2 waveDispTex0   : TEXCOORD1;
	float2 waveDispTex1   : TEXCOORD2;
	float2 waveNormalTex0 : TEXCOORD3;
	float2 waveNormalTex1 : TEXCOORD4;

	//float4 reflectionPosition : TEXCOORD5;
	//float4 refractionPosition : TEXCOORD6;
	//float3 fCameraPosition :  TEXCOORD5;
};

[domain("tri")]
DS_OUTPUT DS(HS_CONSTANT_OUTPUT input, float3 uv : SV_DomainLocation,
	                           OutputPatch<HS_OUTPUT, 3> patch)
{
	DS_OUTPUT output = (DS_OUTPUT)0;

	output.positionW = uv.x * patch[0].positionW + uv.y * patch[1].positionW + uv.z * patch[2].positionW;
	output.normalW = uv.x * patch[0].normalW + uv.y * patch[1].normalW + uv.z * patch[2].normalW;
	output.tangentW = uv.x * patch[0].tangentW + uv.y * patch[1].tangentW + uv.z * patch[2].tangentW;
	output.texCoord = uv.x * patch[0].texCoord + uv.y * patch[1].texCoord + uv.z * patch[2].texCoord;
	//output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.normalW = normalize(output.normalW);

	output.waveDispTex0 = uv.x * patch[0].waveDispTex0 + uv.y * patch[1].waveDispTex0 + uv.z * patch[2].waveDispTex0;
	output.waveDispTex1 = uv.x * patch[0].waveDispTex1 + uv.y * patch[1].waveDispTex1 + uv.z * patch[2].waveDispTex1;
	output.waveNormalTex0 = uv.x * patch[0].waveNormalTex0 + uv.y * patch[1].waveNormalTex0 + uv.z * patch[2].waveNormalTex0;
	output.waveNormalTex1 = uv.x * patch[0].waveNormalTex1 + uv.y * patch[1].waveNormalTex1 + uv.z * patch[2].waveNormalTex1;



	// Sample height map (stored in alpha channel).
	//밉맵 레벨 0으로 가정.
	float mipLevel = 0.0;
	float fHeight1 = gtxtNormal1.SampleLevel(gssNormal1, output.waveDispTex0, mipLevel).a;
	float fHeight2 = gtxtNormal2.SampleLevel(gssNormal1, output.waveDispTex1, mipLevel).a;

	// 원래 상수 버퍼로 전달한다.
	float fHeightScale1 = 1.4;  //0.4   8.4
	float fHeightScale2 = 4.2; //1.2   7.2

	output.positionW.y += fHeightScale1 * fHeight1;
	output.positionW.y += fHeightScale2 * fHeight1;

	// Project to homogeneous clip space.
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.texCoord = 10.0f * output.texCoord;
	//matrix reflectProjectWorld;
	//matrix viewProjectWorld;
	//// Create the reflection projection world matrix.
	//output.reflectionPosition = mul(float4(output.positionW, 1), gmtxReflection);
	//output.reflectionPosition = mul(output.reflectionPosition, gmtxProjection);
	//// 굴절
	//output.refractionPosition = mul(float4(output.positionW, 1), gmtxView);
	//output.refractionPosition = mul(output.refractionPosition, gmtxProjection);



	//float fHeight = gtxtNormal.SampleLevel(gssNormal1, output.texCoord, 0.0).a;
	//float gfHeightScalse = 6.0f;

	//output.positionW += (gfHeightScalse * (fHeight - 1.0f)) * output.normalW;
	//output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);


	return output;
}
float4 PS(DS_OUTPUT input) : SV_Target
{

	float3 N = normalize(input.normalW);
	float3 T = normalize(input.tangentW - dot(input.tangentW, N) * N);
	float3 B = cross(N, T);
	float3x3 TBN = float3x3(T, B, N);
	//1번 노말맵 추출.
	float3 normalMapSample1 = gtxtNormal1.Sample(gssNormal1, input.waveNormalTex0).rgb;
	normalMapSample1 = 2.0f * normalMapSample1 - 1.0f;
	float3 normalW1 = mul(normalMapSample1, TBN);

		//2번 노말맵 추출.
		float3 normalMapSample2 = gtxtNormal2.Sample(gssNormal1, input.waveNormalTex1).rgb;
		normalMapSample2 = 2.0f * normalMapSample2 - 1.0f;
	float3 normalW2 = mul(normalMapSample2, TBN);


		//평균을 낸다.
		float3 bumpedNormalW = normalize(normalW1 + normalW2);
		float4 cIllumination = Lighting(input.positionW, bumpedNormalW);
		float4 TextureColor= gtxtTexture.Sample(gSamplerState, input.texCoord);
		//float4 cColor = gtxtTexture.Sample(gSamplerState, input.texCoord) * cIllumination;

		// 스카이 박스 반사 벡타
		float3 vCamerPosition = gvCameraPosition.xyz;
		float3 eyeDir = input.positionW - vCamerPosition;
		eyeDir = normalize(eyeDir);
	float3 reflectionVector = reflect(eyeDir, bumpedNormalW);
		float4 cReflectionColor = gtxtCubeMapSkyBox.Sample(gssSkyBox, eyeDir);
		float4 reflectMaterial = float4(0.2, 0.3, 0.9, 1.0f);




		//float2 reflectTexCoord;
		//float2 refractTexCoord;
		//float4 reflectionColor;
		//float4 	refractionColor;
		//float reflectRefractScale = 0.3;
		////bumpedNormalW = (bumpedNormalW.xyz * 2.0f) - 1.0f;

		//// -1~ +1 범위에 텍스쳐 좌표안에서 반사와 굴절 좌표 둘다 바꾼다.
		//// Calculate the projected reflection texture coordinates.
		//reflectTexCoord.x = input.reflectionPosition.x / input.reflectionPosition.w / 2.0f + 0.5f;
		//reflectTexCoord.y = -input.reflectionPosition.y / input.reflectionPosition.w / 2.0f + 0.5f;

		//// Calculate the projected refraction texture coordinates.
		//refractTexCoord.x = input.refractionPosition.x / input.refractionPosition.w / 2.0f + 0.5f;
		//refractTexCoord.y = -input.refractionPosition.y / input.refractionPosition.w / 2.0f + 0.5f;

		//reflectTexCoord = reflectTexCoord + (bumpedNormalW.xy * reflectRefractScale);
		//refractTexCoord = refractTexCoord + (bumpedNormalW.xy * reflectRefractScale);

		//// 다음, 업데이트된 텍스쳐 샘플링 좌표들을 기초로한 반사와 굴절 픽셀을 추출한다.
		//// Sample the texture pixels from the textures using the updated texture coordinates.
		//reflectionColor = gtxtReflection.Sample(gssNormal1, reflectTexCoord);
		//refractionColor = gtxtRefraction.Sample(gssNormal1, refractTexCoord);
		//float4 CreatedColor = lerp(reflectionColor, refractionColor, 0.6f);



		///float4 cColor = cIllumination + refractionColor;
		float4 cColor = cIllumination + (reflectMaterial* cReflectionColor);
		cColor = cColor * TextureColor;
		//float4 cColor =   cIllumination;
		//cColor = cColor * cIllumination;
		cColor.a = 0.40;
	return(cColor);
}



	//float3 N = normalize(input.normalW);
	//float3 T = normalize(input.tangentW - dot(input.tangentW, N) * N);
	//float3 B = cross(N, T);
	//float3x3 TBN = float3x3(T, B, N);
	//float3 normal = gtxtNormal.Sample(gssNormal, input.texCoord).rgb;

	//normal = 2.0f * normal - 1.0f;
	//float3 normalW = mul(normal, TBN);


	//float4 cIllumination = Lighting(input.positionW, normalW);
	//float4 cColor = gtxtTexture.Sample(gSamplerState, input.texCoord) * cIllumination;
	//return(cColor);




	//return float4(1.0f, 0.0f, 0.0f, 1.0f);

//============================================================
//===============================변위 매핑=======================
//============================================================

struct VS_SKYBOX_CUBEMAP_INPUT
{
	float3 position : POSITION;
};
struct VS_SKYBOX_CUBEMAP_OUTPUT
{
	float3 position : POSITION;
	float4 positionH : SV_POSITION;
};



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



//텍스쳐를 사용하는 경우 정점 쉐이더의 입력을 위한 구조체이다.
struct VS_TEXTURED_COLOR_INPUT
{
	float3 position : POSITION;
	//float2 texCoord : TEXCOORD0;
};

//텍스쳐를 사용하는 경우 정점 쉐이더의 출력을 위한 구조체이다.
struct VS_TEXTURED_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	//float2 texCoord : TEXCOORD0;
};


VS_TEXTURED_COLOR_OUTPUT VSTexturedColor(VS_TEXTURED_COLOR_INPUT input)
{
	VS_TEXTURED_COLOR_OUTPUT output = (VS_TEXTURED_COLOR_OUTPUT)0;
	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProjection);
	//output.texCoord = input.texCoord;

	return(output);
}

//각 픽셀에 대하여 텍스쳐 샘플링을 하기 위한 픽셀 쉐이더 함수이다.
float4 PSTexturedColor(VS_TEXTURED_COLOR_OUTPUT input) : SV_Target
{
	//float4 cColor = gtxtTexture.Sample(gSamplerState, input.texCoord);
	float4 cColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	return(cColor);
}
