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
	/*버퍼의 초기화 데이터가 없으면 동적 버퍼로 생성한다. 즉, 나중에 매핑을 하여 내용을 채우거나 변경한다.*/
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

	//텍스쳐 리소스를 생성한다.
	ID3D11ShaderResourceView *pd3dsrvTexture = NULL; //Stone Brick
	m_pTexture = new CTexture(1, 1, 0, 0);

	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/MovingObject/ForestGolem-Green.dds"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pTexture->SetTexture(0, pd3dsrvTexture);
	pd3dsrvTexture->Release();

	m_pTexture->SetSampler(0, pd3dSamplerState);
	pd3dSamplerState->Release();


	string strFileName1 = "Data/Golem_Vertex.txt";
	

	m_nObjects = GOLEM_NUM;
	m_ppObjects = new CGameObject*[m_nObjects];

	//  Golem
	CCharacterMesh* pGolemMesh = new CCharacterMesh(pd3dDevice, strFileName1);


	CGameManager* pGameManager = CGameManager::GetCGameManager();
	CGolemObject* pGolemObject = NULL;


	FILE* fp;
	fopen_s(&fp, "Data/ObjectPosition/GolemData.txt", "rt");
	int i = 0;

	for (int k = 0; k < 10; k++)
	{
		pGolemObject = new  	CGolemObject(pd3dDevice, strFileName1);
		pGolemObject->SetMesh(pGolemMesh);
		float fPosition[3];
		fscanf_s(fp, "%f %f %f  \n", &fPosition[0], &fPosition[1], &fPosition[2]);
		pGolemObject->Scale(D3DXVECTOR3(0.6f, 0.6f, 0.6f));
		pGolemObject->SetPosition(fPosition[0], fPosition[1], fPosition[2]);
		m_ppObjects[i] = pGolemObject;
		//m_ppObjects[i]->Scale(D3DXVECTOR3(0.6f, 0.6f, 0.6f));
		pGameManager->m_ppMonster[i] = m_ppObjects[i];
		i++;
	}
	fclose(fp);




	//FILE* fp;
	fopen_s(&fp, "GolemData.txt", "w");
	fprintf(fp, "%d \n", GOLEM_NUM);
	for (int j = 0; j < GOLEM_NUM; j++)
	{
		fprintf(fp, "%d %lf %lf %lf %lf %lf %lf \n", 2, m_ppObjects[j]->GetPosition().x, m_ppObjects[j]->GetPosition().y, m_ppObjects[j]->GetPosition().z,
			m_ppObjects[j]->GetLookAt().x, m_ppObjects[j]->GetLookAt().y, m_ppObjects[j]->GetLookAt().z);
	}
	fclose(fp);
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
void CMonsterShader::AnimateObjects(float fTimeElapsed)
{
	//서버
	ClientServer *s = ClientServer::getInstangce();
	for (auto i = 0; i < m_nObjects; ++i)
	{
		m_ppObjects[i]->SetAnimationState(s->monsterList[i].state);
	
		m_ppObjects[i]->SetPosition(s->monsterList[i].monsterPos);
		m_ppObjects[i]->SetDirection(s->monsterList[i].monsterDir);
		m_ppObjects[i]->RenewWorldMatrix();
		m_ppObjects[i]->Scale(D3DXVECTOR3(0.6f, 0.6f, 0.6f));
		//std::cout << i<<" "<<m_ppObjects[i]->GetPosition().x << " "<< m_ppObjects[i]->GetPosition().z << std::endl;
	}
	////for (auto i = 5; i < m_nObjects; ++i)
	////{
	////	m_ppObjects[i]->moveforward(fTimeElapsed*10.0f);
	////}
	CShader::AnimateObjects(fTimeElapsed);
}


CFakeMonsterShader::CFakeMonsterShader()
{

}
CFakeMonsterShader  ::~CFakeMonsterShader()
{
}

void CFakeMonsterShader::BuildObjects(ID3D11Device *pd3dDevice)
{

	m_pMaterial = new CMaterial();
	m_pMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f); //0.2f, 0.1f, 1.0f, 1.0f);
	m_pMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
	m_pMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(0.8f, 0.8f, 0.8f, 128.0f);
	m_pMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);



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

	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/MovingObject/Goblin.jpg"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pTexture->SetTexture(0, pd3dsrvTexture);
	pd3dsrvTexture->Release();
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/MovingObject/GreenMan.jpg"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pTexture->SetTexture(1, pd3dsrvTexture);
	pd3dsrvTexture->Release();
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/MovingObject/Slime.jpg"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pTexture->SetTexture(2, pd3dsrvTexture);
	pd3dsrvTexture->Release();
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/MovingObject/Fairy.jpg"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pTexture->SetTexture(3, pd3dsrvTexture);
	pd3dsrvTexture->Release();


	m_pTexture->SetSampler(0, pd3dSamplerState);
	pd3dSamplerState->Release();


	m_nObjects = 8;
	m_ppObjects = new CGameObject*[m_nObjects];

	string strFileName1 = "Data/Gob_Vertex.txt";
	string strFileName2 = "Data/Greenman_Vertex.txt";
	string strFileName3 = "Data/Slime.txt";
	string strFileName4 = "Data/F_Vertex.txt";
	


	CCharacterMesh* pGoblinMesh = new CCharacterMesh(pd3dDevice, strFileName1);
	CCharacterMesh* pGreenmanMesh = new CCharacterMesh(pd3dDevice, strFileName2);
	CCharacterMesh* pSlimeMesh = new CCharacterMesh(pd3dDevice, strFileName3);
	CCharacterMesh* pFairyMesh = new CCharacterMesh(pd3dDevice, strFileName4);


	CGoblinObject* pGoblinObject1 = new CGoblinObject(pd3dDevice, strFileName1);
	CGoblinObject* pGoblinObject2 = new CGoblinObject(pd3dDevice, strFileName1);

	CGreenManObject* pGreenManObject1 = new CGreenManObject(pd3dDevice, strFileName2);
	CGreenManObject* pGreenManObject2 = new CGreenManObject(pd3dDevice, strFileName2);

	CSlimeObject* pSlimeObject1 = new CSlimeObject(pd3dDevice, strFileName3);
	CSlimeObject* pSlimeObject2 = new CSlimeObject(pd3dDevice, strFileName3);

	CFairyObject* pFairyObject1 = new 	CFairyObject(pd3dDevice, strFileName4);
	CFairyObject* pFairyObject2 = new 	CFairyObject(pd3dDevice, strFileName4);


	pGoblinObject1->SetMesh(pGoblinMesh);
	pGoblinObject2->SetMesh(pGoblinMesh);
	pGoblinObject1->SetAnimationState(ANIMATAION_CLIP_MONSTER_IDLE);
	pGoblinObject2->SetAnimationState(ANIMATAION_CLIP_MONSTER_ATTACK);

	pGreenManObject1->SetMesh(pGreenmanMesh);
	pGreenManObject2->SetMesh(pGreenmanMesh);
	pGreenManObject1->SetAnimationState(ANIMATAION_CLIP_MONSTER_RUN);
	pGreenManObject2->SetAnimationState(ANIMATAION_CLIP_MONSTER_ATTACK);

	pSlimeObject1->SetMesh(pSlimeMesh);
	pSlimeObject2->SetMesh(pSlimeMesh);
	pSlimeObject1->SetAnimationState(ANIMATAION_CLIP_MONSTER_RUN);
	pSlimeObject2->SetAnimationState(ANIMATAION_CLIP_MONSTER_ATTACK);

	pFairyObject1->SetMesh(pFairyMesh);
	pFairyObject2->SetMesh(pFairyMesh);
	pFairyObject1->SetAnimationState(ANIMATAION_CLIP_MONSTER_RUN);
	pFairyObject2->SetAnimationState(ANIMATAION_CLIP_MONSTER_ATTACK);


	m_ppObjects[0] = pGoblinObject1;
	m_ppObjects[1] = pGoblinObject2;
	m_ppObjects[2] = pGreenManObject1;
	m_ppObjects[3] = pGreenManObject2;
	m_ppObjects[4] = pSlimeObject1;
	m_ppObjects[5] = pSlimeObject2;
	m_ppObjects[6] = pFairyObject1;
	m_ppObjects[7] = pFairyObject2;



	for (int i = 0; i < m_nObjects; i++)
		m_ppObjects[i]->Scale(D3DXVECTOR3(3.0f, 3.0f, 3.0f));





	m_ppObjects[0]->SetPosition(100.0f, 266.0f, 900.0f);
	m_ppObjects[1]->SetPosition(300.0f, 266.0f, 900.0f);
	m_ppObjects[2]->SetPosition(100.0f, 266.0f, 700.0f);
	m_ppObjects[3]->SetPosition(300.0f, 266.0f, 700.0f);
	m_ppObjects[4]->SetPosition(300.0f, 266.0f, 500.0f);
	m_ppObjects[5]->SetPosition(400.0f, 266.0f, 500.0f);
	m_ppObjects[6]->SetPosition(300.0f, 266.0f, 1200.0f);
	m_ppObjects[7]->SetPosition(400.0f, 266.0f, 1200.0f);




	m_ppObjects[0]->Rotate(0.0f, 180.0f, 0.0f);
	m_ppObjects[2]->Rotate(0.0f, 180.0f, 0.0f);
	m_ppObjects[3]->Rotate(0.0f, 180.0f, 0.0f);
	m_ppObjects[4]->Rotate(0.0f, 180.0f, 0.0f);
	m_ppObjects[5]->Rotate(0.0f, 180.0f, 0.0f);
	m_ppObjects[6]->Rotate(0.0f, 180.0f, 0.0f);
	m_ppObjects[7]->Rotate(0.0f, 180.0f, 0.0f);
	
}

void  CFakeMonsterShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera)
{
	OnPrepareRender(pd3dDeviceContext);
	if (m_pMaterial)
		CIlluminatedShader::UpdateShaderVariable(pd3dDeviceContext, &m_pMaterial->m_Material);

	// golem Texture
	if (m_pTexture)
		m_pTexture->UpdateSamplerShaderVariable(pd3dDeviceContext, 0, 0);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			if (m_pTexture && 0<=j && j<=1)
				m_pTexture->UpdateTextureShaderVariable(pd3dDeviceContext, 0, 0);
			else if (m_pTexture && 2 <= j && j <= 3)
				m_pTexture->UpdateTextureShaderVariable(pd3dDeviceContext, 1, 0);		
			else if (m_pTexture && 4 <= j && j <= 5)
				m_pTexture->UpdateTextureShaderVariable(pd3dDeviceContext, 2, 0);
			else if (m_pTexture && 6 <= j && j <= 7)
				m_pTexture->UpdateTextureShaderVariable(pd3dDeviceContext, 3, 0);

			if (m_ppObjects[j]->IsVisible(pCamera))
			{
				m_ppObjects[j]->Render(pd3dDeviceContext, pCamera);
			}
		}
	}
}

void  CFakeMonsterShader::AnimateObjects(float fTimeElapsed)
{

}