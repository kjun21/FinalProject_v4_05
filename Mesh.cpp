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
// �޽��� �׸���. �޽� Ŭ������ ���� ���۸� ����̽� ���ؽ�Ʈ�� �����ϰ� ������Ƽ�� ������ �����Ѵ�.  
void CMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	//�޽��� ������ ���� ���� ���� ���۷� ǥ���ȴ�.
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
	//�ν��Ͻ��� ��� �Է� �����⿡ �޽��� ���� ���ۿ� �ν��Ͻ� ���� ���۰� ����ȴ�.
	pd3dDeviceContext->IASetVertexBuffers(m_nSlot, m_nBuffers, m_ppd3dVertexBuffers, m_pnVertexStrides, m_pnVertexOffsets);
	pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, m_dxgiIndexFormat, m_nIndexOffset);
	pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);

	//��ü���� �ν��Ͻ����� �������Ѵ�. 
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
	//�� ��ǥ���� ������ ������(pd3dxvRayPosition)�� ������ �־��� �� �޽����� �浹 �˻縦 �Ѵ�.
	int nIntersections = 0;
	BYTE *pbPositions = (BYTE *)m_pd3dxvPositions + m_pnVertexOffsets[0];

	int nOffset = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? 3 : 1;
	//�޽��� ������Ƽ��(�ﰢ��)���� �����̴�. �ﰢ�� ����Ʈ�� ��� (������ ���� / 3) �Ǵ� (�ε����� ���� / 3), �ﰢ�� ��Ʈ���� ��� (������ ���� - 2) �Ǵ� (�ε����� ���� - 2)�̴�.
	int nPrimitives = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nVertices / 3) : (m_nVertices - 2);
	if (m_nIndices > 0) nPrimitives = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nIndices / 3) : (m_nIndices - 2);

	D3DXVECTOR3 v0, v1, v2;
	float fuHitBaryCentric, fvHitBaryCentric, fHitDistance, fNearHitDistance = FLT_MAX;
	/*�޽��� ��� ������Ƽ��(�ﰢ��)�鿡 ���Ͽ� ��ŷ �������� �浹�� �˻��Ѵ�. �浹�ϴ� ��� �ﰢ���� ã�� ������ ������(�����δ� ī�޶� ��ǥ���� ����)�� ���� ����� �ﰢ���� ã�´�.*/
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

	//�� �޽��� 2���� ���� ����(��ġ ���� ���ۿ� ���� ����)�� �����ȴ�.
	//�� �޽��� ���� ����(��ġ ���� ����)�� �����Ѵ�.

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

	//�� �޽��� ���� ����(���� ����)�� �����Ѵ�.
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
	/*�ٿ�� �ڽ��� �ּ����� �ִ����� ȸ���� �ϸ� �� �̻� �ּ����� �ִ����� ���� �ʴ´�.
	�׷��Ƿ� �ٿ�� �ڽ��� �ּ����� �ִ������� 8���� ������ ���ϰ� ��ȯ(ȸ��)�� �� ���� �ּ����� �ִ����� �ٽ� ����Ѵ�.*/

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
	//8���� �������� x, y, z ��ǥ�� �ּҰ��� �ִ밪�� ���Ѵ�.
	for (int i = 0; i < 8; i++)
	{
		//������ ��ȯ�Ѵ�.
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

	//������ ��ġ ����, ���� ����, �ؽ��� ��ǥ, ������ �ؽ��� ��ǥ�� ���´�.
	ID3D11Buffer *pd3dBuffers[4] = { m_pd3dPositionBuffer, m_pd3dNormalBuffer, m_pd3dTexCoordBuffer, m_pd3dDetailTexCoordBuffer };
	UINT pnBufferStrides[4] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2), sizeof(D3DXVECTOR2) };
	UINT pnBufferOffsets[4] = { 0, 0, 0, 0 };
	AssembleToVertexBuffer(4, pd3dBuffers, pnBufferStrides, pnBufferOffsets);
	//
	/*���ڴ� �簢������ �����̰� �簢���� �� ���� �ﰢ������ �����ǹǷ� ���ڴ� <�׸� 18>�� ���� �ﰢ������ �����̶�� �� �� �ִ�. ���ڸ� ǥ���ϱ� ���Ͽ� ������ �ﰢ������ �ε����� ǥ���ؾ� �Ѵ�. �ﰢ������ �簢���� �� ������ �Ʒ����� ���� ��������(z-��) �����Ѵ�. ù ��° �簢�� ���� �ﰢ������ ���ʿ��� ����������(x-��) �����Ѵ�. �� ��° ���� �ﰢ������ �����ʿ��� ���� �������� �����Ѵ�. ��, �簢���� ���� �ٲ� ������ ���� ������ �ٲ�� �Ѵ�. <�׸� 18>�� ���ڿ� ���Ͽ� �ﰢ�� ��Ʈ���� ����Ͽ� �ﰢ������ ǥ���ϱ� ���� �ε����� ������ ������ ���� ������ m��° �ٰ� (m+1)��° ���� ���� ��ȣ�� �簢���� ���� ���⿡ ���� ������ �Ʒ�, ��, �Ʒ�, ��, ... ������ �����ϸ� �ȴ�.
	0, 6, 1, 7, 2, 8, 3, 9, 4, 10, 5, 11, 11, 17, 10, 16, 9, 15, 8, 14, 7, 13, 6, 12
	�̷��� �ε����� �����ϸ� �ﰢ�� ��Ʈ���� ����� ���̹Ƿ� ���� �׷����� �ﰢ������ �ε����� ������ ����.
	(0, 6, 1), (1, 6, 7), (1, 7, 2), (2, 7, 8), (2, 8, 3), (3, 8, 9), ...
	�׷��� �̷��� �ε����� ����ϸ� ù ��° ���� �����ϰ� �ﰢ������ ����� �׷����� �ʴ´�. �ֳ��ϸ� �ﰢ�� ��Ʈ�������� ������ 2���� ������ ���ο� �ϳ��� ������ ����Ͽ� ���ο� �ﰢ���� �׸���. �׸��� Ȧ�� ��° �ﰢ���� ���� ���� ����(���ε� ����)�� �ð�����̰� ¦�� ��° �ﰢ���� ���ε� ������ �ݽð�����̾�� �Ѵ�. ������ �簢���� �� �ٿ��� �� ���� �ִ��� ������� �� ���� ������ �ﰢ���� ¦�� ��° �ﰢ���̰� ���ε� ������ �ݽð� �����̴�. �ֳ��ϸ� �簢���� �� ���� �ﰢ������ ���������� �����̴�. ù ��° �ٿ��� �� ��° ���� �ε��� ������ ���� �׷����� �ﰢ������ �ε����� ���캸��.
	..., 4, 10, 5, 11, 11, 17, 10, 16, 9, 15, 8, 14, 7, 13, 6, 12, ...
	..., (4, 10, 5), (5, 10, 11), (5, 11, 11), (11, 11, 17), (11, 17, 10), ...
	�ﰢ�� (5, 10, 11)�� ù ��° ���� ������ �ﰢ���̰� ¦�� ��°�̴�. �ﰢ�� (11, 17, 10)�� �� ��° ���� ù ��° �ﰢ���̰� Ȧ�� ��°�̴�. Ȧ�� ��°�̹Ƿ� ���ε� ������ �ð�����̾�� �ϴµ� ���� ���ε� ������ �ݽð�����̹Ƿ� �׷����� ���� ���̴�. �翬�� ���� �ﰢ���� ���ε� ������ ���� �����Ƿ� �׷����� ���� ���̴�. �ﰢ�� (11, 17, 10)�� ���ε� ������ �ݽð�����̹Ƿ� �׷������� �Ϸ��� �� �ﰢ���� ¦�� ��° �ﰢ���� �ǵ��� �ؾ� �Ѵ�. �̸� ���ؼ� ���� �ٲ� ������ ������ ������ �ε����� �߰��ϵ��� ����. �׷��� ���� �ٲ� ù ��° �ﰢ���� ¦�� ��° �ﰢ���� �ȴ�. ������ �������� 11�� �߰��� ������ ������ �ε����̴�. �̷��� �ϸ� �ﰢ���� ������ �� ��� �׷����� �ʴ� �ﰢ���� �� �ٸ��� 3���� ����� �ȴ�.
	..., 4, 10, 5, 11, 11, 11, 17, 10, 16, 9, 15, 8, 14, 7, 13, 6, 12, ...
	..., (5, 10, 11), (5, 11, 11), (11, 11, 11), (11, 11, 17), (11, 17, 10), ...
	�� ���� �ﰢ�� (5, 11, 11), (11, 11, 11), (11, 11, 17)�� �ﰢ���� ������ �� �����Ƿ� ������ �׷����� �ʴ´�.
	�̷��� �ε����� �����ϸ� �ε��� ���۴� ((nWidth*2)*(nLength-1))+((nLength-1)-1)���� �ε����� ���´�. �簢�� ���� ������ (nLength-1)�̰� �� �ٿ��� (nWidth*2)���� �ε����� ���´�. �׸��� ���� �ٲ� ������ �ε����� �ϳ� �߰��ϹǷ� (nLength-1)-1���� �ε����� �߰��� �ʿ��ϴ�.*/
	m_nIndices = ((nWidth * 2)*(nLength - 1)) + ((nLength - 1) - 1);
	m_pnIndices = new UINT[m_nIndices];
	for (int j = 0, z = 0; z < nLength - 1; z++)
	{
		if ((z % 2) == 0)
		{
			//Ȧ�� ��° ���̹Ƿ�(z = 0, 2, 4, ...) �ε����� ���� ������ ���ʿ��� ������ �����̴�.
			for (int x = 0; x < nWidth; x++)
			{
				//ù ��° ���� �����ϰ� ���� �ٲ� ������(x == 0) ù ��° �ε����� �߰��Ѵ�.
				if ((x == 0) && (z > 0)) m_pnIndices[j++] = (UINT)(x + (z * nWidth));
				//�Ʒ�, ���� ������ �ε����� �߰��Ѵ�.
				m_pnIndices[j++] = (UINT)(x + (z * nWidth));
				m_pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
		else
		{
			//¦�� ��° ���̹Ƿ�(z = 1, 3, 5, ...) �ε����� ���� ������ �����ʿ��� ���� �����̴�.
			for (int x = nWidth - 1; x >= 0; x--)
			{
				//���� �ٲ� ������(x == (nWidth-1)) ù ��° �ε����� �߰��Ѵ�.
				if (x == (nWidth - 1)) m_pnIndices[j++] = (UINT)(x + (z * nWidth));
				//�Ʒ�, ���� ������ �ε����� �߰��Ѵ�.
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
	//���� �� ��ü�� ���� �� �̹����� �ȼ� ���� ������ ���̷� ��ȯ�Ѵ�. 
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
		/*������Ƽ�갡 �ﰢ�� ����Ʈ�� �� �ε��� ���۰� �ִ� ���� ���� ��츦 �����Ͽ� ������ ���� ���͸� ����Ѵ�. �ε��� ���۸� ������� �ʴ� ��� �� ������ ���� ���ʹ� �� ������ ���Ե� �ﰢ���� ���� ���ͷ� ����Ѵ�. �ε��� ���۸� ����ϴ� ��� �� ������ ���� ���ʹ� �� ������ ���Ե� �ﰢ������ ���� ������ �������(���Ͽ�) ����Ѵ�.*/
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		if (m_pnIndices)
			SetAverageVertexNormal(pd3dxvNormals, (m_nIndices / 3), 3, false);
		else
			SetTriAngleListVertexNormal(pd3dxvNormals);
		break;
		/*������Ƽ�갡 �ﰢ�� ��Ʈ���� �� �� ������ ���� ���ʹ� �� ������ ���Ե� �ﰢ������ ���� ������ �������(���Ͽ�) ����Ѵ�.*/
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		SetAverageVertexNormal(pd3dxvNormals, (m_nIndices) ? (m_nIndices - 2) : (m_nVertices - 2), 1, true);
		break;
	default:
		break;
	}
}
void CMeshIlluminated::SetTriAngleListVertexNormal(D3DXVECTOR3 *pd3dxvNormals)
{
	/*�ﰢ��(������Ƽ��)�� ������ ���ϰ� �� �ﰢ���� ���� ���͸� ����ϰ� �ﰢ���� �����ϴ� �� ������ ���� ���ͷ� �����Ѵ�.*/
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

	//������ü�� �� ��(�ﰢ�� 2��)�� �ϳ��� �ؽ��� �̹��� ��ü�� ���εǵ��� �ؽ��� ��ǥ�� �����Ѵ�.
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

	/*������ü�� �� �鿡 �ؽ��ĸ� �����Ϸ��� �ε����� ����� �� �����Ƿ�
	�ε��� ���۴� �������� �ʴ´�.*/

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
	//��ī�� �ڽ��� 6���� ��(�簢��), �簢���� ���� 4��, �׷��Ƿ� 24���� ������ �ʿ��ϴ�.
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

	//�ﰢ�� ��Ʈ������ �簢�� 1���� �׸��� ���� �ε����� 4���� �ʿ��ϴ�.
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
	//��ī�� �ڽ� �簢������ ���� ���� �˰����� �������� �ʰ� ���� ���۸� �������� �ʴ´�.
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
	//���� �߿��Ұ� �����κ��̴�.
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
	//6���� ��ī�� �ڽ� �ؽ��ĸ� �����Ͽ� CTexture ��ü�� �����Ѵ�. //
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

	//��ī�� �ڽ��� �׸��� ���� ���÷� ���� ��ü�� ���� ���ٽ� ���� ��ü�� �����Ѵ�.
	m_pSkyboxTexture->UpdateSamplerShaderVariable(pd3dDeviceContext, 0, 0);
	pd3dDeviceContext->OMSetDepthStencilState(m_pd3dDepthStencilState, 1);

	//��ī�� �ڽ��� 6�� ��(�簢��)�� ������� �׸���.
	for (int i = 0; i < 6; i++)
	{
		//��ī�� �ڽ��� �� ��(�簢��)�� �׸� �� ����� �ؽ��ĸ� �����Ѵ�.
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
	//������ �κ��� LabProject13-2�� CCubeMeshTextured Ŭ������ ������ �κа� �����ϴ�.
	D3DXVECTOR2 pd3dxvTexCoords[36];
	int i = 0;

	//������ü�� �� ��(�ﰢ�� 2��)�� �ϳ��� �ؽ��� �̹��� ��ü�� ���εǵ��� �ؽ��� ��ǥ�� �����Ѵ�.
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

	//���� ���͸� �����ϱ� ���� ���� �ڵ带 �߰��Ѵ�.
	D3DXVECTOR3 pd3dxvNormals[36];
	CalculateVertexNormal(pd3dxvNormals);

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxvNormals;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dNormalBuffer);

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR2) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxvTexCoords;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dTexCoordBuffer);

	//������ ��ġ ����, ���� ����, �ؽ��� ��ǥ�� ���´�.
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


//LabProject13-2�� CSphereMeshTextured Ŭ������ �����ڿ� ���� ���͸� �߰��Ѵ�.
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

	//������ ��ġ ����, ���� ����, �ؽ��� ��ǥ�� ���´�.
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
	//������ �κ��� LabProject13-2�� CCubeMeshTextured Ŭ������ ������ �κа� �����ϴ�.
	D3DXVECTOR2 pd3dxvTexCoords[8];
	int i = 0;

	//������ü�� �� ��(�ﰢ�� 2��)�� �ϳ��� �ؽ��� �̹��� ��ü�� ���εǵ��� �ؽ��� ��ǥ�� �����Ѵ�.
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
	//���� ���͸� �����ϱ� ���� ���� �ڵ带 �߰��Ѵ�.
	D3DXVECTOR3 pd3dxvNormals[8];
	CalculateVertexNormal(pd3dxvNormals);


	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxvNormals;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dNormalBuffer);

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR2) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxvTexCoords;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dTexCoordBuffer);

	//������ ��ġ ����, ���� ����, �ؽ��� ��ǥ�� ���´�.
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
	//�޽��� ������ ���� ���� ���� ���۷� ǥ���ȴ�.
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
	//������ �κ��� LabProject13-2�� CCubeMeshTextured Ŭ������ ������ �κа� �����ϴ�.
	D3DXVECTOR2 pd3dxvTexCoords[8];
	int i = 0;

	//������ü�� �� ��(�ﰢ�� 2��)�� �ϳ��� �ؽ��� �̹��� ��ü�� ���εǵ��� �ؽ��� ��ǥ�� �����Ѵ�.
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
	//���� ���͸� �����ϱ� ���� ���� �ڵ带 �߰��Ѵ�.
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

	//ź��Ʈ
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR2) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxvTangents;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_ppd3dTangentBuffer);

	//������ ��ġ ����, ���� ����, �ؽ��� ��ǥ�� ���´�.
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
//===================�׼����̼�================================
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
	//���� ���͸� �����ϱ� ���� ���� �ڵ带 �߰��Ѵ�.
	D3DXVECTOR3 pd3dxvNormals[4];
	CalculateVertexNormal(pd3dxvNormals);







	//�ε��� ����
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

	//ź��Ʈ
	/*	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR2) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxvTangents;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_ppd3dTangentBuffer)*/;

	//������ ��ġ ����, ���� ����, �ؽ��� ��ǥ�� ���´�.
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
	//�޽��� ������ ���� ���� ���� ���۷� ǥ���ȴ�.
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
		float z = nPatchLength * i; //����
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


	//������ ��ġ ����, ���� ����, �ؽ��� ��ǥ, ������ �ؽ��� ��ǥ�� ���´�.
	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dTexCoordBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	//�ε��� ����
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
	//�޽��� ������ ���� ���� ���� ���۷� ǥ���ȴ�.
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
	// ����ġ
	::fread(m_pWeight, sizeof(D3DXVECTOR3), m_nVertices, pFile);



	// �� �ε���.
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
	////���� ������ ���� ������ ������ ������ ��ġ ���͵��� ������� �д´�.
	//::_wfopen_s(&pFile, L"Data/_pos.txt", L"rb, ccs=UNICODE");
	//
	//int nReadBytes = ::fread(&m_nVertices, sizeof(UINT), 1, pFile);
	//D3DXVECTOR3*pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	//nReadBytes = ::fread(pd3dxvPositions, sizeof(D3DXVECTOR3), m_nVertices, pFile);
	//::fclose(pFile);


	//�΁���
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



	// ���� �ڵ�
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

	//������ �� ���� ��Ҹ� ������ �� ��Ҹ��� �ϳ��� ���۸� �����Ѵ�.
	/*������ ù ��° ��Ҵ� ��ġ �����̰� ��ġ ���� �� ���� 12����Ʈ�̴�.
	������ ù ��° ��Ҹ� ��Ÿ���� ���۸� �����Ѵ�.*/
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




	////���� �� ��° ��Ҵ� ���� �����̰� ���� ���� �� ���� 16����Ʈ�̴�.
	//D3DXCOLOR *pd3dxColors = new D3DXCOLOR[m_nVertices];
	//for (int i = 0; i < m_nVertices; i++)
	//	pd3dxColors[i] = RANDOM_COLOR;

	////������ �� ��° ��Ҹ� ��Ÿ���� ���۸� �����Ѵ�.
	//d3dBufferDesc.ByteWidth = sizeof(D3DXCOLOR) * m_nVertices;
	//d3dBufferData.pSysMem = pd3dxColors;
	//pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dColorBuffer);
	//delete[] pd3dxColors;

	////������ ��Ÿ���� �� ���� ���ۿ� ������ �Է� ������� ������ �� �ִ� ���·� �����Ѵ�.
	//ID3D11Buffer *ppd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dColorBuffer };
	//UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXCOLOR) };
	//UINT pnBufferOffsets[2] = { 0, 0 };
	//AssembleToVertexBuffer(2, ppd3dBuffers, pnBufferStrides, pnBufferOffsets);




	//�ε��� ���۸� �����Ѵ�.
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
	// ������ ������ �д´�.
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
	//�޽��� ������ ���� ���� ���� ���۷� ǥ���ȴ�.
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