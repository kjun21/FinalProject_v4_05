#pragma once
#include "Mesh.h"
#include "WaveMesh.h"
#include "FixedMesh.h"
#include "stdafx.h"
#include "Camera.h"
#include "HeightMap.h"

#define DIR_FORWARD	0x01
#define DIR_BACKWARD	0x02

#define DIR_LEFT	0x04
#define DIR_RIGHT	0x08
#define DIR_UP		0x11
#define DIR_DOWN	0x20
#define ATTACK01 0x50
#define ATTACK02 0x51





#define DIR_FORWARD_RIGHT 0x09
#define DIR_FORWARD_LEFT 0x05
#define DIR_BACKWARD_RIGHT 0x10
#define DIR_BACKWARD_LEFT 0x06




#define TYPE_PLAYER 0x01
#define TYPE_WATER 0x02
#define TYPE_MONSTER 0x03
#define TYPE_BOSS_MONSTER 0x04
#define TYPE_ROCK 0x05
#define TYPE_TREE 0x06


#define TYPE_COPY1 0x07
#define TYPE_COPY2 0x08






struct VS_CB_WAVE_MATRICES
{
	D3DXMATRIX m_d3dxmtxWavesDispTexTransform1;
	D3DXMATRIX m_d3dxmtxWavesDispTexTransform2;
	D3DXMATRIX m_d3dxmtxWavesNormalTexTransform1;
	D3DXMATRIX m_d3dxmtxWavesNormalTexTransform2;
};


struct MATERIAL
{
	D3DXCOLOR m_d3dxcAmbient;
	D3DXCOLOR m_d3dxcDiffuse;
	D3DXCOLOR m_d3dxcSpecular; //(r,g,b,a=power)
	D3DXCOLOR m_d3dxcEmissive;
};

class CMaterial
{
public:
	CMaterial();
	virtual ~CMaterial();

private:
	int m_nReferences;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	MATERIAL m_Material;
};

//���� ��ü�� �ϳ� �̻��� �ؽ��ĸ� ���� �� �ִ�. 
//CTexture�� �ؽ��ĸ� �����ϱ� ���� Ŭ�����̴�.
class CTexture
{
public:
	CTexture(int nTextures = 1, int nSamplers = 1, int nTextureStartSlot = 0, int nSamplerStartSlot = 0);
	virtual ~CTexture();

private:
	int m_nReferences;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

private:
	//�ؽ��� ���ҽ��� �����̴�.
	int m_nTextures;
	ID3D11ShaderResourceView **m_ppd3dsrvTextures;
	//�ؽ��� ���ҽ��� ������ ���� �����̴�.
	int m_nTextureStartSlot;
	//���÷� ���� ��ü�� �����̴�.
	int m_nSamplers;
	ID3D11SamplerState **m_ppd3dSamplerStates;
	//���÷� ���� ��ü�� ������ ���� �����̴�.
	int m_nSamplerStartSlot;

public:
	void SetTexture(int nIndex, ID3D11ShaderResourceView *pd3dsrvTexture);
	void SetSampler(int nIndex, ID3D11SamplerState *pd3dSamplerState);
	//�ؽ��� ���ҽ��� ���÷� ���� ��ü�� ���� ���̴� ������ �����Ѵ�.
	void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext);
	//�ؽ��� ���ҽ��� ���� ���̴� ������ �����Ѵ�.
	void UpdateTextureShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, int nIndex = 0, int nSlot = 0);
	//���÷� ���� ��ü�� ���� ���̴� ������ �����Ѵ�.
	void UpdateSamplerShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, int nIndex = 0, int nSlot = 0);

	void UpdateShaderVariableDS(ID3D11DeviceContext *pd3dDeviceContext);
	void UpdateShaderVariableVS(ID3D11DeviceContext *pd3dDeviceContext);


	void ClearShaderVariableDS(ID3D11DeviceContext *pd3dDeviceContext)
	{
		pd3dDeviceContext->DSSetShaderResources(NULL, NULL, NULL);
		pd3dDeviceContext->DSSetSamplers(NULL, NULL, NULL);
	}


};

class CGameObject
{
public:
	CGameObject(int nMeshes = 0);
	virtual ~CGameObject();

	//���� �������� �����.
public:
	D3DXMATRIX m_d3dxmtxWorld;
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
	//��ü�� ��ġ�� �����Ѵ�.
	virtual void SetPosition(float x, float y, float z);
	virtual void SetPosition(D3DXVECTOR3 d3dxvPosition);
	D3DXVECTOR3 GetPosition();
	void SetDirection(D3DXVECTOR3 d3dxvDirection);
	void SetRight(D3DXVECTOR3 d3dxvRight);
	void SetUp(D3DXVECTOR3 d3dxvUp);
	void SetObjectType(UINT nObjectType);
	void SetPlayerType();
	void SetMonsterType();
	void RenewWorldMatrix();
	UINT GetObjectType() { return m_nObjectType; }

	
private:
	int m_nReferences;
protected:
	UINT m_nObjectType;
public:
	void AddRef();
	void Release();

public:
	//���� x-��, y-��, z-�� �������� �̵��Ѵ�.
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	//���� x-��, y-��, z-�� �������� ȸ���Ѵ�.
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(D3DXVECTOR3 *pd3dxvAxis, float fAngle);
	void Scale(D3DXVECTOR3 pd3dxvAxis);
	//��ü�� ��ġ, ���� x-��, y-��, z-�� ���� ���͸� ��ȯ�Ѵ�.
	D3DXVECTOR3 GetLookAt();
	D3DXVECTOR3 GetUp();
	D3DXVECTOR3 GetRight();

	//��ü�� �������ϱ� ���� ȣ��Ǵ� �Լ��̴�.
	virtual void OnPrepareRender() { }
	virtual void CheckMove(float fTime)
	{
		m_d3dxmtxWorld._43 += fTime;
	}

	D3DXVECTOR3 m_d3dxvScale;
	D3DXMATRIX m_d3dmtxScale;
	void ScalePlayer(D3DXVECTOR3 d3dxvScale)
	{
		D3DXMatrixScaling(&m_d3dmtxScale, d3dxvScale.x, d3dxvScale.y, d3dxvScale.z);
	}
	//��ü�� ī�޶��� ����ü ���ο� �ִ� ���� �Ǵ��Ѵ�. 
public:
	bool IsVisible(CCamera *pCamera = NULL);

private:
	bool m_bActive;
public:
	void SetActive(bool bActive = false) { m_bActive = bActive; }

	//���� ��ǥ���� ��ŷ ������ �����Ѵ�.
	void GenerateRayForPicking(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxWorld, D3DXMATRIX *pd3dxmtxView, D3DXVECTOR3 *pd3dxvPickRayPosition, D3DXVECTOR3 *pd3dxvPickRayDirection);
	//���� ��ǥ���� ��ŷ ������ �����Ѵ�.
	int PickObjectByRayIntersection(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxView, MESHINTERSECTINFO *pd3dxIntersectInfo);

public:
	//��ü�� ������ �޽��鿡 ���� �����Ϳ� �� �����̴�.
	CMesh **m_ppMeshes;
	int m_nMeshes;
	//��ü�� ������ �޽� ��ü�� ���� �ٿ�� �ڽ��̴�.
	AABB m_bcMeshBoundingCube;
	CMaterial *m_pMaterial;
	void SetMaterial(CMaterial *pMaterial);
	void SetMesh(CMesh *pMesh, int nIndex = 0);
	CMesh *GetMesh(int nIndex = 0) { return(m_ppMeshes[nIndex]); }


public:
	//���� ��ü�� �ؽ��� ���� �� �ִ�.
	CTexture *m_pTexture;
	void SetTexture(CTexture *pTexture);
};


class CRevolvingObject : public CGameObject
{
public:
	CRevolvingObject(int nMeshes = 1);
	virtual ~CRevolvingObject();

	virtual void Animate(float fTimeElapsed);

private:
	// ���� ȸ����� ȸ�� �ӷ��� ��Ÿ����.
	D3DXVECTOR3 m_d3dxvRevolutionAxis;
	float m_fRevolutionSpeed;

public:
	// ���� �ӷ��� �����Ѵ�.
	void SetRevolutionSpeed(float fRevolutionSpeed) { m_fRevolutionSpeed = fRevolutionSpeed; }
	// ������ ���� ȸ������ �����Ѵ�.
	void SetRevolutionAxis(D3DXVECTOR3 d3dxvRevolutionAxis) { m_d3dxvRevolutionAxis = d3dxvRevolutionAxis; }

};

// ���� �� ���� Ŭ����
class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(ID3D11Device *pd3dDevice, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, D3DXVECTOR3 d3dxvScale);
	virtual ~CHeightMapTerrain();

private:
	//������ ���� ������ ����� �̹����̴�.
	CHeightMap *m_pHeightMap;

	//������ ���ο� ���� ũ���̴�.
	int m_nWidth;
	int m_nLength;

	//������ ������ �� �� Ȯ���� ���ΰ��� ��Ÿ���� ������ �����̴�.
	D3DXVECTOR3 m_d3dxvScale;

public:
	//������ ���� ���̸� ��ȯ�Ѵ�. ���� ���� ���̿� �������� ���� ���̴�.
	float GetHeight(float x, float z, bool bReverseQuad = false) { return(m_pHeightMap->GetHeight(x, z, bReverseQuad) * m_d3dxvScale.y); }
	D3DXVECTOR3 GetNormal(float x, float z) { return(m_pHeightMap->GetHeightMapNormal(int(x / m_d3dxvScale.x), int(z / m_d3dxvScale.z))); }

	int GetHeightMapWidth() { return(m_pHeightMap->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMap->GetHeightMapLength()); }

	D3DXVECTOR3 GetScale() { return(m_d3dxvScale); }
	//������ ���� ũ��(����/����)�� ��ȯ�Ѵ�. ���� ���� ũ�⿡ �������� ���� ���̴�.
	float GetWidth() { return(m_nWidth * m_d3dxvScale.x); }
	float GetLength() { return(m_nLength * m_d3dxvScale.z); }

	float GetPeakHeight() { return(m_bcMeshBoundingCube.m_d3dxvMaximum.y); }
};

class CSkyBox : public CGameObject
{
public:
	CSkyBox(ID3D11Device *pd3dDevice);
	virtual ~CSkyBox();

	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
};

class CPlaneObject : public CGameObject
{
public:
	CPlaneObject(ID3D11Device *pd3dDevice);
	virtual ~CPlaneObject();

	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
};


class CPlaneNormalMappingObject : public CPlaneObject
{
private:
	CTexture* m_pNoramlMap;

public:
	CPlaneNormalMappingObject(ID3D11Device *pd3dDevice);
	virtual ~CPlaneNormalMappingObject();

	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
};

class CTessellationObject : public CGameObject
{
public:
	CTessellationObject(ID3D11Device *pd3dDevice);
	virtual ~CTessellationObject();

	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
};

class CWaveObject : public CGameObject
{
public:
	CWaveObject(ID3D11Device *pd3dDevice);
	virtual ~CWaveObject();

	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
	void CreateTextures(ID3D11Device *pd3dDevice);
	virtual void Animate(float fTimeElapsed);
	void CreateWaveConsBuffer(ID3D11Device *pd3dDevice);
	void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext);
	void CreateBlendingState(ID3D11Device *pd3dDevice);
protected:
	CTexture* m_pStonesTexture;

	CTexture* m_pWavesNormalMap1;
	CTexture* m_pWavesNormalMap2;

	D3DXVECTOR2 m_WavesDispOffset1;
	D3DXVECTOR2 m_WavesDispOffset2;
	D3DXVECTOR2 m_WavesNormalOffset1;
	D3DXVECTOR2 m_WavesNormalOffset2;

	ID3D11Buffer	 *m_pd3dcbWavesTranformMatrices;
	ID3D11BlendState* m_pd3dBlendingState;

	D3DXMATRIX m_d3dxmtxWavesDispTexTransform1;
	D3DXMATRIX m_d3dxmtxWavesDispTexTransform2;
	D3DXMATRIX m_d3dxmtxWavesNormalTexTransform1;
	D3DXMATRIX m_d3dxmtxWavesNormalTexTransform2;

	//D3DXMATRIX m_d3d

	//CTexture* m_pStones;
};

class CDisplacementMappingObject : public CGameObject
{
private:
	CTexture* m_pNoramlMap;
public:
	CDisplacementMappingObject(ID3D11Device *pd3dDevice);
	virtual ~CDisplacementMappingObject();
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
};




class CTestDisplacementMappingObject : public CGameObject
{
private:
	CTexture* m_pNoramlMap;
public:
	CTestDisplacementMappingObject(ID3D11Device *pd3dDevice);
	virtual ~CTestDisplacementMappingObject();
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
};




// ���� �� ���� Ŭ����


class CTessellationHeightMapTerrain : public CGameObject
{
public:
	CTessellationHeightMapTerrain(ID3D11Device *pd3dDevice, int nWidth, int nLength,
		D3DXVECTOR3 d3dxvScale);
	virtual ~CTessellationHeightMapTerrain();
private:
	int m_nWidth;
	int m_nLength;
	D3DXVECTOR3 m_d3dxvScale;
};


class CHumanObject : public CGameObject
{
public:
	CHumanObject(ID3D11Device *pd3dDevice);
	virtual ~CHumanObject();

	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);

};

class CWizardObject : public CGameObject
{
protected:
	AnimationClip* m_AnimationClip;
	UINT m_nAnimationState;
	ID3D11Buffer	 *m_pd3dcbAnimation;
	D3DXMATRIX **m_ppResultMatrix;
	VS_CB_RESULT_MATRIX *m_cbMapData;
public:
	CWizardObject(ID3D11Device *pd3dDevice, string strFileName);
	virtual ~CWizardObject();
	
	AnimationClip* CreateAnimation(AnimationClip* animationClip);
	void LoadAnimation(AnimationClip* animationClip, UINT i);
	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
	virtual void Animate(float fTimeElapsed);

};

class CWoodObject : public CGameObject
{
public:
	CWoodObject(ID3D11Device *pd3dDevice, string strFileName);
	virtual ~CWoodObject();

};
class CLeavesObject : public CGameObject
{
public:
	CLeavesObject(ID3D11Device *pd3dDevice, string strFileName);
	virtual ~CLeavesObject();
};

class CBoundingBoxObject : public CGameObject
{
public:
	CBoundingBoxObject(ID3D11Device *pd3dDevice);
	virtual ~CBoundingBoxObject();
};
