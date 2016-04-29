#include "WaveShader.h"
#include "Direct3DBase.h"


ID3D11ShaderResourceView *CRefractionShader::m_pd3dRefractionShaderResourceView = NULL;
ID3D11ShaderResourceView * CReflectionShader::m_pd3dReflectionShaderResourceView = NULL;

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
	m_pMaterial = pMaterial;
	if (pMaterial) pMaterial->AddRef();


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
	if (m_pMaterial)
		CIlluminatedShader::UpdateShaderVariable(pd3dDeviceContext, &m_pMaterial->m_Material);
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
	//pd3dDeviceContext->PSSetShaderResources(0x04, 1, &CReflectionShader::m_pd3dReflectionShaderResourceView);
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


CReflectionShader::CReflectionShader()
{
}
CReflectionShader::~CReflectionShader()
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


bool CReflectionShader::CreateRenderToTexture(ID3D11Device *pd3dDevice, int textureWidth, int textureHeight)
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

void CReflectionShader::CreateShader(ID3D11Device *pd3dDevice)
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
void  CReflectionShader::BuildObjects(ID3D11Device *pd3dDevice)
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

void CReflectionShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera)
{
	OnPrepareRender(pd3dDeviceContext);
	if (m_pMaterial)
		CIlluminatedShader::UpdateShaderVariable(pd3dDeviceContext, &m_pMaterial->m_Material);

	if (m_pTexture)
	{
		m_pTexture->UpdateSamplerShaderVariable(pd3dDeviceContext, 0, 0);
		m_pTexture->UpdateTextureShaderVariable(pd3dDeviceContext, 0, 0);
	}


	// Clear the refraction render to texture.
	//다시 원래 카메라 행렬로 복구 시킨다. (오류의 원인이 될수도 있다.)
	pCamera->GenerateViewMatrix();
	pCamera->UpdateShaderVariables(pd3dDeviceContext);




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

	m_pDirect3D->SetBackBufferRenderTarget();
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
