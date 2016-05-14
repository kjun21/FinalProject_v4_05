#include "WaveMesh.h"


CWaveMesh::CWaveMesh(ID3D11Device *pd3dDevice, UINT m, UINT n,
	float fWidth, float fHeight, float fDepth) : CMeshTexturedIlluminated(pd3dDevice)
{
	// 말 그대로 길이다. 길이. 명심할 것.

	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;


	// 정점의 개수 80 * 80 = 1600우와..
	m_nVertices = m*n;
	UINT nFaceCount = (m - 1)*(n - 1) * 2;

	//
	// Create the vertices.
	//

	float fHalfWidth = 0.5f*fWidth;
	float fHalfDepth = 0.5f*fDepth;

	float dx = fWidth / (n - 1);
	float dz = fDepth / (m - 1);

	float du = 1.0f / (n - 1);
	float dv = 1.0f / (m - 1);

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR3* pd3dxvNormals = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR3* pd3dxvTangents = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR2* pd3dxvTexCoords = new D3DXVECTOR2[m_nVertices];

	float fy = 0.0;


	for (UINT i = 0; i < m; ++i)
	{
		float fz = fHalfDepth - i*dz;
		for (UINT j = 0; j < n; ++j)
		{
			float fx = -fHalfWidth + j*dx;

			m_pd3dxvPositions[i*n + j] = D3DXVECTOR3(fx, fy, fz);
			pd3dxvNormals[i*n + j] = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			pd3dxvTangents[i*n + j] = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

			// Stretch texture over grid.
			pd3dxvTexCoords[i*n + j] = D3DXVECTOR2(j*du, i*dv);
		}
	}

	//
	// Create the indices.
	//

	m_nIndices = nFaceCount * 3; // 3 indices per face
	m_pnIndices = new UINT[m_nIndices];

	// Iterate over each quad and compute indices.
	UINT k = 0;
	for (UINT i = 0; i < m - 1; ++i)
	{
		for (UINT j = 0; j < n - 1; ++j)
		{
			m_pnIndices[k] = i*n + j;
			m_pnIndices[k + 1] = i*n + j + 1;
			m_pnIndices[k + 2] = (i + 1)*n + j;

			m_pnIndices[k + 3] = (i + 1)*n + j;
			m_pnIndices[k + 4] = i*n + j + 1;
			m_pnIndices[k + 5] = (i + 1)*n + j + 1;

			k += 6; // next quad
		}
	}

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

	d3dBufferDesc.ByteWidth = sizeof(UINT)* m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//d3dBufferDesc.CPUAccessFlags = 0;
	//d3dBufferDesc.MiscFlags = 0;
	//ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pnIndices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);


	//m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fx, -fy, -fz);
	//m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fx, +fy, +fz);
	//


	

	D3D11_RASTERIZER_DESC d3dxRasterizer;
	ZeroMemory(&d3dxRasterizer, sizeof(D3D11_RASTERIZER_DESC));
	d3dxRasterizer.FillMode = D3D11_FILL_WIREFRAME;
	d3dxRasterizer.CullMode = D3D11_CULL_BACK;
	pd3dDevice->CreateRasterizerState(&d3dxRasterizer, &m_pd3dRasterizerState);
}

CWaveMesh::~CWaveMesh()
{
}
void CWaveMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	//메쉬의 정점은 여러 개의 정점 버퍼로 표현된다.
	pd3dDeviceContext->IASetVertexBuffers(m_nSlot, m_nBuffers, m_ppd3dVertexBuffers, m_pnVertexStrides, m_pnVertexOffsets);
	pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, m_dxgiIndexFormat, m_nIndexOffset);
	pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);


	if (GetAsyncKeyState('2') & 0x8000)
		pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);

	if (m_pd3dIndexBuffer)
		pd3dDeviceContext->DrawIndexed(m_nIndices, m_nStartIndex, m_nBaseVertex);
	else
		pd3dDeviceContext->Draw(m_nVertices, m_nStartVertex);
	pd3dDeviceContext->RSSetState(NULL);
}





