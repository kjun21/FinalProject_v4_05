//-----------------------------------------------------------------------------
// File: CScene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Scene.h"



CScene::CScene()
{
	m_ppShaders = NULL;
	m_nShaders = 0;

	m_pCamera = NULL;
	m_pSelectedObject = NULL;

	m_pLights = NULL;
	m_pd3dcbLights = NULL;
}

CScene::~CScene()
{
}

void CScene::BuildObjects(ID3D11Device *pd3dDevice)
{
	//텍스쳐 맵핑에 사용할 샘플러 상태 객체를 생성한다.
	ID3D11SamplerState *pd3dSamplerState = NULL;
	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dSamplerState);

	//텍스쳐 리소스를 생성한다.
	ID3D11ShaderResourceView *pd3dsrvTexture = NULL; //Stone Brick
	CTexture *pStoneTexture = new CTexture(1, 1, 0, 0);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/111.jpg"), NULL, NULL, &pd3dsrvTexture, NULL);
	pStoneTexture->SetTexture(0, pd3dsrvTexture);
	pStoneTexture->SetSampler(0, pd3dSamplerState);
	pd3dsrvTexture->Release();

	CTexture *pBrickTexture = new CTexture(1, 1, 0, 0);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/222.jpg"), NULL, NULL, &pd3dsrvTexture, NULL);
	pBrickTexture->SetTexture(0, pd3dsrvTexture);
	pBrickTexture->SetSampler(0, pd3dSamplerState);
	pd3dsrvTexture->Release();

	CTexture *pWoodTexture = new CTexture(1, 1, 0, 0);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/Wood01.jpg"), NULL, NULL, &pd3dsrvTexture, NULL);
	pWoodTexture->SetTexture(0, pd3dsrvTexture);
	pWoodTexture->SetSampler(0, pd3dSamplerState);
	pd3dsrvTexture->Release();

	CTexture *p = new CTexture(1, 1, 0, 0);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/stones.dds"), NULL, NULL, &pd3dsrvTexture, NULL);
	p->SetTexture(0, pd3dsrvTexture);
	p->SetSampler(0, pd3dSamplerState);
	pd3dsrvTexture->Release();


	pd3dSamplerState->Release();


	m_nShaders = 3;
	m_ppShaders = new CShader*[m_nShaders];

	//첫 번째로 그릴 객체는 스카이 박스이다.

	m_ppShaders[0] = new CTerrainShader();
	m_ppShaders[0]->CreateShader(pd3dDevice);
	m_ppShaders[0]->BuildObjects(pd3dDevice);


	//Instancing
	CMaterial *pInstancingMaterials[3];
	

	//재질을 생성한다.
	CMaterial *pRedMaterial = new CMaterial();
	pRedMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pRedMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pRedMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 5.0f);
	pRedMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

	CMaterial *pGreenMaterial = new CMaterial();
	pGreenMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
	pGreenMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
	pGreenMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 3.0f);
	pGreenMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

	CMaterial *pBlueMaterial = new CMaterial();
	pBlueMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	pBlueMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	pBlueMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 4.0f);
	pBlueMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);



	CMaterial *pWhiteMaterial = new CMaterial();
	pWhiteMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pWhiteMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pWhiteMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 4.0f);
	pWhiteMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);


	CMaterial*  pWaterMaterial = new CMaterial();
	pWaterMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);  //0,4 0.4 0.7
	pWaterMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.1f, 0.3f, 1.0f);
	pWaterMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(0.8f, 0.8f, 0.8f, 128.0f);
	pWaterMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);




	CTreeObjectShader* pTreeObjectShader = new CTreeObjectShader();
	pTreeObjectShader->CreateShader(pd3dDevice);
	pTreeObjectShader->BuildObjects(pd3dDevice);
	m_ppShaders[1] = pTreeObjectShader;


	CWaveShader* pWaveShader = new CWaveShader();
	pWaveShader->CreateShader(pd3dDevice);
	pWaveShader->BuildObjects(pd3dDevice, pWaterMaterial, p);
	m_ppShaders[2] = pWaveShader;


	//CWizardShader* pHuman = new CWizardShader();
	//pHuman->CreateShader(pd3dDevice);
	//pHuman->BuildObjects(pd3dDevice);
	//m_ppShaders[3] = pHuman;




	//CHumanShader* pHuman = new CHumanShader();
	//pHuman->CreateShader(pd3dDevice);
	//pHuman->BuildObjects(pd3dDevice);
	//m_ppShaders[1] = pHuman;





	// 같은 그리기는 모아서 한꺼번에 처리해야하는데...
	//CTessellationShader* pTessllationShader = new CTessellationShader();
	//pTessllationShader->CreateShader(pd3dDevice);
	//pTessllationShader->BuildObjects(pd3dDevice, pWhiteMaterial, p);
	//m_ppShaders[1] = pTessllationShader;



	//CDisplacementMappingShader* pDisplacementMappingShader = new CDisplacementMappingShader();
	//pDisplacementMappingShader->CreateShader(pd3dDevice);
	//pDisplacementMappingShader->BuildObjects(pd3dDevice, pWhiteMaterial, p);
	//m_ppShaders[2] = pDisplacementMappingShader;



	//CNormalMappingShader* pNormalMappingShader = new CNormalMappingShader();
	//pNormalMappingShader->CreateShader(pd3dDevice);
	//pNormalMappingShader->BuildObjects(pd3dDevice, pWhiteMaterial, p);
	//m_ppShaders[3] = pNormalMappingShader;




	//CTessellationTerrainShader* pTessellationTerrainShader = new CTessellationTerrainShader();
	//pTessellationTerrainShader->CreateShader(pd3dDevice);
	//pTessellationTerrainShader->BuildObjects(pd3dDevice);
	//m_ppShaders[4] = pTessellationTerrainShader;



	//CTextureLightingShader* pTextureLightingShader = new CTextureLightingShader();
	//pTextureLightingShader->CreateShader(pd3dDevice);
	//pTextureLightingShader->BuildObjects(pd3dDevice, pWhiteMaterial, p);
	//m_ppShaders[4] = pTextureLightingShader;



	//
	//CTestTessellationShader* pTestTessellationShader = new CTestTessellationShader();
	//pTestTessellationShader->CreateShader(pd3dDevice);
	//pTestTessellationShader->BuildObjects(pd3dDevice, pWhiteMaterial, p);
	//m_ppShaders[6] = pTestTessellationShader;





	//CRefractionShader* pGrassShader = new CRefractionShader();
	//pGrassShader ->CreateShader(pd3dDevice);
	//pGrassShader->BuildObjects(pd3dDevice);
	//m_ppShaders[6] = pGrassShader;

	//CReflectionShader* pReflectionShader = new CReflectionShader();
	//pReflectionShader->CreateShader(pd3dDevice);
	//pReflectionShader->BuildObjects(pd3dDevice);
	//m_ppShaders[7] = pReflectionShader;



	//CGeneralShader* pGeneralShader = new CGeneralShader();
	//pGeneralShader->CreateShader(pd3dDevice);
	//pGeneralShader->BuildObjects(pd3dDevice);
	//m_ppShaders[6] = pGeneralShader;



	

	
	//CCrushBoxShader* pCrushBoxShaser = new CCrushBoxShader();
	//pCrushBoxShaser->CreateShader(pd3dDevice);
	//pCrushBoxShaser->BuildObjects(pd3dDevice);
	//m_ppShaders[8] = pCrushBoxShaser;

	CreateShaderVariables(pd3dDevice);

}

void CScene::ReleaseObjects()
{
	ReleaseShaderVariables();

	for (int j = 0; j < m_nShaders; j++)
	{
		if (m_ppShaders[j]) m_ppShaders[j]->ReleaseObjects();
		if (m_ppShaders[j]) delete m_ppShaders[j];
	}
	if (m_ppShaders) delete[] m_ppShaders;
}


bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		m_pSelectedObject = PickObjectPointedByCursor(LOWORD(lParam), HIWORD(lParam));
		break;
	}
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::ProcessInput()
{
	return(false);
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	
	ClientServer *s = ClientServer::getInstangce();
	if (m_pLights && m_pd3dcbLights)
	{
		//현재 카메라의 위치 벡터를 조명을 나타내는 상수 버퍼에 설정한다.
		D3DXVECTOR3 d3dxvCameraPosition = m_pCamera->GetPosition();
		m_pLights->m_d3dxvCameraPosition = D3DXVECTOR4(d3dxvCameraPosition, 1.0f);
		//cout << d3dxvCameraPosition.x << "." << d3dxvCameraPosition.z << endl;
		//for (auto i = 0; i < 4; ++i)
		//{
		//	if (i == s->getMyId())
		//	{
		//		D3DXVECTOR3 d3dxvCameraPosition = s->Player[i].getPlayerPosition();
		//		m_pLights->m_d3dxvCameraPosition = D3DXVECTOR4(d3dxvCameraPosition, 1.0f);
		//		cout << d3dxvCameraPosition.x <<"."<< d3dxvCameraPosition.z <<"i : "<<i<< endl;
		//		break;
		//	}
		//}

		//점 조명이 지형의 중앙을 중심으로 회전하도록 설정한다.


	}

	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->AnimateObjects(fTimeElapsed);
	}
}

void CScene::Render(ID3D11DeviceContext*pd3dDeviceContext,  CDirect3DBase* m_pDirect3D, CCamera *pCamera)
{
	if (m_pLights && m_pd3dcbLights) UpdateShaderVariable(pd3dDeviceContext, m_pLights);

	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->Render(pd3dDeviceContext, m_pDirect3D, pCamera);
	}
}

CGameObject *CScene::PickObjectPointedByCursor(int xClient, int yClient)
{
	if (!m_pCamera) return(NULL);

	D3DXMATRIX d3dxmtxView = m_pCamera->GetViewMatrix();
	D3DXMATRIX d3dxmtxProjection = m_pCamera->GetProjectionMatrix();
	D3D11_VIEWPORT d3dViewport = m_pCamera->GetViewport();

	D3DXVECTOR3 d3dxvPickPosition;
	d3dxvPickPosition.x = (((2.0f * xClient) / d3dViewport.Width) - 1) / d3dxmtxProjection._11;
	d3dxvPickPosition.y = -(((2.0f * yClient) / d3dViewport.Height) - 1) / d3dxmtxProjection._22;
	d3dxvPickPosition.z = 1.0f;

	int nIntersected = 0;
	float fNearHitDistance = FLT_MAX;
	MESHINTERSECTINFO d3dxIntersectInfo;
	CGameObject *pIntersectedObject = NULL, *pNearestObject = NULL;
	for (int i = 0; i < m_nShaders; i++)
	{
		pIntersectedObject = m_ppShaders[i]->PickObjectByRayIntersection(&d3dxvPickPosition, &d3dxmtxView, &d3dxIntersectInfo);
		if (pIntersectedObject && (d3dxIntersectInfo.m_fDistance < fNearHitDistance))
		{
			fNearHitDistance = d3dxIntersectInfo.m_fDistance;
			pNearestObject = pIntersectedObject;
		}
	}
	return(pNearestObject);
}

CHeightMapTerrain *CScene::GetTerrain()
{
	CTerrainShader *pTerrainShader = (CTerrainShader *)m_ppShaders[0];
	return(pTerrainShader->GetTerrain());
}

void CScene::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));
	//게임 월드 전체를 비추는 주변조명을 설정한다.
	m_pLights->m_d3dxcGlobalAmbient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);




	m_pLights->m_pLights[0].m_bEnable = 1.0f;
	m_pLights->m_pLights[0].m_nType = DIRECTIONAL_LIGHT;
	//m_pLights->m_pLights[0].m_d3dxcAmbient = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
	//m_pLights->m_pLights[0].m_d3dxcDiffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	//m_pLights->m_pLights[0].m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	//m_pLights->m_pLights[0].m_d3dxvDirection = D3DXVECTOR3(0.0f, -0.31622f, -0.9486f);

	m_pLights->m_pLights[0].m_d3dxcAmbient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights->m_pLights[0].m_d3dxcDiffuse = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	m_pLights->m_pLights[0].m_d3dxcSpecular = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	m_pLights->m_pLights[0].m_d3dxvDirection = D3DXVECTOR3(0.0f, -0.31622f, -0.9486f);



	m_pLights->m_pLights[1].m_bEnable = 1.0f;
	m_pLights->m_pLights[1].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[1].m_d3dxcAmbient = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	m_pLights->m_pLights[1].m_d3dxcDiffuse = D3DXCOLOR(0.40f, 0.40f, 0.40f, 1.0f);
	m_pLights->m_pLights[1].m_d3dxcSpecular = D3DXCOLOR(0.45f, 0.45f, 0.45f, 1.0f);
	m_pLights->m_pLights[1].m_d3dxvDirection = D3DXVECTOR3(0.57735f, -0.57735f, 0.57735f);


	//m_pLights->m_pLights[2].m_bEnable = 1.0f;
	//m_pLights->m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	//m_pLights->m_pLights[2].m_d3dxcAmbient = D3DXCOLOR(0.5f, 0.5f, 0.2f, 1.0f);
	//m_pLights->m_pLights[2].m_d3dxcDiffuse = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);
	//m_pLights->m_pLights[2].m_d3dxcSpecular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	//m_pLights->m_pLights[2].m_d3dxvDirection = D3DXVECTOR3(0.987f, -1.0f, 0.0f);


	//m_pLights->m_pLights[2].m_bEnable = 1.0f;
	//m_pLights->m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	//m_pLights->m_pLights[2].m_d3dxcAmbient = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	//m_pLights->m_pLights[2].m_d3dxcDiffuse = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
	//m_pLights->m_pLights[2].m_d3dxcSpecular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	//m_pLights->m_pLights[2].m_d3dxvDirection = D3DXVECTOR3(0.0f, -0.707f, 0.707f);








	//3개의 조명(점 광원, 스팟 광원, 방향성 광원)을 설정한다.
	//m_pLights->m_pLights[0].m_bEnable = 1.0f;
	//m_pLights->m_pLights[0].m_nType = POINT_LIGHT;
	//m_pLights->m_pLights[0].m_fRange = 300.0f;
	//m_pLights->m_pLights[0].m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.0f, 0.0f, 1.0f);
	//m_pLights->m_pLights[0].m_d3dxcDiffuse = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	//m_pLights->m_pLights[0].m_d3dxcSpecular = D3DXCOLOR(0.1f, 0.1f, 0.1f, 0.0f);
	//m_pLights->m_pLights[0].m_d3dxvPosition = D3DXVECTOR3(300.0f, 300.0f, 300.0f);
	//m_pLights->m_pLights[0].m_d3dxvDirection = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	//m_pLights->m_pLights[0].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.001f, 0.0001f);

	//m_pLights->m_pLights[1].m_bEnable = 1.0f;
	//m_pLights->m_pLights[1].m_nType = SPOT_LIGHT;
	//m_pLights->m_pLights[1].m_fRange = 100.0f;
	//m_pLights->m_pLights[1].m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
	//m_pLights->m_pLights[1].m_d3dxcDiffuse = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
	//m_pLights->m_pLights[1].m_d3dxcSpecular = D3DXCOLOR(0.1f, 0.1f, 0.1f, 0.0f);
	//m_pLights->m_pLights[1].m_d3dxvPosition = D3DXVECTOR3(500.0f, 300.0f, 500.0f);
	//m_pLights->m_pLights[1].m_d3dxvDirection = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	//m_pLights->m_pLights[1].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.01f, 0.0001f);
	//m_pLights->m_pLights[1].m_fFalloff = 8.0f;
	//m_pLights->m_pLights[1].m_fPhi = (float)cos(D3DXToRadian(40.0f));
	//m_pLights->m_pLights[1].m_fTheta = (float)cos(D3DXToRadian(22.0f));

	//m_pLights->m_pLights[2].m_bEnable = 1.0f;
	//m_pLights->m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	//m_pLights->m_pLights[2].m_d3dxcAmbient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
	//m_pLights->m_pLights[2].m_d3dxcDiffuse = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);
	//m_pLights->m_pLights[2].m_d3dxcSpecular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	//m_pLights->m_pLights[2].m_d3dxvDirection = D3DXVECTOR3(0.0f, -1.0f, 0.0f);


	//m_pLights->m_pLights[3].m_bEnable = 1.0f;
	//m_pLights->m_pLights[3].m_nType = SPOT_LIGHT;
	//m_pLights->m_pLights[3].m_fRange = 60.0f;
	//m_pLights->m_pLights[3].m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.0f, 0.0f, 1.0f);
	//m_pLights->m_pLights[3].m_d3dxcDiffuse = D3DXCOLOR(0.5f, 0.0f, 0.0f, 1.0f);
	//m_pLights->m_pLights[3].m_d3dxcSpecular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	//m_pLights->m_pLights[3].m_d3dxvPosition = D3DXVECTOR3(500.0f, 300.0f, 500.0f);
	//m_pLights->m_pLights[3].m_d3dxvDirection = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	//m_pLights->m_pLights[3].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.01f, 0.0001f);
	//m_pLights->m_pLights[3].m_fFalloff = 20.0f;
	//m_pLights->m_pLights[3].m_fPhi = (float)cos(D3DXToRadian(40.0f));
	//m_pLights->m_pLights[3].m_fTheta = (float)cos(D3DXToRadian(15.0f));

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(d3dBufferDesc));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(LIGHTS);
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pLights;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dcbLights);
}
void CScene::ReleaseShaderVariables()
{
	if (m_pLights) delete m_pLights;
	if (m_pd3dcbLights) m_pd3dcbLights->Release();
}
void CScene::UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, LIGHTS *pLights)
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbLights, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	LIGHTS *pcbLight = (LIGHTS *)d3dMappedResource.pData;
	memcpy(pcbLight, pLights, sizeof(LIGHTS));
	pd3dDeviceContext->Unmap(m_pd3dcbLights, 0);
	pd3dDeviceContext->PSSetConstantBuffers(PS_SLOT_LIGHT, 1, &m_pd3dcbLights);

}