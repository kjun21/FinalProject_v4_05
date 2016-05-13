#pragma once
#include "stdafx.h"
#include "Vertex.h"
#include "DiffusedVertex.h"
using namespace std;
/*������ ������ ��������(Random) �����ϱ� ���� ����Ѵ�. �� ������ ������ ����(Random Number)�� �����Ͽ� �����Ѵ�.*/
#define RANDOM_COLOR D3DXCOLOR((rand() * 0xFFFFFF) / RAND_MAX)


struct MESHINTERSECTINFO {
	DWORD m_dwFaceIndex;
	float m_fU;
	float m_fV;
	float m_fDistance;
};


class AABB
{
public:
	//�ٿ�� �ڽ��� �ּ����� �ִ����� ��Ÿ���� �����̴�.
	D3DXVECTOR3 m_d3dxvMinimum;
	D3DXVECTOR3 m_d3dxvMaximum;

public:
	AABB() { m_d3dxvMinimum = D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX); m_d3dxvMaximum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX); }
	AABB(D3DXVECTOR3 d3dxvMinimum, D3DXVECTOR3 d3dxvMaximum) { m_d3dxvMinimum = d3dxvMinimum; m_d3dxvMaximum = d3dxvMaximum; }

	//�� ���� �ٿ�� �ڽ��� ���Ѵ�.
	void Union(D3DXVECTOR3& d3dxvMinimum, D3DXVECTOR3& d3dxvMaximum);
	void Union(AABB *pAABB);
	//�ٿ�� �ڽ��� 8���� �������� ��ķ� ��ȯ�ϰ� �ּ����� �ִ����� �ٽ� ����Ѵ�.
	void Update(D3DXMATRIX *pd3dxmtxTransform);
};


class CMesh
{
public:
	CMesh(ID3D11Device *pd3dDevice);
	virtual ~CMesh();

private:
	int m_nReferences;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	//Mesh.cpp ���Ͽ��� CMesh::AddRef(), CMesh::Release() �Լ��� ���Ǹ� �����Ѵ�. 

protected:
	AABB m_bcBoundingCube;

protected:
	D3D11_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology;

	//������ ��ġ ���Ϳ� ������ �����ϱ� ���� ���ۿ� ���� �������̽� �������̴�. 
	ID3D11Buffer *m_pd3dPositionBuffer;
	
	//���۵��� �Է������⿡ �����ϱ� ���� ���� ���� ��ȣ�̴�. 
	UINT m_nSlot;
	/*�ν��Ͻ��� ���� ���� ���۴� �޽��� ���� �����Ϳ� �ν��Ͻ� ������(��ü�� ��ġ�� ����)�� ���´�. �׷��Ƿ� �ν��Ͻ��� ���� ���� ���۴� �ϳ��� �ƴ϶� ���۵��� �迭�̴�. ������ ��ҵ��� ��Ÿ���� ���۵��� �Է������⿡ �����ϱ� ���� �����̴�.*/
	ID3D11Buffer **m_ppd3dVertexBuffers;
	//������ �����ϱ� ���� �ʿ��� ������ �����̴�. 
	int m_nBuffers;

	//������ �����̴�. 
	int m_nVertices;
	UINT m_nStartVertex;
	//������ ��ҵ��� ��Ÿ���� ���۵��� ������ ����Ʈ ���� ��Ÿ���� �迭�̴�. 
	UINT *m_pnVertexStrides;
	//������ ��ҵ��� ��Ÿ���� ���۵��� ���� ��ġ(����Ʈ ��)�� ��Ÿ���� �迭�̴�. 
	UINT *m_pnVertexOffsets;

	//�ε��� ����(�ε����� �迭)�� ���� �������̽� �������̴�. 
	ID3D11Buffer *m_pd3dIndexBuffer;
	//�ε��� ���۰� �����ϴ� �ε����� �����̴�. 
	UINT m_nIndices;
	//�ε��� ���ۿ��� �޽��� ǥ���ϱ� ���� ���Ǵ� ���� �ε����̴�. 
	UINT m_nStartIndex;
	//�� �ε����� ������ �ε����̴�. 
	int m_nBaseVertex;
	UINT m_nIndexOffset;
	//�� �ε����� ����(DXGI_FORMAT_R32_UINT �Ǵ� DXGI_FORMAT_R16_UINT)�̴�. 
	DXGI_FORMAT	m_dxgiIndexFormat;

	ID3D11RasterizerState *m_pd3dRasterizerState;

public:
	AABB GetBoundingCube() { return(m_bcBoundingCube); }

	//�޽��� ���� ���۵��� �迭�� �����Ѵ�. 
	void AssembleToVertexBuffer(int nBuffers = 0, ID3D11Buffer **m_pd3dBuffers = NULL, UINT *pnBufferStrides = NULL, UINT *pnBufferOffsets = NULL);

	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice) { }
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	//�ν��Ͻ��� ����Ͽ� �������Ѵ�. 
	virtual void RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances = 0, int nStartInstance = 0);

public:
	/*�� ������ ��ġ ���͸� ��ŷ�� ���Ͽ� �����Ѵ�
	(���� ���۸� DYNAMIC���� �����ϰ� Map()�� ���� �ʾƵ� �ǵ���).*/
	D3DXVECTOR3 *m_pd3dxvPositions;
	/*�޽��� �ε����� �����Ѵ�(�ε��� ���۸� DYNAMIC���� �����ϰ� Map()�� ���� �ʾƵ� �ǵ���).*/
	UINT *m_pnIndices;

	int CheckRayIntersection(D3DXVECTOR3 *pd3dxvRayPosition, D3DXVECTOR3 *pd3dxvRayDirection, MESHINTERSECTINFO *pd3dxIntersectInfo);
};
class CMeshDiffused : public CMesh
{
public:
	CMeshDiffused(ID3D11Device *pd3dDevice);
	virtual ~CMeshDiffused();

protected:
	ID3D11Buffer *m_pd3dColorBuffer;
};
class CMeshIlluminated : public CMesh
{
public:
	CMeshIlluminated(ID3D11Device *pd3dDevice);
	virtual ~CMeshIlluminated();

protected:
	//������ ������ ����ϱ� ���Ͽ� �������Ͱ� �ʿ��ϴ�.
	ID3D11Buffer *m_pd3dNormalBuffer;

public:
	//������ ���Ե� �ﰢ���� �������͸� ����ϴ� �Լ��̴�.
	D3DXVECTOR3 CalculateTriAngleNormal(UINT nIndex0, UINT nIndex1, UINT nIndex2);
	void SetTriAngleListVertexNormal(D3DXVECTOR3 *pd3dxvNormals);
	//������ ���������� ����� ����ϴ� �Լ��̴�.
	void SetAverageVertexNormal(D3DXVECTOR3 *pd3dxvNormals, int nPrimitives, int nOffset, bool bStrip);
	void CalculateVertexNormal(D3DXVECTOR3 *pd3dxvNormals);
};
class CCubeMeshIlluminated : public CMeshIlluminated
{
public:
	CCubeMeshIlluminated(ID3D11Device *pd3dDevice, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshIlluminated();
};
class CSphereMeshIlluminated : public CMeshIlluminated
{
public:
	CSphereMeshIlluminated(ID3D11Device *pd3dDevice, float fRadius = 2.0f, int nSlices = 20, int nStacks = 20);
	virtual ~CSphereMeshIlluminated();
};

class CSphereMeshDiffused : public CMeshDiffused
{
public:
	CSphereMeshDiffused(ID3D11Device *pd3dDevice, float fRadius = 2.0f, int nSlices = 20, int nStacks = 20, D3DXCOLOR d3dxColor = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f));
	virtual ~CSphereMeshDiffused();
};

class CMeshTextured : public CMesh
{
public:
	CMeshTextured(ID3D11Device *pd3dDevice);
	virtual ~CMeshTextured();

protected:
	//�ؽ��� ������ �ϱ� ���Ͽ� �ؽ��� ��ǥ�� �ʿ��ϴ�.
	ID3D11Buffer *m_pd3dTexCoordBuffer;
};

//========================================================
class CTexture;

class CSkyBoxMesh : public CMeshTextured
{
protected:
	ID3D11DepthStencilState *m_pd3dDepthStencilState;

	CTexture *m_pSkyboxTexture;
	ID3D11ShaderResourceView* m_pd3dsrvCubeMap;
public:
	CSkyBoxMesh(ID3D11Device *pd3dDevice, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 20.0f);
	virtual ~CSkyBoxMesh();

	void OnChangeSkyBoxTextures(ID3D11Device *pd3dDevice, int nIndex = 0);

	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};
//=========================================================

class CMeshDetailTextured : public CMeshTextured
{
public:
	CMeshDetailTextured(ID3D11Device *pd3dDevice);
	virtual ~CMeshDetailTextured();

protected:
	ID3D11Buffer *m_pd3dDetailTexCoordBuffer;
};







/*�ؽ��� ������ ����Ͽ� ������ �����ϱ� ���Ͽ� 
   ������ �ؽ��� ��ǥ�� ���� ������ü �޽� Ŭ�����̴�.*/
class CCubeMeshTextured : public CMeshTextured
{
public:
	CCubeMeshTextured(ID3D11Device *pd3dDevice, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshTextured();
};

/*�ؽ��� ������ ����Ͽ� ������ �����ϱ� ���Ͽ� 
  ������ �ؽ��� ��ǥ�� ���� �� �޽� Ŭ�����̴�.*/
class CSphereMeshTextured : public CMeshTextured
{
public:
	CSphereMeshTextured(ID3D11Device *pd3dDevice, float fRadius = 2.0f, int nSlices = 20, int nStacks = 20);
	virtual ~CSphereMeshTextured();
};


//======================================================

class CMeshTexturedIlluminated : public CMeshIlluminated
{
public:
	CMeshTexturedIlluminated(ID3D11Device *pd3dDevice);
	virtual ~CMeshTexturedIlluminated();

protected:
	ID3D11Buffer *m_pd3dTexCoordBuffer;
};

class CMeshDetailTexturedIlluminated : public CMeshIlluminated
{
public:
	CMeshDetailTexturedIlluminated(ID3D11Device *pd3dDevice);
	virtual ~CMeshDetailTexturedIlluminated();

protected:
	ID3D11Buffer *m_pd3dTexCoordBuffer;
	ID3D11Buffer *m_pd3dDetailTexCoordBuffer;
};

class CCubeMeshTexturedIlluminated : public CMeshTexturedIlluminated
{
public:
	CCubeMeshTexturedIlluminated(ID3D11Device *pd3dDevice, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshTexturedIlluminated();
};

class CSphereMeshTexturedIlluminated : public CMeshTexturedIlluminated
{
public:
	CSphereMeshTexturedIlluminated(ID3D11Device *pd3dDevice, float fRadius = 2.0f, int nSlices = 20, int nStacks = 20);
	virtual ~CSphereMeshTexturedIlluminated();
};


/*CHeightMapGridMesh Ŭ������ ���̽� Ŭ������ CMeshDetailTextured�� �����Ѵ�.
���� �޽��� �� ������ �� ���� �ؽ��� ��ǥ�� ���´�.*/

//CHeightMapGridMesh Ŭ������ ���̽� Ŭ������ CMeshDetailTexturedIlluminated�� �����Ѵ�.
class CHeightMapGridMesh : public CMeshDetailTexturedIlluminated
{
protected:
	int m_nWidth;
	int m_nLength;
	D3DXVECTOR3 m_d3dxvScale;

public:
	//�����ڸ� ������ ���� �����Ѵ�.
	CHeightMapGridMesh(ID3D11Device *pd3dDevice, int xStart, int zStart, int nWidth, int nLength, D3DXVECTOR3 d3dxvScale = D3DXVECTOR3(1.0f, 1.0f, 1.0f), void *pContext = NULL);
	virtual ~CHeightMapGridMesh();

	D3DXVECTOR3 GetScale() { return(m_d3dxvScale); }
	int GetWidth() { return(m_nWidth); }
	int GetLength() { return(m_nLength); }

	virtual float OnGetHeight(int x, int z, void *pContext);
};

class CPlaneMesh : public CMeshTexturedIlluminated
{
public:
	CPlaneMesh(ID3D11Device *pd3dDevice, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CPlaneMesh();
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};

class CNormalMappingPlaneMesh : public CMeshTexturedIlluminated
{
private:
	ID3D11Buffer *m_ppd3dTangentBuffer;

public:
	CNormalMappingPlaneMesh(ID3D11Device *pd3dDevice, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CNormalMappingPlaneMesh();
};

class CTessellationMesh : public CMeshTexturedIlluminated
{
private:
	ID3D11Buffer *m_ppd3dTangentBuffer;

public:
	CTessellationMesh(ID3D11Device *pd3dDevice, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CTessellationMesh();
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);

};


class CDisplacementMappingMesh : public CMeshTexturedIlluminated
{
private:
	ID3D11Buffer *m_ppd3dTangentBuffer;
public:
	CDisplacementMappingMesh(ID3D11Device *pd3dDevice, 
		                           float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CDisplacementMappingMesh();
};





class CTessellationHeightMapGridMesh : public CMeshDetailTexturedIlluminated
{
protected:
	int m_nWidth;
	int m_nLength;
	D3DXVECTOR3 m_d3dxvScale;
public:
	CTessellationHeightMapGridMesh(ID3D11Device *pd3dDevice, int nWidth, int nLength, 
		                                           D3DXVECTOR3 d3dxvScale = D3DXVECTOR3(1.0f, 1.0f, 1.0f));
	virtual ~CTessellationHeightMapGridMesh();

};

struct DisplacementVertex
{
	D3DXVECTOR3 m_pd3dxvPositions;
	D3DXVECTOR3 m_pd3dxvNormals;
	D3DXVECTOR3 m_pd3dxvTangents;
	D3DXVECTOR2 m_pd3dxvTexCoords;
};


class CTestDisplacementMappingMesh : public CMeshTexturedIlluminated
{
private:
	ID3D11Buffer *m_ppd3dTangentBuffer;
public:
	CTestDisplacementMappingMesh(ID3D11Device *pd3dDevice,
		float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CTestDisplacementMappingMesh();
	virtual  void Render(ID3D11DeviceContext *pd3dDeviceContext);
};




class CHumanMesh : public CMesh
{
public:
	virtual ~CHumanMesh();
	CHumanMesh(ID3D11Device *pd3dDevice);

private:
	ID3D11Buffer* m_pd3dColorBuffer;

public:
	//void LoadFbx();

};
class CCharacterMesh : public CMeshTexturedIlluminated
{
public:
	CCharacterMesh(ID3D11Device *pd3dDevice, string strFileName);
	virtual ~CCharacterMesh();
	D3DXMATRIX*  Rotate(D3DXVECTOR3 *pd3dxvAxis, float fAngle);
	void CreateChileMesh(ID3D11Device *pd3dDevice);
	virtual  void Render(ID3D11DeviceContext *pd3dDeviceContext);

protected:
	ID3D11Buffer *m_pd3dWeightBuffer1;
	ID3D11Buffer *m_pd3dBoneIndexBuffer1;
	ID3D11Buffer *m_pd3dWeightBuffer2;
	ID3D11Buffer *m_pd3dBoneIndexBuffer2;
	ID3D11Buffer* m_pd3dColorBuffer;

	BoneWeightData1*  m_pWeight1;
	BoneIndexData1    *m_pBoneIndex1;
	
	BoneWeightData2*  m_pWeight2;
	BoneIndexData2    *m_pBoneIndex2;
	string strMeshName;
};

class CBoundingMesh : public CMesh
{
public:
	ID3D11Buffer* m_pd3dColorBuffer;
	CBoundingMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth);
	virtual ~CBoundingMesh();
};

class CBoundingSphere : public CMesh
{
public:
	ID3D11Buffer* m_pd3dColorBuffer;
	CBoundingSphere(ID3D11Device *pd3dDevice, float fRadius, int nSlices, int nStacks);
	virtual ~CBoundingSphere();
	virtual  void Render(ID3D11DeviceContext *pd3dDeviceContext);
};


class CBoundingCircle : public CMesh
{
public:
	ID3D11Buffer* m_pd3dColorBuffer;
	CBoundingCircle(ID3D11Device *pd3dDevice, float fRadius, int nSlices, int nStacks);
	virtual ~CBoundingCircle();
};

class CUIMesh :public CMeshTextured
{
public:
	CUIMesh(ID3D11Device *pd3dDevice, float width, float height);
	virtual ~CUIMesh();

};


class CAlphaBlendingMirrorMeshTextured : public CMeshTextured
{
public:
	CAlphaBlendingMirrorMeshTextured(ID3D11Device *pd3dDevice,
		float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CAlphaBlendingMirrorMeshTextured();
	//virtual  void Render(ID3D11DeviceContext *pd3dDeviceContext);
};
