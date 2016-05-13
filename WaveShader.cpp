#include "WaveShader.h"
#include "Direct3DBase.h"


ID3D11ShaderResourceView *CRefractionShader::m_pd3dRefractionShaderResourceView = NULL;
ID3D11ShaderResourceView * CRealityShader ::m_pd3dReflectionShaderResourceView = NULL;

CWaveShader::CWaveShader()
{
}

CWaveShader ::~CWaveShader()
{
	//if (m_pMaterial) m_pMaterial->Release();
	//if (m_pMaterial)m_pMaterial->Release();
	//if (m_pTexture)m_pTexture->Release();

}

void  CWaveShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);

	//CreateVertexShaderFromCompiledFile(pd3dDevice, L"DisplacementMapping_VS.fxo", d3dInputElements, nElements);
	//CreatePixelShaderFromCompiledFile(pd3dDevice, L"DisplacementMapping_PS.fxo");
	//CreateHullShaderFromCompiledFile(pd3dDevice, L"DisplacementMapping_HS.fxo");
	//CreateDomainShaderFromCompiledFile(pd3dDevice, L"DisplacementMapping_DS.fxo");



	CreateVertexShaderFromFile(pd3dDevice, L"Wave.fx", "VS", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Wave.fx", "PS", "ps_5_0", &m_pd3dPixelShader);
	CreateHullShaderFromFile(pd3dDevice, L"Wave.fx", "HS", "hs_5_0", &m_pd3dHullShader);
	CreateDomainShaderFromFile(pd3dDevice, L"Wave.fx", "DS", "ds_5_0", &m_pd3dDomainShader);
}
void CWaveShader::CreateConstantBuffer(ID3D11Device *pd3dDevice)
{
	D3D11_BUFFER_DESC clipPlaneBufferDesc;

	// 클립 평면 상수 버퍼를 설정한다. 우리는 정점 셰이더 안에 잇는 클립 평면에 접속하기 위해.

	// Setup the description of the clip plane dynamic constant buffer that is in the vertex shader.
	clipPlaneBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	clipPlaneBufferDesc.ByteWidth = sizeof(ReflectionMatrix);
	clipPlaneBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	clipPlaneBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	clipPlaneBufferDesc.MiscFlags = 0;
	clipPlaneBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	pd3dDevice->CreateBuffer(&clipPlaneBufferDesc, NULL, &m_pd3dcbReflectionMatrixBuffer);
}

void   CWaveShader::BuildObjects(ID3D11Device *pd3dDevice, CMaterial *pMaterial, CTexture *pTexture)
{
	//m_pMaterial = pMaterial;
	//if (pMaterial) pMaterial->AddRef();


	//m_pTexture = pTexture;
	//if (pTexture) pTexture->AddRef();

	// 굴절 생성할 때 필요하다.
	//CreateConstantBuffer(pd3dDevice);
	m_nObjects = 1;
	m_ppObjects = new CGameObject*[m_nObjects];

	CWaveObject *pPlane = new  CWaveObject(pd3dDevice);
	m_ppObjects[0] = pPlane;
	m_ppObjects[0]->SetPosition(1310.0f, 246.0f, 1990.0f); //220    1950
	CreateCameraPositionBuffer(pd3dDevice);


	//m_pMaterial = new CMaterial();
	//m_pMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);  //0,4 0.4 0.7
	//m_pMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.1f, 0.3f, 1.0f);
	//m_pMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(0.8f, 0.8f, 0.8f, 128.0f);
	//m_pMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);



}
void  CWaveShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera)
{
	OnPrepareRender(pd3dDeviceContext);
	//if (m_pMaterial)
	//	CIlluminatedShader::UpdateShaderVariable(pd3dDeviceContext, &m_pMaterial->m_Material);
	//if (m_pTexture)  //텍스쳐
	//	m_pTexture->UpdateShaderVariable(pd3dDeviceContext);

	pd3dDeviceContext->HSSetShader(m_pd3dHullShader, NULL, 0);
	pd3dDeviceContext->DSSetShader(m_pd3dDomainShader, NULL, 0);
	//카메라-투영 행렬을 도메인 셰이더에 연결한다.
	//pCamera->DSUpdateShaderVariables(pd3dDeviceContext);

	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbCameraPosition, 0, D3D11_MAP_WRITE_DISCARD, 0,
		&d3dMappedResource);
	D3DXVECTOR4 *pcbCameraPosition = (D3DXVECTOR4*)d3dMappedResource.pData;
	pcbCameraPosition->x = pCamera->GetPosition().x;
	pcbCameraPosition->y = pCamera->GetPosition().y;
	pcbCameraPosition->z = pCamera->GetPosition().z;
	pd3dDeviceContext->Unmap(m_pd3dcbCameraPosition, 0);

	//정점 셰이더에 카메라 좌표를 연결한다.
	pd3dDeviceContext->VSSetConstantBuffers(0x03, 1, &m_pd3dcbCameraPosition);

	// 여러가지 해줄것들....
	//pd3dDeviceContext->PSSetShaderResources(0x03, 1, &CRefractionShader::m_pd3dRefractionShaderResourceView);
	//pd3dDeviceContext->PSSetShaderResources(0x04, 1, &CRealityShader ::m_pd3dReflectionShaderResourceView);
	// 반사 카메라 행렬을 연결한다.
	//pd3dDeviceContext->Map(m_pd3dcbReflectionMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	//ReflectionMatrix* cbmtxReflection = (ReflectionMatrix*)d3dMappedResource.pData;
	//D3DXMatrixTranspose(&cbmtxReflection->m_d3dmtxReflection, &pCamera->GetReflectionViewMatrix());
	//pd3dDeviceContext->Unmap(m_pd3dcbReflectionMatrixBuffer, 0);
	//pd3dDeviceContext->DSSetConstantBuffers(0x04, 1, &m_pd3dcbReflectionMatrixBuffer);

	m_ppObjects[0]->Render(pd3dDeviceContext, pCamera);

	//pd3dDeviceContext->HSSetConstantBuffers(NULL, NULL, NULL);
	//pCamera->DSClearShaderVariables(pd3dDeviceContext);
	pd3dDeviceContext->HSSetShader(NULL, NULL, 0);
	pd3dDeviceContext->DSSetShader(NULL, NULL, 0);
}

CRefractionShader::CRefractionShader()
{
}
CRefractionShader::~CRefractionShader()
{
}


bool CRefractionShader::CreateRenderToTexture(ID3D11Device *pd3dDevice, int textureWidth, int textureHeight)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the render target texture.
	// 
	result = pd3dDevice->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = pd3dDevice->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	result = pd3dDevice->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_pd3dRefractionShaderResourceView);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}


void CRefractionShader::SetRenderTarget(ID3D11DeviceContext* deviceContext, CDirect3DBase* m_pDirect3D)
{
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	// 렌더 타겟 뷰와 깊이-스텐실 버퍼를 출력 렌더 파이프라인에 묶는다
	deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_pDirect3D->GetDepthStencilView());

	return;
}


void CRefractionShader::ClearRenderTarget(ID3D11DeviceContext* deviceContext, CDirect3DBase* m_pDirect3D,
	float red, float green, float blue, float alpha)
{
	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(m_pDirect3D->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}
void CRefractionShader::CreateConstantBuffer(ID3D11Device *pd3dDevice)
{
	D3D11_BUFFER_DESC clipPlaneBufferDesc;

	// 클립 평면 상수 버퍼를 설정한다. 우리는 정점 셰이더 안에 잇는 클립 평면에 접속하기 위해.

	// Setup the description of the clip plane dynamic constant buffer that is in the vertex shader.
	clipPlaneBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	clipPlaneBufferDesc.ByteWidth = sizeof(ClipPlaneBufferType);
	clipPlaneBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	clipPlaneBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	clipPlaneBufferDesc.MiscFlags = 0;
	clipPlaneBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	pd3dDevice->CreateBuffer(&clipPlaneBufferDesc, NULL, &m_clipPlaneBuffer);
}


void CRefractionShader::CreateShader(ID3D11Device *pd3dDevice)
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



	CreateVertexShaderFromFile(pd3dDevice, L"Refraction.fx", "VSInstancedTexturedLightingColor", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Refraction.fx", "PSInstancedTexturedLightingColor", "ps_5_0", &m_pd3dPixelShader);
}

void  CRefractionShader::BuildObjects(ID3D11Device *pd3dDevice)
{
	//굴절 관련 변수 초기화.
	CreateRenderToTexture(pd3dDevice, 1200, 780);
	CreateConstantBuffer(pd3dDevice);

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
	m_pTexture = new CTexture(3, 1, 0, 0);

	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/FixObject/Flower-Lily.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pTexture->SetTexture(0, pd3dsrvTexture);
	pd3dsrvTexture->Release();
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/FixObject/Wood-Root.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pTexture->SetTexture(1, pd3dsrvTexture);
	pd3dsrvTexture->Release();
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/FixObject/Rock-Multiple-01.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pTexture->SetTexture(2, pd3dsrvTexture);
	pd3dsrvTexture->Release();

	m_pTexture->SetSampler(0, pd3dSamplerState);
	pd3dSamplerState->Release();





	//에러의 원인
	m_nInstanceBufferStride = sizeof(VS_VB_INSTANCE);
	m_nInstanceBufferOffset = 0;

	string strFileName1 = "Data/NewWaterLily01_Vertex.txt";
	string strFileName2 = "Data/NewWaterLily02_Vertex.txt";
	// Stone mesh
	string strFileName3 = "Data/NewRootTree_Vertex.txt";
	string strFileName4 = "Data/NewFlatRock02_Vertex.txt";
	string strFileName5 = "Data/NewRockFlat03_Vertex.txt";
	string strFileName6 = "Data/NewRockTall01_Vertex.txt";

	CFixedMesh* pWaterLily01Mesh = new CFixedMesh(pd3dDevice, strFileName1);
	CFixedMesh* pWaterLily02Mesh = new CFixedMesh(pd3dDevice, strFileName2);
	CFixedMesh* pRootTreeMesh = new CFixedMesh(pd3dDevice, strFileName3);

	//CFixedMesh * pFlatRockMesh01 = new CFixedMesh(pd3dDevice, strFileName3);
	//CFixedMesh * pFlatRockMesh02 = new CFixedMesh(pd3dDevice, strFileName4);
	//CFixedMesh * pFlatRockMesh03 = new CFixedMesh(pd3dDevice, strFileName5);
	//CFixedMesh * pTallRockMesh01 = new CFixedMesh(pd3dDevice, strFileName6);



	int  i = 0;
	//m_nObjects = 200;
	m_nObjects = WATER_RILY01_NUM + WATER_RILY02_NUM + ROOT_TREE_NUM;
	m_ppObjects = new CGameObject*[m_nObjects];
	CWoodObject *pWaterLilyObject = NULL;
	/*구는 3가지 종류(재질에 따라)이다. 다른 재질의 구들이 번갈아 나열되도록 한다. 재질의 종류에 따라 k가 0, 1, 2의 값을 가지고 k에 따라 객체의 위치를 다르게 설정한다.*/

	int x = 0;
	int z;
	for (int x = 0; x < 2; x++)
	{
		for (int z = 0; z < 4; z++)
		{
			pWaterLilyObject = new  CWoodObject(pd3dDevice, strFileName1);
			pWaterLilyObject->SetMesh(pWaterLily01Mesh);
			pWaterLilyObject->Scale(D3DXVECTOR3(8.0, 8.0, 8.0));
			pWaterLilyObject->SetPosition((x + 1) * 300 + 0, 240.0f, (z + 1) *(300) + 0);
			m_ppObjects[i++] = pWaterLilyObject;
		}
	}
	m_pd3dWaterLily01InstanceBuffer = CreateInstanceBuffer(pd3dDevice, WATER_RILY01_NUM, m_nInstanceBufferStride, NULL);
	pWaterLily01Mesh->AssembleToVertexBuffer(1, &m_pd3dWaterLily01InstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);


	for (int x = 0; x < 2; x++)
	{
		for (int z = 0; z < 4; z++)
		{
			pWaterLilyObject = new  CWoodObject(pd3dDevice, strFileName2);
			pWaterLilyObject->SetMesh(pWaterLily02Mesh);
			pWaterLilyObject->Scale(D3DXVECTOR3(8.0, 8.0, 8.0));
			pWaterLilyObject->SetPosition((x + 1) * 320, 240.0f + 0, 10.0f + (z + 1) *(300) + 0);
			m_ppObjects[i++] = pWaterLilyObject;
		}
	}
	m_pd3dWaterLily02InstanceBuffer = CreateInstanceBuffer(pd3dDevice, WATER_RILY02_NUM, m_nInstanceBufferStride, NULL);
	pWaterLily02Mesh->AssembleToVertexBuffer(1, &m_pd3dWaterLily02InstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);


	for (int x = 0; x < 2; x++)
	{
		for (int z = 0; z < 2; z++)
		{
			pWaterLilyObject = new  CWoodObject(pd3dDevice, strFileName2);
			pWaterLilyObject->SetMesh(pRootTreeMesh);
			pWaterLilyObject->Scale(D3DXVECTOR3(7.0, 7.0, 7.0));
			float fAngle = rand() % 60;
			pWaterLilyObject->Rotate(&D3DXVECTOR3(0.0f, 1.0f, 0.0f), (float)D3DXToDegree(fAngle));
			pWaterLilyObject->SetPosition((x + 1) * 300 + 240, 230.0f, (z + 1) *(150) + 800);
			m_ppObjects[i++] = pWaterLilyObject;
		}
	}
	m_pd3dRootTreeInstanceBuffer = CreateInstanceBuffer(pd3dDevice, ROOT_TREE_NUM, m_nInstanceBufferStride, NULL);
	pRootTreeMesh->AssembleToVertexBuffer(1, &m_pd3dRootTreeInstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);



}

void CRefractionShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera)
{
	OnPrepareRender(pd3dDeviceContext);
	if (m_pMaterial)
		CIlluminatedShader::UpdateShaderVariable(pd3dDeviceContext, &m_pMaterial->m_Material);

	if (m_pTexture)
	{
		m_pTexture->UpdateSamplerShaderVariable(pd3dDeviceContext, 0, 0);
		m_pTexture->UpdateTextureShaderVariable(pd3dDeviceContext, 0, 0);
	}
	D3DXVECTOR4 clipPlane;
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;


	// 물 평면 위에 있는 모든것을 자르기 위해 물의 높이를 기본으로된 클리핑 평면을 설정한다.
	// Setup a clipping plane based on the height of the water to clip everything above it.
	float  fWaterHeight = WATER_HEIGHT;
	clipPlane = D3DXVECTOR4(0.0f, -1.0f, 0.0f, fWaterHeight + 0.1f);
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	// Set the render target to be the refraction render to texture.
	// 렌더 텍스쳐에 굴절이 되도록 하기 위해 렌더 타켓에 연결한다.
	SetRenderTarget(pd3dDeviceContext, m_pDirect3D);

	// Clear the refraction render to texture.
	ClearRenderTarget(pd3dDeviceContext, m_pDirect3D, 0.0f, 0.0f, 0.0f, 1.0f);

	ClipPlaneBufferType* dataPtr3;
	result = pd3dDeviceContext->Map(m_clipPlaneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr3 = (ClipPlaneBufferType*)mappedResource.pData;
	dataPtr3->clipPlane = clipPlane;
	pd3dDeviceContext->Unmap(m_clipPlaneBuffer, 0);
	pd3dDeviceContext->VSSetConstantBuffers(2, 1, &m_clipPlaneBuffer);






	//**********************************************
	int nWaterLily01Objects = m_nObjects / 2;

	int nWaterLily01Instances = 0;
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dWaterLily01InstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_VB_INSTANCE *pnWaterLily01Instances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
	for (int j = 0; j < WATER_RILY01_NUM; j++)
	{
		if (m_ppObjects[j]->IsVisible(pCamera))
		{
			if (m_ppObjects[j]->IsVisible(pCamera))
				D3DXMatrixTranspose(&pnWaterLily01Instances[nWaterLily01Instances++].m_d3dxTransform, &m_ppObjects[j]->m_d3dxmtxWorld);
		}
	}
	pd3dDeviceContext->Unmap(m_pd3dWaterLily01InstanceBuffer, 0);

	CMesh *pWaterLily01Mesh = m_ppObjects[WATER_RILY01_NUM - 1]->GetMesh();
	pWaterLily01Mesh->RenderInstanced(pd3dDeviceContext, nWaterLily01Instances, 0);

	// Render WaterLily02 
	int nWaterLily02Instances = 0;
	pd3dDeviceContext->Map(m_pd3dWaterLily01InstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_VB_INSTANCE *pnWaterLily02Instances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
	for (int j = WATER_RILY01_NUM; j < WATER_RILY01_NUM + WATER_RILY02_NUM; j++)
	{
		if (m_ppObjects[j]->IsVisible(pCamera))
		{
			if (m_ppObjects[j]->IsVisible(pCamera))
				D3DXMatrixTranspose(&pnWaterLily02Instances[nWaterLily02Instances++].m_d3dxTransform, &m_ppObjects[j]->m_d3dxmtxWorld);
		}
	}
	pd3dDeviceContext->Unmap(m_pd3dWaterLily01InstanceBuffer, 0);

	CMesh *pWaterLily02Mesh = m_ppObjects[WATER_RILY01_NUM + WATER_RILY02_NUM - 1]->GetMesh();
	pWaterLily02Mesh->RenderInstanced(pd3dDeviceContext, nWaterLily02Instances, 0);

	if (m_pTexture)
		m_pTexture->UpdateTextureShaderVariable(pd3dDeviceContext, 1, 0);


	// Render RootTree
	int nRootTreeInstances = 0;
	pd3dDeviceContext->Map(m_pd3dRootTreeInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_VB_INSTANCE *pnRootTreeInstances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
	for (int j = WATER_RILY01_NUM + WATER_RILY02_NUM;
		j < WATER_RILY01_NUM + WATER_RILY02_NUM + ROOT_TREE_NUM; j++)
	{
		if (m_ppObjects[j]->IsVisible(pCamera))
		{
			if (m_ppObjects[j]->IsVisible(pCamera))
				D3DXMatrixTranspose(&pnRootTreeInstances[nRootTreeInstances++].m_d3dxTransform, &m_ppObjects[j]->m_d3dxmtxWorld);
		}
	}
	pd3dDeviceContext->Unmap(m_pd3dRootTreeInstanceBuffer, 0);

	CMesh *pRootTreeMesh = m_ppObjects[m_nObjects - 1]->GetMesh();
	pRootTreeMesh->RenderInstanced(pd3dDeviceContext, nRootTreeInstances, 0);

	// 다음 그리는 상태가 일반적인 렌더링이 아니라면 구지 바꿀 필요가 없다.
	//m_pDirect3D->BeginScene();
	m_pDirect3D->SetBackBufferRenderTarget();
}

//===========================================


CRealityShader ::CRealityShader ()
{
}
CRealityShader ::~CRealityShader ()
{
}


//void CReflectionnShader::CreateConstantBuffer(ID3D11Device *pd3dDevice)
//{
//
//	D3D11_BUFFER_DESC clipPlaneBufferDesc;
//
//	// 클립 평면 상수 버퍼를 설정한다. 우리는 정점 셰이더 안에 잇는 클립 평면에 접속하기 위해.
//
//	// Setup the description of the clip plane dynamic constant buffer that is in the vertex shader.
//	clipPlaneBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
//	clipPlaneBufferDesc.ByteWidth = sizeof(ClipPlaneBufferType);
//	clipPlaneBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	clipPlaneBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//	clipPlaneBufferDesc.MiscFlags = 0;
//	clipPlaneBufferDesc.StructureByteStride = 0;
//
//	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
//	pd3dDevice->CreateBuffer(&clipPlaneBufferDesc, NULL, &m_clipPlaneBuffer);
//}


bool CRealityShader ::CreateRenderToTexture(ID3D11Device *pd3dDevice, int textureWidth, int textureHeight)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the render target texture.
	// 
	result = pd3dDevice->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = pd3dDevice->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	result = pd3dDevice->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_pd3dReflectionShaderResourceView);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void CRealityShader ::CreateShader(ID3D11Device *pd3dDevice)
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
void  CRealityShader ::BuildObjects(ID3D11Device *pd3dDevice)
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
	m_pTexture = new CTexture(4, 1, 0, 0);

	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/FixObject/Flower-Lily.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pTexture->SetTexture(0, pd3dsrvTexture);
	pd3dsrvTexture->Release();
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/FixObject/Wood-Root.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pTexture->SetTexture(1, pd3dsrvTexture);
	pd3dsrvTexture->Release();
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/FixObject/Rock-Multiple-01.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pTexture->SetTexture(2, pd3dsrvTexture);
	pd3dsrvTexture->Release();
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/FixObject/WoodPine.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pTexture->SetTexture(3, pd3dsrvTexture);
	pd3dsrvTexture->Release();

	m_pTexture->SetSampler(0, pd3dSamplerState);
	pd3dSamplerState->Release();





	//에러의 원인
	m_nInstanceBufferStride = sizeof(VS_VB_INSTANCE);
	m_nInstanceBufferOffset = 0;

	string strFileName1 = "Data/NewWaterLily01_Vertex.txt";
	string strFileName2 = "Data/NewWaterLily02_Vertex.txt";
	// Stone mesh
	string strFileName3 = "Data/NewRootTree_Vertex.txt";
	string strFileName4 = "Data/NewFlatRock02_Vertex.txt";
	string strFileName5 = "Data/NewRockFlat03_Vertex.txt";
	string strFileName6 = "Data/NewRockTall01_Vertex.txt";
	string strFileName7 = "Data/PineLeaves_Vertex.txt";
	string strFileName8 = "Data/PineWood_Vertex.txt";

	CFixedMesh* pWaterLily01Mesh = new CFixedMesh(pd3dDevice, strFileName1);
	CFixedMesh* pWaterLily02Mesh = new CFixedMesh(pd3dDevice, strFileName2);
	CFixedMesh* pRootTreeMesh = new CFixedMesh(pd3dDevice, strFileName3);
	CFixedMesh* pPineLeavesMesh = new CFixedMesh(pd3dDevice, strFileName7);
	CFixedMesh* pPineWoodMesh = new CFixedMesh(pd3dDevice, strFileName8);


	//CFixedMesh * pFlatRockMesh01 = new CFixedMesh(pd3dDevice, strFileName3);
	//CFixedMesh * pFlatRockMesh02 = new CFixedMesh(pd3dDevice, strFileName4);
	//CFixedMesh * pFlatRockMesh03 = new CFixedMesh(pd3dDevice, strFileName5);
	//CFixedMesh * pTallRockMesh01 = new CFixedMesh(pd3dDevice, strFileName6);



	int  i = 0;
	//m_nObjects = 200;
	m_nObjects = WATER_RILY01_NUM + WATER_RILY02_NUM + ROOT_TREE_NUM + PINE_TREE_NUM;
	m_ppObjects = new CGameObject*[m_nObjects];
	CWoodObject *pWaterLilyObject = NULL;
	/*구는 3가지 종류(재질에 따라)이다. 다른 재질의 구들이 번갈아 나열되도록 한다. 재질의 종류에 따라 k가 0, 1, 2의 값을 가지고 k에 따라 객체의 위치를 다르게 설정한다.*/

	int x = 0;
	int z;
	for (int x = 0; x < 2; x++)
	{
		for (int z = 0; z < 4; z++)
		{
			pWaterLilyObject = new  CWoodObject(pd3dDevice, strFileName1);
			pWaterLilyObject->SetMesh(pWaterLily01Mesh);
			pWaterLilyObject->Scale(D3DXVECTOR3(8.0, 8.0, 8.0));
			pWaterLilyObject->SetPosition((x + 1) * 300 + 0, 240.0f, (z + 1) *(300) + 0);
			m_ppObjects[i++] = pWaterLilyObject;
		}
	}
	m_pd3dWaterLily01InstanceBuffer = CreateInstanceBuffer(pd3dDevice, WATER_RILY01_NUM, m_nInstanceBufferStride, NULL);
	pWaterLily01Mesh->AssembleToVertexBuffer(1, &m_pd3dWaterLily01InstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);


	for (int x = 0; x < 2; x++)
	{
		for (int z = 0; z < 4; z++)
		{
			pWaterLilyObject = new  CWoodObject(pd3dDevice, strFileName2);
			pWaterLilyObject->SetMesh(pWaterLily02Mesh);
			pWaterLilyObject->Scale(D3DXVECTOR3(8.0, 8.0, 8.0));
			pWaterLilyObject->SetPosition((x + 1) * 320, 240.0f + 0, 10.0f + (z + 1) *(300) + 0);
			m_ppObjects[i++] = pWaterLilyObject;
		}
	}
	m_pd3dWaterLily02InstanceBuffer = CreateInstanceBuffer(pd3dDevice, WATER_RILY02_NUM, m_nInstanceBufferStride, NULL);
	pWaterLily02Mesh->AssembleToVertexBuffer(1, &m_pd3dWaterLily02InstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);




	//Wood
	FILE* fp;
	fopen_s(&fp, "Data/ObjectPosition/WoodInWater.txt", "rt");
	for (int k = 0; k < 4; k++)
	{
		pWaterLilyObject = new  CWoodObject(pd3dDevice, strFileName1);
		pWaterLilyObject->SetMesh(pRootTreeMesh);
		pWaterLilyObject->Scale(D3DXVECTOR3(10.0f, 10.0f, 10.0f));
		pWaterLilyObject->Rotate(0.0f, 0.0f, -45.0f);

		float fPosition[3];
		fscanf_s(fp, "%f %f %f  \n", &fPosition[0], &fPosition[1], &fPosition[2]);
		pWaterLilyObject->SetPosition(fPosition[0], fPosition[1], fPosition[2]);
		m_ppObjects[i] = pWaterLilyObject;
		i++;
	}
	m_pd3dRootTreeInstanceBuffer = CreateInstanceBuffer(pd3dDevice, ROOT_TREE_NUM, m_nInstanceBufferStride, NULL);
	pRootTreeMesh->AssembleToVertexBuffer(1, &m_pd3dRootTreeInstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);
	fclose(fp);

	fopen_s(&fp, "Data/ObjectPosition/PineWood.txt", "rt");
	for (int k = 0; k < PINE_TREE_NUM/2; k++) //0 1 2 3
	{
		pWaterLilyObject = new  CWoodObject(pd3dDevice, strFileName1);
		pWaterLilyObject->SetMesh(pPineWoodMesh);
		pWaterLilyObject->Scale(D3DXVECTOR3(0.6f, 0.6f, 0.6f));
		pWaterLilyObject->Rotate(&D3DXVECTOR3(1.0f, 0.0f, 0.0f), -90.0f);
		pWaterLilyObject->Rotate(&D3DXVECTOR3(0.0f, 0.0f, 1.0f), +90.0f);
		pWaterLilyObject->Rotate(&D3DXVECTOR3(1.0f, 0.0f, 0.0f), +20.0f); // x축
		if (k >= 2)
		{
			pWaterLilyObject->Rotate(&D3DXVECTOR3(0.0f, 0.0f, 1.0f), -180.0f);
			pWaterLilyObject->Rotate(&D3DXVECTOR3(1.0f, 0.0f, 0.0f), +30.0f); // x축
			//pWaterLilyObject->Rotate(&D3DXVECTOR3(1.0f, 0.0f, 0.0f), +20.0f); // x축
		}
		
		//pWaterLilyObject->Rotate(-90.0f, 90.0f, 0.0f);
		//pWaterLilyObject->Rotate(&D3DXVECTOR3(0.0f, 0.0f, 1.0f), 30.0f)

		float fPosition[3];
		fscanf_s(fp, "%f %f %f  \n", &fPosition[0], &fPosition[1], &fPosition[2]);
		pWaterLilyObject->SetPosition(fPosition[0], fPosition[1], fPosition[2]);
		m_ppObjects[i] = pWaterLilyObject;
		i++;
	}
	m_pd3dPineWoodInstanceBuffer = CreateInstanceBuffer(pd3dDevice, PINE_TREE_NUM / 2, m_nInstanceBufferStride, NULL);
	pPineWoodMesh->AssembleToVertexBuffer(1, &m_pd3dPineWoodInstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);
	fclose(fp);

	fopen_s(&fp, "Data/ObjectPosition/PineLeaves.txt", "rt");
	for (int k = 0; k < PINE_TREE_NUM/2; k++)
	{
		pWaterLilyObject = new  CWoodObject(pd3dDevice, strFileName1);
		pWaterLilyObject->SetMesh(pPineLeavesMesh);
		pWaterLilyObject->Scale(D3DXVECTOR3(0.6f, 0.6f, 0.6f));
		pWaterLilyObject->Rotate(&D3DXVECTOR3(1.0f, 0.0f, 0.0f),  -90.0f);
		pWaterLilyObject->Rotate(&D3DXVECTOR3(0.0f, 0.0f, 1.0f), +90.0f); //y축 회전
		pWaterLilyObject->Rotate(&D3DXVECTOR3(1.0f, 0.0f, 0.0f), +20.0f); // x축
		if (k >= 2)
		{
			pWaterLilyObject->Rotate(&D3DXVECTOR3(0.0f, 0.0f, 1.0f), 160.0f);
			pWaterLilyObject->Rotate(&D3DXVECTOR3(0.0f, 1.0f, 0.0f), -20.0f);
			pWaterLilyObject->Rotate(&D3DXVECTOR3(1.0f, 0.0f, 0.0f), -20.0f); //
			pWaterLilyObject->Rotate(&D3DXVECTOR3(1.0f, 1.0f, 0.0f), 60.0f);//x축
			//pWaterLilyObject->Rotate(&D3DXVECTOR3(0.0f, 0.0f, 1.0f), 20.0f);
			//pWaterLilyObject->Rotate(&D3DXVECTOR3(0.0f, 1.0f, 0.0f), 40.0f);
			//pWaterLilyObject->Rotate(&D3DXVECTOR3(0.0f, 0.0f, 1.0f), -210.0f);
			//pWaterLilyObject->Rotate(&D3DXVECTOR3(1.0f, 0.0f, 0.0f), 40.0f); // x축
			//pWaterLilyObject->Rotate(&D3DXVECTOR3(0.0f, 0.0f, 1.0f), +40.0f); //y축 회전
		}
			
		//pWaterLilyObject->Rotate(&D3DXVECTOR3(0.0f, 1.0f, 0.0f), +90.0f);

		//pWaterLilyObject->Rotate(-90.0f, 90.0f, 0.0f);

		float fPosition[3];
		fscanf_s(fp, "%f %f %f  \n", &fPosition[0], &fPosition[1], &fPosition[2]);
		pWaterLilyObject->SetPosition(fPosition[0], fPosition[1], fPosition[2]);
		m_ppObjects[i] = pWaterLilyObject;
		i++;
	}
	m_pd3dPineLeavesInstanceBuffer = CreateInstanceBuffer(pd3dDevice, PINE_TREE_NUM / 2, m_nInstanceBufferStride, NULL);
	pPineLeavesMesh->AssembleToVertexBuffer(1, &m_pd3dPineLeavesInstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);

	fclose(fp);


}

void CRealityShader ::Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera)
{
	OnPrepareRender(pd3dDeviceContext);
	if (m_pMaterial)
		CIlluminatedShader::UpdateShaderVariable(pd3dDeviceContext, &m_pMaterial->m_Material);

	if (m_pTexture)
	{
		m_pTexture->UpdateSamplerShaderVariable(pd3dDeviceContext, 0, 0);
		m_pTexture->UpdateTextureShaderVariable(pd3dDeviceContext, 0, 0);
	}



	int nWaterLily01Objects = m_nObjects / 2;
	int nWaterLily01Instances = 0;
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dWaterLily01InstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_VB_INSTANCE *pnWaterLily01Instances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
	for (int j = 0; j < WATER_RILY01_NUM; j++)
	{
		if (m_ppObjects[j]->IsVisible(pCamera))
		{
			if (m_ppObjects[j]->IsVisible(pCamera))
				D3DXMatrixTranspose(&pnWaterLily01Instances[nWaterLily01Instances++].m_d3dxTransform, &m_ppObjects[j]->m_d3dxmtxWorld);
		}
	}
	pd3dDeviceContext->Unmap(m_pd3dWaterLily01InstanceBuffer, 0);

	CMesh *pWaterLily01Mesh = m_ppObjects[WATER_RILY01_NUM - 1]->GetMesh();
	pWaterLily01Mesh->RenderInstanced(pd3dDeviceContext, nWaterLily01Instances, 0);

	// Render WaterLily02 
	int nWaterLily02Instances = 0;
	pd3dDeviceContext->Map(m_pd3dWaterLily01InstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_VB_INSTANCE *pnWaterLily02Instances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
	for (int j = WATER_RILY01_NUM; j < WATER_RILY01_NUM + WATER_RILY02_NUM; j++)
	{
		if (m_ppObjects[j]->IsVisible(pCamera))
		{
			if (m_ppObjects[j]->IsVisible(pCamera))
				D3DXMatrixTranspose(&pnWaterLily02Instances[nWaterLily02Instances++].m_d3dxTransform, &m_ppObjects[j]->m_d3dxmtxWorld);
		}
	}
	pd3dDeviceContext->Unmap(m_pd3dWaterLily01InstanceBuffer, 0);

	CMesh *pWaterLily02Mesh = m_ppObjects[WATER_RILY01_NUM + WATER_RILY02_NUM - 1]->GetMesh();
	pWaterLily02Mesh->RenderInstanced(pd3dDeviceContext, nWaterLily02Instances, 0);

	// 1번 텍스쳐
	if (m_pTexture)
		m_pTexture->UpdateTextureShaderVariable(pd3dDeviceContext, 1, 0);


	// Render RootTree
	int nRootTreeInstances = 0;
	pd3dDeviceContext->Map(m_pd3dRootTreeInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_VB_INSTANCE *pnRootTreeInstances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
	for (int j = WATER_RILY01_NUM + WATER_RILY02_NUM;
		j < WATER_RILY01_NUM + WATER_RILY02_NUM + ROOT_TREE_NUM; j++)
	{
		if (m_ppObjects[j]->IsVisible(pCamera))
		{
			if (m_ppObjects[j]->IsVisible(pCamera))
				D3DXMatrixTranspose(&pnRootTreeInstances[nRootTreeInstances++].m_d3dxTransform, &m_ppObjects[j]->m_d3dxmtxWorld);
		}
	}
	pd3dDeviceContext->Unmap(m_pd3dRootTreeInstanceBuffer, 0);
	CMesh *pRootTreeMesh = m_ppObjects[WATER_RILY01_NUM + WATER_RILY02_NUM + ROOT_TREE_NUM - 1]->GetMesh();
	pRootTreeMesh->RenderInstanced(pd3dDeviceContext, nRootTreeInstances, 0);


	//  3번 텍스쳐
	if (m_pTexture)
		m_pTexture->UpdateTextureShaderVariable(pd3dDeviceContext, 3, 0);

	// Render PineWood
	int nPineWoodInstances = 0;
	pd3dDeviceContext->Map(m_pd3dPineWoodInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_VB_INSTANCE *pnPineWoodInstances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
	for (int j = WATER_RILY01_NUM + WATER_RILY02_NUM + ROOT_TREE_NUM;
		j < WATER_RILY01_NUM + WATER_RILY02_NUM + ROOT_TREE_NUM + PINE_TREE_NUM / 2; j++)
	{
		if (m_ppObjects[j]->IsVisible(pCamera))
		{
			if (m_ppObjects[j]->IsVisible(pCamera))
				D3DXMatrixTranspose(&pnPineWoodInstances[nPineWoodInstances++].m_d3dxTransform, &m_ppObjects[j]->m_d3dxmtxWorld);
		}
	}
	pd3dDeviceContext->Unmap(m_pd3dPineWoodInstanceBuffer, 0);
	CMesh *pPineWoodMesh = m_ppObjects[WATER_RILY01_NUM + WATER_RILY02_NUM + ROOT_TREE_NUM + PINE_TREE_NUM / 2 - 1]->GetMesh();
	pPineWoodMesh->RenderInstanced(pd3dDeviceContext, nPineWoodInstances, 0);
	if (m_pTexture)
		m_pTexture->UpdateTextureShaderVariable(pd3dDeviceContext, 3, 0);


	// Render PineLeaves
	int nPineLeavesInstances = 0;
	pd3dDeviceContext->Map(m_pd3dPineLeavesInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_VB_INSTANCE *pnPineLeavesInstances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
	for (int j = WATER_RILY01_NUM + WATER_RILY02_NUM + ROOT_TREE_NUM + PINE_TREE_NUM / 2;
		j < WATER_RILY01_NUM + WATER_RILY02_NUM + ROOT_TREE_NUM + PINE_TREE_NUM; j++)
	{
		if (m_ppObjects[j]->IsVisible(pCamera))
		{
			if (m_ppObjects[j]->IsVisible(pCamera))
				D3DXMatrixTranspose(&pnPineLeavesInstances[nPineLeavesInstances++].m_d3dxTransform, &m_ppObjects[j]->m_d3dxmtxWorld);
		}
	}
	pd3dDeviceContext->Unmap(m_pd3dPineLeavesInstanceBuffer, 0);
	CMesh *pPineLeavesMesh = m_ppObjects[m_nObjects - 1]->GetMesh();
	pPineLeavesMesh->RenderInstanced(pd3dDeviceContext, nPineLeavesInstances, 0);
}

CReflectionShader::CReflectionShader()
{
}
CReflectionShader ::~CReflectionShader()
{
}

void CReflectionShader::BuildObjects(ID3D11Device *pd3dDevice)
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

	ID3D11ShaderResourceView *pd3dsrvTexture = NULL;
	m_pTexture = new CTexture(2, 1, 0, 0);


	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/FixObject/Wood-Root.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pTexture->SetTexture(0, pd3dsrvTexture);
	pd3dsrvTexture->Release();
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/FixObject/WoodPine.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pTexture->SetTexture(1, pd3dsrvTexture);
	pd3dsrvTexture->Release();

	m_pTexture->SetSampler(0, pd3dSamplerState);
	pd3dSamplerState->Release();


	m_nInstanceBufferStride = sizeof(VS_VB_INSTANCE);
	m_nInstanceBufferOffset = 0;
	string strFileName7 = "Data/PineLeaves_Vertex.txt";
	string strFileName3 = "Data/NewRootTree_Vertex.txt";



	CFixedMesh* pRootTreeMesh = new CFixedMesh(pd3dDevice, strFileName3);

	CFixedMesh* pPineLeavesMesh = new CFixedMesh(pd3dDevice, strFileName7);

	int  i = 0;

	m_nObjects = ROOT_TREE_NUM + PINE_TREE_NUM/2;
	m_ppObjects = new CGameObject*[m_nObjects];
	CWoodObject *pWaterLilyObject = NULL;




	//Wood
	FILE* fp;
	fopen_s(&fp, "Data/ObjectPosition/WoodInWater.txt", "rt");
	for (int k = 0; k < ROOT_TREE_NUM; k++)
	{
		pWaterLilyObject = new  CWoodObject(pd3dDevice, strFileName3);
		pWaterLilyObject->SetMesh(pRootTreeMesh);
		pWaterLilyObject->Scale(D3DXVECTOR3(10.0f, 10.0f, 10.0f));
		pWaterLilyObject->Rotate(0.0f, 0.0f, -45.0f);

		float fPosition[3];
		fscanf_s(fp, "%f %f %f  \n", &fPosition[0], &fPosition[1], &fPosition[2]);
		pWaterLilyObject->SetPosition(fPosition[0], fPosition[1], fPosition[2]);
		m_ppObjects[i] = pWaterLilyObject;
		i++;
	}
	m_pd3dRootTreeInstanceBuffer = CreateInstanceBuffer(pd3dDevice, ROOT_TREE_NUM, m_nInstanceBufferStride, NULL);
	pRootTreeMesh->AssembleToVertexBuffer(1, &m_pd3dRootTreeInstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);
	fclose(fp);





	//Leaves
	
	fopen_s(&fp, "Data/ObjectPosition/PineLeaves.txt", "rt");
	for (int k = 0; k < PINE_TREE_NUM/2; k++)
	{
		pWaterLilyObject = new  CWoodObject(pd3dDevice, strFileName7);
		pWaterLilyObject->SetMesh(pPineLeavesMesh);
		pWaterLilyObject->Scale(D3DXVECTOR3(0.6f, 0.6f, 0.6f));
		//pWaterLilyObject->Rotate(-90.0f, 90.0f, 0.0f);
		pWaterLilyObject->Rotate(&D3DXVECTOR3(1.0f, 0.0f, 0.0f), -90.0f);
		pWaterLilyObject->Rotate(&D3DXVECTOR3(0.0f, 0.0f, 1.0f), +90.0f); //y축 회전
		pWaterLilyObject->Rotate(&D3DXVECTOR3(1.0f, 0.0f, 0.0f), +20.0f); // x축
		if (k >=2)
		{
			pWaterLilyObject->Rotate(&D3DXVECTOR3(0.0f, 0.0f, 1.0f), 160.0f);
			pWaterLilyObject->Rotate(&D3DXVECTOR3(0.0f, 1.0f, 0.0f), -20.0f);
			pWaterLilyObject->Rotate(&D3DXVECTOR3(1.0f, 0.0f, 0.0f), -20.0f); //
			pWaterLilyObject->Rotate(&D3DXVECTOR3(1.0f, 1.0f, 0.0f), 60.0f);//x축
		}
			
		float fPosition[3];
		fscanf_s(fp, "%f %f %f  \n", &fPosition[0], &fPosition[1], &fPosition[2]);
		pWaterLilyObject->SetPosition(fPosition[0], fPosition[1], fPosition[2]);
		m_ppObjects[i] = pWaterLilyObject;
		i++;
	}

	m_pd3dPineLeavesInstanceBuffer = CreateInstanceBuffer(pd3dDevice, PINE_TREE_NUM / 2, m_nInstanceBufferStride, NULL);
	pPineLeavesMesh->AssembleToVertexBuffer(1, &m_pd3dPineLeavesInstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);
	fclose(fp);


	D3D11_RASTERIZER_DESC d3dRasterizerState;
	d3dRasterizerState.FrontCounterClockwise = true;
	d3dRasterizerState.FillMode = D3D11_FILL_SOLID;
	d3dRasterizerState.CullMode = D3D11_CULL_NONE;
	d3dRasterizerState.DepthBias = false;
	d3dRasterizerState.DepthBiasClamp = 0;
	d3dRasterizerState.SlopeScaledDepthBias = 0;
	d3dRasterizerState.DepthClipEnable = true;
	d3dRasterizerState.ScissorEnable = false;
	d3dRasterizerState.MultisampleEnable = false;
	d3dRasterizerState.AntialiasedLineEnable = false;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerState, &m_pd3dRasterizerState);


	D3D11_DEPTH_STENCIL_DESC d3dDepthSencilDesc;
	ZeroMemory(&d3dDepthSencilDesc, sizeof(d3dDepthSencilDesc));
	d3dDepthSencilDesc.DepthEnable = true;
	//계속 그릴 때 마다 카메라에 가까운 객체가 그려지기 때문에
	d3dDepthSencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	d3dDepthSencilDesc.DepthFunc = D3D11_COMPARISON_LESS;//카메라 더 가까운 놈이 깊이 검사 성공.
	d3dDepthSencilDesc.StencilEnable = true;
	d3dDepthSencilDesc.StencilReadMask = 0xff;
	d3dDepthSencilDesc.StencilWriteMask = 0xff;
	//OP_KEEP -> 어떤 상황에서도 스텐실 버퍼 값은 변경하지 않겠다.(거울에만 그려라) -> 그래도 스텐실 검사는 함.
	d3dDepthSencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;//버퍼 변경하지 않겠다.
	d3dDepthSencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthSencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//거울을 가리는 놈이 없다.
	d3dDepthSencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;// 

	d3dDepthSencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthSencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthSencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthSencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	//반사된 객체를 그릴 때만 사용.
	pd3dDevice->CreateDepthStencilState(&d3dDepthSencilDesc, &m_pd3dReflectDepthStencilState);
}

void CReflectionShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera)
{
	OnPrepareRender(pd3dDeviceContext);
	if (m_pMaterial)
		CIlluminatedShader::UpdateShaderVariable(pd3dDeviceContext, &m_pMaterial->m_Material);

	if (m_pTexture)
	{
		m_pTexture->UpdateTextureShaderVariable(pd3dDeviceContext, 0, 0);
		m_pTexture->UpdateSamplerShaderVariable(pd3dDeviceContext, 0, 0);
	}

	//660   660  660 660

	D3DXMATRIX d3dxmtxReflect;
	D3DXMatrixIdentity(&d3dxmtxReflect);
	D3DXPLANE reflectPlane(0.0f, 1.0f, 0.0f, -205.0f);
	D3DXMatrixReflect(&d3dxmtxReflect, &reflectPlane);



	pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);
	pd3dDeviceContext->OMSetDepthStencilState(m_pd3dReflectDepthStencilState, 1);






	// Render RootTree
	int nRootTreeInstances = 0;
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dRootTreeInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_VB_INSTANCE *pnRootTreeInstances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
	for (int j = 0;
		j < ROOT_TREE_NUM; j++)
	{
		if (m_ppObjects[j]->IsVisible(pCamera))
		{
			D3DXMATRIX d3dxmtxObject = m_ppObjects[j]->m_d3dxmtxWorld;
			D3DXMatrixReflect(&d3dxmtxReflect, &reflectPlane);
			D3DXMatrixMultiply(&m_ppObjects[j]->m_d3dxmtxWorld, &m_ppObjects[j]->m_d3dxmtxWorld, &d3dxmtxReflect);
			D3DXMatrixTranspose(&pnRootTreeInstances[nRootTreeInstances++].m_d3dxTransform, &m_ppObjects[j]->m_d3dxmtxWorld);
			m_ppObjects[j]->m_d3dxmtxWorld = d3dxmtxObject;
		}
	}
	pd3dDeviceContext->Unmap(m_pd3dRootTreeInstanceBuffer, 0);
	CMesh *pRootTreeMesh = m_ppObjects[ ROOT_TREE_NUM - 1]->GetMesh();
	pRootTreeMesh->RenderInstanced(pd3dDeviceContext, nRootTreeInstances, 0);



	if (m_pTexture)
		m_pTexture->UpdateTextureShaderVariable(pd3dDeviceContext, 1, 0);

	// Render PineLeaves


	D3DXMatrixIdentity(&d3dxmtxReflect);
	reflectPlane = D3DXPLANE(0.0f, 1.0f, 0.0f, -286.0f);
	D3DXMatrixReflect(&d3dxmtxReflect, &reflectPlane);



	int nPineLeavesInstances = 0;
	pd3dDeviceContext->Map(m_pd3dPineLeavesInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_VB_INSTANCE *pnPineLeavesInstances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
	for (int j = ROOT_TREE_NUM; j < m_nObjects; j++)
	{
		//if (m_ppObjects[j]->IsVisible(pCamera))
		{
			D3DXMATRIX d3dxmtxObject = m_ppObjects[j]->m_d3dxmtxWorld;
			D3DXMatrixReflect(&d3dxmtxReflect, &reflectPlane);
			D3DXMatrixMultiply(&m_ppObjects[j]->m_d3dxmtxWorld, &m_ppObjects[j]->m_d3dxmtxWorld, &d3dxmtxReflect);
			D3DXMatrixTranspose(&pnPineLeavesInstances[nPineLeavesInstances++].m_d3dxTransform, &m_ppObjects[j]->m_d3dxmtxWorld);
			m_ppObjects[j]->m_d3dxmtxWorld = d3dxmtxObject;
		}
	}
	pd3dDeviceContext->Unmap(m_pd3dPineLeavesInstanceBuffer, 0);
	CMesh *pPineLeavesMesh = m_ppObjects[m_nObjects - 1]->GetMesh();
	pPineLeavesMesh->RenderInstanced(pd3dDeviceContext, nPineLeavesInstances, 0);

	pd3dDeviceContext->RSSetState(NULL);
	pd3dDeviceContext->OMSetDepthStencilState(NULL, 0);
}




CAlphaBlendingMirrorShader::CAlphaBlendingMirrorShader()
{}
CAlphaBlendingMirrorShader::~CAlphaBlendingMirrorShader()
{}
void  CAlphaBlendingMirrorShader::CreateBlendingState(ID3D11Device *pd3dDevice)
{
	D3D11_BLEND_DESC d3dBlendDesc;
	ZeroMemory(&d3dBlendDesc, sizeof(d3dBlendDesc));
	d3dBlendDesc.AlphaToCoverageEnable = false;
	d3dBlendDesc.IndependentBlendEnable = false;
	d3dBlendDesc.RenderTarget[0].BlendEnable = false;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	//실제 그리더라도 블렌딩이 안 일어난다. 
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = 0;
	pd3dDevice->CreateBlendState(&d3dBlendDesc, &m_pd3dBlendingState);
}

void CAlphaBlendingMirrorShader::CreateDepthStencilState(ID3D11Device *pd3dDevice)
{
	D3D11_DEPTH_STENCIL_DESC d3dDepthSencilDesc;
	ZeroMemory(&d3dDepthSencilDesc, sizeof(d3dDepthSencilDesc));
	d3dDepthSencilDesc.DepthEnable = true;
	d3dDepthSencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	d3dDepthSencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	d3dDepthSencilDesc.StencilEnable = true;
	d3dDepthSencilDesc.StencilReadMask = 0xff;
	d3dDepthSencilDesc.StencilWriteMask = 0xff;
	d3dDepthSencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthSencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthSencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	d3dDepthSencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	d3dDepthSencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthSencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthSencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	d3dDepthSencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	pd3dDevice->CreateDepthStencilState(&d3dDepthSencilDesc, &m_pd3dMirrorToStencilState);
}

void CAlphaBlendingMirrorShader::BuildObjects(ID3D11Device *pd3dDevice)
{
	m_nObjects = 1;
	m_ppObjects = new CGameObject*[m_nObjects];
	CAlphaBlendingMirrorObject* pMirror = new CAlphaBlendingMirrorObject(pd3dDevice);
	m_ppObjects[0] = pMirror;
	//m_ppObjects[0]->SetPosition(D3DXVECTOR3(828.0f, 220.0f + 50.0f, 1050.0f));
	m_ppObjects[0]->SetPosition(D3DXVECTOR3(1310.0f, 246.0f, 1990.0f)); //220    1950

	CreateBlendingState(pd3dDevice);
	CreateDepthStencilState(pd3dDevice);
}


void CAlphaBlendingMirrorShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		//{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT nElements = ARRAYSIZE(d3dInputElements);
	CreateVertexShaderFromFile(pd3dDevice, L"Wave.fx", "VSTexturedColor", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Wave.fx", "PSTexturedColor", "ps_5_0", &m_pd3dPixelShader);
}

//스텐실 버퍼에 거울을 그린다.
void CAlphaBlendingMirrorShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera)
{
	float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	CShader::OnPrepareRender(pd3dDeviceContext);

	pd3dDeviceContext->OMSetDepthStencilState(m_pd3dMirrorToStencilState, 1);
	pd3dDeviceContext->OMSetBlendState(m_pd3dBlendingState, blendFactors, 0xfffffff);

	for (int i = 0; i < m_nObjects; i++)
		m_ppObjects[i]->Render(pd3dDeviceContext, pCamera);

	pd3dDeviceContext->OMSetBlendState(NULL, blendFactors, 0xffffffff);
	pd3dDeviceContext->OMSetDepthStencilState(NULL, 0);
}

















//===========================================
//===========================================
//===========================================

CGeneralShader::CGeneralShader()
{
}
CGeneralShader::~CGeneralShader()
{
}
void CGeneralShader::CreateShader(ID3D11Device *pd3dDevice)
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
void  CGeneralShader::BuildObjects(ID3D11Device *pd3dDevice)
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
	m_pTexture = new CTexture(3, 1, 0, 0);

	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/FixObject/Flower-Lily.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pTexture->SetTexture(0, pd3dsrvTexture);
	pd3dsrvTexture->Release();
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/FixObject/Wood-Root.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pTexture->SetTexture(1, pd3dsrvTexture);
	pd3dsrvTexture->Release();
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/FixObject/Rock-Multiple-01.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pTexture->SetTexture(2, pd3dsrvTexture);
	pd3dsrvTexture->Release();

	m_pTexture->SetSampler(0, pd3dSamplerState);
	pd3dSamplerState->Release();





	//에러의 원인
	m_nInstanceBufferStride = sizeof(VS_VB_INSTANCE);
	m_nInstanceBufferOffset = 0;

	string strFileName1 = "Data/NewWaterLily01_Vertex.txt";
	string strFileName2 = "Data/NewWaterLily02_Vertex.txt";
	// Stone mesh
	string strFileName3 = "Data/NewRootTree_Vertex.txt";
	string strFileName4 = "Data/NewFlatRock02_Vertex.txt";
	string strFileName5 = "Data/NewRockFlat03_Vertex.txt";
	string strFileName6 = "Data/NewRockTall01_Vertex.txt";

	CFixedMesh* pWaterLily01Mesh = new CFixedMesh(pd3dDevice, strFileName1);
	CFixedMesh* pWaterLily02Mesh = new CFixedMesh(pd3dDevice, strFileName2);
	CFixedMesh* pRootTreeMesh = new CFixedMesh(pd3dDevice, strFileName3);

	//CFixedMesh * pFlatRockMesh01 = new CFixedMesh(pd3dDevice, strFileName3);
	//CFixedMesh * pFlatRockMesh02 = new CFixedMesh(pd3dDevice, strFileName4);
	//CFixedMesh * pFlatRockMesh03 = new CFixedMesh(pd3dDevice, strFileName5);
	//CFixedMesh * pTallRockMesh01 = new CFixedMesh(pd3dDevice, strFileName6);



	int  i = 0;
	//m_nObjects = 200;
	m_nObjects = WATER_RILY01_NUM + WATER_RILY02_NUM + ROOT_TREE_NUM;
	m_ppObjects = new CGameObject*[m_nObjects];
	CWoodObject *pWaterLilyObject = NULL;
	/*구는 3가지 종류(재질에 따라)이다. 다른 재질의 구들이 번갈아 나열되도록 한다. 재질의 종류에 따라 k가 0, 1, 2의 값을 가지고 k에 따라 객체의 위치를 다르게 설정한다.*/

	int x = 0;
	int z;
	for (int x = 0; x < 2; x++)
	{
		for (int z = 0; z < 4; z++)
		{
			pWaterLilyObject = new  CWoodObject(pd3dDevice, strFileName1);
			pWaterLilyObject->SetMesh(pWaterLily01Mesh);
			pWaterLilyObject->Scale(D3DXVECTOR3(8.0, 8.0, 8.0));
			pWaterLilyObject->SetPosition((x + 1) * 300 + 0, 240.0f, (z + 1) *(300) + 0);
			m_ppObjects[i++] = pWaterLilyObject;
		}
	}
	m_pd3dWaterLily01InstanceBuffer = CreateInstanceBuffer(pd3dDevice, WATER_RILY01_NUM, m_nInstanceBufferStride, NULL);
	pWaterLily01Mesh->AssembleToVertexBuffer(1, &m_pd3dWaterLily01InstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);


	for (int x = 0; x < 2; x++)
	{
		for (int z = 0; z < 4; z++)
		{
			pWaterLilyObject = new  CWoodObject(pd3dDevice, strFileName2);
			pWaterLilyObject->SetMesh(pWaterLily02Mesh);
			pWaterLilyObject->Scale(D3DXVECTOR3(8.0, 8.0, 8.0));
			pWaterLilyObject->SetPosition((x + 1) * 320, 240.0f + 0, 10.0f + (z + 1) *(300) + 0);
			m_ppObjects[i++] = pWaterLilyObject;
		}
	}
	m_pd3dWaterLily02InstanceBuffer = CreateInstanceBuffer(pd3dDevice, WATER_RILY02_NUM, m_nInstanceBufferStride, NULL);
	pWaterLily02Mesh->AssembleToVertexBuffer(1, &m_pd3dWaterLily02InstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);


	for (int x = 0; x < 2; x++)
	{
		for (int z = 0; z < 2; z++)
		{
			pWaterLilyObject = new  CWoodObject(pd3dDevice, strFileName2);
			pWaterLilyObject->SetMesh(pRootTreeMesh);
			pWaterLilyObject->Scale(D3DXVECTOR3(7.0, 7.0, 7.0));
			float fAngle = rand() % 60;
			pWaterLilyObject->Rotate(&D3DXVECTOR3(0.0f, 1.0f, 0.0f), (float)D3DXToDegree(fAngle));
			pWaterLilyObject->SetPosition((x + 1) * 300 + 240, 230.0f, (z + 1) *(150) + 800);
			m_ppObjects[i++] = pWaterLilyObject;
		}
	}
	m_pd3dRootTreeInstanceBuffer = CreateInstanceBuffer(pd3dDevice, ROOT_TREE_NUM, m_nInstanceBufferStride, NULL);
	pRootTreeMesh->AssembleToVertexBuffer(1, &m_pd3dRootTreeInstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);



}

void CGeneralShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera)
{
	OnPrepareRender(pd3dDeviceContext);
	if (m_pMaterial)
		CIlluminatedShader::UpdateShaderVariable(pd3dDeviceContext, &m_pMaterial->m_Material);

	if (m_pTexture)
	{
		m_pTexture->UpdateSamplerShaderVariable(pd3dDeviceContext, 0, 0);
		m_pTexture->UpdateTextureShaderVariable(pd3dDeviceContext, 0, 0);
	}
	int nWaterLily01Objects = m_nObjects / 2;

	int nWaterLily01Instances = 0;
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dWaterLily01InstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_VB_INSTANCE *pnWaterLily01Instances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
	for (int j = 0; j < WATER_RILY01_NUM; j++)
	{
		if (m_ppObjects[j]->IsVisible(pCamera))
		{
			if (m_ppObjects[j]->IsVisible(pCamera))
				D3DXMatrixTranspose(&pnWaterLily01Instances[nWaterLily01Instances++].m_d3dxTransform, &m_ppObjects[j]->m_d3dxmtxWorld);
		}
	}
	pd3dDeviceContext->Unmap(m_pd3dWaterLily01InstanceBuffer, 0);

	CMesh *pWaterLily01Mesh = m_ppObjects[WATER_RILY01_NUM - 1]->GetMesh();
	pWaterLily01Mesh->RenderInstanced(pd3dDeviceContext, nWaterLily01Instances, 0);

	// Render WaterLily02 
	int nWaterLily02Instances = 0;
	pd3dDeviceContext->Map(m_pd3dWaterLily01InstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_VB_INSTANCE *pnWaterLily02Instances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
	for (int j = WATER_RILY01_NUM; j < WATER_RILY01_NUM + WATER_RILY02_NUM; j++)
	{
		if (m_ppObjects[j]->IsVisible(pCamera))
		{
			if (m_ppObjects[j]->IsVisible(pCamera))
				D3DXMatrixTranspose(&pnWaterLily02Instances[nWaterLily02Instances++].m_d3dxTransform, &m_ppObjects[j]->m_d3dxmtxWorld);
		}
	}
	pd3dDeviceContext->Unmap(m_pd3dWaterLily01InstanceBuffer, 0);

	CMesh *pWaterLily02Mesh = m_ppObjects[WATER_RILY01_NUM + WATER_RILY02_NUM - 1]->GetMesh();
	pWaterLily02Mesh->RenderInstanced(pd3dDeviceContext, nWaterLily02Instances, 0);

	if (m_pTexture)
		m_pTexture->UpdateTextureShaderVariable(pd3dDeviceContext, 1, 0);


	// Render RootTree
	int nRootTreeInstances = 0;
	pd3dDeviceContext->Map(m_pd3dRootTreeInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_VB_INSTANCE *pnRootTreeInstances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
	for (int j = WATER_RILY01_NUM + WATER_RILY02_NUM;
		j < WATER_RILY01_NUM + WATER_RILY02_NUM + ROOT_TREE_NUM; j++)
	{
		if (m_ppObjects[j]->IsVisible(pCamera))
		{
			if (m_ppObjects[j]->IsVisible(pCamera))
				D3DXMatrixTranspose(&pnRootTreeInstances[nRootTreeInstances++].m_d3dxTransform, &m_ppObjects[j]->m_d3dxmtxWorld);
		}
	}
	pd3dDeviceContext->Unmap(m_pd3dRootTreeInstanceBuffer, 0);

	CMesh *pRootTreeMesh = m_ppObjects[m_nObjects - 1]->GetMesh();
	pRootTreeMesh->RenderInstanced(pd3dDeviceContext, nRootTreeInstances, 0);

}
