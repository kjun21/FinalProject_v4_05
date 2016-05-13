#pragma once
#include "stdafx.h"
#include "Vertex.h"
#include "DiffusedVertex.h"
using namespace std;
/*정점의 색상을 무작위로(Random) 설정하기 위해 사용한다. 각 정점의 색상은 난수(Random Number)를 생성하여 지정한다.*/
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
	//바운딩 박스의 최소점과 최대점을 나타내는 벡터이다.
	D3DXVECTOR3 m_d3dxvMinimum;
	D3DXVECTOR3 m_d3dxvMaximum;

public:
	AABB() { m_d3dxvMinimum = D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX); m_d3dxvMaximum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX); }
	AABB(D3DXVECTOR3 d3dxvMinimum, D3DXVECTOR3 d3dxvMaximum) { m_d3dxvMinimum = d3dxvMinimum; m_d3dxvMaximum = d3dxvMaximum; }

	//두 개의 바운딩 박스를 합한다.
	void Union(D3DXVECTOR3& d3dxvMinimum, D3DXVECTOR3& d3dxvMaximum);
	void Union(AABB *pAABB);
	//바운딩 박스의 8개의 꼭지점을 행렬로 변환하고 최소점과 최대점을 다시 계산한다.
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
	//Mesh.cpp 파일에서 CMesh::AddRef(), CMesh::Release() 함수의 정의를 삭제한다. 

protected:
	AABB m_bcBoundingCube;

protected:
	D3D11_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology;

	//정점의 위치 벡터와 색상을 저장하기 위한 버퍼에 대한 인터페이스 포인터이다. 
	ID3D11Buffer *m_pd3dPositionBuffer;
	
	//버퍼들을 입력조립기에 연결하기 위한 시작 슬롯 번호이다. 
	UINT m_nSlot;
	/*인스턴싱을 위한 정점 버퍼는 메쉬의 정점 데이터와 인스턴싱 데이터(객체의 위치와 방향)를 갖는다. 그러므로 인스턴싱을 위한 정점 버퍼는 하나가 아니라 버퍼들의 배열이다. 정점의 요소들을 나타내는 버퍼들을 입력조립기에 전달하기 위한 버퍼이다.*/
	ID3D11Buffer **m_ppd3dVertexBuffers;
	//정점을 조립하기 위해 필요한 버퍼의 개수이다. 
	int m_nBuffers;

	//정점의 개수이다. 
	int m_nVertices;
	UINT m_nStartVertex;
	//정점의 요소들을 나타내는 버퍼들의 원소의 바이트 수를 나타내는 배열이다. 
	UINT *m_pnVertexStrides;
	//정점의 요소들을 나타내는 버퍼들의 시작 위치(바이트 수)를 나타내는 배열이다. 
	UINT *m_pnVertexOffsets;

	//인덱스 버퍼(인덱스의 배열)에 대한 인터페이스 포인터이다. 
	ID3D11Buffer *m_pd3dIndexBuffer;
	//인덱스 버퍼가 포함하는 인덱스의 개수이다. 
	UINT m_nIndices;
	//인덱스 버퍼에서 메쉬를 표현하기 위해 사용되는 시작 인덱스이다. 
	UINT m_nStartIndex;
	//각 인덱스에 더해질 인덱스이다. 
	int m_nBaseVertex;
	UINT m_nIndexOffset;
	//각 인덱스의 형식(DXGI_FORMAT_R32_UINT 또는 DXGI_FORMAT_R16_UINT)이다. 
	DXGI_FORMAT	m_dxgiIndexFormat;

	ID3D11RasterizerState *m_pd3dRasterizerState;

public:
	AABB GetBoundingCube() { return(m_bcBoundingCube); }

	//메쉬의 정점 버퍼들을 배열로 조립한다. 
	void AssembleToVertexBuffer(int nBuffers = 0, ID3D11Buffer **m_pd3dBuffers = NULL, UINT *pnBufferStrides = NULL, UINT *pnBufferOffsets = NULL);

	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice) { }
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	//인스턴싱을 사용하여 렌더링한다. 
	virtual void RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances = 0, int nStartInstance = 0);

public:
	/*각 정점의 위치 벡터를 픽킹을 위하여 저장한다
	(정점 버퍼를 DYNAMIC으로 생성하고 Map()을 하지 않아도 되도록).*/
	D3DXVECTOR3 *m_pd3dxvPositions;
	/*메쉬의 인덱스를 저장한다(인덱스 버퍼를 DYNAMIC으로 생성하고 Map()을 하지 않아도 되도록).*/
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
	//조명의 영향을 계산하기 위하여 법선벡터가 필요하다.
	ID3D11Buffer *m_pd3dNormalBuffer;

public:
	//정점이 포함된 삼각형의 법선벡터를 계산하는 함수이다.
	D3DXVECTOR3 CalculateTriAngleNormal(UINT nIndex0, UINT nIndex1, UINT nIndex2);
	void SetTriAngleListVertexNormal(D3DXVECTOR3 *pd3dxvNormals);
	//정점의 법선벡터의 평균을 계산하는 함수이다.
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
	//텍스쳐 매핑을 하기 위하여 텍스쳐 좌표가 필요하다.
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







/*텍스쳐 매핑을 사용하여 색상을 결정하기 위하여 
   정점이 텍스쳐 좌표를 갖는 직육면체 메쉬 클래스이다.*/
class CCubeMeshTextured : public CMeshTextured
{
public:
	CCubeMeshTextured(ID3D11Device *pd3dDevice, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshTextured();
};

/*텍스쳐 매핑을 사용하여 색상을 결정하기 위하여 
  정점이 텍스쳐 좌표를 갖는 구 메쉬 클래스이다.*/
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


/*CHeightMapGridMesh 클래스의 베이스 클래스를 CMeshDetailTextured로 변경한다.
지형 메쉬의 각 정점은 두 개의 텍스쳐 좌표를 갖는다.*/

//CHeightMapGridMesh 클래스의 베이스 클래스를 CMeshDetailTexturedIlluminated로 변경한다.
class CHeightMapGridMesh : public CMeshDetailTexturedIlluminated
{
protected:
	int m_nWidth;
	int m_nLength;
	D3DXVECTOR3 m_d3dxvScale;

public:
	//생성자를 다음과 같이 변경한다.
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
