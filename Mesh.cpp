#include "Mesh.h"

#include "Object.h"
using namespace std;
CMesh::CMesh(ID3D11Device *pd3dDevice)
{
	m_nBuffers = 0;
	m_pd3dPositionBuffer = NULL;
	m_ppd3dVertexBuffers = NULL;

	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_nSlot = 0;
	m_nStartVertex = 0;

	m_pd3dIndexBuffer = NULL;
	m_nIndices = 0;
	m_nStartIndex = 0;
	m_nBaseVertex = 0;
	m_nIndexOffset = 0;
	m_dxgiIndexFormat = DXGI_FORMAT_R32_UINT;

	m_pd3dRasterizerState = NULL;

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	m_nReferences = 0;

	m_pd3dxvPositions = NULL;
	m_pnIndices = NULL;
}

CMesh::~CMesh()
{
	if (m_pd3dRasterizerState) m_pd3dRasterizerState->Release();
	if (m_pd3dPositionBuffer) m_pd3dPositionBuffer->Release();
	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();

	if (m_ppd3dVertexBuffers) delete[] m_ppd3dVertexBuffers;
	if (m_pnVertexStrides) delete[] m_pnVertexStrides;
	if (m_pnVertexOffsets) delete[] m_pnVertexOffsets;

	if (m_pd3dxvPositions) delete[] m_pd3dxvPositions;
	if (m_pnIndices) delete[] m_pnIndices;
}
// 메쉬를 그린다. 메쉬 클래스의 정점 버퍼를 디바이스 컨텍스트에 연결하고 프리미티브 유형을 설정한다.  
void CMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	//메쉬의 정점은 여러 개의 정점 버퍼로 표현된다.
	pd3dDeviceContext->IASetVertexBuffers(m_nSlot, m_nBuffers, m_ppd3dVertexBuffers, m_pnVertexStrides, m_pnVertexOffsets);
	pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, m_dxgiIndexFormat, m_nIndexOffset);
	pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);

	if (m_pd3dIndexBuffer)
		pd3dDeviceContext->DrawIndexed(m_nIndices, m_nStartIndex, m_nBaseVertex);
	else
		pd3dDeviceContext->Draw(m_nVertices, m_nStartVertex);
	pd3dDeviceContext->RSSetState(NULL);
}
void CMesh::RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances, int nStartInstance)
{
	//인스턴싱의 경우 입력 조립기에 메쉬의 정점 버퍼와 인스턴스 정점 버퍼가 연결된다.
	pd3dDeviceContext->IASetVertexBuffers(m_nSlot, m_nBuffers, m_ppd3dVertexBuffers, m_pnVertexStrides, m_pnVertexOffsets);
	pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, m_dxgiIndexFormat, m_nIndexOffset);
	pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);

	//객체들의 인스턴스들을 렌더링한다. 
	if (m_pd3dIndexBuffer)
		pd3dDeviceContext->DrawIndexedInstanced(m_nIndices, nInstances, m_nStartIndex, m_nBaseVertex, nStartInstance);
	else
		pd3dDeviceContext->DrawInstanced(m_nVertices, nInstances, m_nStartVertex, nStartInstance);
}
//void CMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
//{
//}

void CMesh::AssembleToVertexBuffer(int nBuffers, ID3D11Buffer **ppd3dBuffers, UINT *pnBufferStrides, UINT *pnBufferOffsets)
{
	ID3D11Buffer **ppd3dNewVertexBuffers = new ID3D11Buffer*[m_nBuffers + nBuffers];
	UINT *pnNewVertexStrides = new UINT[m_nBuffers + nBuffers];
	UINT *pnNewVertexOffsets = new UINT[m_nBuffers + nBuffers];

	if (m_nBuffers > 0)
	{
		for (int i = 0; i < m_nBuffers; i++)
		{
			ppd3dNewVertexBuffers[i] = m_ppd3dVertexBuffers[i];
			pnNewVertexStrides[i] = m_pnVertexStrides[i];
			pnNewVertexOffsets[i] = m_pnVertexOffsets[i];
		}
		if (m_ppd3dVertexBuffers) delete[] m_ppd3dVertexBuffers;
		if (m_pnVertexStrides) delete[] m_pnVertexStrides;
		if (m_pnVertexOffsets) delete[] m_pnVertexOffsets;
	}

	for (int i = 0; i < nBuffers; i++)
	{
		ppd3dNewVertexBuffers[m_nBuffers + i] = ppd3dBuffers[i];
		pnNewVertexStrides[m_nBuffers + i] = pnBufferStrides[i];
		pnNewVertexOffsets[m_nBuffers + i] = pnBufferOffsets[i];
	}

	m_nBuffers += nBuffers;
	m_ppd3dVertexBuffers = ppd3dNewVertexBuffers;

	m_pnVertexStrides = pnNewVertexStrides;

	m_pnVertexOffsets = pnNewVertexOffsets;
}


int CMesh::CheckRayIntersection(D3DXVECTOR3 *pd3dxvRayPosition, D3DXVECTOR3 *pd3dxvRayDirection, MESHINTERSECTINFO *pd3dxIntersectInfo)
{
	//모델 좌표계의 광선의 시작점(pd3dxvRayPosition)과 방향이 주어질 때 메쉬와의 충돌 검사를 한다.
	int nIntersections = 0;
	BYTE *pbPositions = (BYTE *)m_pd3dxvPositions + m_pnVertexOffsets[0];

	int nOffset = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? 3 : 1;
	//메쉬의 프리미티브(삼각형)들의 개수이다. 삼각형 리스트인 경우 (정점의 개수 / 3) 또는 (인덱스의 개수 / 3), 삼각형 스트립의 경우 (정점의 개수 - 2) 또는 (인덱스의 개수 - 2)이다.
	int nPrimitives = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nVertices / 3) : (m_nVertices - 2);
	if (m_nIndices > 0) nPrimitives = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nIndices / 3) : (m_nIndices - 2);

	D3DXVECTOR3 v0, v1, v2;
	float fuHitBaryCentric, fvHitBaryCentric, fHitDistance, fNearHitDistance = FLT_MAX;
	/*메쉬의 모든 프리미티브(삼각형)들에 대하여 픽킹 광선과의 충돌을 검사한다. 충돌하는 모든 삼각형을 찾아 광선의 시작점(실제로는 카메라 좌표계의 원점)에 가장 가까운 삼각형을 찾는다.*/
	for (int i = 0; i < nPrimitives; i++)
	{
		v0 = *(D3DXVECTOR3 *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i*nOffset) + 0]) : ((i*nOffset) + 0)) * m_pnVertexStrides[0]);
		v1 = *(D3DXVECTOR3 *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i*nOffset) + 1]) : ((i*nOffset) + 1)) * m_pnVertexStrides[0]);
		v2 = *(D3DXVECTOR3 *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i*nOffset) + 2]) : ((i*nOffset) + 2)) * m_pnVertexStrides[0]);
		if (D3DXIntersectTri(&v0, &v1, &v2, pd3dxvRayPosition, pd3dxvRayDirection, &fuHitBaryCentric, &fvHitBaryCentric, &fHitDistance))
		{
			if (fHitDistance < fNearHitDistance)
			{
				fNearHitDistance = fHitDistance;
				if (pd3dxIntersectInfo)
				{
					pd3dxIntersectInfo->m_dwFaceIndex = i;
					pd3dxIntersectInfo->m_fU = fuHitBaryCentric;
					pd3dxIntersectInfo->m_fV = fvHitBaryCentric;
					pd3dxIntersectInfo->m_fDistance = fHitDistance;
				}
			}
			nIntersections++;
		}
	}
	return(nIntersections);
}
CMeshDiffused::CMeshDiffused(ID3D11Device *pd3dDevice) : CMesh(pd3dDevice)
{
	m_pd3dColorBuffer = NULL;
}

CMeshDiffused::~CMeshDiffused()
{
	if (m_pd3dColorBuffer) m_pd3dColorBuffer->Release();
}
CSphereMeshDiffused::CSphereMeshDiffused(ID3D11Device *pd3dDevice, float fRadius, int nSlices, int nStacks, D3DXCOLOR d3dxColor) : CMeshDiffused(pd3dDevice)
{
	m_nVertices = (nSlices * nStacks) * 3 * 2;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//구 메쉬는 2개의 정점 버퍼(위치 벡터 버퍼와 색상 버퍼)로 구성된다.
	//구 메쉬의 정점 버퍼(위치 벡터 버퍼)를 생성한다.

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	//D3DXVECTOR3 *pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	float theta_i, theta_ii, phi_j, phi_jj, fRadius_j, fRadius_jj, y_j, y_jj;
	for (int j = 0, k = 0; j < nStacks; j++)
	{
		phi_j = float(D3DX_PI / nStacks) * j;
		phi_jj = float(D3DX_PI / nStacks) * (j + 1);
		fRadius_j = fRadius * sinf(phi_j);
		fRadius_jj = fRadius * sinf(phi_jj);
		y_j = fRadius*cosf(phi_j);
		y_jj = fRadius*cosf(phi_jj);
		for (int i = 0; i < nSlices; i++)
		{
			theta_i = float(2 * D3DX_PI / nSlices) * i;
			theta_ii = float(2 * D3DX_PI / nSlices) * (i + 1);
			m_pd3dxvPositions[k++] = D3DXVECTOR3(fRadius_j*cosf(theta_i), y_j, fRadius_j*sinf(theta_i));
			m_pd3dxvPositions[k++] = D3DXVECTOR3(fRadius_jj*cosf(theta_i), y_jj, fRadius_jj*sinf(theta_i));
			m_pd3dxvPositions[k++] = D3DXVECTOR3(fRadius_j*cosf(theta_ii), y_j, fRadius_j*sinf(theta_ii));
			m_pd3dxvPositions[k++] = D3DXVECTOR3(fRadius_jj*cosf(theta_i), y_jj, fRadius_jj*sinf(theta_i));
			m_pd3dxvPositions[k++] = D3DXVECTOR3(fRadius_jj*cosf(theta_ii), y_jj, fRadius_jj*sinf(theta_ii));
			m_pd3dxvPositions[k++] = D3DXVECTOR3(fRadius_j*cosf(theta_ii), y_j, fRadius_j*sinf(theta_ii));
		}
	}

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	//delete[] m_pd3dxvPositions;

	//구 메쉬의 정점 버퍼(색상 버퍼)를 생성한다.
	D3DXCOLOR *pd3dxColors = new D3DXCOLOR[m_nVertices];
	for (int i = 0; i < m_nVertices; i++) pd3dxColors[i] = d3dxColor + RANDOM_COLOR;

	d3dBufferDesc.ByteWidth = sizeof(D3DXCOLOR) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxColors;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dColorBuffer);

	delete[] pd3dxColors;

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dColorBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXCOLOR) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fRadius, -fRadius, -fRadius);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fRadius, +fRadius, +fRadius);
}

CSphereMeshDiffused::~CSphereMeshDiffused()
{
}

void AABB::Union(D3DXVECTOR3& d3dxvMinimum, D3DXVECTOR3& d3dxvMaximum)
{
	if (d3dxvMinimum.x < m_d3dxvMinimum.x) m_d3dxvMinimum.x = d3dxvMinimum.x;
	if (d3dxvMinimum.y < m_d3dxvMinimum.y) m_d3dxvMinimum.y = d3dxvMinimum.y;
	if (d3dxvMinimum.z < m_d3dxvMinimum.z) m_d3dxvMinimum.z = d3dxvMinimum.z;
	if (d3dxvMaximum.x > m_d3dxvMaximum.x) m_d3dxvMaximum.x = d3dxvMaximum.x;
	if (d3dxvMaximum.y > m_d3dxvMaximum.y) m_d3dxvMaximum.y = d3dxvMaximum.y;
	if (d3dxvMaximum.z > m_d3dxvMaximum.z) m_d3dxvMaximum.z = d3dxvMaximum.z;
}

void AABB::Union(AABB *pAABB)
{
	Union(pAABB->m_d3dxvMinimum, pAABB->m_d3dxvMaximum);
}

void AABB::Update(D3DXMATRIX *pmtxTransform)
{
	/*바운딩 박스의 최소점과 최대점은 회전을 하면 더 이상 최소점과 최대점이 되지 않는다.
	그러므로 바운딩 박스의 최소점과 최대점에서 8개의 정점을 구하고 변환(회전)을 한 다음 최소점과 최대점을 다시 계산한다.*/

	D3DXVECTOR3 vVertices[8];
	vVertices[0] = D3DXVECTOR3(m_d3dxvMinimum.x, m_d3dxvMinimum.y, m_d3dxvMinimum.z);
	vVertices[1] = D3DXVECTOR3(m_d3dxvMinimum.x, m_d3dxvMinimum.y, m_d3dxvMaximum.z);
	vVertices[2] = D3DXVECTOR3(m_d3dxvMaximum.x, m_d3dxvMinimum.y, m_d3dxvMaximum.z);
	vVertices[3] = D3DXVECTOR3(m_d3dxvMaximum.x, m_d3dxvMinimum.y, m_d3dxvMinimum.z);
	vVertices[4] = D3DXVECTOR3(m_d3dxvMinimum.x, m_d3dxvMaximum.y, m_d3dxvMinimum.z);
	vVertices[5] = D3DXVECTOR3(m_d3dxvMinimum.x, m_d3dxvMaximum.y, m_d3dxvMaximum.z);
	vVertices[6] = D3DXVECTOR3(m_d3dxvMaximum.x, m_d3dxvMaximum.y, m_d3dxvMaximum.z);
	vVertices[7] = D3DXVECTOR3(m_d3dxvMaximum.x, m_d3dxvMaximum.y, m_d3dxvMinimum.z);
	m_d3dxvMinimum = D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX);
	m_d3dxvMaximum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	//8개의 정점에서 x, y, z 좌표의 최소값과 최대값을 구한다.
	for (int i = 0; i < 8; i++)
	{
		//정점을 변환한다.
		D3DXVec3TransformCoord(&vVertices[i], &vVertices[i], pmtxTransform);
		if (vVertices[i].x < m_d3dxvMinimum.x) m_d3dxvMinimum.x = vVertices[i].x;
		if (vVertices[i].y < m_d3dxvMinimum.y) m_d3dxvMinimum.y = vVertices[i].y;
		if (vVertices[i].z < m_d3dxvMinimum.z) m_d3dxvMinimum.z = vVertices[i].z;
		if (vVertices[i].x > m_d3dxvMaximum.x) m_d3dxvMaximum.x = vVertices[i].x;
		if (vVertices[i].y > m_d3dxvMaximum.y) m_d3dxvMaximum.y = vVertices[i].y;
		if (vVertices[i].z > m_d3dxvMaximum.z) m_d3dxvMaximum.z = vVertices[i].z;
	}
}

CHeightMapGridMesh::CHeightMapGridMesh(ID3D11Device *pd3dDevice, int xStart, int zStart, int nWidth, int nLength, D3DXVECTOR3 d3dxvScale, void *pContext) : CMeshDetailTexturedIlluminated(pd3dDevice)
{
	m_nVertices = nWidth * nLength; //257 257
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR3 *pd3dxvNormals = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR2 *pd3dxvTexCoords = new D3DXVECTOR2[m_nVertices];
	D3DXVECTOR2 *pd3dxvDetailTexCoords = new D3DXVECTOR2[m_nVertices];

	m_nWidth = nWidth;
	m_nLength = nLength;
	m_d3dxvScale = d3dxvScale;

	CHeightMap *pHeightMap = (CHeightMap *)pContext;
	int cxHeightMap = pHeightMap->GetHeightMapWidth();
	int czHeightMap = pHeightMap->GetHeightMapLength();
	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;
	for (int i = 0, z = zStart; z < (zStart + nLength); z++)
	{
		for (int x = xStart; x < (xStart + nWidth); x++, i++)
		{
			fHeight = OnGetHeight(x, z, pContext);
			m_pd3dxvPositions[i] = D3DXVECTOR3((x*m_d3dxvScale.x), fHeight, (z*m_d3dxvScale.z));

			pd3dxvNormals[i] = pHeightMap->GetHeightMapNormal(x, z);
			pd3dxvTexCoords[i] = D3DXVECTOR2(float(x) / float(cxHeightMap - 1), float(czHeightMap - 1 - z) / float(czHeightMap - 1));
			pd3dxvDetailTexCoords[i] = D3DXVECTOR2(float(x) / float(m_d3dxvScale.x*0.125f), float(z) / float(m_d3dxvScale.z*0.125f));
			if (fHeight < fMinHeight) fMinHeight = fHeight;
			if (fHeight > fMaxHeight) fMaxHeight = fHeight;
		}
	}


	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxvNormals;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dNormalBuffer);

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR2) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxvTexCoords;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dTexCoordBuffer);

	d3dBufferData.pSysMem = pd3dxvDetailTexCoords;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dDetailTexCoordBuffer);

	if (pd3dxvNormals) delete[] pd3dxvNormals;
	if (pd3dxvTexCoords) delete[] pd3dxvTexCoords;
	if (pd3dxvDetailTexCoords) delete[] pd3dxvDetailTexCoords;

	//정점은 위치 벡터, 법선 벡터, 텍스쳐 좌표, 디테일 텍스쳐 좌표를 갖는다.
	ID3D11Buffer *pd3dBuffers[4] = { m_pd3dPositionBuffer, m_pd3dNormalBuffer, m_pd3dTexCoordBuffer, m_pd3dDetailTexCoordBuffer };
	UINT pnBufferStrides[4] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2), sizeof(D3DXVECTOR2) };
	UINT pnBufferOffsets[4] = { 0, 0, 0, 0 };
	AssembleToVertexBuffer(4, pd3dBuffers, pnBufferStrides, pnBufferOffsets);
	//
	/*격자는 사각형들의 집합이고 사각형은 두 개의 삼각형으로 구성되므로 격자는 <그림 18>과 같이 삼각형들의 집합이라고 할 수 있다. 격자를 표현하기 위하여 격자의 삼각형들을 인덱스로 표현해야 한다. 삼각형들은 사각형의 줄 단위로 아래에서 위쪽 방향으로(z-축) 나열한다. 첫 번째 사각형 줄의 삼각형들은 왼쪽에서 오른쪽으로(x-축) 나열한다. 두 번째 줄의 삼각형들은 오른쪽에서 왼쪽 방향으로 나열한다. 즉, 사각형의 줄이 바뀔 때마다 나열 순서가 바뀌도록 한다. <그림 18>의 격자에 대하여 삼각형 스트립을 사용하여 삼각형들을 표현하기 위한 인덱스의 나열은 다음과 같이 격자의 m번째 줄과 (m+1)번째 줄의 정점 번호를 사각형의 나열 방향에 따라 번갈아 아래, 위, 아래, 위, ... 순서로 나열하면 된다.
	0, 6, 1, 7, 2, 8, 3, 9, 4, 10, 5, 11, 11, 17, 10, 16, 9, 15, 8, 14, 7, 13, 6, 12
	이렇게 인덱스를 나열하면 삼각형 스트립을 사용할 것이므로 실제 그려지는 삼각형들의 인덱스는 다음과 같다.
	(0, 6, 1), (1, 6, 7), (1, 7, 2), (2, 7, 8), (2, 8, 3), (3, 8, 9), ...
	그러나 이러한 인덱스를 사용하면 첫 번째 줄을 제외하고 삼각형들이 제대로 그려지지 않는다. 왜냐하면 삼각형 스트립에서는 마지막 2개의 정점과 새로운 하나의 정점을 사용하여 새로운 삼각형을 그린다. 그리고 홀수 번째 삼각형의 정점 나열 순서(와인딩 순서)는 시계방향이고 짝수 번째 삼각형의 와인딩 순서는 반시계방향이어야 한다. 격자의 사각형이 한 줄에서 몇 개가 있던지 상관없이 한 줄의 마지막 삼각형은 짝수 번째 삼각형이고 와인딩 순서는 반시계 방향이다. 왜냐하면 사각형은 두 개의 삼각형으로 나누어지기 때문이다. 첫 번째 줄에서 두 번째 줄의 인덱스 나열과 실제 그려지는 삼각형들의 인덱스를 살펴보자.
	..., 4, 10, 5, 11, 11, 17, 10, 16, 9, 15, 8, 14, 7, 13, 6, 12, ...
	..., (4, 10, 5), (5, 10, 11), (5, 11, 11), (11, 11, 17), (11, 17, 10), ...
	삼각형 (5, 10, 11)은 첫 번째 줄의 마지막 삼각형이고 짝수 번째이다. 삼각형 (11, 17, 10)은 두 번째 줄의 첫 번째 삼각형이고 홀수 번째이다. 홀수 번째이므로 와인딩 순서가 시계방향이어야 하는데 실제 와인딩 순서는 반시계방향이므로 그려지지 않을 것이다. 당연히 다음 삼각형도 와인딩 순서가 맞지 않으므로 그려지지 않을 것이다. 삼각형 (11, 17, 10)의 와인딩 순서가 반시계방향이므로 그려지도록 하려면 이 삼각형이 짝수 번째 삼각형이 되도록 해야 한다. 이를 위해서 줄이 바뀔 때마다 마지막 정점의 인덱스를 추가하도록 하자. 그러면 줄이 바뀐 첫 번째 삼각형은 짝수 번째 삼각형이 된다. 다음의 예에서는 11이 추가된 마지막 정점의 인덱스이다. 이렇게 하면 삼각형을 구성할 수 없어서 그려지지 않는 삼각형이 각 줄마다 3개씩 생기게 된다.
	..., 4, 10, 5, 11, 11, 11, 17, 10, 16, 9, 15, 8, 14, 7, 13, 6, 12, ...
	..., (5, 10, 11), (5, 11, 11), (11, 11, 11), (11, 11, 17), (11, 17, 10), ...
	세 개의 삼각형 (5, 11, 11), (11, 11, 11), (11, 11, 17)은 삼각형을 구성할 수 없으므로 실제로 그려지지 않는다.
	이렇게 인덱스를 나열하면 인덱스 버퍼는 ((nWidth*2)*(nLength-1))+((nLength-1)-1)개의 인덱스를 갖는다. 사각형 줄의 개수는 (nLength-1)이고 한 줄에서 (nWidth*2)개의 인덱스를 갖는다. 그리고 줄이 바뀔 때마다 인덱스를 하나 추가하므로 (nLength-1)-1개의 인덱스가 추가로 필요하다.*/
	m_nIndices = ((nWidth * 2)*(nLength - 1)) + ((nLength - 1) - 1);
	m_pnIndices = new UINT[m_nIndices];
	for (int j = 0, z = 0; z < nLength - 1; z++)
	{
		if ((z % 2) == 0)
		{
			//홀수 번째 줄이므로(z = 0, 2, 4, ...) 인덱스의 나열 순서는 왼쪽에서 오른쪽 방향이다.
			for (int x = 0; x < nWidth; x++)
			{
				//첫 번째 줄을 제외하고 줄이 바뀔 때마다(x == 0) 첫 번째 인덱스를 추가한다.
				if ((x == 0) && (z > 0)) m_pnIndices[j++] = (UINT)(x + (z * nWidth));
				//아래, 위의 순서로 인덱스를 추가한다.
				m_pnIndices[j++] = (UINT)(x + (z * nWidth));
				m_pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
		else
		{
			//짝수 번째 줄이므로(z = 1, 3, 5, ...) 인덱스의 나열 순서는 오른쪽에서 왼쪽 방향이다.
			for (int x = nWidth - 1; x >= 0; x--)
			{
				//줄이 바뀔 때마다(x == (nWidth-1)) 첫 번째 인덱스를 추가한다.
				if (x == (nWidth - 1)) m_pnIndices[j++] = (UINT)(x + (z * nWidth));
				//아래, 위의 순서로 인덱스를 추가한다.
				m_pnIndices[j++] = (UINT)(x + (z * nWidth));
				m_pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
	}

	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(UINT) * m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pnIndices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(xStart*m_d3dxvScale.x, fMinHeight, zStart*m_d3dxvScale.z);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3((xStart + nWidth)*m_d3dxvScale.x, fMaxHeight, (zStart + nLength)*m_d3dxvScale.z);



	//D3D11_RASTERIZER_DESC d3dxRasterizer;
	//ZeroMemory(&d3dxRasterizer, sizeof(D3D11_RASTERIZER_DESC));
	//d3dxRasterizer.FillMode = D3D11_FILL_WIREFRAME;
	//d3dxRasterizer.CullMode = D3D11_CULL_BACK;
	//pd3dDevice->CreateRasterizerState(&d3dxRasterizer, &m_pd3dRasterizerState);

}

CHeightMapGridMesh::~CHeightMapGridMesh()
{
}

float CHeightMapGridMesh::OnGetHeight(int x, int z, void *pContext)
{
	//높이 맵 객체의 높이 맵 이미지의 픽셀 값을 지형의 높이로 반환한다. 
	CHeightMap *pHeightMap = (CHeightMap *)pContext;
	BYTE *pHeightMapImage = pHeightMap->GetHeightMapImage();


	D3DXVECTOR3 d3dxvScale = pHeightMap->GetScale();
	int cxTerrain = pHeightMap->GetHeightMapWidth();
	float fHeight = pHeightMapImage[x + (z*cxTerrain)] * d3dxvScale.y;
	return(fHeight);
}

CMeshIlluminated::CMeshIlluminated(ID3D11Device *pd3dDevice) : CMesh(pd3dDevice)
{
	m_pd3dNormalBuffer = NULL;
}

CMeshIlluminated::~CMeshIlluminated()
{
	if (m_pd3dNormalBuffer) m_pd3dNormalBuffer->Release();
}
void CMeshIlluminated::CalculateVertexNormal(D3DXVECTOR3 *pd3dxvNormals)
{
	switch (m_d3dPrimitiveTopology)
	{
		/*프리미티브가 삼각형 리스트일 때 인덱스 버퍼가 있는 경우와 없는 경우를 구분하여 정점의 법선 벡터를 계산한다. 인덱스 버퍼를 사용하지 않는 경우 각 정점의 법선 벡터는 그 정점이 포함된 삼각형의 법선 벡터로 계산한다. 인덱스 버퍼를 사용하는 경우 각 정점의 법선 벡터는 그 정점이 포함된 삼각형들의 법선 벡터의 평균으로(더하여) 계산한다.*/
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		if (m_pnIndices)
			SetAverageVertexNormal(pd3dxvNormals, (m_nIndices / 3), 3, false);
		else
			SetTriAngleListVertexNormal(pd3dxvNormals);
		break;
		/*프리미티브가 삼각형 스트립일 때 각 정점의 법선 벡터는 그 정점이 포함된 삼각형들의 법선 벡터의 평균으로(더하여) 계산한다.*/
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		SetAverageVertexNormal(pd3dxvNormals, (m_nIndices) ? (m_nIndices - 2) : (m_nVertices - 2), 1, true);
		break;
	default:
		break;
	}
}
void CMeshIlluminated::SetTriAngleListVertexNormal(D3DXVECTOR3 *pd3dxvNormals)
{
	/*삼각형(프리미티브)의 개수를 구하고 각 삼각형의 법선 벡터를 계산하고 삼각형을 구성하는 각 정점의 법선 벡터로 지정한다.*/
	int nPrimitives = m_nVertices / 3;
	for (int i = 0; i < nPrimitives; i++)
	{
		D3DXVECTOR3 d3dxvNormal = CalculateTriAngleNormal((i * 3 + 0), (i * 3 + 1), (i * 3 + 2));
		pd3dxvNormals[i * 3 + 0] = pd3dxvNormals[i * 3 + 1] = pd3dxvNormals[i * 3 + 2] = d3dxvNormal;
	}
}
D3DXVECTOR3 CMeshIlluminated::CalculateTriAngleNormal(UINT nIndex0, UINT nIndex1, UINT nIndex2)
{
	D3DXVECTOR3 d3dxvNormal;
	D3DXVECTOR3 d3dxvP0 = m_pd3dxvPositions[nIndex0];
	D3DXVECTOR3 d3dxvP1 = m_pd3dxvPositions[nIndex1];
	D3DXVECTOR3 d3dxvP2 = m_pd3dxvPositions[nIndex2];
	D3DXVECTOR3 d3dxvEdge1 = d3dxvP1 - d3dxvP0;
	D3DXVECTOR3 d3dxvEdge2 = d3dxvP2 - d3dxvP0;
	D3DXVec3Cross(&d3dxvNormal, &d3dxvEdge1, &d3dxvEdge2);
	D3DXVec3Normalize(&d3dxvNormal, &d3dxvNormal);
	return(d3dxvNormal);
}
void CMeshIlluminated::SetAverageVertexNormal(D3DXVECTOR3 *pd3dxvNormals, int nPrimitives, int nOffset, bool bStrip)
{
	for (int j = 0; j < m_nVertices; j++)
	{
		D3DXVECTOR3 d3dxvSumOfNormal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		for (int i = 0; i < nPrimitives; i++)
		{
			UINT nIndex0 = (bStrip) ? (((i % 2) == 0) ? (i*nOffset + 0) : (i*nOffset + 1)) : (i*nOffset + 0);
			if (m_pnIndices) nIndex0 = m_pnIndices[nIndex0];
			UINT nIndex1 = (bStrip) ? (((i % 2) == 0) ? (i*nOffset + 1) : (i*nOffset + 0)) : (i*nOffset + 1);
			if (m_pnIndices) nIndex1 = m_pnIndices[nIndex1];
			UINT nIndex2 = (m_pnIndices) ? m_pnIndices[i*nOffset + 2] : (i*nOffset + 2);
			if ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j)) d3dxvSumOfNormal += CalculateTriAngleNormal(nIndex0, nIndex1, nIndex2);
		}
		D3DXVec3Normalize(&d3dxvSumOfNormal, &d3dxvSumOfNormal);
		pd3dxvNormals[j] = d3dxvSumOfNormal;
	}
}
CCubeMeshIlluminated::CCubeMeshIlluminated(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth) : CMeshIlluminated(pd3dDevice)
{
	m_nVertices = 8;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];

	m_pd3dxvPositions[0] = D3DXVECTOR3(-fx, +fy, -fz);
	m_pd3dxvPositions[1] = D3DXVECTOR3(+fx, +fy, -fz);
	m_pd3dxvPositions[2] = D3DXVECTOR3(+fx, +fy, +fz);
	m_pd3dxvPositions[3] = D3DXVECTOR3(-fx, +fy, +fz);
	m_pd3dxvPositions[4] = D3DXVECTOR3(-fx, -fy, -fz);
	m_pd3dxvPositions[5] = D3DXVECTOR3(+fx, -fy, -fz);
	m_pd3dxvPositions[6] = D3DXVECTOR3(+fx, -fy, +fz);
	m_pd3dxvPositions[7] = D3DXVECTOR3(-fx, -fy, +fz);

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3)* m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	m_nIndices = 36;
	m_pnIndices = new UINT[m_nIndices];

	m_pnIndices[0] = 3; m_pnIndices[1] = 1; m_pnIndices[2] = 0;
	m_pnIndices[3] = 2; m_pnIndices[4] = 1; m_pnIndices[5] = 3;
	m_pnIndices[6] = 0; m_pnIndices[7] = 5; m_pnIndices[8] = 4;
	m_pnIndices[9] = 1; m_pnIndices[10] = 5; m_pnIndices[11] = 0;
	m_pnIndices[12] = 3; m_pnIndices[13] = 4; m_pnIndices[14] = 7;
	m_pnIndices[15] = 0; m_pnIndices[16] = 4; m_pnIndices[17] = 3;
	m_pnIndices[18] = 1; m_pnIndices[19] = 6; m_pnIndices[20] = 5;
	m_pnIndices[21] = 2; m_pnIndices[22] = 6; m_pnIndices[23] = 1;
	m_pnIndices[24] = 2; m_pnIndices[25] = 7; m_pnIndices[26] = 6;
	m_pnIndices[27] = 3; m_pnIndices[28] = 7; m_pnIndices[29] = 2;
	m_pnIndices[30] = 6; m_pnIndices[31] = 4; m_pnIndices[32] = 5;
	m_pnIndices[33] = 7; m_pnIndices[34] = 4; m_pnIndices[35] = 6;

	D3DXVECTOR3 pd3dxvNormals[8];
	CalculateVertexNormal(pd3dxvNormals);

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3)* m_nVertices;
	d3dBufferData.pSysMem = pd3dxvNormals;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dNormalBuffer);

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dNormalBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(UINT)* m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pnIndices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fx, -fy, -fz);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fx, +fy, +fz);
}

CCubeMeshIlluminated::~CCubeMeshIlluminated()
{
}
#define _WITH_INDEX_BUFFER

CSphereMeshIlluminated::CSphereMeshIlluminated(ID3D11Device *pd3dDevice, float fRadius, int nSlices, int nStacks) : CMeshIlluminated(pd3dDevice)
{
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fDeltaPhi = float(D3DX_PI / nStacks);
	float fDeltaTheta = float((2.0f * D3DX_PI) / nSlices);
	int k = 0;

#ifdef _WITH_INDEX_BUFFER
	m_nVertices = 2 + (nSlices * (nStacks - 1));
	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR3 *pd3dxvNormals = new D3DXVECTOR3[m_nVertices];

	m_pd3dxvPositions[k] = D3DXVECTOR3(0.0f, +fRadius, 0.0f);
	D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
	float theta_i, phi_j;
	for (int j = 1; j < nStacks; j++)
	{
		phi_j = fDeltaPhi * j;
		for (int i = 0; i < nSlices; i++)
		{
			theta_i = fDeltaTheta * i;
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*sinf(phi_j)*cosf(theta_i), fRadius*cosf(phi_j), fRadius*sinf(phi_j)*sinf(theta_i));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
		}
	}
	m_pd3dxvPositions[k] = D3DXVECTOR3(0.0f, -fRadius, 0.0f);
	D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3)* m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	d3dBufferData.pSysMem = pd3dxvNormals;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dNormalBuffer);

	if (pd3dxvNormals) delete[] pd3dxvNormals;

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dNormalBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	k = 0;
	m_nIndices = (nSlices * 3) * 2 + (nSlices * (nStacks - 2) * 3 * 2);
	m_pnIndices = new UINT[m_nIndices];
	for (int i = 0; i < nSlices; i++)
	{
		m_pnIndices[k++] = 0;
		m_pnIndices[k++] = 1 + ((i + 1) % nSlices);
		m_pnIndices[k++] = 1 + i;
	}
	for (int j = 0; j < nStacks - 2; j++)
	{
		for (int i = 0; i < nSlices; i++)
		{
			m_pnIndices[k++] = 1 + (i + (j * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			m_pnIndices[k++] = 1 + (i + ((j + 1) * nSlices));
			m_pnIndices[k++] = 1 + (i + ((j + 1) * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + ((j + 1) * nSlices));
		}
	}
	for (int i = 0; i < nSlices; i++)
	{
		m_pnIndices[k++] = (m_nVertices - 1);
		m_pnIndices[k++] = ((m_nVertices - 1) - nSlices) + i;
		m_pnIndices[k++] = ((m_nVertices - 1) - nSlices) + ((i + 1) % nSlices);
	}

	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(UINT)* m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pnIndices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);
#else
	m_nVertices = (nSlices * 3) * 2 + (nSlices * (nStacks - 2) * 3 * 2);
	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR3 *pd3dxvNormals = new D3DXVECTOR3[m_nVertices];

	float theta_i, theta_ii, phi_j = 0.0f, phi_jj = fDeltaPhi;
	for (int i = 0; i < nSlices; i++)
	{
		theta_i = fDeltaTheta * i;
		theta_ii = fDeltaTheta * (i + 1);
		m_pd3dxvPositions[k] = D3DXVECTOR3(0.0f, +fRadius, 0.0f);
		D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
		m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*cosf(theta_i)*sinf(phi_jj), fRadius*cosf(phi_jj), fRadius*sinf(theta_i)*sinf(phi_jj));
		D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
		m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*cosf(theta_ii)*sinf(phi_jj), fRadius*cosf(phi_jj), fRadius*sinf(theta_ii)*sinf(phi_jj));
		D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
	}

	for (int j = 1; j < nStacks - 1; j++)
	{
		phi_j = fDeltaPhi * j;
		phi_jj = fDeltaPhi * (j + 1);
		for (int i = 0; i < nSlices; i++)
		{
			theta_i = fDeltaTheta * i;
			theta_ii = fDeltaTheta * (i + 1);
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*cosf(theta_i)*sinf(phi_j), fRadius*cosf(phi_j), fRadius*sinf(theta_i)*sinf(phi_j));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*cosf(theta_i)*sinf(phi_jj), fRadius*cosf(phi_jj), fRadius*sinf(theta_i)*sinf(phi_jj));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*cosf(theta_ii)*sinf(phi_j), fRadius*cosf(phi_j), fRadius*sinf(theta_ii)*sinf(phi_j));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*cosf(theta_i)*sinf(phi_jj), fRadius*cosf(phi_jj), fRadius*sinf(theta_i)*sinf(phi_jj));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*cosf(theta_ii)*sinf(phi_jj), fRadius*cosf(phi_jj), fRadius*sinf(theta_ii)*sinf(phi_jj));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*cosf(theta_ii)*sinf(phi_j), fRadius*cosf(phi_j), fRadius*sinf(theta_ii)*sinf(phi_j));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
		}
	}
	phi_j = fDeltaPhi * (nStacks - 1);
	for (int i = 0; i < nSlices; i++)
	{
		theta_i = fDeltaTheta * i;
		theta_ii = fDeltaTheta * (i + 1);
		m_pd3dxvPositions[k] = D3DXVECTOR3(0.0f, -fRadius, 0.0f);
		D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
		m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*cosf(theta_ii)*sinf(phi_j), fRadius*cosf(phi_j), fRadius*sinf(theta_ii)*sinf(phi_j));
		D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
		m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*cosf(theta_i)*sinf(phi_j), fRadius*cosf(phi_j), fRadius*sinf(theta_i)*sinf(phi_j));
		D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
	}

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3)* m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	d3dBufferData.pSysMem = pd3dxvNormals;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dNormalBuffer);

	if (pd3dxvNormals) delete[] pd3dxvNormals;

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dNormalBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);
#endif

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fRadius, -fRadius, -fRadius);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fRadius, +fRadius, +fRadius);
}

CSphereMeshIlluminated::~CSphereMeshIlluminated()
{
}



CMeshTextured::CMeshTextured(ID3D11Device *pd3dDevice) : CMesh(pd3dDevice)
{
	m_pd3dTexCoordBuffer = NULL;
}

CMeshTextured::~CMeshTextured()
{
	if (m_pd3dTexCoordBuffer) m_pd3dTexCoordBuffer->Release();
}

CCubeMeshTextured::CCubeMeshTextured(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth) : CMeshTextured(pd3dDevice)
{
	m_nVertices = 36;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR2 pd3dxvTexCoords[36];
	int i = 0;

	//직육면체의 각 면(삼각형 2개)에 하나의 텍스쳐 이미지 전체가 맵핑되도록 텍스쳐 좌표를 설정한다.
	//
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);


	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	d3dBufferData.pSysMem = pd3dxvTexCoords;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dTexCoordBuffer);

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dTexCoordBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	/*직육면체의 각 면에 텍스쳐를 맵핑하려면 인덱스를 사용할 수 없으므로
	인덱스 버퍼는 생성하지 않는다.*/

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fx, -fy, -fz);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fx, +fy, +fz);
}

CCubeMeshTextured::~CCubeMeshTextured()
{
}

CSphereMeshTextured::CSphereMeshTextured(ID3D11Device *pd3dDevice, float fRadius, int nSlices, int nStacks) : CMeshTextured(pd3dDevice)
{
	m_nVertices = (nSlices * nStacks) * 3 * 2;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR2 *pd3dxTexCoords = new D3DXVECTOR2[m_nVertices];

	float theta_i, theta_ii, phi_j, phi_jj, fRadius_j, fRadius_jj, y_j, y_jj;
	for (int j = 0, k = 0; j < nStacks; j++)
	{
		phi_j = float(D3DX_PI / nStacks) * j;
		phi_jj = float(D3DX_PI / nStacks) * (j + 1);
		fRadius_j = fRadius * sinf(phi_j);
		fRadius_jj = fRadius * sinf(phi_jj);
		y_j = fRadius * cosf(phi_j);
		y_jj = fRadius * cosf(phi_jj);
		for (int i = 0; i < nSlices; i++)
		{
			theta_i = float(2 * D3DX_PI / nSlices) * i;
			theta_ii = float(2 * D3DX_PI / nSlices) * (i + 1);
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius_j*cosf(theta_i), y_j, fRadius_j*sinf(theta_i));
			pd3dxTexCoords[k++] = D3DXVECTOR2(float(i) / float(nSlices), float(j) / float(nStacks));
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius_jj*cosf(theta_i), y_jj, fRadius_jj*sinf(theta_i));
			pd3dxTexCoords[k++] = D3DXVECTOR2(float(i) / float(nSlices), float(j + 1) / float(nStacks));
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius_j*cosf(theta_ii), y_j, fRadius_j*sinf(theta_ii));
			pd3dxTexCoords[k++] = D3DXVECTOR2(float(i + 1) / float(nSlices), float(j) / float(nStacks));
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius_jj*cosf(theta_i), y_jj, fRadius_jj*sinf(theta_i));
			pd3dxTexCoords[k++] = D3DXVECTOR2(float(i) / float(nSlices), float(j + 1) / float(nStacks));
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius_jj*cosf(theta_ii), y_jj, fRadius_jj*sinf(theta_ii));
			pd3dxTexCoords[k++] = D3DXVECTOR2(float(i + 1) / float(nSlices), float(j + 1) / float(nStacks));
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius_j*cosf(theta_ii), y_j, fRadius_j*sinf(theta_ii));
			pd3dxTexCoords[k++] = D3DXVECTOR2(float(i + 1) / float(nSlices), float(j) / float(nStacks));
		}
	}

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR2) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxTexCoords;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dTexCoordBuffer);

	delete[] pd3dxTexCoords;

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dTexCoordBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fRadius, -fRadius, -fRadius);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fRadius, +fRadius, +fRadius);
}

CSphereMeshTextured::~CSphereMeshTextured()
{
}


CMeshDetailTextured::CMeshDetailTextured(ID3D11Device *pd3dDevice) : CMeshTextured(pd3dDevice)
{
	m_pd3dDetailTexCoordBuffer = NULL;
}

CMeshDetailTextured::~CMeshDetailTextured()
{
	if (m_pd3dDetailTexCoordBuffer) m_pd3dDetailTexCoordBuffer->Release();
}

CSkyBoxMesh::CSkyBoxMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth) : CMeshTextured(pd3dDevice)
{
	//스카이 박스는 6개의 면(사각형), 사각형은 정점 4개, 그러므로 24개의 정점이 필요하다.
	m_nVertices = 24;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR2 *pd3dxvTexCoords = new D3DXVECTOR2[m_nVertices];

	int i = 0;
	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;
	// Front Quad 
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fx, +fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fx, +fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fx, +fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fx, +fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);
	// Back Quad
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fx, -fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fx, -fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fx, -fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fx, -fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);
	// Left Quad
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fx, -fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fx, +fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fx, +fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fx, -fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);
	// Right Quad
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fx, +fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fx, -fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fx, -fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fx, +fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);
	// Top Quad
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fx, -fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fx, -fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fx, +fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fx, +fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);
	// Bottom Quad
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fx, +fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fx, +fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fx, -fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fx, -fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	D3D11_BUFFER_DESC d3dBufferDesc;
	::ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	::ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR2) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxvTexCoords;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dTexCoordBuffer);

	delete[] pd3dxvTexCoords;

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dTexCoordBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	//삼각형 스트립으로 사각형 1개를 그리기 위해 인덱스는 4개가 필요하다.
	m_nIndices = 4;
	m_pnIndices = new UINT[m_nIndices];

	m_pnIndices[0] = 0;
	m_pnIndices[1] = 1;
	m_pnIndices[2] = 3;
	m_pnIndices[3] = 2;

	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(UINT) * m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pnIndices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);

	D3D11_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	//스카이 박스 사각형들은 깊이 버퍼 알고리즘을 적용하지 않고 깊이 버퍼를 변경하지 않는다.
	d3dDepthStencilDesc.DepthEnable = false;
	d3dDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	d3dDepthStencilDesc.DepthFunc = D3D11_COMPARISON_NEVER;
	d3dDepthStencilDesc.StencilEnable = false;
	d3dDepthStencilDesc.StencilReadMask = 0xFF;
	d3dDepthStencilDesc.StencilWriteMask = 0xFF;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	pd3dDevice->CreateDepthStencilState(&d3dDepthStencilDesc, &m_pd3dDepthStencilState);

	ID3D11SamplerState *pd3dSamplerState = NULL;
	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dSamplerState);

	m_pSkyboxTexture = new CTexture(6, 1, 0, 0);
	m_pSkyboxTexture->SetSampler(0, pd3dSamplerState);
	pd3dSamplerState->Release();
	m_pSkyboxTexture->AddRef();
	//왠지 중요할거 같은부분이다.
	OnChangeSkyBoxTextures(pd3dDevice, 4);


	//m_pSkyboxTexture = new CTexture(1, 1, 0, 0);
	//m_pSkyboxTexture->SetSampler(0, pd3dSamplerState);
	//pd3dSamplerState->Release();
	//m_pSkyboxTexture->AddRef();


	//D3DX11CreateShaderResourceViewFromFile(pd3dDevice, L"skyBox.dds", NULL, NULL, &m_pd3dsrvCubeMap, NULL);
	////pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &m_pd3dSamplerState);

	//m_pSkyboxTexture->SetTexture(0, m_pd3dsrvCubeMap);
	//m_pd3dsrvCubeMap->Release();


}

CSkyBoxMesh::~CSkyBoxMesh()
{
	if (m_pd3dDepthStencilState) m_pd3dDepthStencilState->Release();
	if (m_pSkyboxTexture) m_pSkyboxTexture->Release();
}

void CSkyBoxMesh::OnChangeSkyBoxTextures(ID3D11Device *pd3dDevice, int nIndex)
{
	//6개의 스카이 박스 텍스쳐를 생성하여 CTexture 객체에 연결한다. //
	_TCHAR pstrTextureName[80];
	ID3D11ShaderResourceView *pd3dsrvTexture = NULL;
	_stprintf_s(pstrTextureName, _T("Image/front.jpg"), nIndex, 80);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, pstrTextureName, NULL, NULL, &pd3dsrvTexture, NULL);
	m_pSkyboxTexture->SetTexture(0, pd3dsrvTexture);
	pd3dsrvTexture->Release();

	_stprintf_s(pstrTextureName, _T("Image/back.jpg"), nIndex, 80);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, pstrTextureName, NULL, NULL, &pd3dsrvTexture, NULL);
	m_pSkyboxTexture->SetTexture(1, pd3dsrvTexture);
	pd3dsrvTexture->Release();

	_stprintf_s(pstrTextureName, _T("Image/left.jpg"), nIndex, 80);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, pstrTextureName, NULL, NULL, &pd3dsrvTexture, NULL);
	m_pSkyboxTexture->SetTexture(2, pd3dsrvTexture);
	pd3dsrvTexture->Release();

	_stprintf_s(pstrTextureName, _T("Image/right.jpg"), nIndex, 80);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, pstrTextureName, NULL, NULL, &pd3dsrvTexture, NULL);
	m_pSkyboxTexture->SetTexture(3, pd3dsrvTexture);
	pd3dsrvTexture->Release();

	_stprintf_s(pstrTextureName, _T("Image/top.jpg"), nIndex, 80);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, pstrTextureName, NULL, NULL, &pd3dsrvTexture, NULL);
	m_pSkyboxTexture->SetTexture(4, pd3dsrvTexture);
	pd3dsrvTexture->Release();

	_stprintf_s(pstrTextureName, _T("Image/bottom.jpg"), nIndex, 80);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, pstrTextureName, NULL, NULL, &pd3dsrvTexture, NULL);
	m_pSkyboxTexture->SetTexture(5, pd3dsrvTexture);
	pd3dsrvTexture->Release();
}

void CSkyBoxMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	pd3dDeviceContext->IASetVertexBuffers(m_nSlot, m_nBuffers, m_ppd3dVertexBuffers, m_pnVertexStrides, m_pnVertexOffsets);
	pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, m_dxgiIndexFormat, m_nIndexOffset);
	pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	//스카이 박스를 그리기 위한 샘플러 상태 객체와 깊이 스텐실 상태 객체를 설정한다.
	m_pSkyboxTexture->UpdateSamplerShaderVariable(pd3dDeviceContext, 0, 0);
	pd3dDeviceContext->OMSetDepthStencilState(m_pd3dDepthStencilState, 1);

	//스카이 박스의 6개 면(사각형)을 순서대로 그린다.
	for (int i = 0; i < 6; i++)
	{
		//스카이 박스의 각 면(사각형)을 그릴 때 사용할 텍스쳐를 설정한다.
		m_pSkyboxTexture->UpdateTextureShaderVariable(pd3dDeviceContext, i, 0);
		pd3dDeviceContext->DrawIndexed(4, 0, i * 4);
	}




	//pd3dDeviceContext->OMSetDepthStencilState(m_pd3dDepthStencilState, 1);
	//m_pSkyboxTexture->UpdateShaderVariable(pd3dDeviceContext);
	////pd3dDeviceContext->PSSetSamplers(3, 1, &m_pd3dSamplerState);
	////pd3dDeviceContext->PSSetShaderResources(2, 1, &m_pd3dsrvCubeMap);
	//for (int i = 0; i < 6; i++)
	//	pd3dDeviceContext->DrawIndexed(4, 0, i * 4);


	pd3dDeviceContext->OMSetDepthStencilState(NULL, 1);
}


CMeshTexturedIlluminated::CMeshTexturedIlluminated(ID3D11Device *pd3dDevice) : CMeshIlluminated(pd3dDevice)
{
	m_pd3dTexCoordBuffer = NULL;
}

CMeshTexturedIlluminated::~CMeshTexturedIlluminated()
{
	if (m_pd3dTexCoordBuffer) m_pd3dTexCoordBuffer->Release();
}

CMeshDetailTexturedIlluminated::CMeshDetailTexturedIlluminated(ID3D11Device *pd3dDevice) : CMeshIlluminated(pd3dDevice)
{
	m_pd3dTexCoordBuffer = NULL;
	m_pd3dDetailTexCoordBuffer = NULL;
}

CMeshDetailTexturedIlluminated::~CMeshDetailTexturedIlluminated()
{
	if (m_pd3dTexCoordBuffer) m_pd3dTexCoordBuffer->Release();
	if (m_pd3dDetailTexCoordBuffer) m_pd3dDetailTexCoordBuffer->Release();
}

CCubeMeshTexturedIlluminated::CCubeMeshTexturedIlluminated(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth) : CMeshTexturedIlluminated(pd3dDevice)
{
	m_nVertices = 36;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	//생략된 부분은 LabProject13-2의 CCubeMeshTextured 클래스의 생성자 부분과 동일하다.
	D3DXVECTOR2 pd3dxvTexCoords[36];
	int i = 0;

	//직육면체의 각 면(삼각형 2개)에 하나의 텍스쳐 이미지 전체가 맵핑되도록 텍스쳐 좌표를 설정한다.
	//
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);


	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, +fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);


	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	//법선 벡터를 생성하기 위한 다음 코드를 추가한다.
	D3DXVECTOR3 pd3dxvNormals[36];
	CalculateVertexNormal(pd3dxvNormals);

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxvNormals;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dNormalBuffer);

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR2) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxvTexCoords;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dTexCoordBuffer);

	//정점은 위치 벡터, 법선 벡터, 텍스쳐 좌표를 갖는다.
	ID3D11Buffer *pd3dBuffers[3] = { m_pd3dPositionBuffer, m_pd3dNormalBuffer, m_pd3dTexCoordBuffer };
	UINT pnBufferStrides[3] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2) };
	UINT pnBufferOffsets[3] = { 0, 0, 0 };
	AssembleToVertexBuffer(3, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fx, -fy, -fz);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fx, +fy, +fz);
}

CCubeMeshTexturedIlluminated::~CCubeMeshTexturedIlluminated()
{
}


//LabProject13-2의 CSphereMeshTextured 클래스의 생성자에 법선 벡터를 추가한다.
CSphereMeshTexturedIlluminated::CSphereMeshTexturedIlluminated(ID3D11Device *pd3dDevice, float fRadius, int nSlices, int nStacks) : CMeshTexturedIlluminated(pd3dDevice)
{
	m_nVertices = (nSlices * nStacks) * 3 * 2;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR3 *pd3dxvNormals = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR2 *pd3dxTexCoords = new D3DXVECTOR2[m_nVertices];

	float theta_i, theta_ii, phi_j, phi_jj, fRadius_j, fRadius_jj, y_j, y_jj;
	for (int j = 0, k = 0; j < nStacks; j++)
	{
		phi_j = float(D3DX_PI / nStacks) * j;
		phi_jj = float(D3DX_PI / nStacks) * (j + 1);
		fRadius_j = fRadius * sinf(phi_j);
		fRadius_jj = fRadius * sinf(phi_jj);
		y_j = fRadius*cosf(phi_j);
		y_jj = fRadius*cosf(phi_jj);
		for (int i = 0; i < nSlices; i++)
		{
			theta_i = float(2 * D3DX_PI / nSlices) * i;
			theta_ii = float(2 * D3DX_PI / nSlices) * (i + 1);
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius_j*cosf(theta_i), y_j, fRadius_j*sinf(theta_i));
			pd3dxTexCoords[k] = D3DXVECTOR2(float(i) / float(nSlices), float(j) / float(nStacks));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius_jj*cosf(theta_i), y_jj, fRadius_jj*sinf(theta_i));
			pd3dxTexCoords[k] = D3DXVECTOR2(float(i) / float(nSlices), float(j + 1) / float(nStacks));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius_j*cosf(theta_ii), y_j, fRadius_j*sinf(theta_ii));
			pd3dxTexCoords[k] = D3DXVECTOR2(float(i + 1) / float(nSlices), float(j) / float(nStacks));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius_jj*cosf(theta_i), y_jj, fRadius_jj*sinf(theta_i));
			pd3dxTexCoords[k] = D3DXVECTOR2(float(i) / float(nSlices), float(j + 1) / float(nStacks));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius_jj*cosf(theta_ii), y_jj, fRadius_jj*sinf(theta_ii));
			pd3dxTexCoords[k] = D3DXVECTOR2(float(i + 1) / float(nSlices), float(j + 1) / float(nStacks));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius_j*cosf(theta_ii), y_j, fRadius_j*sinf(theta_ii));
			pd3dxTexCoords[k] = D3DXVECTOR2(float(i + 1) / float(nSlices), float(j) / float(nStacks));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
		}
	}

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxvNormals;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dNormalBuffer);

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR2) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxTexCoords;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dTexCoordBuffer);

	delete[] pd3dxvNormals;
	delete[] pd3dxTexCoords;

	//정점은 위치 벡터, 법선 벡터, 텍스쳐 좌표를 갖는다.
	ID3D11Buffer *pd3dBuffers[3] = { m_pd3dPositionBuffer, m_pd3dNormalBuffer, m_pd3dTexCoordBuffer };
	UINT pnBufferStrides[3] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2) };
	UINT pnBufferOffsets[3] = { 0, 0, 0 };
	AssembleToVertexBuffer(3, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fRadius, -fRadius, -fRadius);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fRadius, +fRadius, +fRadius);
}

CSphereMeshTexturedIlluminated::~CSphereMeshTexturedIlluminated()
{
}
//=============================================================

CPlaneMesh::CPlaneMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth) : CMeshTexturedIlluminated(pd3dDevice)
{
	m_nVertices = 8;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	//생략된 부분은 LabProject13-2의 CCubeMeshTextured 클래스의 생성자 부분과 동일하다.
	D3DXVECTOR2 pd3dxvTexCoords[8];
	int i = 0;

	//직육면체의 각 면(삼각형 2개)에 하나의 텍스쳐 이미지 전체가 맵핑되도록 텍스쳐 좌표를 설정한다.
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);
	//법선 벡터를 생성하기 위한 다음 코드를 추가한다.
	D3DXVECTOR3 pd3dxvNormals[8];
	CalculateVertexNormal(pd3dxvNormals);


	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxvNormals;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dNormalBuffer);

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR2) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxvTexCoords;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dTexCoordBuffer);

	//정점은 위치 벡터, 법선 벡터, 텍스쳐 좌표를 갖는다.
	ID3D11Buffer *pd3dBuffers[3] = { m_pd3dPositionBuffer, m_pd3dNormalBuffer, m_pd3dTexCoordBuffer };
	UINT pnBufferStrides[3] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2) };
	UINT pnBufferOffsets[3] = { 0, 0, 0 };
	AssembleToVertexBuffer(3, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fx, -fy, -fz);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fx, +fy, +fz);

	//D3D11_RASTERIZER_DESC d3dxRasterizer;
	//ZeroMemory(&d3dxRasterizer, sizeof(D3D11_RASTERIZER_DESC));
	//d3dxRasterizer.FillMode = D3D11_FILL_WIREFRAME;
	//d3dxRasterizer.CullMode = D3D11_CULL_BACK;
	//pd3dDevice->CreateRasterizerState(&d3dxRasterizer, &m_pd3dRasterizerState);


}

CPlaneMesh::~CPlaneMesh()
{
}


void CPlaneMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	//메쉬의 정점은 여러 개의 정점 버퍼로 표현된다.
	pd3dDeviceContext->IASetVertexBuffers(m_nSlot, m_nBuffers, m_ppd3dVertexBuffers, m_pnVertexStrides, m_pnVertexOffsets);
	pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, m_dxgiIndexFormat, m_nIndexOffset);
	pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);


	pd3dDeviceContext->Draw(m_nVertices, m_nStartVertex);
	pd3dDeviceContext->RSSetState(NULL);
}


//=============================================================

CNormalMappingPlaneMesh::CNormalMappingPlaneMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth) : CMeshTexturedIlluminated(pd3dDevice)
{
	m_nVertices = 8;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	//생략된 부분은 LabProject13-2의 CCubeMeshTextured 클래스의 생성자 부분과 동일하다.
	D3DXVECTOR2 pd3dxvTexCoords[8];
	int i = 0;

	//직육면체의 각 면(삼각형 2개)에 하나의 텍스쳐 이미지 전체가 맵핑되도록 텍스쳐 좌표를 설정한다.
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);
	//법선 벡터를 생성하기 위한 다음 코드를 추가한다.
	D3DXVECTOR3 pd3dxvNormals[8];
	CalculateVertexNormal(pd3dxvNormals);


	D3DXVECTOR3 pd3dxvTangents[8];
	for (int j = 0; j < 8; j++)
		pd3dxvTangents[j] = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxvNormals;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dNormalBuffer);

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR2) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxvTexCoords;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dTexCoordBuffer);

	//탄젠트
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR2) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxvTangents;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_ppd3dTangentBuffer);

	//정점은 위치 벡터, 법선 벡터, 텍스쳐 좌표를 갖는다.
	ID3D11Buffer *pd3dBuffers[4] = { m_pd3dPositionBuffer, m_pd3dNormalBuffer, m_ppd3dTangentBuffer, m_pd3dTexCoordBuffer };
	UINT pnBufferStrides[4] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2) };
	UINT pnBufferOffsets[4] = { 0, 0, 0, 0 };
	AssembleToVertexBuffer(4, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fx, -fy, -fz);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fx, +fy, +fz);
}

CNormalMappingPlaneMesh::~CNormalMappingPlaneMesh()
{
}
//==============================================================
//===================테셀레이션================================
//=============================================================

CTessellationMesh::CTessellationMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth) : CMeshTexturedIlluminated(pd3dDevice)
{
	m_nVertices = 4;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR2 pd3dxvTexCoords[4];
	int i = 0;




	m_pd3dxvPositions[0] = D3DXVECTOR3(-fx, +fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);

	m_pd3dxvPositions[1] = D3DXVECTOR3(+fx, +fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	m_pd3dxvPositions[2] = D3DXVECTOR3(-fx, -fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	m_pd3dxvPositions[3] = D3DXVECTOR3(+fx, -fy, -fz);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);



	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);
	//법선 벡터를 생성하기 위한 다음 코드를 추가한다.
	D3DXVECTOR3 pd3dxvNormals[4];
	CalculateVertexNormal(pd3dxvNormals);







	//인덱스 버퍼
	m_nIndices = 4;
	m_pnIndices = new UINT[m_nIndices];

	m_pnIndices[0] = 0;
	m_pnIndices[1] = 1;
	m_pnIndices[2] = 2;
	m_pnIndices[3] = 3;

	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(UINT) * m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pnIndices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);



	//D3DXVECTOR3 pd3dxvTangents[8];
	//for (int j = 0; j < 8; j++)
	//	pd3dxvTangents[j] = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxvNormals;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dNormalBuffer);

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR2) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxvTexCoords;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dTexCoordBuffer);

	//탄젠트
	/*	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR2) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxvTangents;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_ppd3dTangentBuffer)*/;

	//정점은 위치 벡터, 법선 벡터, 텍스쳐 좌표를 갖는다.
	ID3D11Buffer *pd3dBuffers[3] = { m_pd3dPositionBuffer, m_pd3dNormalBuffer, m_pd3dTexCoordBuffer };
	UINT pnBufferStrides[3] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2) };
	UINT pnBufferOffsets[3] = { 0, 0, 0 };
	AssembleToVertexBuffer(3, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fx, -fy, -fz);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fx, +fy, +fz);

	D3D11_RASTERIZER_DESC d3dxRasterizer;
	ZeroMemory(&d3dxRasterizer, sizeof(D3D11_RASTERIZER_DESC));
	d3dxRasterizer.FillMode = D3D11_FILL_WIREFRAME;
	d3dxRasterizer.CullMode = D3D11_CULL_BACK;
	pd3dDevice->CreateRasterizerState(&d3dxRasterizer, &m_pd3dRasterizerState);

}

CTessellationMesh::~CTessellationMesh()
{
}

void CTessellationMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	//메쉬의 정점은 여러 개의 정점 버퍼로 표현된다.
	pd3dDeviceContext->IASetVertexBuffers(m_nSlot, m_nBuffers, m_ppd3dVertexBuffers, m_pnVertexStrides, m_pnVertexOffsets);
	pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, m_dxgiIndexFormat, m_nIndexOffset);
	pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);

	if (m_pd3dIndexBuffer)
		pd3dDeviceContext->DrawIndexed(m_nIndices, m_nStartIndex, m_nBaseVertex);
	else
		pd3dDeviceContext->Draw(m_nVertices, m_nStartVertex);
	pd3dDeviceContext->RSSetState(NULL);
}




CDisplacementMappingMesh::CDisplacementMappingMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth) : CMeshTexturedIlluminated(pd3dDevice)
{
	m_nVertices = 4;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR2 pd3dxvTexCoords[4];

	m_pd3dxvPositions[0] = D3DXVECTOR3(-fx, -fy, -fz);
	pd3dxvTexCoords[0] = D3DXVECTOR2(0.0f, 1.0f);

	m_pd3dxvPositions[1] = D3DXVECTOR3(-fx, +fy, -fz);
	pd3dxvTexCoords[1] = D3DXVECTOR2(0.0f, 0.0f);

	m_pd3dxvPositions[2] = D3DXVECTOR3(+fx, +fy, -fz);
	pd3dxvTexCoords[2] = D3DXVECTOR2(1.0f, 0.0f);

	m_pd3dxvPositions[3] = D3DXVECTOR3(+fx, -fy, -fz);
	pd3dxvTexCoords[3] = D3DXVECTOR2(1.0f, 1.0f);

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	//d3dBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;

	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	d3dBufferDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	//D3DXVECTOR3 pd3dxvNormals[4];
	//CalculateVertexNormal(pd3dxvNormals);

	D3DXVECTOR3 pd3dxvNormals[4];
	pd3dxvNormals[0] = D3DXVECTOR3(0.0, 0.0f, -1.0f);
	pd3dxvNormals[1] = D3DXVECTOR3(0.0, 0.0f, -1.0f);
	pd3dxvNormals[2] = D3DXVECTOR3(0.0, 0.0f, -1.0f);
	pd3dxvNormals[3] = D3DXVECTOR3(0.0, 0.0f, -1.0f);


	D3DXVECTOR3 pd3dxvTangents[4];
	for (int j = 0; j < 4; j++)
		pd3dxvTangents[j] = D3DXVECTOR3(1.0f, 0.0f, 0.0f);


	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxvNormals;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dNormalBuffer);

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR2) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxvTexCoords;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dTexCoordBuffer);

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxvTangents;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_ppd3dTangentBuffer);



	ID3D11Buffer *pd3dBuffers[4] = { m_pd3dPositionBuffer, m_pd3dNormalBuffer, m_ppd3dTangentBuffer, m_pd3dTexCoordBuffer };
	UINT pnBufferStrides[4] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2) };
	UINT pnBufferOffsets[4] = { 0, 0, 0, 0 };
	AssembleToVertexBuffer(4, pd3dBuffers, pnBufferStrides, pnBufferOffsets);


	m_nIndices = 6;
	m_pnIndices = new UINT[m_nIndices];
	m_pnIndices[0] = 0; m_pnIndices[1] = 1; m_pnIndices[2] = 2;
	m_pnIndices[3] = 0; m_pnIndices[4] = 2; m_pnIndices[5] = 3;

	//d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	//d3dBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;


	d3dBufferDesc.ByteWidth = sizeof(UINT)* m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//d3dBufferDesc.CPUAccessFlags = 0;
	//d3dBufferDesc.MiscFlags = 0;
	//ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pnIndices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);


	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fx, -fy, -fz);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fx, +fy, +fz);

	/*D3D11_RASTERIZER_DESC d3dxRasterizer;
	ZeroMemory(&d3dxRasterizer, sizeof(D3D11_RASTERIZER_DESC));
	d3dxRasterizer.FillMode = D3D11_FILL_WIREFRAME;
	d3dxRasterizer.CullMode = D3D11_CULL_BACK;
	pd3dDevice->CreateRasterizerState(&d3dxRasterizer, &m_pd3dRasterizerState);*/
}

CDisplacementMappingMesh::~CDisplacementMappingMesh()
{
}







CTessellationHeightMapGridMesh::CTessellationHeightMapGridMesh(ID3D11Device *pd3dDevice, int nWidth, int nLength, D3DXVECTOR3 d3dxvScale) : CMeshDetailTexturedIlluminated(pd3dDevice)
{
	m_nWidth = nWidth;
	m_nLength = nLength;
	m_d3dxvScale = d3dxvScale;

	int nCellsPerPatch = 64;
	int nNumPatchVertWidth = ((m_nWidth - 1) / nCellsPerPatch) + 1;
	int nNumPatchVertLength = ((m_nWidth - 1) / nCellsPerPatch) + 1;

	int nNumPatchVertices = nNumPatchVertWidth  * nNumPatchVertLength;
	int nNumPatchQuadFaces = (nNumPatchVertWidth - 1) * (nNumPatchVertLength - 1);

	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;

	m_pd3dxvPositions = new D3DXVECTOR3[nNumPatchVertices];
	D3DXVECTOR2 *pd3dxvTexCoords = new D3DXVECTOR2[nNumPatchVertices];

	float fUOffset = 1.0f / (nNumPatchVertWidth - 1);
	float fVOffset = 1.0f / (nNumPatchVertLength - 1);

	int nPatchWidth = 64;
	int nPatchLength = 64;


	int q = 0;


	for (int i = nNumPatchVertLength - 1, k = 0; i >= 0; i--)
	{
		float z = nPatchLength * i; //감소
		for (int j = 0; j < nNumPatchVertWidth; j++, k++)
		{
			float x = nPatchWidth * j;
			m_pd3dxvPositions[k] = D3DXVECTOR3(x *m_d3dxvScale.x, 0.0f * m_d3dxvScale.y,
				z * m_d3dxvScale.z);
			pd3dxvTexCoords[k] = D3DXVECTOR2(j *  fUOffset, q * fVOffset);
		}
		q++;
	}

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * nNumPatchVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);



	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR2) * nNumPatchVertices;
	d3dBufferData.pSysMem = pd3dxvTexCoords;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dTexCoordBuffer);


	if (pd3dxvTexCoords) delete[] pd3dxvTexCoords;


	//정점은 위치 벡터, 법선 벡터, 텍스쳐 좌표, 디테일 텍스쳐 좌표를 갖는다.
	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dTexCoordBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	//인덱스 버퍼
	m_nIndices = nNumPatchQuadFaces * 4;
	m_pnIndices = new UINT[m_nIndices];

	int k = 0;
	for (int i = 0; i < nNumPatchVertLength - 1; ++i)
	{
		for (int j = 0; j <nNumPatchVertWidth - 1; ++j)
		{

			m_pnIndices[k] = i*nNumPatchVertWidth + j;
			m_pnIndices[k + 1] = i*nNumPatchVertWidth + j + 1;


			m_pnIndices[k + 2] = (i + 1)*nNumPatchVertWidth + j;
			m_pnIndices[k + 3] = (i + 1)*nNumPatchVertWidth + j + 1;

			k += 4;
		}
	}


	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(UINT) * m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pnIndices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);

	//D3D11_RASTERIZER_DESC d3dxRasterizer;
	//ZeroMemory(&d3dxRasterizer, sizeof(D3D11_RASTERIZER_DESC));
	//d3dxRasterizer.FillMode = D3D11_FILL_WIREFRAME;
	//d3dxRasterizer.CullMode = D3D11_CULL_BACK;
	//pd3dDevice->CreateRasterizerState(&d3dxRasterizer, &m_pd3dRasterizerState);

}

CTessellationHeightMapGridMesh::~CTessellationHeightMapGridMesh()
{
}





CTestDisplacementMappingMesh::CTestDisplacementMappingMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth) : CMeshTexturedIlluminated(pd3dDevice)
{
	m_nVertices = 4;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	DisplacementVertex vertexInfo[4];
	vertexInfo[0].m_pd3dxvPositions = D3DXVECTOR3(-fx, -fy, -fz);
	vertexInfo[1].m_pd3dxvPositions = D3DXVECTOR3(-fx, +fy, -fz);
	vertexInfo[2].m_pd3dxvPositions = D3DXVECTOR3(+fx, +fy, -fz);
	vertexInfo[3].m_pd3dxvPositions = D3DXVECTOR3(+fx, -fy, -fz);

	vertexInfo[0].m_pd3dxvTexCoords = D3DXVECTOR2(0.0f, 1.0f);
	vertexInfo[1].m_pd3dxvTexCoords = D3DXVECTOR2(0.0f, 0.0f);
	vertexInfo[2].m_pd3dxvTexCoords = D3DXVECTOR2(1.0f, 0.0f);
	vertexInfo[3].m_pd3dxvTexCoords = D3DXVECTOR2(1.0f, 1.0f);

	m_pnVertexStrides = NULL;

	D3DXVECTOR3 pd3dxvNormals[4];

	CalculateVertexNormal(pd3dxvNormals);
	for (int j = 0; j < 4; j++)
	{
		vertexInfo[j].m_pd3dxvTangents = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
		vertexInfo[j].m_pd3dxvNormals = pd3dxvNormals[j];
	}



	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	//d3dBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;

	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(DisplacementVertex) * 4;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	d3dBufferDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = vertexInfo;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);


	m_nIndices = 6;
	m_pnIndices = new UINT[m_nIndices];
	m_pnIndices[0] = 0; m_pnIndices[1] = 1; m_pnIndices[2] = 2;
	m_pnIndices[3] = 0; m_pnIndices[4] = 2; m_pnIndices[5] = 3;

	//d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	//d3dBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;


	d3dBufferDesc.ByteWidth = sizeof(UINT)* m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//d3dBufferDesc.CPUAccessFlags = 0;
	//d3dBufferDesc.MiscFlags = 0;
	//ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pnIndices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);


	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fx, -fy, -fz);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fx, +fy, +fz);




	//m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	//D3DXVECTOR2 pd3dxvTexCoords[4];

	//m_pd3dxvPositions[0] = D3DXVECTOR3(-fx, -fy, -fz);
	//pd3dxvTexCoords[0] = D3DXVECTOR2(0.0f, 1.0f);

	//m_pd3dxvPositions[1] = D3DXVECTOR3(-fx, +fy, -fz);
	//pd3dxvTexCoords[1] = D3DXVECTOR2(0.0f, 0.0f);

	//m_pd3dxvPositions[2] = D3DXVECTOR3(+fx, +fy, -fz);
	//pd3dxvTexCoords[2] = D3DXVECTOR2(1.0f, 0.0f);

	//m_pd3dxvPositions[3] = D3DXVECTOR3(+fx, -fy, -fz);
	//pd3dxvTexCoords[3] = D3DXVECTOR2(1.0f, 1.0f);

	//D3D11_BUFFER_DESC d3dBufferDesc;
	//ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	////d3dBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;

	//d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	//d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//d3dBufferDesc.CPUAccessFlags = 0;
	//d3dBufferDesc.MiscFlags = 0;
	//D3D11_SUBRESOURCE_DATA d3dBufferData;
	//ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	//d3dBufferData.pSysMem = m_pd3dxvPositions;
	//pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	//D3DXVECTOR3 pd3dxvNormals[4];
	//CalculateVertexNormal(pd3dxvNormals);

	//D3DXVECTOR3 pd3dxvTangents[4];
	//for (int j = 0; j < 4; j++)
	//	pd3dxvTangents[j] = D3DXVECTOR3(1.0f, 0.0f, 0.0f);


	//d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	//d3dBufferData.pSysMem = pd3dxvNormals;
	//pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dNormalBuffer);

	//d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR2) * m_nVertices;
	//d3dBufferData.pSysMem = pd3dxvTexCoords;
	//pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dTexCoordBuffer);

	//d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	//d3dBufferData.pSysMem = pd3dxvTangents;
	//pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_ppd3dTangentBuffer);



	//ID3D11Buffer *pd3dBuffers[4] = { m_pd3dPositionBuffer, m_pd3dNormalBuffer, m_ppd3dTangentBuffer, m_pd3dTexCoordBuffer };
	//UINT pnBufferStrides[4] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2) };
	//UINT pnBufferOffsets[4] = { 0, 0, 0, 0 };
	//AssembleToVertexBuffer(4, pd3dBuffers, pnBufferStrides, pnBufferOffsets);


	//m_nIndices = 6;
	//m_pnIndices = new UINT[m_nIndices];
	//m_pnIndices[0] = 0; m_pnIndices[1] = 1; m_pnIndices[2] = 2;
	//m_pnIndices[3] = 0; m_pnIndices[4] = 2; m_pnIndices[5] = 3;

	////d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	////ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	////d3dBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;


	//d3dBufferDesc.ByteWidth = sizeof(UINT)* m_nIndices;
	//d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	////d3dBufferDesc.CPUAccessFlags = 0;
	////d3dBufferDesc.MiscFlags = 0;
	////ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	//d3dBufferData.pSysMem = m_pnIndices;
	//pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);


	//m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fx, -fy, -fz);
	//m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fx, +fy, +fz);






	//D3D11_RASTERIZER_DESC d3dxRasterizer;
	//ZeroMemory(&d3dxRasterizer, sizeof(D3D11_RASTERIZER_DESC));
	//d3dxRasterizer.FillMode = D3D11_FILL_WIREFRAME;
	//d3dxRasterizer.CullMode = D3D11_CULL_BACK;
	//pd3dDevice->CreateRasterizerState(&d3dxRasterizer, &m_pd3dRasterizerState);

}

CTestDisplacementMappingMesh::~CTestDisplacementMappingMesh()
{
}

void CTestDisplacementMappingMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	//메쉬의 정점은 여러 개의 정점 버퍼로 표현된다.
	UINT nStrides = sizeof(DisplacementVertex);
	UINT nOffset = 0;
	pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_pd3dPositionBuffer, &nStrides, &nOffset);
	pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, m_dxgiIndexFormat, m_nIndexOffset);
	pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);

	if (m_pd3dIndexBuffer)
		pd3dDeviceContext->DrawIndexed(m_nIndices, m_nStartIndex, m_nBaseVertex);
	else
		pd3dDeviceContext->Draw(m_nVertices, m_nStartVertex);
	pd3dDeviceContext->RSSetState(NULL);
}

CHumanMesh::CHumanMesh(ID3D11Device *pd3dDevice) : CMesh(pd3dDevice)
{
	D3DXVECTOR3* m_pWeight;
	BoneIndexData* m_pBoneIndex;
	FILE* pFile = NULL;
	::_wfopen_s(&pFile, L"Data/MachineGun_01.FTAAMesh", L"rb,ccs = UNICODE");
	::fread(&m_nVertices, sizeof(int), 1, pFile);
	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR3 *pd3dxvNormals = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR2 *pd3dxvTexCoords = new D3DXVECTOR2[m_nVertices];
	m_pWeight = new D3DXVECTOR3[m_nVertices];
	m_pBoneIndex = new BoneIndexData[m_nVertices];

	::fread(m_pd3dxvPositions, sizeof(D3DXVECTOR3), m_nVertices, pFile);
	::fread(pd3dxvNormals, sizeof(D3DXVECTOR3), m_nVertices, pFile);
	::fread(pd3dxvTexCoords, sizeof(D3DXVECTOR2), m_nVertices, pFile);
	// 가중치
	::fread(m_pWeight, sizeof(D3DXVECTOR3), m_nVertices, pFile);



	// 본 인덱스.
	::fread(m_pBoneIndex, sizeof(BoneIndexData), m_nVertices, pFile); // x, y, z, w

	/*for (int i = 0; i < m_nVertices; i++)
	{
		cout << m_pBoneIndex[i].x << "  " << m_pBoneIndex[i].y << "  " << m_pBoneIndex[i].z << "  " << m_pBoneIndex[i].w << endl;

	}*/

	::fread(&m_nIndices, sizeof(UINT), 1, pFile);
	m_pnIndices = new UINT[m_nIndices];
	::fread(m_pnIndices, sizeof(UINT), m_nIndices, pFile);

	::fclose(pFile);








	//FILE *pFile = NULL;
	////이진 파일을 열어 정점의 개수와 정점의 위치 벡터들을 순서대로 읽는다.
	//::_wfopen_s(&pFile, L"Data/_pos.txt", L"rb, ccs=UNICODE");
	//
	//int nReadBytes = ::fread(&m_nVertices, sizeof(UINT), 1, pFile);
	//D3DXVECTOR3*pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	//nReadBytes = ::fread(pd3dxvPositions, sizeof(D3DXVECTOR3), m_nVertices, pFile);
	//::fclose(pFile);


	//인겍스
	//::_wfopen_s(&pFile, L"s_index.txt", L"rb, ccs=UNICODE");
	//nReadBytes = ::fread(&m_nIndices, sizeof(UINT), 1, pFile);
	//UINT *pnIndices = new UINT[m_nIndices];
	//nReadBytes = ::fread(pnIndices, sizeof(UINT), m_nIndices, pFile);
	//::fclose(pFile);


	//int nTexNum = 0;
	//::_wfopen_s(&pFile, L"mesh01_tex.txt", L"rb, ccs=UNICODE");
	//nReadBytes = ::fread(&nTexNum, sizeof(UINT), 1, pFile);
	//D3DXVECTOR2*pd3dxvTexCoord = new D3DXVECTOR2[nTexNum];
	//nReadBytes = ::fread(pd3dxvTexCoord, sizeof(D3DXVECTOR2), nTexNum, pFile);
	//::fclose(pFile);



	//for (int i = 0; i < m_nIndices; i++)
	//	cout << pnIndices[i] << endl;



	//for (int i = 0; i < m_nVertices; i++)
	//{
	//	cout << pd3dxvPositions[i].x << "  " << pd3dxvPositions[i].y << " " << pd3dxvPositions[i].z << endl;
	//}



	// 샘플 코드
	/*m_nVertices = 4;
	D3DXVECTOR3*pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	pd3dxvPositions[0] = D3DXVECTOR3(-50.0f, -50.0f, +0.0f);
	pd3dxvPositions[1] = D3DXVECTOR3(-50.0f, +50.0f, +0.0f);
	pd3dxvPositions[2] = D3DXVECTOR3(+50.0f, +50.0f, +0.0f);
	pd3dxvPositions[3] = D3DXVECTOR3(+50.0f, -50.0f, +0.0f);

	m_nIndices = 6;
	UINT *pnIndices = new UINT[m_nIndices];
	pnIndices[0] = 0, pnIndices[1] = 1, pnIndices[2] = 2;
	pnIndices[3] = 0, pnIndices[4] = 2, pnIndices[5] = 3;*/


	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//정점은 두 개의 요소를 가지고 각 요소마다 하나의 버퍼를 생성한다.
	/*정점의 첫 번째 요소는 위치 벡터이고 위치 벡터 한 개는 12바이트이다.
	정점의 첫 번째 요소를 나타내는 버퍼를 생성한다.*/
	//D3D11_BUFFER_DESC d3dBufferDesc;
	//ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	//d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	//d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//d3dBufferDesc.CPUAccessFlags = 0;
	//D3D11_SUBRESOURCE_DATA d3dBufferData;
	//ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	//d3dBufferData.pSysMem = pd3dxvPositions;
	//pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);
	//delete[] pd3dxvPositions;




	////정점 두 번째 요소는 색상 벡터이고 색상 벡터 한 개는 16바이트이다.
	//D3DXCOLOR *pd3dxColors = new D3DXCOLOR[m_nVertices];
	//for (int i = 0; i < m_nVertices; i++)
	//	pd3dxColors[i] = RANDOM_COLOR;

	////정점의 두 번째 요소를 나타내는 버퍼를 생성한다.
	//d3dBufferDesc.ByteWidth = sizeof(D3DXCOLOR) * m_nVertices;
	//d3dBufferData.pSysMem = pd3dxColors;
	//pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dColorBuffer);
	//delete[] pd3dxColors;

	////정점을 나타내는 두 개의 버퍼와 정보를 입력 조립기로 전달할 수 있는 형태로 구성한다.
	//ID3D11Buffer *ppd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dColorBuffer };
	//UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXCOLOR) };
	//UINT pnBufferOffsets[2] = { 0, 0 };
	//AssembleToVertexBuffer(2, ppd3dBuffers, pnBufferStrides, pnBufferOffsets);




	//인덱스 버퍼를 생성한다.
	//::ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	//d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//d3dBufferDesc.ByteWidth = sizeof(UINT) * m_nIndices;
	//d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//d3dBufferDesc.CPUAccessFlags = 0;
	//::ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	//d3dBufferData.pSysMem = pnIndices;
	//pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);

	//delete[] pnIndices;



	//D3D11_RASTERIZER_DESC d3dxRasterizer;
	//ZeroMemory(&d3dxRasterizer, sizeof(D3D11_RASTERIZER_DESC));
	//d3dxRasterizer.FillMode = D3D11_FILL_WIREFRAME;
	//d3dxRasterizer.CullMode = D3D11_CULL_BACK;
	//pd3dDevice->CreateRasterizerState(&d3dxRasterizer, &m_pd3dRasterizerState);
}
CHumanMesh::~CHumanMesh()
{
}
D3DXMATRIX* CCharacterMesh::Rotate(D3DXVECTOR3 *pd3dxvAxis, float fAngle)
{
	D3DXMATRIX mtxRotate;
	D3DXMatrixRotationAxis(&mtxRotate, pd3dxvAxis, (float)D3DXToRadian(fAngle));
	return  &mtxRotate;
}


CCharacterMesh::CCharacterMesh(ID3D11Device *pd3dDevice, string strFileName) : CMeshTexturedIlluminated(pd3dDevice)
{
	FILE* pFile = NULL;
	string fileName = strFileName;
	//string fileName = "Data/";
	//fileName += "_pos.txt";
	::fopen_s(&pFile, (fileName.c_str()), "rb,ccs = UNICODE");
	// 정점의 개수를 읽는다.
	::fread(&m_nVertices, sizeof(int), 1, pFile);
	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR3 *pd3dxvNormals = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR2 *pd3dxvTexCoords = new D3DXVECTOR2[m_nVertices];
	m_pBoneIndex1 = new BoneIndexData1[m_nVertices];
	m_pWeight1 = new BoneWeightData1[m_nVertices];
	m_pBoneIndex2 = new BoneIndexData2[m_nVertices];
	m_pWeight2 = new BoneWeightData2[m_nVertices];


	::fread(m_pd3dxvPositions, sizeof(D3DXVECTOR3), m_nVertices, pFile);
	::fread(pd3dxvNormals, sizeof(D3DXVECTOR3), m_nVertices, pFile);
	::fread(pd3dxvTexCoords, sizeof(D3DXVECTOR2), m_nVertices, pFile);
	fread(&m_pWeight1[0], sizeof(BoneWeightData1), m_nVertices, pFile);
	fread(&m_pWeight2[0], sizeof(BoneWeightData2), m_nVertices, pFile);
	fread(&m_pBoneIndex1[0], sizeof(BoneIndexData1), m_nVertices, pFile);
	fread(&m_pBoneIndex2[0], sizeof(BoneIndexData2), m_nVertices, pFile);
	::fclose(pFile);


	D3DXMATRIX scaleMat;
	D3DXMatrixScaling(&scaleMat, 0.1f, 0.1f, 0.1f);
	D3DXMATRIX *tempMat = new D3DXMATRIX[m_nVertices];

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3)* m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;


	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3)* m_nVertices;
	d3dBufferData.pSysMem = pd3dxvNormals;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dNormalBuffer);

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR2)* m_nVertices;
	d3dBufferData.pSysMem = pd3dxvTexCoords;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dTexCoordBuffer);

	d3dBufferDesc.ByteWidth = sizeof(BoneWeightData1)* m_nVertices;
	d3dBufferData.pSysMem = m_pWeight1;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dWeightBuffer1);

	d3dBufferDesc.ByteWidth = sizeof(BoneIndexData1)* m_nVertices;
	d3dBufferData.pSysMem = m_pBoneIndex1;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dBoneIndexBuffer1);

	d3dBufferDesc.ByteWidth = sizeof(BoneWeightData2)* m_nVertices;
	d3dBufferData.pSysMem = m_pWeight2;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dWeightBuffer2);

	d3dBufferDesc.ByteWidth = sizeof(BoneIndexData2)* m_nVertices;
	d3dBufferData.pSysMem = m_pBoneIndex2;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dBoneIndexBuffer2);


	UINT BufferCount = 7; //5
	ID3D11Buffer *pd3dBuffers[7] = {
		m_pd3dPositionBuffer,
		m_pd3dNormalBuffer,
		m_pd3dTexCoordBuffer,
		m_pd3dWeightBuffer1,
		m_pd3dWeightBuffer2,
		m_pd3dBoneIndexBuffer1,
		m_pd3dBoneIndexBuffer2
	};
	UINT pnBufferStrides[7] = {
		sizeof(D3DXVECTOR3),
		sizeof(D3DXVECTOR3),
		sizeof(D3DXVECTOR2),
		sizeof(BoneWeightData1),
		sizeof(BoneWeightData2),
		sizeof(BoneIndexData1),
		sizeof(BoneIndexData2)
	};
	UINT pnBufferOffsets[7] = { 0, 0, 0, 0, 0, 0, 0 }; // 5
	AssembleToVertexBuffer(BufferCount, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	//ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	//d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//d3dBufferDesc.ByteWidth = sizeof(UINT)* m_nIndices;
	//d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//d3dBufferDesc.CPUAccessFlags = 0;
	//ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	//d3dBufferData.pSysMem = m_pnIndices;
	//pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);

	D3DXVECTOR3 min = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 max = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < m_nVertices; ++i){
		if (m_pd3dxvPositions[i].x < min.x) min.x = m_pd3dxvPositions[i].x;
		if (m_pd3dxvPositions[i].y < min.y) min.y = m_pd3dxvPositions[i].y;
		if (m_pd3dxvPositions[i].z < min.z) min.z = m_pd3dxvPositions[i].z;

		if (m_pd3dxvPositions[i].x > max.x) max.x = m_pd3dxvPositions[i].x;
		if (m_pd3dxvPositions[i].y > max.y) max.y = m_pd3dxvPositions[i].y;
		if (m_pd3dxvPositions[i].z > max.z) max.z = m_pd3dxvPositions[i].z;
	}

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(min.x, min.y, min.z);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(max.x, max.y, max.z);
}

CCharacterMesh::~CCharacterMesh()
{
	if (m_pd3dWeightBuffer1) m_pd3dWeightBuffer1->Release();
	if (m_pd3dBoneIndexBuffer1)m_pd3dBoneIndexBuffer1->Release();
	if (m_pWeight1) {
		delete[] m_pWeight1;
	}
	if (m_pBoneIndex1){
		delete[] m_pBoneIndex1;
	}

	if (m_pWeight2) {
		delete[] m_pWeight2;
	}
	if (m_pBoneIndex2){
		delete[] m_pBoneIndex2;
	}
}
void CCharacterMesh::CreateChileMesh(ID3D11Device *pd3dDevice)
{

	string strFileName = "Data/walk01Cylinder005";
	int nChileNum = 1;
	pChildMeshs = new CCharacterMesh*[nChileNum];
	pChildMeshs[0] = new  CCharacterMesh(pd3dDevice, strFileName);
	m_vChildList.push_back(pChildMeshs[0]);
}

void  CCharacterMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	//메쉬의 정점은 여러 개의 정점 버퍼로 표현된다.
	pd3dDeviceContext->IASetVertexBuffers(m_nSlot, m_nBuffers, m_ppd3dVertexBuffers, m_pnVertexStrides, m_pnVertexOffsets);
	pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, m_dxgiIndexFormat, m_nIndexOffset);
	pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);

	if (m_pd3dIndexBuffer)
		pd3dDeviceContext->DrawIndexed(m_nIndices, m_nStartIndex, m_nBaseVertex);
	else
		pd3dDeviceContext->Draw(m_nVertices, m_nStartVertex);

	//for (int i = 0; i < 1; i++)
	//	m_vChildList[i]->Render(pd3dDeviceContext);

	//for (auto iter : this->m_vChildList)
	//	iter->Render(pd3dDeviceContext);


	pd3dDeviceContext->RSSetState(NULL);
}