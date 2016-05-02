#include "MonsterShader.h"
#include "GameFramework.h"

CMonsterShader ::CMonsterShader ()
{

}
CMonsterShader ::~CMonsterShader ()
{
}


void  CMonsterShader ::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 3, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 4, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 5, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE", 0, DXGI_FORMAT_R32G32B32A32_UINT, 6, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		/*{ "INSTANCEPOS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 7, D3D11_APPEND_ALIGNED_ELEMENT,  D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEPOS", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 7, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEPOS", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 7, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEPOS", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 7, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "STATE", 0,                                       DXGI_FORMAT_R32_UINT, 7, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }*/
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);

	//CreateVertexShaderFromCompiledFile(pd3dDevice, L"Human_VS.fxo", d3dInputElements, nElements);
	//CreatePixelShaderFromCompiledFile(pd3dDevice, L"Human_PS.fxo");
	


	CreateVertexShaderFromFile(pd3dDevice, L"Monster.fx", "VS", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Monster.fx", "PS", "ps_5_0", &m_pd3dPixelShader);

}

ID3D11Buffer *CMonsterShader::CreateInstanceBuffer(ID3D11Device *pd3dDevice, int nObjects, UINT nBufferStride, void *pBufferData)
{
	ID3D11Buffer *pd3dInstanceBuffer = NULL;
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	/*������ �ʱ�ȭ �����Ͱ� ������ ���� ���۷� �����Ѵ�. ��, ���߿� ������ �Ͽ� ������ ä��ų� �����Ѵ�.*/
	d3dBufferDesc.Usage = (pBufferData) ? D3D11_USAGE_DEFAULT : D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = nBufferStride * nObjects;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = (pBufferData) ? 0 : D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pBufferData;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, (pBufferData) ? &d3dBufferData : NULL, &pd3dInstanceBuffer);
	return(pd3dInstanceBuffer);
}

void   CMonsterShader ::BuildObjects(ID3D11Device *pd3dDevice)
{


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

	//�ؽ��� ���ҽ��� �����Ѵ�.
	ID3D11ShaderResourceView *pd3dsrvTexture = NULL; //Stone Brick
	m_pTexture = new CTexture(4, 1, 0, 0);

	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/MovingObject/ForestGolem-Green.dds"), NULL, NULL, &pd3dsrvTexture, NULL);
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




	string strFileName1 = "Data/Golem_Vertex.txt";
	string strFileName2 = "Data/NewFantaLeaves02_Vertex.txt";
	// Stone mesh
	string strFileName3 = "Data/NewRockFlat01_Vertex.txt";
	string strFileName4 = "Data/NewFlatRock02_Vertex.txt";
	string strFileName5 = "Data/NewRockFlat03_Vertex.txt";
	string strFileName6 = "Data/NewRockTall01_Vertex.txt";

	string strFileName7 = "Data/NewRockTall02_Vertex.txt";
	// �ּ� x : -53.8551 z: -51.4314
	// �ִ� x : 53.8551   z:  -51.4314
	//CCharacterMesh* pGolemMesh = new CCharacterMesh(pd3dDevice, strFileName1);

	//CFixedMesh* pLeavesMesh = new CFixedMesh(pd3dDevice, strFileName2);
	//CFixedMesh * pFlatRockMesh01 = new CFixedMesh(pd3dDevice, strFileName3);
	//CFixedMesh * pFlatRockMesh02 = new CFixedMesh(pd3dDevice, strFileName4);
	//CFixedMesh * pFlatRockMesh03 = new CFixedMesh(pd3dDevice, strFileName5);
	//CFixedMesh * pTallRockMesh01 = new CFixedMesh(pd3dDevice, strFileName6);
	//CFixedMesh * pCliff01Mesh = new CFixedMesh(pd3dDevice, strFileName7);



	int  i = 0;
	//m_nObjects = 200;
	m_nObjects = GOLEM_NUM - 4;
	m_ppObjects = new CGameObject*[m_nObjects];
	CMonsterObject *pMonsterObject1 = new CMonsterObject(pd3dDevice, strFileName1);
	CMonsterObject *pMonsterObject2 = new CMonsterObject(pd3dDevice, strFileName1);
	CMonsterObject *pMonsterObject3 = new CMonsterObject(pd3dDevice, strFileName1);
	CMonsterObject *pMonsterObject4 = new CMonsterObject(pd3dDevice, strFileName1);
	CMonsterObject *pMonsterObject5 = new CMonsterObject(pd3dDevice, strFileName1);
	m_ppObjects[0] = pMonsterObject1;
	//m_ppObjects[1] = pMonsterObject2;
	//m_ppObjects[2] = pMonsterObject3;
	//m_ppObjects[3] = pMonsterObject4;
	//m_ppObjects[4] = pMonsterObject5;

	CGameManager* pGameManager = CGameManager::GetCGameManager();

	for (int i = 0; i < m_nObjects; i++)
	{
		m_ppObjects[i]->SetPosition(400.0f, 266.0f, 300 * (i + 1));
		m_ppObjects[i]->Rotate(0.0f, 45.0f, 0.0f);
		m_ppObjects[i]->Scale(D3DXVECTOR3(0.6, 0.6, 0.6));
		pGameManager->m_ppMonster[i] = m_ppObjects[i];
	}
	


	


	//  Golem
	//for (int z = 0; z < 1; z++)
	//{
	//	for (int x = 0; x < GOLEM_NUM; x++)
	//	{
	//		pMonsterObject = new  CMonsterObject(pd3dDevice, strFileName1);
	//		pMonsterObject->SetMesh(pGolemMesh);
	//		pMonsterObject->Rotate(0, 180.0f, 0);
	//	//	pMonsterObject->Scale(D3DXVECTOR3(0.03, 0.03, 0.03));
	//		pMonsterObject->SetPosition(x * 600 + 650, 325.0f, (z * 300) + 350);

	//		pMonsterObject->SetAnimationState(x%4);
	//		m_ppObjects[i++] = pMonsterObject;
	//	
	//	}
	//}



	// Leaves
	//for (int z = 0; z < 9; z++)
	//{
	//	for (int x = 0; x < 4; x++)
	//	{
	//		pLeavesObject = new  CLeavesObject(pd3dDevice, strFileName2);
	//		pLeavesObject->SetMesh(pLeavesMesh);
	//		pLeavesObject->Scale(D3DXVECTOR3(0.03, 0.03, 0.03));
	//		if (x == 0) //���� ó��
	//			pLeavesObject->SetPosition(x * 600 + 200, 425.0f + 0, 10.0f + (z * 400) + 450);
	//		else
	//			pLeavesObject->SetPosition(x * 600 + 650, 425.0f + 0, 10.0f + (z * 400) + 350);
	//		m_ppObjects[i++] = pLeavesObject;
	//	}
	//}
	//m_pd3dLeavesInstanceBuffer = CreateInstanceBuffer(pd3dDevice, TREE_NUM / 2, m_nInstanceBufferStride, NULL);
	//pLeavesMesh->AssembleToVertexBuffer(1, &m_pd3dLeavesInstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);


	
	//// Create FlatStone02
	//for (int x = 0; x < 1; x++)
	//{
	//	for (int z = 0; z < FLAT_ROCK02_NUM; z++)
	//	{
	//		pWoodObject = new  CWoodObject(pd3dDevice, strFileName1);
	//		pWoodObject->SetMesh(pFlatRockMesh02);
	//		float fAngle = rand() % 60;
	//		pWoodObject->Scale(D3DXVECTOR3(5.0, 5.0, 5.0));
	//		pWoodObject->Rotate(&D3DXVECTOR3(0.0f, 1.0f, 0.0f), (float)D3DXToDegree(fAngle));
	//		if (z >= 8)
	//			pWoodObject->SetPosition(1050 + 900, 270.0f, 1483 + (130)* (z - 8));
	//		else
	//			pWoodObject->SetPosition(670, 270.0f, 1483 + (130)*z);
	//		m_ppObjects[i++] = pWoodObject;
	//	}
	//}
	//m_pd3dFlatStone02InstanceBuffer = CreateInstanceBuffer(pd3dDevice, FLAT_ROCK02_NUM, m_nInstanceBufferStride, NULL);
	//pFlatRockMesh02->AssembleToVertexBuffer(1, &m_pd3dFlatStone02InstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);





	////Create FlatStone03
	//for (int x = 0; x < 1; x++)
	//{
	//	for (int z = 0; z <FLAT_ROCK03_NUM; z++)
	//	{
	//		pWoodObject = new  CWoodObject(pd3dDevice, strFileName1);
	//		pWoodObject->SetMesh(pFlatRockMesh03);
	//		float fAngle = rand() % 60;
	//		pWoodObject->Scale(D3DXVECTOR3(5.0, 5.0, 5.0));
	//		pWoodObject->Rotate(&D3DXVECTOR3(0.0f, 1.0f, 0.0f), (float)D3DXToDegree(fAngle));
	//		if (z >= 8)
	//			pWoodObject->SetPosition(1050 + 900, 270.0f, 1483 + (130)*(z - 8) + 55);
	//		else
	//			pWoodObject->SetPosition(670, 270.0f, 1483 + (130)*z + 55);
	//		m_ppObjects[i++] = pWoodObject;
	//	}
	//}
	//m_pd3dFlatStone03InstanceBuffer = CreateInstanceBuffer(pd3dDevice, FLAT_ROCK03_NUM, m_nInstanceBufferStride, NULL);
	//pFlatRockMesh03->AssembleToVertexBuffer(1, &m_pd3dFlatStone03InstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);




	//for (int x = 0; x < 1; x++)
	//{
	//	for (int z = 0; z <TALL_STONE01_NUM; z++)
	//	{
	//		pWoodObject = new  CWoodObject(pd3dDevice, strFileName7);
	//		pWoodObject->SetMesh(pCliff01Mesh);
	//		pWoodObject->Scale(D3DXVECTOR3(10.0, 10.0, 10.0));
	//		//pWoodObject->Rotate(&D3DXVECTOR3(0.0f, 1.0f, 0.0f), (float)D3DXToDegree(fAngle));

	//		pWoodObject->SetPosition(900 * (z + 1), 240.0f, 1050);
	//		/*
	//		if (z % 2 == 0)
	//		pWoodObject->SetPosition(700, 220.0f, 1050);
	//		else
	//		{
	//		pWoodObject->Rotate(&D3DXVECTOR3(0.0f, 1.0f, 0.0f), 180.0);
	//		pWoodObject->SetPosition(701, 220.0f, 972);
	//		}*/




	//		m_ppObjects[i++] = pWoodObject;
	//	}
	//}
	//m_pd3dCliff01InstanceBuffer = CreateInstanceBuffer(pd3dDevice, TALL_STONE01_NUM, m_nInstanceBufferStride, NULL);
	//pCliff01Mesh->AssembleToVertexBuffer(1, &m_pd3dCliff01InstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);

}



void CMonsterShader ::Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera)
{
	OnPrepareRender(pd3dDeviceContext);
	if (m_pMaterial)
		CIlluminatedShader::UpdateShaderVariable(pd3dDeviceContext, &m_pMaterial->m_Material);

	// golem Texture
	if (m_pTexture)
	{
		m_pTexture->UpdateSamplerShaderVariable(pd3dDeviceContext, 0, 0);
		m_pTexture->UpdateTextureShaderVariable(pd3dDeviceContext, 0, 0);
	}

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			if (m_ppObjects[j]->IsVisible(pCamera))
			{
				m_ppObjects[j]->Render(pd3dDeviceContext, pCamera);
			}
		}
	}

}