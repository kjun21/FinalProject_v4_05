#pragma once
#include "stdafx.h"
#include "Shader.h"

#define GOLEM_NUM 5

struct State
{
	int x, y, z, a;
};
struct VS_VB_INSTANCE_ANIMATION
{
	D3DXMATRIX m_d3dxTransform;
	UINT m_uiAnimationState;
	//State m_State;
	//D3DXCOLOR m_d3dxColor;
};

class CMonsterShader : public CInstancingShader
{
public:
	CMonsterShader();
	virtual ~CMonsterShader();
	virtual void BuildObjects(ID3D11Device *pd3dDevice);
	virtual void CreateShader(ID3D11Device *pd3dDevice);
	//virtual void AnimateObjects(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera);
	virtual ID3D11Buffer* CreateInstanceBuffer(ID3D11Device *pd3dDevice, int nObjects, UINT nBufferStride, void *pBufferData);
protected:
	ID3D11Buffer *m_pd3dGolemInstanceBuffer;
	UINT m_nInstanceBufferStride;
	UINT m_nInstanceBufferOffset;
	float fTime;
};
