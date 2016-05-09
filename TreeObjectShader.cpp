#include "TreeObjectShader.h"


CTreeObjectShader::CTreeObjectShader()
{
	m_uiWoodNum = m_uiLeavesNum = m_uiTreeNum = m_uiTallStoneNum = 0;
}
CTreeObjectShader::~CTreeObjectShader()
{
}


void  CTreeObjectShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "INSTANCEPOS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 3, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEPOS", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 3, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEPOS", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 3, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEPOS", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 3, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);

	//CreateVertexShaderFromCompiledFile(pd3dDevice, L"Human_VS.fxo", d3dInputElements, nElements);
	//CreatePixelShaderFromCompiledFile(pd3dDevice, L"Human_PS.fxo");



	CreateVertexShaderFromFile(pd3dDevice, L"Tree.fx", "VSInstancedTexturedLightingColor", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Tree.fx", "PSInstancedTexturedLightingColor", "ps_5_0", &m_pd3dPixelShader);

}
void   CTreeObjectShader::BuildObjects(ID3D11Device *pd3dDevice)
{


	CMaterial *pWhiteMaterial = new CMaterial();
	pWhiteMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pWhiteMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 4.0f);
	pWhiteMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 4.0f);
	pWhiteMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
	m_pMaterial = pWhiteMaterial;

	ID3D11SamplerState *pd3dSamplerState = NULL;
	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dSamplerState);

	//텍스쳐 리소스를 생성한다.
	ID3D11ShaderResourceView *pd3dsrvTexture = NULL; //Stone Brick
	m_pTexture = new CTexture(4, 1, 0, 0);

	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/FixObject/Tree 3-4 Diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pTexture->SetTexture(0, pd3dsrvTexture);
	pd3dsrvTexture->Release();
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/FixObject/LeavesDiffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pTexture->SetTexture(1, pd3dsrvTexture);
	pd3dsrvTexture->Release();
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/FixObject/Rock-Multiple-01.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pTexture->SetTexture(2, pd3dsrvTexture);
	pd3dsrvTexture->Release();
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/FixObject/Cliff-Diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pTexture->SetTexture(3, pd3dsrvTexture);
	pd3dsrvTexture->Release();



	m_pTexture->SetSampler(0, pd3dSamplerState);
	pd3dSamplerState->Release();


	//m_pLeavesTexture = new CTexture(1, 1, 0, 0);
	//D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/LeavesDiffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	//m_pLeavesTexture->SetTexture(0, pd3dsrvTexture);
	//m_pLeavesTexture->SetSampler(0, pd3dSamplerState);
	//pd3dsrvTexture->Release();

	//pd3dsrvTexture->Release();



	//에러의 원인
	m_nInstanceBufferStride = sizeof(VS_VB_INSTANCE);
	m_nInstanceBufferOffset = 0;

	string strFileName1 = "Data/NewTree02_Vertex.txt";
	string strFileName2 = "Data/NewFantaLeaves02_Vertex.txt";
	// Stone mesh
	string strFileName3 = "Data/NewRockFlat01_Vertex.txt";
	string strFileName4 = "Data/NewFlatRock02_Vertex.txt";
	string strFileName5 = "Data/NewRockFlat03_Vertex.txt";
	string strFileName6 = "Data/NewRockTall01_Vertex.txt";

	string strFileName7 = "Data/NewRockTall02_Vertex.txt";





	// 최소 x : -53.8551 z: -51.4314
	// 최대 x : 53.8551   z:  -51.4314
	CFixedMesh* pWoodMesh = new CFixedMesh(pd3dDevice, strFileName1);
	CFixedMesh* pLeavesMesh = new CFixedMesh(pd3dDevice, strFileName2);

	CFixedMesh * pFlatRockMesh01 = new CFixedMesh(pd3dDevice, strFileName3);
	CFixedMesh * pFlatRockMesh02 = new CFixedMesh(pd3dDevice, strFileName4);
	CFixedMesh * pFlatRockMesh03 = new CFixedMesh(pd3dDevice, strFileName5);
	CFixedMesh * pTallRockMesh01 = new CFixedMesh(pd3dDevice, strFileName6);

	CFixedMesh * pCliff01Mesh = new CFixedMesh(pd3dDevice, strFileName7);



	int  i = 0;
	//m_nObjects = 200;
	m_nObjects = TREE_NUM + FLAT_ROCK02_NUM + FLAT_ROCK03_NUM + TALL_STONE01_NUM;
	m_ppObjects = new CGameObject*[m_nObjects];
	CWoodObject *pWoodObject = NULL;
	CLeavesObject *pLeavesObject = NULL;


	CGameManager* pGameManager = CGameManager::GetCGameManager();

	FILE* fp;
	//Wood
	fopen_s(&fp, "Data/ObjectPosition/WoodData.txt", "rt");
	for (int k = 0; k < TREE_NUM / 2; k++)
	{
		pWoodObject = new  CWoodObject(pd3dDevice, strFileName1);
		pWoodObject->CreateBoundingBox(53.855099, 51.431400);
		pWoodObject->SetMesh(pWoodMesh);
		pWoodObject->Scale(D3DXVECTOR3(0.03, 0.03, 0.03));
		float fPosition[3];
		fscanf_s(fp, "%f %f %f  \n", &fPosition[0], &fPosition[1], &fPosition[2]);
		pWoodObject->SetPosition(fPosition[0], fPosition[1], fPosition[2]);
		m_ppObjects[i] = pWoodObject;
		pGameManager->m_ppStaticObject[i] = m_ppObjects[i++];

	}
	fclose(fp);



	fopen_s(&fp, "tree0.txt", "w");
	fprintf(fp, "%d \n", TREE_NUM / 2);
	for (int j = 0; j < TREE_NUM / 2; j++)
	{
		fprintf(fp, "%lf %lf %lf %lf %lf \n", m_ppObjects[j]->GetPosition().x, m_ppObjects[j]->GetPosition().y, m_ppObjects[j]->GetPosition().z, 53.8551f, 51.4314f);
	}
	fclose(fp);




	m_pd3dWoodInstanceBuffer = CreateInstanceBuffer(pd3dDevice, TREE_NUM / 2, m_nInstanceBufferStride, NULL);
	pWoodMesh->AssembleToVertexBuffer(1, &m_pd3dWoodInstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);

	// Leaves
	//FILE* fp;
	fopen_s(&fp, "Data/ObjectPosition/LeavesData.txt", "rt");


	for (int k = 0; k < TREE_NUM / 2; k++)
	{
		pLeavesObject = new  CLeavesObject(pd3dDevice, strFileName2);
		pLeavesObject->SetMesh(pLeavesMesh);
		pLeavesObject->Scale(D3DXVECTOR3(0.03, 0.03, 0.03));
		float fPosition[3];
		fscanf_s(fp, "%f %f %f  \n", &fPosition[0], &fPosition[1], &fPosition[2]);
		pLeavesObject->SetPosition(fPosition[0], 390.0f, fPosition[2] + 10.0f);
		m_ppObjects[i++] = pLeavesObject;
	}
	fclose(fp);


	m_pd3dLeavesInstanceBuffer = CreateInstanceBuffer(pd3dDevice, TREE_NUM / 2, m_nInstanceBufferStride, NULL);
	pLeavesMesh->AssembleToVertexBuffer(1, &m_pd3dLeavesInstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);


	// Create FlatStone02
	for (int x = 0; x < 1; x++)
	{
		for (int z = 0; z < FLAT_ROCK02_NUM; z++)
		{
			pWoodObject = new  CWoodObject(pd3dDevice, strFileName1);
			pWoodObject->SetMesh(pFlatRockMesh02);
			float fAngle = rand() % 60;
			pWoodObject->Scale(D3DXVECTOR3(5.0, 5.0, 5.0));
			pWoodObject->Rotate(&D3DXVECTOR3(0.0f, 1.0f, 0.0f), (float)D3DXToDegree(fAngle));
			if (z >= 8)
				pWoodObject->SetPosition(1050 + 900, 270.0f, 1483 + (130)* (z - 8));
			else
				pWoodObject->SetPosition(670, 270.0f, 1483 + (130)*z);
			m_ppObjects[i++] = pWoodObject;
		}
	}
	m_pd3dFlatStone02InstanceBuffer = CreateInstanceBuffer(pd3dDevice, FLAT_ROCK02_NUM, m_nInstanceBufferStride, NULL);
	pFlatRockMesh02->AssembleToVertexBuffer(1, &m_pd3dFlatStone02InstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);





	//Create FlatStone03
	for (int x = 0; x < 1; x++)
	{
		for (int z = 0; z <FLAT_ROCK03_NUM; z++)
		{
			pWoodObject = new  CWoodObject(pd3dDevice, strFileName1);
			pWoodObject->SetMesh(pFlatRockMesh03);
			float fAngle = rand() % 60;
			pWoodObject->Scale(D3DXVECTOR3(5.0, 5.0, 5.0));
			pWoodObject->Rotate(&D3DXVECTOR3(0.0f, 1.0f, 0.0f), (float)D3DXToDegree(fAngle));
			if (z >= 8)
				pWoodObject->SetPosition(1050 + 900, 270.0f, 1483 + (130)*(z - 8) + 55);
			else
				pWoodObject->SetPosition(670, 270.0f, 1483 + (130)*z + 55);
			m_ppObjects[i++] = pWoodObject;
		}
	}




	m_pd3dFlatStone03InstanceBuffer = CreateInstanceBuffer(pd3dDevice, FLAT_ROCK03_NUM, m_nInstanceBufferStride, NULL);
	pFlatRockMesh03->AssembleToVertexBuffer(1, &m_pd3dFlatStone03InstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);


	//for (int x = 0; x < 1; x++)
	//{
	//	for (int z = 0; z <TALL_STONE01_NUM; z++)
	//	{
	//		pWoodObject = new  CWoodObject(pd3dDevice, strFileName7);
	//		pWoodObject->SetMesh(pCliff01Mesh);
	//		pWoodObject->Scale(D3DXVECTOR3(10.0, 10.0, 10.0));
	//		//pWoodObject->Rotate(&D3DXVECTOR3(0.0f, 1.0f, 0.0f), (float)D3DXToDegree(fAngle));
	//
	//		pWoodObject->SetPosition(200 * (z + 1), 240.0f, 1050);
	//		
	//		m_ppObjects[i++] = pWoodObject;
	//	}
	//}


	//FlatRock03
	fopen_s(&fp, "Data/ObjectPosition/FlatRock03Data.txt", "rt");
	for (int k = 0; k < TALL_STONE01_NUM; k++)
	{
		pWoodObject = new   CWoodObject(pd3dDevice, strFileName2);
		pWoodObject->SetMesh(pCliff01Mesh);
		pWoodObject->Scale(D3DXVECTOR3(20.0, 20.0, 20.0));
		float fAngle = rand() % 60;
		pWoodObject->Rotate(&D3DXVECTOR3(0.0f, 1.0f, 0.0f), (float)D3DXToDegree(fAngle));
		float fPosition[3];
		fscanf_s(fp, "%f %f %f  \n", &fPosition[0], &fPosition[1], &fPosition[2]);
		pWoodObject->SetPosition(fPosition[0], fPosition[1], fPosition[2]);
		m_ppObjects[i++] = pWoodObject;
	}
	fclose(fp);
	m_pd3dCliff01InstanceBuffer = CreateInstanceBuffer(pd3dDevice, TALL_STONE01_NUM, m_nInstanceBufferStride, NULL);
	pCliff01Mesh->AssembleToVertexBuffer(1, &m_pd3dCliff01InstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);




}



void CTreeObjectShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera)
{
	OnPrepareRender(pd3dDeviceContext);
	if (m_pMaterial)
		CIlluminatedShader::UpdateShaderVariable(pd3dDeviceContext, &m_pMaterial->m_Material);

	if (m_pTexture)
	{
		m_pTexture->UpdateSamplerShaderVariable(pd3dDeviceContext, 0, 0);
		m_pTexture->UpdateTextureShaderVariable(pd3dDeviceContext, 0, 0);
	}
	//m_pWoodTexture->UpdateShaderVariable(pd3dDeviceContext);

	int nWoodObjects = TREE_NUM / 2;

	int nWoodInstances = 0;
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dWoodInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_VB_INSTANCE *pnSphereInstances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
	for (int j = 0; j < TREE_NUM / 2; j++)
	{
		if (m_ppObjects[j]->IsVisible(pCamera))
		{
			if (m_ppObjects[j]->IsVisible(pCamera))
			{
				D3DXMatrixTranspose(&pnSphereInstances[nWoodInstances++].m_d3dxTransform, &m_ppObjects[j]->m_d3dxmtxWorld);
			}
		}
	}
	pd3dDeviceContext->Unmap(m_pd3dWoodInstanceBuffer, 0);

	CMesh *pWoodMesh = m_ppObjects[0]->GetMesh();
	pWoodMesh->RenderInstanced(pd3dDeviceContext, nWoodInstances, 0);


	//if (m_pLeavesTexture)  m_pLeavesTexture->UpdateShaderVariable(pd3dDeviceContext);
	if (m_pTexture)
	{
		m_pTexture->UpdateTextureShaderVariable(pd3dDeviceContext, 1, 0);
		//m_pWoodTexture->UpdateTextureShaderVariable(pd3dDeviceContext, 0, 0);
	}

	int nLeavesInstances = 0;
	pd3dDeviceContext->Map(m_pd3dLeavesInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_VB_INSTANCE *pLeavesInstances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
	for (int j = nWoodObjects; j < TREE_NUM; j++)
	{
		if (m_ppObjects[j])
		{
			if (m_ppObjects[j]->IsVisible(pCamera))
			{
				D3DXMatrixTranspose(&pLeavesInstances[nLeavesInstances++].m_d3dxTransform, &m_ppObjects[j]->m_d3dxmtxWorld);

			}
		}
	}
	pd3dDeviceContext->Unmap(m_pd3dLeavesInstanceBuffer, 0);

	CMesh *pLeavsMesh = m_ppObjects[TREE_NUM - 1]->GetMesh();
	pLeavsMesh->RenderInstanced(pd3dDeviceContext, nLeavesInstances, 0);


	// Render FlatRock02
	if (m_pTexture)
	{
		m_pTexture->UpdateTextureShaderVariable(pd3dDeviceContext, 2, 0);
		//m_pWoodTexture->UpdateTextureShaderVariable(pd3dDeviceContext, 0, 0);
	}
	int nFlatRock02Instances = 0;
	pd3dDeviceContext->Map(m_pd3dFlatStone02InstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_VB_INSTANCE *pFlatRock02Instances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
	for (int j = TREE_NUM; j < TREE_NUM + FLAT_ROCK02_NUM; j++)
	{
		if (m_ppObjects[j])
		{
			if (m_ppObjects[j]->IsVisible(pCamera))
				D3DXMatrixTranspose(&pFlatRock02Instances[nFlatRock02Instances++].m_d3dxTransform, &m_ppObjects[j]->m_d3dxmtxWorld);
		}
	}
	pd3dDeviceContext->Unmap(m_pd3dFlatStone02InstanceBuffer, 0);

	CMesh *pFlatRockMesh02 = m_ppObjects[TREE_NUM + FLAT_ROCK02_NUM - 1]->GetMesh();
	pFlatRockMesh02->RenderInstanced(pd3dDeviceContext, nFlatRock02Instances, 0);



	// Render FlatRock03
	int nFlatRock03Instances = 0;
	pd3dDeviceContext->Map(m_pd3dFlatStone03InstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_VB_INSTANCE *pFlatRock03Instances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
	for (int j = TREE_NUM + FLAT_ROCK02_NUM; j < TREE_NUM + FLAT_ROCK02_NUM + FLAT_ROCK03_NUM; j++)
	{
		if (m_ppObjects[j])
		{
			if (m_ppObjects[j]->IsVisible(pCamera))
				D3DXMatrixTranspose(&pFlatRock03Instances[nFlatRock03Instances++].m_d3dxTransform, &m_ppObjects[j]->m_d3dxmtxWorld);
		}
	}
	pd3dDeviceContext->Unmap(m_pd3dFlatStone03InstanceBuffer, 0);

	CMesh *pFlatRockMesh03 = m_ppObjects[TREE_NUM + FLAT_ROCK02_NUM + FLAT_ROCK03_NUM - 1]->GetMesh();
	pFlatRockMesh03->RenderInstanced(pd3dDeviceContext, nFlatRock03Instances, 0);


	//Render Cliff


	int nCliff01Instances = 0;
	pd3dDeviceContext->Map(m_pd3dCliff01InstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_VB_INSTANCE *pCliff01Instances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
	for (int j = TREE_NUM + FLAT_ROCK02_NUM + FLAT_ROCK03_NUM; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			if (m_ppObjects[j]->IsVisible(pCamera))
				D3DXMatrixTranspose(&pCliff01Instances[nCliff01Instances++].m_d3dxTransform, &m_ppObjects[j]->m_d3dxmtxWorld);
		}
	}
	pd3dDeviceContext->Unmap(m_pd3dCliff01InstanceBuffer, 0);

	CMesh *pCliff01Mesh = m_ppObjects[m_nObjects - 1]->GetMesh();
	pCliff01Mesh->RenderInstanced(pd3dDeviceContext, nCliff01Instances, 0);

}