#include "UiShader.h"




CUiShader::CUiShader()
{
}


CUiShader::~CUiShader()
{
	if (m_pTexture) m_pTexture->Release();
}

// 3D 모델과는 달리 BitmapClass에서는 화면 크기, 이미지 크기, 이전에 그려졌던 위치를 기억해야 합니다. 
// 따라서 이 정보들을 추적하는 전용 변수를 더합니다.



void CUiShader::CreateDisabledDepthStencilState(ID3D11Device * pd3dDevice)
{
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;

	// Clear the second depth stencil state before setting the parameters.
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
	// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;




		// Create the state using the device.
	pd3dDevice->CreateDepthStencilState(&depthDisabledStencilDesc, &m_pd3dDisabledDepthStencilState);


}

void CUiShader::BuildObjects(ID3D11Device *pd3dDevice)
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

	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/Ui/hp.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pTexture->SetTexture(0, pd3dsrvTexture);
	pd3dsrvTexture->Release();
	m_pTexture->SetSampler(0, pd3dSamplerState);
	pd3dSamplerState->Release();



	m_nScreenWidth = 0;
	m_nScreenHeight = 0;
	m_nObjects = PLAYERS_NUMS + MONSTERS_NUMS + 1;
	m_ppObjects = new CGameObject*[m_nObjects];
	

	CUIMesh* pHpMesh = new CUIMesh(pd3dDevice, 100.0f, 20.0f);

	//CUIMesh* pHpMesh = new CUIMesh(pd3dDevice, 1200.0f, 780.0f);
	CHpObject* pHpObject1 = new CHpObject(pd3dDevice);
	CHpObject* pHpObject2 = new CHpObject(pd3dDevice);
	CHpObject* pHpObject3 = new CHpObject(pd3dDevice);
	CHpObject* pHpObject4 = new CHpObject(pd3dDevice);
	CHpObject* pHpObject5 = new CHpObject(pd3dDevice);
	CHpObject* pHpObject6 = new CHpObject(pd3dDevice);
	CHpObject* pHpObject7 = new CHpObject(pd3dDevice);
	CHpObject* pHpObject8 = new CHpObject(pd3dDevice);
	CHpObject* pHpObject9 = new CHpObject(pd3dDevice);
	CHpObject* pHpObject10 = new CHpObject(pd3dDevice);

	pHpObject1->SetMesh(pHpMesh);
	pHpObject2->SetMesh(pHpMesh);
	pHpObject3->SetMesh(pHpMesh);
	pHpObject4->SetMesh(pHpMesh);
	pHpObject5->SetMesh(pHpMesh);
	pHpObject6->SetMesh(pHpMesh);
	pHpObject7->SetMesh(pHpMesh);
	pHpObject8->SetMesh(pHpMesh);
	pHpObject9->SetMesh(pHpMesh);
	pHpObject10->SetMesh(pHpMesh);


	m_ppObjects[0] = pHpObject1;
	m_ppObjects[1] = pHpObject2;
	m_ppObjects[2] = pHpObject3;
	m_ppObjects[3] = pHpObject4;
	m_ppObjects[4] = pHpObject5;
	m_ppObjects[5] = pHpObject6;
	m_ppObjects[6] = pHpObject7;
	m_ppObjects[7] = pHpObject8;
	m_ppObjects[8] = pHpObject9;
	m_ppObjects[9] = pHpObject10;
	//m_ppObjects[9]->SetPosition(50.0f, 300.0f, 50.0f);

}
void CUiShader::CreateShaderVariables(ID3D11Device * pd3dDevice)
{

}
void CUiShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera)
{
	
	OnPrepareRender(pd3dDeviceContext);
	if (m_pTexture)
		m_pTexture->UpdateShaderVariable(pd3dDeviceContext);

	pd3dDeviceContext->OMSetDepthStencilState(m_pd3dDisabledDepthStencilState, 1);
	pCamera->UpdateOrthoMartirxShaderVariables(pd3dDeviceContext);
	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			//if (m_ppObjects[j]->IsVisible(pCamera))
			{
				
				m_ppObjects[j]->Render(pd3dDeviceContext, pCamera);
				
			}
		}
	}
	pCamera->UpdateShaderVariables(pd3dDeviceContext);
	pd3dDeviceContext->OMSetDepthStencilState(NULL, 1);
}
void CUiShader::AnimateObjects(float fTimeElapsed)
{
	CGameManager* pGameManager = CGameManager::GetCGameManager();
	int j=0,  k = 0;
	for (int i = 0; i < m_nObjects-1; i++)
	{
		if (i <= 3)
		{
			m_ppObjects[i]->
				SetPosition(D3DXVECTOR3(pGameManager->m_pPlayers[j]->GetPosition().x
				                                               , pGameManager->m_pPlayers[j]->GetPosition().y  ,
				                                                  pGameManager->m_pPlayers[j]->GetPosition().z));
			j++;
		}
		else if (i > 3)
		{
			m_ppObjects[i]->
				SetPosition(D3DXVECTOR3(pGameManager->m_ppMonster[k]->GetPosition().x
				                                                 , pGameManager->m_ppMonster[k]->GetPosition().y , 
																 pGameManager->m_ppMonster[k]->GetPosition().z));
			k++;
		}
	}
	// 

	//cout << m_ppObjects[4]->GetPosition().x << "  " << m_ppObjects[4]->GetPosition().y << "  " << m_ppObjects[4]->GetPosition().z << endl;
}