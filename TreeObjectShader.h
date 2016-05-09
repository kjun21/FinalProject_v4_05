#pragma once
#include "stdafx.h"
#include "Shader.h"
#include "GameManager.h"

#define TREE_NUM 100

#define FLAT_ROCK01_NUM 10

#define FLAT_ROCK02_NUM 16
#define FLAT_ROCK03_NUM 16





#define ROCK_NUM 100
#define TALL_STONE01_NUM 10

class CTreeObjectShader : public CInstancingShader
{
public:
	CTreeObjectShader();
	virtual ~CTreeObjectShader();
	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void BuildObjects(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera);
protected:
	ID3D11Buffer *m_pd3dWoodInstanceBuffer;
	ID3D11Buffer *m_pd3dLeavesInstanceBuffer;
	ID3D11Buffer *m_pd3dFlatStone02InstanceBuffer;
	ID3D11Buffer *m_pd3dFlatStone03InstanceBuffer;
	ID3D11Buffer *m_pd3dCliff01InstanceBuffer;

	UINT m_nInstanceBufferStride;
	UINT m_nInstanceBufferOffset;
	//CTexture* m_pTexture;
	CTexture* m_pLeavesTexture;
	//CMaterial* m_pMaterial;

	UINT m_uiWoodNum;
	UINT m_uiLeavesNum;
	UINT m_uiTreeNum;

	UINT m_uiFlatStone01Num;
	UINT m_uiTallStone02Num;
	UINT m_uiTallStone03Num;

	UINT m_uiTallStoneNum;
};
