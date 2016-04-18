#pragma once
#include "stdafx.h"
#include "Object.h"
#include "Camera.h"
#include "Player.h"


#include "CubeMesh.h"
#include "CRotatingObject.h"
#include "AirplaneMesh.h"
#include "AirplanePlayer.h"


#define MAXANINUM  1

struct VS_CB_WORLD_MATRIX
{
	D3DXMATRIX m_d3dxmtxWorld;
};

struct VS_CB_RESULT_MATRIX
{
	D3DXMATRIX m_d3dxmtxResult[200];
};


struct VS_VB_INSTANCE
{
	D3DXMATRIX m_d3dxTransform;
	D3DXCOLOR m_d3dxColor;
};

class CShader
{
public:
	CShader();
	virtual ~CShader();

public:
	//컴파일된 이펙트 파일에서 정점 쉐이더와 픽셀 쉐이더를 생성하는 함수이다.
	void CreateVertexShaderFromCompiledFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout, UINT nElements);
	void CreatePixelShaderFromCompiledFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName);
	void CreateHullShaderFromCompiledFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName);
	void CreateDomainShaderFromCompiledFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName);


	void CreateVertexShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11VertexShader **ppd3dVertexShader, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout, UINT nElements, ID3D11InputLayout **ppd3dVertexLayout);
	void CreatePixelShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11PixelShader **ppd3dPixelShader);
	void CreateHullShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11HullShader **ppd3dHullShader);
	void CreateDomainShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11DomainShader **ppd3dDomainShader);

	virtual void CreateShader(ID3D11Device *pd3dDevice);

	static void CreateShaderVariables(ID3D11Device *pd3dDevice);
	static void ReleaseShaderVariables();
	static void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld);

	//게임 객체들을 생성하고 애니메이션 처리를 하고 렌더링하기 위한 함수이다.
	virtual void BuildObjects(ID3D11Device *pd3dDevice);
	virtual void ReleaseObjects();
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void OnPrepareRender(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera = NULL);

protected:
	ID3D11VertexShader *m_pd3dVertexShader;
	ID3D11InputLayout *m_pd3dVertexLayout;

	ID3D11PixelShader *m_pd3dPixelShader;

	//==============================================
	//테셀레이션 관련 
	ID3D11HullShader *m_pd3dHullShader;
	ID3D11DomainShader *m_pd3dDomainShader;
	ID3D11Buffer* m_pd3dcbCameraPosition;

	//쉐이더 객체가 게임 객체들의 리스트를 가진다.
	CGameObject **m_ppObjects;
	int m_nObjects;

	//월드 변환 행렬을 위한 상수 버퍼는 하나만 있어도 되므로 정적 멤버로 선언한다.
	static ID3D11Buffer *m_pd3dcbWorldMatrix;

	
public:
	virtual CGameObject *PickObjectByRayIntersection(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxView, MESHINTERSECTINFO *pd3dxIntersectInfo);
	void CreateCameraPositionBuffer(ID3D11Device *pd3dDevice);
	static void UpdateWorldMatrixForTess(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld);
};
class CDiffusedShader : public CShader
{
public:
	CDiffusedShader();
	virtual ~CDiffusedShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
};


class CSceneShader : public CShader
{
public:
	CSceneShader();
	virtual ~CSceneShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void BuildObjects(ID3D11Device *pd3dDevice);
};

//플레이어를 렌더링하기 위한 쉐이더 클래스이다.
class CPlayerShader : public CDiffusedShader
{
public:
	CPlayerShader();
	virtual ~CPlayerShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void BuildObjects(ID3D11Device *pd3dDevice);
	virtual  void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
	virtual void CreateShaderVariables(ID3D11Device *pd3dDevice);
	CPlayer *GetPlayer(int nIndex = 0) { return((CPlayer *)m_ppObjects[nIndex]); }
	//virtual void AnimateObjects(float fTimeElapsed);

protected:
	AnimationClip* CreateAnimation(AnimationClip* animationClip);
	void LoadAnimation(AnimationClip* animationClip, UINT i);
	CTexture* m_pTexture;

	static ID3D11Buffer	 *m_pd3dcbPlayerResult;
protected:
	long long m_llAniTime;
	UINT m_uiBoneIndexCount;
	//ID3D11Buffer *g_pd3dcbBoneMatrix = nullptr;
	D3DXMATRIX **m_ppResultMatrix;


	VS_CB_RESULT_MATRIX **m_pvscbResultMatrix;
	VS_CB_RESULT_MATRIX *m_cbResult;
	VS_CB_RESULT_MATRIX *m_cbMapData;

	float m_fTimePos;
	
	
	
};
class CIlluminatedShader : public CShader
{
public:
	CIlluminatedShader();
	virtual ~CIlluminatedShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);

	//재질을 설정하기 위한 상수 버퍼이다.
	static ID3D11Buffer	 *m_pd3dcbMaterial;

	static void CreateShaderVariables(ID3D11Device *pd3dDevice);
	static void ReleaseShaderVariables();
	//재질을 쉐이더 변수에 설정(연결)하기 위한 함수이다.
	static void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, MATERIAL *pMaterial);
};



class CTexturedShader : public CShader
{
public:
	CTexturedShader();
	virtual ~CTexturedShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
};


class CInstancingShader : public CTexturedShader
{
public:
	CInstancingShader();
	virtual ~CInstancingShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);

public:
	virtual void BuildObjects(ID3D11Device *pd3dDevice, CHeightMapTerrain *pHeightMapTerrain, CMaterial *pMaterial, CTexture *pTexture, int k);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);

	//다음 멤버변수를 추가한다.
private:
	CTexture *m_pTexture;

private:
	CMaterial *m_pMaterial;
	UINT m_nInstanceBufferStride;
	UINT m_nInstanceBufferOffset;

	ID3D11Buffer *m_pd3dCubeInstanceBuffer;
	ID3D11Buffer *m_pd3dSphereInstanceBuffer;

public:
	ID3D11Buffer *CreateInstanceBuffer(ID3D11Device *pd3dDevice, int nObjects, UINT nBufferStride, void *pBufferData);
};


class CDetailTexturedShader : public CTexturedShader
{
public:
	CDetailTexturedShader();
	virtual ~CDetailTexturedShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
};

//CTerrainShader 클래스의 베이스 클래스를 CDetailTexturedIlluminatedShader 클래스로 변경한다.


class CSkyBoxShader : public CTexturedShader
{
public:
	CSkyBoxShader();
	virtual ~CSkyBoxShader();

	virtual void BuildObjects(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
};

class CTexturedIlluminatedShader : public CIlluminatedShader
{
public:
	CTexturedIlluminatedShader();
	virtual ~CTexturedIlluminatedShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
};

class CDetailTexturedIlluminatedShader : public CTexturedIlluminatedShader
{
public:
	CDetailTexturedIlluminatedShader();
	virtual ~CDetailTexturedIlluminatedShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
};

class CTerrainShader : public CDetailTexturedIlluminatedShader
{
public:
	CTerrainShader();
	virtual ~CTerrainShader();

	virtual void BuildObjects(ID3D11Device *pd3dDevice);

	CHeightMapTerrain *GetTerrain();
};
class CNormalMappingShader : public CTexturedShader
{
private:
	CTexture *m_pTexture;
	CMaterial *m_pMaterial;

private:
public:
	CNormalMappingShader();
	virtual ~CNormalMappingShader();

	virtual void BuildObjects(ID3D11Device *pd3dDevice, CMaterial *pMaterial, CTexture *pTexture);
	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
};

class CTextureLightingShader : public CTexturedShader
{
private:
	CTexture *m_pTexture;
	CMaterial *m_pMaterial;

private:
public:
	CTextureLightingShader();
	virtual ~CTextureLightingShader();

	virtual void BuildObjects(ID3D11Device *pd3dDevice, CMaterial *pMaterial, CTexture *pTexture);
	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
};

class CTessellationShader : public CTexturedShader
{
protected:
	CTexture *m_pTexture;
	CMaterial *m_pMaterial;

public:
	CTessellationShader();
	virtual ~CTessellationShader();
	virtual void BuildObjects(ID3D11Device *pd3dDevice, CMaterial *pMaterial, CTexture *pTexture);
	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
};




class CDisplacementMappingShader : public CTessellationShader
{
public:
	CDisplacementMappingShader();
	virtual ~CDisplacementMappingShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void BuildObjects(ID3D11Device *pd3dDevice, CMaterial *pMaterial, CTexture *pTexture);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);

};

class CWaveShader : public CDisplacementMappingShader
{
public:
	CWaveShader();

	virtual ~CWaveShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void BuildObjects(ID3D11Device *pd3dDevice, CMaterial *pMaterial, CTexture *pTexture);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
};

class CTestTessellationShader : public CTessellationShader
{
public:
	CTestTessellationShader();
	virtual ~CTestTessellationShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void BuildObjects(ID3D11Device *pd3dDevice, CMaterial *pMaterial, CTexture *pTexture);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
};



class CTessellationTerrainShader : public CDetailTexturedIlluminatedShader
{
protected:
	CTexture* m_pHeightMap;
public:
	CTessellationTerrainShader();
	virtual ~CTessellationTerrainShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void BuildObjects(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
};

class CHumanShader : public CShader
{
public:
	CHumanShader();
	virtual ~CHumanShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void BuildObjects(ID3D11Device *pd3dDevice);
	//virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
};


class CWizardShader : public CShader
{
public:
	CWizardShader();
	virtual ~CWizardShader();
	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void BuildObjects(ID3D11Device *pd3dDevice);
	virtual  void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
	virtual void CreateShaderVariables(ID3D11Device *pd3dDevice);
	virtual void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, 
		VS_CB_RESULT_MATRIX **pResult, VS_CB_RESULT_MATRIX *mapdata, int k);
	//virtual void AnimateObjects(float fTimeElapsed);
	virtual void AnimateObjects(float fTimeElapsed);


	static ID3D11Buffer	 *m_pd3dcbResult;
protected:
	CTexture* m_pTexture;
	long long m_llAniTime;
	UINT m_uiBoneIndexCount;

	// 뭐임 이거???
	//ID3D11Buffer *g_pd3dcbBoneMatrix = nullptr;
	D3DXMATRIX **m_ppResultMatrix;


	VS_CB_RESULT_MATRIX **m_pvscbResultMatrix;
	//VS_CB_RESULT_MATRIX *m_cbResult;
	VS_CB_RESULT_MATRIX *m_cbMapData;

	float m_fTimePos;
};

class CFixedObjectShader : public CShader
{
public:
	CFixedObjectShader();
	virtual ~CFixedObjectShader();
	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void BuildObjects(ID3D11Device *pd3dDevice);
//	virtual  void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
};