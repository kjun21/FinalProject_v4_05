#include "FixedMesh.h"


CFixedMesh::CFixedMesh(ID3D11Device *pd3dDevice, string strFileName) : CMeshTexturedIlluminated(pd3dDevice)
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


	::fread(m_pd3dxvPositions, sizeof(D3DXVECTOR3), m_nVertices, pFile);
	//::fread(&m_nIndices, sizeof(UINT), 1, pFile);

//	m_pnIndices = new UINT[m_nIndices];
	//::fread(m_pnIndices, sizeof(UINT), m_nIndices, pFile);
	::fread(pd3dxvNormals, sizeof(D3DXVECTOR3), m_nVertices, pFile);
	::fread(pd3dxvTexCoords, sizeof(D3DXVECTOR2), m_nVertices, pFile);
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




	UINT BufferCount = 3; //5
	ID3D11Buffer *pd3dBuffers[7] = {
		m_pd3dPositionBuffer,
		m_pd3dNormalBuffer,
		m_pd3dTexCoordBuffer,
	
	};
	UINT pnBufferStrides[3] = {
		sizeof(D3DXVECTOR3),
		sizeof(D3DXVECTOR3),
		sizeof(D3DXVECTOR2),
		
	};
	UINT pnBufferOffsets[3] = { 0, 0, 0 }; // 5
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


CFixedMesh::~CFixedMesh()
{
}
