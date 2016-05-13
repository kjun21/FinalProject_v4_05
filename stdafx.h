// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

//#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#define _WINSOCK_DEPRECATED_NO_WARNINGS //Server Winsock
// Windows 헤더 파일:
#include <windows.h>

// C의 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>

#include <d3d11.h>
#include <d3dx11.h>
#include <DXGI.h>

#include <d3dx10.h>

#include <Mmsystem.h>
#include  <iostream>
#include <vector>

#include <d3dcompiler.h>  	//쉐이더 컴파일 함수를 사용하기 위한 헤더 파일
#include <D3DX10Math.h>	//Direct3D 수학 함수를 사용하기 위한 헤더 파일
#include <D3D9Types.h>
#include <DxErr.h>
#include <fstream>
#include <map>
#include <stdio.h>
#include <string>

//Server header
#include <time.h>
#include <WinSock2.h>
#include "struct.h"
#include "protocolType.h"
#include "protocol.h"
#include "ClientServer.h"

#define CRTDBG_MAP_ALLOC 

#define PLAYER_SPEED 1000

#define FRAME_BUFFER_WIDTH		1200
#define FRAME_BUFFER_HEIGHT		800
#define VS_SLOT_CAMERA				0x00
#define VS_SLOT_WORLD_MATRIX			0x01
#define VS_SLOT_WAVE_MATRICES 0x06

#define VS_SLOT_RESULT_MATRIX 0x05

#define PS_SLOT_COLOR	0x00
#define PS_SLOT_LIGHT			0x00
#define PS_SLOT_MATERIAL		0x01

//텍스쳐와 샘플러 상태를 설정하기 위한 쉐이더의 슬롯 번호를 정의한다. 
#define PS_SLOT_TEXTURE		0x00
#define PS_SLOT_SAMPLER_STATE		0x00

//#define MaxBone 57

#define MaxBone 57
#define GOLEM_MAX_BONE 71
#define OBJECT_NUM 3


// 애니메이션 상태.
#define ANIMATAION_CLIP_IDLE 0x00
#define ANIMATAION_CLIP_RUN 0x01
#define ANIMATAION_CLIP_ATTACK1 0x02
#define ANIMATAION_CLIP_ATTACK2 0x03
#define ANIMATAION_CLIP_DEATH 0x04


#define ANIMATAION_CLIP_MONSTER_IDLE 300
#define ANIMATAION_CLIP_MONSTER_ATTACK 301
#define ANIMATAION_CLIP_MONSTER_RUN 302
// 공백의 303
#define ANIMATAION_CLIP_MONSTER_DEATH 304


#define MONSTER_STATE_LIVING  100
#define MONSTER_STATE_DYING 101
#define MONSTER_STATE_DIE 102


// 공격 키 입력
#define ATTACK01 0x50
#define ATTACK02 0x51

#define PLAYERS_NUMS 4
#define MONSTERS_NUMS 5

#define GOLEM_NUM 10
#define SLIME_NUM 1


//플레이어 피격 반경 30
//플레이어 공격1 반경 120

// 골렘 공격 반경 140, 피격 반경 60


struct AnimationClip
{
	UINT m_nAnimationState;
	UINT m_uiBoneIndexCount;
	long long m_llAniTime;
	D3DXMATRIX **m_ppResultMatrix;
	string m_strFileName;
	float m_fTimePos;
	long long llNowTime;
	// 애니메이션마다 공격 시작~끝이 존재한다(물론 공격 애니메이션만 해당.)
	long long m_fAttackStartTime;
	long long m_fAttackEndTime;

	void ReleaseObjects() {
		for (int i = 0; i < m_llAniTime / 10; i++)
		{
			if (m_ppResultMatrix[i])
				delete m_ppResultMatrix[i];
		}
		if (m_ppResultMatrix)
			delete m_ppResultMatrix;
	}

};
// 애니메이션 정보를 정점 셰이더에 전달해줄 구조체
struct VS_CB_RESULT_MATRIX
{
	D3DXMATRIX m_d3dxmtxResult[200];
};

struct VS_CB_MONSTER_RESULT_MATRIX
{
	D3DXMATRIX m_d3dxmtxIdle[200];

};


struct BoneIndexData
{
	int a, b, c, d, e, f, g, h;
};

struct BoneWeightData
{
	float a, b, c, d, e, f, g, h;
};



struct BoneIndexData1
{
	int a, b, c, d;
};

struct BoneIndexData2
{
	int e, f, g, h;
};



struct BoneWeightData1
{
	float a, b, c, d;
};

struct BoneWeightData2
{
	float e, f, g, h;
};


struct VERTEX
{
	D3DXVECTOR3 m_d3dxPosition;
	D3DXCOLOR m_d3dxColor;
	D3DXVECTOR3 m_d3dxNormal;
	BoneIndexData m_IndexData;
	BoneWeightData m_WeightData;
};



#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)                                              \
		{                                                          \
		HRESULT hr = (x);                                      \
		if(FAILED(hr))                                         \
				{                                                      \
			DXTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true); \
				}                                                      \
		}
#endif

#else
#ifndef HR
#define HR(x) (x)
#endif
#endif 

#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console") 


//#ifndef _DEBUG
//
//#define new new(_CLIENT_BLOCK,__FILE__,__LINE)
//
//#endif

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
