#include "AirplaneMesh.h"


CAirplaneMeshDiffused::CAirplaneMeshDiffused(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth, D3DXCOLOR d3dxColor) : CMeshDiffused(pd3dDevice)
{
	m_nVertices = 24 * 3;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	//비행기 메쉬는 2개의 정점 버퍼(위치 벡터 버퍼와 색상 버퍼)로 구성된다.
	//D3DXVECTOR3 m_pd3dxvPositions[24 * 3];

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];

	float x1 = fx * 0.2f, y1 = fy * 0.2f, x2 = fx * 0.1f, y3 = fy * 0.3f, y2 = ((y1 - (fy - y3)) / x1) * x2 + (fy - y3);
	int i = 0;
	//Upper Plane
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, -fz);

	//Lower Plane
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, +fz);

	//Right Plane
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, +fz);

	//Back/Right Plane
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, +fz);

	//Left Plane
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, +fz);

	//Back/Left Plane
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, -fz);

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

	D3DXCOLOR pd3dxColors[24 * 3];
	for (int j = 0; j < m_nVertices; j++) pd3dxColors[j] = d3dxColor + RANDOM_COLOR;
	pd3dxColors[0] = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pd3dxColors[3] = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pd3dxColors[6] = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	pd3dxColors[9] = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	pd3dxColors[12] = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pd3dxColors[15] = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pd3dxColors[18] = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	pd3dxColors[21] = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);

	d3dBufferDesc.ByteWidth = sizeof(D3DXCOLOR) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxColors;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dColorBuffer);

	ID3D11Buffer *ppd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dColorBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXCOLOR) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, ppd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fx, -fy, -fz);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fx, +fy, +fz);
}


CAirplaneMeshDiffused::~CAirplaneMeshDiffused()
{
}


