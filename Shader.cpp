#include "Shader.h"
#include "GameFramework.h" //깜짝이야
using namespace std;
//월드 변환 행렬을 위한 상수 버퍼는 쉐이더 객체의 정적(static) 데이터 멤버이다.
ID3D11Buffer *CShader::m_pd3dcbWorldMatrix = NULL;
ID3D11Buffer *CIlluminatedShader::m_pd3dcbMaterial = NULL;


ID3D11Buffer *COtherPlayerShader::m_pd3dcbResult = NULL;
ID3D11Buffer *CPlayerShader::m_pd3dcbPlayerResult = NULL;

CShader::CShader()
{
	m_ppObjects = NULL;
	m_nObjects = 0;

	m_pd3dVertexShader = NULL;
	m_pd3dVertexLayout = NULL;
	m_pd3dPixelShader = NULL;
}

CShader::~CShader()
{
	if (m_pd3dVertexShader) m_pd3dVertexShader->Release();
	if (m_pd3dVertexLayout) m_pd3dVertexLayout->Release();
	if (m_pd3dPixelShader) m_pd3dPixelShader->Release();
	//테셀레이션 사용
	if (m_pd3dcbCameraPosition) m_pd3dcbCameraPosition->Release();
	if (m_pd3dHullShader) m_pd3dHullShader->Release();
	if (m_pd3dDomainShader) m_pd3dDomainShader->Release();
}

void CShader::BuildObjects(ID3D11Device *pd3dDevice)
{
}

void CShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
	/*		if (m_ppObjects[j])
				m_ppObjects[j]->Release();*/
			if (m_ppObjects[j])
				delete m_ppObjects[j];
		}
			
		delete[] m_ppObjects;
	}
}

void CShader::AnimateObjects(float fTimeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(fTimeElapsed);
	}
}

void CShader::OnPrepareRender(ID3D11DeviceContext *pd3dDeviceContext)
{
	pd3dDeviceContext->IASetInputLayout(m_pd3dVertexLayout);
	pd3dDeviceContext->VSSetShader(m_pd3dVertexShader, NULL, 0);
	pd3dDeviceContext->PSSetShader(m_pd3dPixelShader, NULL, 0);
}


void CShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera)
{
	OnPrepareRender(pd3dDeviceContext);

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

void CShader::CreateVertexShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel,
	ID3D11VertexShader **ppd3dVertexShader, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout, UINT nElements, ID3D11InputLayout **ppd3dVertexLayout)
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dShaderBlob = NULL, *pd3dErrorBlob = NULL;
	/*파일(pszFileName)에서 쉐이더 함수(pszShaderName)를 컴파일하여
	컴파일된 쉐이더 코드의 메모리 주소(pd3dShaderBlob)를 반환한다.*/
	if (SUCCEEDED(hResult = D3DX11CompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderModel, dwShaderFlags, 0, NULL, &pd3dShaderBlob, &pd3dErrorBlob, NULL)))
	{
		//컴파일된 쉐이더 코드의 메모리 주소에서 정점-쉐이더를 생성한다. 
		pd3dDevice->CreateVertexShader(pd3dShaderBlob->GetBufferPointer(), pd3dShaderBlob->GetBufferSize(), NULL, ppd3dVertexShader);
		//컴파일된 쉐이더 코드의 메모리 주소와 입력 레이아웃에서 정점 레이아웃을 생성한다. 
		pd3dDevice->CreateInputLayout(pd3dInputLayout, nElements, pd3dShaderBlob->GetBufferPointer(), pd3dShaderBlob->GetBufferSize(), ppd3dVertexLayout);
		pd3dShaderBlob->Release();
	}
	if (pd3dErrorBlob != 0)
	{
		MessageBoxA(0, (char*)pd3dErrorBlob->GetBufferPointer(), 0, 0);
	}
}
void CShader::CreatePixelShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11PixelShader **ppd3dPixelShader)
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dShaderBlob = NULL, *pd3dErrorBlob = NULL;
	/*파일(pszFileName)에서 쉐이더 함수(pszShaderName)를 컴파일하여 컴파일된 쉐이더 코드의 메모리 주소(pd3dShaderBlob)를 반환한다.*/
	if (SUCCEEDED(hResult = D3DX11CompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderModel, dwShaderFlags, 0, NULL, &pd3dShaderBlob, &pd3dErrorBlob, NULL)))
	{
		//컴파일된 쉐이더 코드의 메모리 주소에서 픽셀-쉐이더를 생성한다. 
		pd3dDevice->CreatePixelShader(pd3dShaderBlob->GetBufferPointer(), pd3dShaderBlob->GetBufferSize(), NULL, ppd3dPixelShader);
		pd3dShaderBlob->Release();
	}
	if (pd3dErrorBlob != 0)
	{
		MessageBoxA(0, (char*)pd3dErrorBlob->GetBufferPointer(), 0, 0);
	}
}
void CShader::CreateHullShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11HullShader **ppd3dHullShader)
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dShaderBlob = NULL, *pd3dErrorBlob = NULL;
	/*파일(pszFileName)에서 쉐이더 함수(pszShaderName)를 컴파일하여 컴파일된 쉐이더 코드의 메모리 주소(pd3dShaderBlob)를 반환한다.*/
	if (SUCCEEDED(hResult = D3DX11CompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderModel, dwShaderFlags, 0, NULL, &pd3dShaderBlob, &pd3dErrorBlob, NULL)))
	{
		//컴파일된 쉐이더 코드의 메모리 주소에서 픽셀-쉐이더를 생성한다. 
		pd3dDevice->CreateHullShader(pd3dShaderBlob->GetBufferPointer(), pd3dShaderBlob->GetBufferSize(), NULL, ppd3dHullShader);
		pd3dShaderBlob->Release();
	}
	if (pd3dErrorBlob != 0)
	{
		MessageBoxA(0, (char*)pd3dErrorBlob->GetBufferPointer(), 0, 0);
	}
}

void CShader::CreateDomainShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11DomainShader **ppd3dDomainShader)
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dShaderBlob = NULL, *pd3dErrorBlob = NULL;
	/*파일(pszFileName)에서 쉐이더 함수(pszShaderName)를 컴파일하여 컴파일된 쉐이더 코드의 메모리 주소(pd3dShaderBlob)를 반환한다.*/
	if (SUCCEEDED(hResult = D3DX11CompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderModel, dwShaderFlags, 0, NULL, &pd3dShaderBlob, &pd3dErrorBlob, NULL)))
	{
		//컴파일된 쉐이더 코드의 메모리 주소에서 픽셀-쉐이더를 생성한다. 
		pd3dDevice->CreateDomainShader(pd3dShaderBlob->GetBufferPointer(), pd3dShaderBlob->GetBufferSize(), NULL, ppd3dDomainShader);
		pd3dShaderBlob->Release();
	}
	if (pd3dErrorBlob != 0)
	{
		MessageBoxA(0, (char*)pd3dErrorBlob->GetBufferPointer(), 0, 0);
	}
}


BYTE *ReadCompiledEffectFile(WCHAR *pszFileName, int *pnReadBytes)
{
	FILE *pFile = NULL;
	::_wfopen_s(&pFile, pszFileName, L"rb");
	::fseek(pFile, 0, SEEK_END);
	int nFileSize = ::ftell(pFile);
	BYTE *pByteCode = new BYTE[nFileSize];
	::rewind(pFile);
	*pnReadBytes = ::fread(pByteCode, sizeof(BYTE), nFileSize, pFile);
	::fclose(pFile);
	return(pByteCode);
}

//컴파일된 쉐이더 코드에서 정점 쉐이더를 생성한다.
void CShader::CreateVertexShaderFromCompiledFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout, UINT nElements)
{
	int nReadBytes = 0;
	BYTE *pByteCode = ReadCompiledEffectFile(pszFileName, &nReadBytes);
	HRESULT hResult = pd3dDevice->CreateVertexShader(pByteCode, nReadBytes, NULL, &m_pd3dVertexShader);
	pd3dDevice->CreateInputLayout(pd3dInputLayout, nElements, pByteCode, nReadBytes, &m_pd3dVertexLayout);

	if (pByteCode) delete[] pByteCode;
}

//컴파일된 쉐이더 코드에서 픽셀 쉐이더를 생성한다.
void CShader::CreatePixelShaderFromCompiledFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName)
{
	int nReadBytes = 0;
	BYTE *pByteCode = ReadCompiledEffectFile(pszFileName, &nReadBytes);
	HRESULT hResult = pd3dDevice->CreatePixelShader(pByteCode, nReadBytes, NULL, &m_pd3dPixelShader);

	if (pByteCode) delete[] pByteCode;
}


void CShader::CreateHullShaderFromCompiledFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName)
{
	int nReadBytes = 0;
	BYTE *pByteCode = ReadCompiledEffectFile(pszFileName, &nReadBytes);
	HRESULT hResult = pd3dDevice->CreateHullShader(pByteCode, nReadBytes, NULL, &m_pd3dHullShader);

	if (pByteCode) delete[] pByteCode;
}
void CShader::CreateDomainShaderFromCompiledFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName)
{
	int nReadBytes = 0;
	BYTE *pByteCode = ReadCompiledEffectFile(pszFileName, &nReadBytes);
	HRESULT hResult = pd3dDevice->CreateDomainShader(pByteCode, nReadBytes, NULL, &m_pd3dDomainShader);

	if (pByteCode) delete[] pByteCode;
}


void CShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);


	//컴파일된 쉐이더 코드의 이름이 VS.fxo와 PS.fxo이다.
	//CreateVertexShaderFromCompiledFile(pd3dDevice, L"Effect_VS.fxo", d3dInputLayout, nElements);
	//CreatePixelShaderFromCompiledFile(pd3dDevice, L"Effect_PS.fxo");



	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSDiffusedColor", "vs_5_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSDiffusedColor", "ps_5_0", &m_pd3dPixelShader);
}

void CShader::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	//월드 변환 행렬을 위한 상수 버퍼를 생성한다.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_WORLD_MATRIX);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbWorldMatrix);
}
void CShader::ReleaseShaderVariables()
{
	//월드 변환 행렬을 위한 상수 버퍼를 반환한다.
	if (m_pd3dcbWorldMatrix)
		m_pd3dcbWorldMatrix->Release();
}
void CShader::UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld)
{
	//월드 변환 행렬을 상수 버퍼에 복사한다.
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbWorldMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_CB_WORLD_MATRIX *pcbWorldMatrix = (VS_CB_WORLD_MATRIX *)d3dMappedResource.pData;
	D3DXMatrixTranspose(&pcbWorldMatrix->m_d3dxmtxWorld, pd3dxmtxWorld);
	pd3dDeviceContext->Unmap(m_pd3dcbWorldMatrix, 0);

	//상수 버퍼를 디바이스의 슬롯(VS_SLOT_WORLD_MATRIX)에 연결한다.
	pd3dDeviceContext->VSSetConstantBuffers(VS_SLOT_WORLD_MATRIX, 1, &m_pd3dcbWorldMatrix);
}


//=====================테셀레이션===================
void CShader::UpdateWorldMatrixForTess(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld)
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbWorldMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_CB_WORLD_MATRIX *pcbWorldMatrix = (VS_CB_WORLD_MATRIX *)d3dMappedResource.pData;
	D3DXMatrixTranspose(&pcbWorldMatrix->m_d3dxmtxWorld, pd3dxmtxWorld);
	pd3dDeviceContext->Unmap(m_pd3dcbWorldMatrix, 0);

	//상수 버퍼를 디바이스의 슬롯(VS_SLOT_WORLD_MATRIX)에 연결한다.
	pd3dDeviceContext->DSSetConstantBuffers(VS_SLOT_WORLD_MATRIX, 1, &m_pd3dcbWorldMatrix);
	pd3dDeviceContext->HSSetConstantBuffers(VS_SLOT_WORLD_MATRIX, 1, &m_pd3dcbWorldMatrix);
}




CGameObject *CShader::PickObjectByRayIntersection(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxView, MESHINTERSECTINFO *pd3dxIntersectInfo)
{
	int nIntersected = 0;
	float fNearHitDistance = FLT_MAX;
	CGameObject *pSelectedObject = NULL;
	MESHINTERSECTINFO d3dxIntersectInfo;
	/*쉐이더 객체에 포함되는 모든 객체들에 대하여 픽킹 광선을 생성하고 객체의 바운딩 박스와의 교차를 검사한다. 교차하는 객체들 중에 카메라와 가장 가까운 객체의 정보와 객체를 반환한다.*/
	for (int i = 0; i < m_nObjects; i++)
	{
		nIntersected = m_ppObjects[i]->PickObjectByRayIntersection(pd3dxvPickPosition, pd3dxmtxView, &d3dxIntersectInfo);
		if ((nIntersected > 0) && (d3dxIntersectInfo.m_fDistance < fNearHitDistance))
		{
			fNearHitDistance = d3dxIntersectInfo.m_fDistance;
			pSelectedObject = m_ppObjects[i];
			if (pd3dxIntersectInfo) *pd3dxIntersectInfo = d3dxIntersectInfo;
		}
	}
	return(pSelectedObject);
}


CSceneShader::CSceneShader()
{
}

CSceneShader::~CSceneShader()
{
}
void CSceneShader::CreateShader(ID3D11Device *pd3dDevice)
{
	CShader::CreateShader(pd3dDevice);
}

void CSceneShader::BuildObjects(ID3D11Device *pd3dDevice)
{

}
//=====================================


CPlayerShader::CPlayerShader()
{
}

CPlayerShader::~CPlayerShader()
{
	if (m_pTexture) m_pTexture->Release();
}


void CPlayerShader::CreateShader(ID3D11Device *pd3dDevice)
{
	//CShader::CreateShader(pd3dDevice);
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 3, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 4, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 5, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE", 0, DXGI_FORMAT_R32G32B32A32_UINT, 6, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);

	//CreateVertexShaderFromCompiledFile(pd3dDevice, L"Human_VS.fxo", d3dInputElements, nElements);
	//CreatePixelShaderFromCompiledFile(pd3dDevice, L"Human_PS.fxo");
	CreateVertexShaderFromFile(pd3dDevice, L"Shader.fx", "VS", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Shader.fx", "PS", "ps_5_0", &m_pd3dPixelShader);
}

void CPlayerShader::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	//월드 변환 행렬을 위한 상수 버퍼를 생성한다.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_RESULT_MATRIX);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	if (pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbPlayerResult) == S_OK)
	{
		std::cout << "success" << std::endl;

	}
}


void CPlayerShader::BuildObjects(ID3D11Device *pd3dDevice, CCharacterMesh *pWarriorMesh)
{
	m_nObjects = 1;
	m_ppObjects = new CGameObject*[m_nObjects];

	//m_ppObjects[0]->Scale(D3DXVECTOR3(0.3f, 0.3f, 0.3f));
	CMaterial *pPlayerMaterial = new CMaterial();
	pPlayerMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(0.6f, 0.2f, 0.2f, 1.0f);
	pPlayerMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
	pPlayerMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 5.0f);
	pPlayerMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.2f, 1.0f);

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
	m_pTexture = new CTexture(1, 1, 0, 0);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/MovingObject/StoneKing.dds"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pTexture->SetTexture(0, pd3dsrvTexture);
	m_pTexture->SetSampler(0, pd3dSamplerState);
	pd3dsrvTexture->Release();
	pd3dSamplerState->Release();



	//string strFileName = "Data/warrior_Vertex.txt";
	//CCharacterMesh* pPlayerMesh = new CCharacterMesh(pd3dDevice, strFileName);
	CTerrainPlayer *pTerrainPlayer = new CTerrainPlayer(pd3dDevice, 1);
	pTerrainPlayer->SetMesh(pWarriorMesh);

	AnimationClip* animationClip = NULL;
	//animationClip = new AnimationClip;
	animationClip = CreateAnimation(animationClip);


	//pTerrainPlayer->SetMesh(pCubeMesh);
	//pTerrainPlayer->Scale(D3DXVECTOR3(0.3f, 0.3f, 0.3f));
	pTerrainPlayer->CreateShaderVariables(pd3dDevice);
	pTerrainPlayer->ChangeCamera(pd3dDevice, THIRD_PERSON_CAMERA, 0.0f);
	pTerrainPlayer->SetMaterial(pPlayerMaterial);
	pTerrainPlayer->SetAnimationClip(animationClip);

	m_ppObjects[0] = pTerrainPlayer;



	CGameManager* pGameManager = CGameManager::GetCGameManager();
	pGameManager->m_pPlayers[0] = m_ppObjects[0];



	string strFileName = "Data/warrior_Vertex.txt";
	//CWizardObject* pWizardObject1 = new CWizardObject(pd3dDevice, strFileName);
	//CWizardObject* pWizardObject2 = new CWizardObject(pd3dDevice, strFileName);
	//CWizardObject* pWizardObject3 = new CWizardObject(pd3dDevice, strFileName);


	////CCharacterMesh *pHumanMesh = new CCharacterMesh(pd3dDevice, strFileName);
	//pWizardObject1->SetMesh(pWarriorMesh);
	//pWizardObject2->SetMesh(pWarriorMesh);
	//pWizardObject3->SetMesh(pWarriorMesh);




	//m_ppObjects[1] = pWizardObject1;
	//m_ppObjects[2] = pWizardObject2;
	//m_ppObjects[3] = pWizardObject3;

	//m_ppObjects[1]->SetPosition(D3DXVECTOR3(200, 260.0f, 200.0f));
	//m_ppObjects[2]->SetPosition(D3DXVECTOR3(200, 260.0f, 400.0f));
	//m_ppObjects[3]->SetPosition(D3DXVECTOR3(200, 260.0f, 600.0f));



	
}
AnimationClip* CPlayerShader::CreateAnimation(AnimationClip* animationClip)
{
	UINT uiAnimationClipNums = 5;
	animationClip = new AnimationClip[uiAnimationClipNums];
	animationClip[0].m_nAnimationState = ANIMATAION_CLIP_IDLE;
	animationClip[1].m_nAnimationState = ANIMATAION_CLIP_ATTACK1;
	animationClip[2].m_nAnimationState = ANIMATAION_CLIP_ATTACK2;
	animationClip[3].m_nAnimationState = ANIMATAION_CLIP_RUN;
	animationClip[4].m_nAnimationState = ANIMATAION_CLIP_DEATH;

	animationClip[0].m_strFileName = "StoneKing_AttackReady.txt";
	animationClip[1].m_strFileName = "StoneKing_Attack3-1.txt";
	animationClip[2].m_strFileName = "StoneKing_Attack1.txt";
	animationClip[3].m_strFileName = "StoneKing_Run.txt";
	animationClip[4].m_strFileName = "StoneKing_Death.txt";

	for (int i = 0; i < uiAnimationClipNums; i++)
	{
		LoadAnimation(animationClip, i);
		//cout << i << "번째 로딩 성공" << endl;
	}
	return animationClip;
}
void CPlayerShader::LoadAnimation(AnimationClip* animationClip, UINT k)
{
	FILE *pFile = NULL;
	long long llAniTime = 0;
	string strFileName = "Data/";
	strFileName += animationClip[k].m_strFileName;
	//_wfopen_s(&pFile, L"Data/firstStepBox001_matrix.txt", L"rt");
	fopen_s(&pFile, strFileName.c_str(), "rt");
	fscanf_s(pFile, "%d \n", &llAniTime);
	fscanf_s(pFile, "%d \n", &animationClip[k].m_uiBoneIndexCount);
	animationClip[k].m_llAniTime = llAniTime;

	animationClip[k].m_ppResultMatrix = new D3DXMATRIX*[animationClip[k].m_llAniTime / 10];
	animationClip[k].m_fTimePos = 0.0f;
	animationClip[k].llNowTime = 0;


	for (long long i = 0; i < animationClip[k].m_llAniTime / 10; ++i)
	{
		// 최종 행렬 -> 매 초마다 해당하는    (포인터 배열을 가지고 있는)
		animationClip[k].m_ppResultMatrix[i] = new D3DXMATRIX[animationClip[k].m_uiBoneIndexCount];
	}

	for (long long i = 0; i <animationClip[k].m_llAniTime / 10; ++i)
	{
		for (unsigned int j = 0; j < animationClip[k].m_uiBoneIndexCount; ++j)
		{
			//cout << i << "  " << j << endl;
			fscanf_s(pFile, "%f %f %f %f  \n",
				&animationClip[k].m_ppResultMatrix[i][j]._11, &animationClip[k].m_ppResultMatrix[i][j]._12, &animationClip[k].m_ppResultMatrix[i][j]._13, &animationClip[k].m_ppResultMatrix[i][j]._14);
			fscanf_s(pFile, "%f %f %f %f  \n",
				&animationClip[k].m_ppResultMatrix[i][j]._21, &animationClip[k].m_ppResultMatrix[i][j]._22, &animationClip[k].m_ppResultMatrix[i][j]._23, &animationClip[k].m_ppResultMatrix[i][j]._24);
			fscanf_s(pFile, "%f %f %f %f  \n",
				&animationClip[k].m_ppResultMatrix[i][j]._31, &animationClip[k].m_ppResultMatrix[i][j]._32, &animationClip[k].m_ppResultMatrix[i][j]._33, &animationClip[k].m_ppResultMatrix[i][j]._34);
			fscanf_s(pFile, "%f %f %f %f  \n",
				&animationClip[k].m_ppResultMatrix[i][j]._41, &animationClip[k].m_ppResultMatrix[i][j]._42, &animationClip[k].m_ppResultMatrix[i][j]._43, &animationClip[k].m_ppResultMatrix[i][j]._44);
		}
	}

	::fclose(pFile);
}




void CPlayerShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera)
{
	//3인칭 카메라일 때 플레이어를 렌더링한다.
	DWORD nCameraMode = (pCamera) ? pCamera->GetMode() : 0x00;
	if (nCameraMode == THIRD_PERSON_CAMERA)
	{
		OnPrepareRender(pd3dDeviceContext);


		if (m_pTexture)
			m_pTexture->UpdateTextureShaderVariable(pd3dDeviceContext);

		for (int j = 0; j < m_nObjects; j++)
		{
			if (m_ppObjects[j])
			{
				// 플레이어가 움직이는지 확인하는 함수..... 실제로 다른가보다.(가리키는 객체가 다른건가???)
				//m_ppObjects[j]->CheckMove(GameTimer->GetTimeElapsed()*50);
				if (m_ppObjects[j]->IsVisible(pCamera))
				{
					m_ppObjects[j]->Render(pd3dDeviceContext, pCamera);

				}
			}
		}
	}
}



CInstancingShader::CInstancingShader()
{
	m_pd3dCubeInstanceBuffer = NULL;
	m_pd3dSphereInstanceBuffer = NULL;
	m_pMaterial = NULL;
	m_pTexture = NULL;
}

CInstancingShader::~CInstancingShader()
{
	if (m_pd3dCubeInstanceBuffer) m_pd3dCubeInstanceBuffer->Release();
	if (m_pd3dSphereInstanceBuffer) m_pd3dSphereInstanceBuffer->Release();
	if (m_pMaterial) m_pMaterial->Release();
	if (m_pTexture) m_pTexture->Release();
}

void CInstancingShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "INSTANCEPOS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 3, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEPOS", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 3, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEPOS", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 3, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEPOS", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 3, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSInstancedTexturedLightingColor", "vs_5_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSInstancedTexturedLightingColor", "ps_5_0", &m_pd3dPixelShader);

	// VSInstancedTexturedColor

	// VSInstancedTexturedLightingColor
}


ID3D11Buffer *CInstancingShader::CreateInstanceBuffer(ID3D11Device *pd3dDevice, int nObjects, UINT nBufferStride, void *pBufferData)
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

void CInstancingShader::BuildObjects(ID3D11Device *pd3dDevice, CHeightMapTerrain *pHeightMapTerrain, CMaterial *pMaterial, CTexture *pTexture, int k)
{
	m_pMaterial = pMaterial;
	if (pMaterial) pMaterial->AddRef();
	m_pTexture = pTexture;
	if (pTexture) pTexture->AddRef();


	//에러의 원인
	//m_nInstanceBufferStride = sizeof(D3DXMATRIX);
	m_nInstanceBufferStride = sizeof(VS_VB_INSTANCE);
	m_nInstanceBufferOffset = 0;

	//CCubeMeshTextured *pCubeMesh = new CCubeMeshTextured(pd3dDevice, 12.0f, 12.0f, 12.0f);
	//CSphereMeshTextured *pSphereMesh = new CSphereMeshTextured(pd3dDevice, 12.0f, 20, 20);

	CCubeMeshTexturedIlluminated *pCubeMesh = new CCubeMeshTexturedIlluminated(pd3dDevice, 12.0f, 12.0f, 12.0f);
	CSphereMeshTexturedIlluminated *pSphereMesh = new CSphereMeshTexturedIlluminated(pd3dDevice, 12.0f, 12.0f, 12.0f);
	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pHeightMapTerrain->GetWidth();
	float fTerrainLength = pHeightMapTerrain->GetLength();

	/*두 가지(직육면체와 구) 객체들을 지형에 일정한 간격으로 배치한다. 지형의 표면에 직육면체를 배치하고 직육면체 위에 구가 배치된다. 직육면체와 구는 빨강색, 녹색, 파랑색이 반복되도록 배치된다.*/
	int xObjects = int(fTerrainWidth / (fxPitch * 3.0f)), yObjects = 2, zObjects = int(fTerrainLength / (fzPitch * 3.0f)), i = 0;
	m_nObjects = xObjects * yObjects * zObjects;

	m_ppObjects = new CGameObject*[m_nObjects];

	D3DXVECTOR3 d3dxvRotateAxis;
	CRotatingObject *pRotatingObject = NULL;

	/*구는 3가지 종류(재질에 따라)이다. 다른 재질의 구들이 번갈아 나열되도록 한다. 재질의 종류에 따라 k가 0, 1, 2의 값을 가지고 k에 따라 객체의 위치를 다르게 설정한다.*/
	for (int x = 0; x < xObjects; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			pRotatingObject = new CRotatingObject();
			pRotatingObject->SetMesh(pSphereMesh);
			float xPosition = (k * fxPitch) + (x * fxPitch * 3.0f);
			float zPosition = (k * fzPitch) + (z * fxPitch * 3.0f);
			float fHeight = pHeightMapTerrain->GetHeight(xPosition, zPosition);
			pRotatingObject->SetPosition(xPosition, fHeight + (fyPitch * 4), zPosition);
			pRotatingObject->SetRotationAxis(D3DXVECTOR3(0.0f, 1.0f, 0.0f));
			pRotatingObject->SetRotationSpeed(36.0f * (i % 10) + 36.0f);
			m_ppObjects[i++] = pRotatingObject;
		}
	}
	m_pd3dSphereInstanceBuffer = CreateInstanceBuffer(pd3dDevice, xObjects * zObjects, m_nInstanceBufferStride, NULL);
	pSphereMesh->AssembleToVertexBuffer(1, &m_pd3dSphereInstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);

	for (int x = 0; x < xObjects; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			pRotatingObject = new CRotatingObject();
			pRotatingObject->SetMesh(pCubeMesh);
			float xPosition = (k * fxPitch) + (x * fxPitch * 3.0f);
			float zPosition = (k * fzPitch) + (z * fxPitch * 3.0f);
			float fHeight = pHeightMapTerrain->GetHeight(xPosition, zPosition);
			pRotatingObject->SetPosition(xPosition, fHeight + 6.0f, zPosition);
			D3DXVECTOR3 d3dxvSurfaceNormal = pHeightMapTerrain->GetNormal(xPosition, zPosition);
			D3DXVec3Cross(&d3dxvRotateAxis, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), &d3dxvSurfaceNormal);
			float fAngle = acos(D3DXVec3Dot(&D3DXVECTOR3(0.0f, 1.0f, 0.0f), &d3dxvSurfaceNormal));
			pRotatingObject->Rotate(&d3dxvRotateAxis, (float)D3DXToDegree(fAngle));
			pRotatingObject->SetRotationAxis(D3DXVECTOR3(0.0f, 1.0f, 0.0f));
			pRotatingObject->SetRotationSpeed(18.0f * (i % 10) + 10.0f);
			m_ppObjects[i++] = pRotatingObject;
		}
	}
	m_pd3dCubeInstanceBuffer = CreateInstanceBuffer(pd3dDevice, xObjects * zObjects, m_nInstanceBufferStride, NULL);
	pCubeMesh->AssembleToVertexBuffer(1, &m_pd3dCubeInstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);
}


void CInstancingShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera)
{
	OnPrepareRender(pd3dDeviceContext);
	if (m_pMaterial)
		CIlluminatedShader::UpdateShaderVariable(pd3dDeviceContext, &m_pMaterial->m_Material);

	if (m_pTexture) m_pTexture->UpdateShaderVariable(pd3dDeviceContext);

	int nSphereObjects = m_nObjects / 2;

	int nSphereInstances = 0;
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dSphereInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_VB_INSTANCE *pnSphereInstances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
	for (int j = 0; j < nSphereObjects; j++)
	{
		if (m_ppObjects[j]->IsVisible(pCamera))
		{
			if (m_ppObjects[j]->IsVisible(pCamera))
			{
				D3DXMatrixTranspose(&pnSphereInstances[nSphereInstances].m_d3dxTransform, &m_ppObjects[j]->m_d3dxmtxWorld);
				//pnSphereInstances[nSphereInstances++].m_d3dxColor = RANDOM_COLOR;
			}
		}
	}
	pd3dDeviceContext->Unmap(m_pd3dSphereInstanceBuffer, 0);

	CMesh *pSphereMesh = m_ppObjects[0]->GetMesh();
	pSphereMesh->RenderInstanced(pd3dDeviceContext, nSphereInstances, 0);




	int nCubeInstances = 0;
	pd3dDeviceContext->Map(m_pd3dCubeInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_VB_INSTANCE *pCubeInstances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
	for (int j = nSphereObjects; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			if (m_ppObjects[j]->IsVisible(pCamera))
			{
				D3DXMatrixTranspose(&pCubeInstances[nCubeInstances].m_d3dxTransform, &m_ppObjects[j]->m_d3dxmtxWorld);
				//pCubeInstances[nCubeInstances++].m_d3dxColor = RANDOM_COLOR;
			}
		}
	}
	pd3dDeviceContext->Unmap(m_pd3dCubeInstanceBuffer, 0);

	CMesh *pCubeMesh = m_ppObjects[m_nObjects - 1]->GetMesh();
	pCubeMesh->RenderInstanced(pd3dDeviceContext, nCubeInstances, 0);
}

CTerrainShader::CTerrainShader()
{
}

CTerrainShader::~CTerrainShader()
{
	if (m_d3dsrcTextureArray) m_d3dsrcTextureArray->Release();
}

void CTerrainShader::BuildObjects(ID3D11Device *pd3dDevice)
{
	m_nObjects = 1;
	m_ppObjects = new CGameObject*[m_nObjects];

	//지형은 텍스쳐가 2개이므로 2개의 샘플러 객체가 필요하다. 
	ID3D11SamplerState *pd3dBaseSamplerState = NULL;
	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;
	//pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dBaseSamplerState);

	ID3D11SamplerState *pd3dDetailSamplerState = NULL;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dDetailSamplerState);
	///
	CTexture *pTerrainTexture = new CTexture(1, 1, 0, 0);
	//ID3D11ShaderResourceView *pd3dsrvBaseTexture = NULL;
	//D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/Terrain/grass-diffuse.png"), NULL, NULL, &pd3dsrvBaseTexture, NULL);
	//pTerrainTexture->SetTexture(0, pd3dsrvBaseTexture);
	//pTerrainTexture->SetSampler(0, pd3dBaseSamplerState);
	//pd3dsrvBaseTexture->Release();
	//pd3dBaseSamplerState->Release();

	//ID3D11ShaderResourceView *pd3dsrvDetailTexture = NULL;
	//D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/Terrain/Detail_Texture_7.jpg"), NULL, NULL, &pd3dsrvDetailTexture, NULL);
	//pTerrainTexture->SetTexture(1, pd3dsrvDetailTexture);
	//pTerrainTexture->SetSampler(1, pd3dDetailSamplerState);
	//pd3dsrvDetailTexture->Release();

	ID3D11ShaderResourceView *pd3dsrvBlendMap = NULL;
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/Terrain/blend3.dds"), NULL, NULL, &pd3dsrvBlendMap, NULL);
	pTerrainTexture->SetTexture(0, pd3dsrvBlendMap);
	pTerrainTexture->SetSampler(0, pd3dDetailSamplerState);
	pd3dsrvBlendMap->Release();

	pd3dDetailSamplerState->Release();
	// 17*4
	// bigMap2  , 513, 513, 34, 34
	D3DXVECTOR3 d3dxvScale(8.0f, 2.0f, 8.0f); //"Image/black.raw" 257  // T("Image/9.raw"), 513, 513, 513, 513
	m_ppObjects[0] = new CHeightMapTerrain(pd3dDevice, _T("Image/Terrain/terrain.raw"), 1025, 1025, 33, 33, d3dxvScale);
	//m_ppObjects[0] = new CHeightMapTerrain(pd3dDevice, _T("Image/Terrain/bigMap2.raw"), 513, 513, 33, 33, d3dxvScale);

	m_ppObjects[0]->SetTexture(pTerrainTexture);

	m_d3dsrcTextureArray = CreateTexture2DArray(pd3dDevice, _T("TreeArray/tree"), 3);





	CMaterial *pTerrainMaterial = new CMaterial();
	//pTerrainMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(0.8f, 1.0f, 0.2f, 1.0f);
	//pTerrainMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.3f, 0.1f, 1.0f);
	//pTerrainMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 5.0f);
	//pTerrainMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);



	pTerrainMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pTerrainMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pTerrainMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 64.0f);
	pTerrainMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);


	m_ppObjects[0]->SetMaterial(pTerrainMaterial);
}

ID3D11ShaderResourceView* CTerrainShader::CreateTexture2DArray(ID3D11Device* pd3dDevice, TCHAR* pPath, int nTextures)
{
	D3DX11_IMAGE_LOAD_INFO d3dxImageLoadInfo;
	d3dxImageLoadInfo.Width = D3DX11_FROM_FILE;
	d3dxImageLoadInfo.Height = D3DX11_FROM_FILE;
	d3dxImageLoadInfo.Depth = D3DX11_FROM_FILE;
	d3dxImageLoadInfo.FirstMipLevel = 0;
	d3dxImageLoadInfo.MipLevels = D3DX11_FROM_FILE;
	d3dxImageLoadInfo.Usage = D3D11_USAGE_STAGING;
	d3dxImageLoadInfo.BindFlags = 0;
	d3dxImageLoadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	d3dxImageLoadInfo.MiscFlags = 0;
	d3dxImageLoadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	d3dxImageLoadInfo.Filter = D3DX11_FILTER_NONE;
	d3dxImageLoadInfo.MipFilter = D3DX11_FILTER_LINEAR;
	d3dxImageLoadInfo.pSrcInfo = 0;

	ID3D11Texture2D** pd3dTextures = new ID3D11Texture2D*[nTextures];
	_TCHAR pstrTexturesName[80];
	//for (int i = 0; i < nTextures; i++)
	//{
	//	_stprintf_s(pstrTexturesName, 80, _T("%s%02d.dds"), pPath, i);
	//	D3DX11CreateTextureFromFile(pd3dDevice, pstrTexturesName, &d3dxImageLoadInfo, 0,
	//		(ID3D11Resource**)&pd3dTextures[i], 0);
	//}

	D3DX11CreateTextureFromFile(pd3dDevice, _T("Image/Terrain/grass-diffuse.png"), &d3dxImageLoadInfo, 0,
		(ID3D11Resource**)&pd3dTextures[0], 0);
	D3DX11CreateTextureFromFile(pd3dDevice, _T("Image/Terrain/dirt-diffuse.png"), &d3dxImageLoadInfo, 0,
		(ID3D11Resource**)&pd3dTextures[1], 0);
	D3DX11CreateTextureFromFile(pd3dDevice, _T("Image/Terrain/Terrain-RockPath.png"), &d3dxImageLoadInfo, 0,
		(ID3D11Resource**)&pd3dTextures[2], 0);

	//D3DX11CreateTextureFromFile(pd3dDevice, _T("TreeArray/Tree3.dds"), &d3dxImageLoadInfo, 0,
	//	(ID3D11Resource**)&pd3dTextures[3], 0);
	D3D11_TEXTURE2D_DESC d3xTexture2DDesc;
	ZeroMemory(&d3xTexture2DDesc, sizeof(d3xTexture2DDesc));
	pd3dTextures[0]->GetDesc(&d3xTexture2DDesc);


	//===============================================

	D3D11_TEXTURE2D_DESC d3dTexture2DArrayDesc;
	d3dTexture2DArrayDesc.Width = d3xTexture2DDesc.Width;
	d3dTexture2DArrayDesc.Height = d3xTexture2DDesc.Height;
	d3dTexture2DArrayDesc.MipLevels = d3xTexture2DDesc.MipLevels;
	d3dTexture2DArrayDesc.ArraySize = nTextures;

	d3dTexture2DArrayDesc.Format = d3xTexture2DDesc.Format;
	d3dTexture2DArrayDesc.SampleDesc.Count = 1;
	d3dTexture2DArrayDesc.SampleDesc.Quality = 0;
	d3dTexture2DArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dTexture2DArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	d3dTexture2DArrayDesc.CPUAccessFlags = 0;
	d3dTexture2DArrayDesc.MiscFlags = 0;

	ID3D11Texture2D* pd3dTexture2DArray;
	pd3dDevice->CreateTexture2D(&d3dTexture2DArrayDesc, 0, &pd3dTexture2DArray);

	//
	// Copy individual texture elements into texture array.
	//

	// for each texture element...
	ID3D11DeviceContext* pd3dDeviceContext;
	pd3dDevice->GetImmediateContext(&pd3dDeviceContext);
	D3D11_MAPPED_SUBRESOURCE d3dMappedTexture2D;
	for (int t = 0; t < nTextures; t++)
	{
		for (UINT m = 0; m < d3xTexture2DDesc.MipLevels; m++)
		{
			pd3dDeviceContext->Map(pd3dTextures[t], m, D3D11_MAP_READ, 0, &d3dMappedTexture2D);
			pd3dDeviceContext->UpdateSubresource(pd3dTexture2DArray, D3D11CalcSubresource(m, t, d3xTexture2DDesc.MipLevels),
				0, d3dMappedTexture2D.pData, d3dMappedTexture2D.RowPitch, d3dMappedTexture2D.DepthPitch);
			pd3dDeviceContext->Unmap(pd3dTextures[t], m);
		}
	}
	//
	// Create a resource view to the texture array.
	//

	D3D11_SHADER_RESOURCE_VIEW_DESC d3dTexturesSRCDesc;
	d3dTexturesSRCDesc.Format = d3dTexture2DArrayDesc.Format;
	d3dTexturesSRCDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	d3dTexturesSRCDesc.Texture2DArray.MostDetailedMip = 0;
	d3dTexturesSRCDesc.Texture2DArray.MipLevels = d3dTexture2DArrayDesc.MipLevels;
	d3dTexturesSRCDesc.Texture2DArray.FirstArraySlice = 0;
	d3dTexturesSRCDesc.Texture2DArray.ArraySize = nTextures;

	ID3D11ShaderResourceView* pd3dsrcArray = 0;
	(pd3dDevice->CreateShaderResourceView(pd3dTexture2DArray, &d3dTexturesSRCDesc, &pd3dsrcArray));

	if (pd3dTexture2DArray)
		pd3dTexture2DArray->Release();

	for (int i = 0; i < nTextures; i++)
		if (pd3dTextures[i])
			pd3dTextures[i]->Release();
	delete[]pd3dTextures;

	return (pd3dsrcArray);
}


void  CTerrainShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera)
{
	pd3dDeviceContext->PSSetShaderResources(0x03, 1, &m_d3dsrcTextureArray);
	CShader::Render(pd3dDeviceContext, m_pDirect3D, pCamera);
}
CHeightMapTerrain *CTerrainShader::GetTerrain()
{
	return((CHeightMapTerrain *)m_ppObjects[0]);
}

CIlluminatedShader::CIlluminatedShader()
{
}

CIlluminatedShader::~CIlluminatedShader()
{
}
void CIlluminatedShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);



	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSLightingColor", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSLightingColor", "ps_5_0", &m_pd3dPixelShader);
}
void CIlluminatedShader::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	d3dBufferDesc.ByteWidth = sizeof(MATERIAL);
	pd3dDevice->CreateBuffer(&d3dBufferDesc, NULL, &m_pd3dcbMaterial);
}

void CIlluminatedShader::ReleaseShaderVariables()
{
	if (m_pd3dcbMaterial) m_pd3dcbMaterial->Release();
}

void CIlluminatedShader::UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, MATERIAL *pMaterial)
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbMaterial, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	MATERIAL *pcbMaterial = (MATERIAL *)d3dMappedResource.pData;
	memcpy(pcbMaterial, pMaterial, sizeof(MATERIAL));
	pd3dDeviceContext->Unmap(m_pd3dcbMaterial, 0);
	pd3dDeviceContext->PSSetConstantBuffers(PS_SLOT_MATERIAL, 1, &m_pd3dcbMaterial);
}
CDiffusedShader::CDiffusedShader()
{
}

CDiffusedShader::~CDiffusedShader()
{
}

void CDiffusedShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSDiffusedColor", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSDiffusedColor", "ps_5_0", &m_pd3dPixelShader);
}

CTexturedShader::CTexturedShader()
{
}

CTexturedShader::~CTexturedShader()
{
}

void CTexturedShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSTexturedColor", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSTexturedColor", "ps_5_0", &m_pd3dPixelShader);
}


CDetailTexturedShader::CDetailTexturedShader()
{
}

CDetailTexturedShader::~CDetailTexturedShader()
{
}

void CDetailTexturedShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);
	//CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSDetailTexturedColor", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	//CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSDetailTexturedColor", "ps_5_0", &m_pd3dPixelShader);


	CreateVertexShaderFromCompiledFile(pd3dDevice, L"VSDetailTexturedColor.fxo", d3dInputElements, nElements);
	CreatePixelShaderFromCompiledFile(pd3dDevice, L"PSDetailTexturedColor.fxo");

}

CSkyBoxShader::CSkyBoxShader()
{
}

CSkyBoxShader::~CSkyBoxShader()
{
}
void CSkyBoxShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);
	CreateVertexShaderFromFile(pd3dDevice, L"Wave.fx", "VSSkyBox", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Wave.fx", "PSSkyBox", "ps_5_0", &m_pd3dPixelShader);
}


void CSkyBoxShader::BuildObjects(ID3D11Device *pd3dDevice)
{
	m_nObjects = 1;
	m_ppObjects = new CGameObject*[m_nObjects];

	CSkyBox *pSkyBox = new CSkyBox(pd3dDevice);
	m_ppObjects[0] = pSkyBox;
}

void CSkyBoxShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera)
{
	CShader::OnPrepareRender(pd3dDeviceContext);

	m_ppObjects[0]->Render(pd3dDeviceContext, pCamera);
}


CTexturedIlluminatedShader::CTexturedIlluminatedShader()
{
}

CTexturedIlluminatedShader::~CTexturedIlluminatedShader()
{
}

void CTexturedIlluminatedShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSTexturedLightingColor", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSTexturedLightingColor", "ps_5_0", &m_pd3dPixelShader);
}

CDetailTexturedIlluminatedShader::CDetailTexturedIlluminatedShader()
{
}

CDetailTexturedIlluminatedShader::~CDetailTexturedIlluminatedShader()
{
}

void CDetailTexturedIlluminatedShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);
	CreateVertexShaderFromFile(pd3dDevice, L"Terrain.fx", "VSDetailTexturedLightingColor", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Terrain.fx", "PSDetailTexturedLightingColor", "ps_5_0", &m_pd3dPixelShader);
}
//=============================================================

CNormalMappingShader::CNormalMappingShader()
{
}

CNormalMappingShader::~CNormalMappingShader()
{
	if (m_pMaterial)m_pMaterial->Release();
	if (m_pTexture)m_pTexture->Release();
}

void CNormalMappingShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);


	//CreateVertexShaderFromCompiledFile(pd3dDevice, L"NormalMapping_VS.fxo", d3dInputElements, nElements);
	//CreatePixelShaderFromCompiledFile(pd3dDevice, L"NormalMapping_PS.fxo");

	CreateVertexShaderFromFile(pd3dDevice, L"NormalMapping.fx", "VSNormalMapping", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"NormalMapping.fx", "PSNormalMapping", "ps_5_0", &m_pd3dPixelShader);
}
void  CNormalMappingShader::BuildObjects(ID3D11Device *pd3dDevice, CMaterial *pMaterial, CTexture *pTexture)
{
	m_pMaterial = pMaterial;
	if (pMaterial) pMaterial->AddRef();
	m_pTexture = pTexture;
	if (pTexture) pTexture->AddRef();

	m_nObjects = 1;
	m_ppObjects = new CGameObject*[m_nObjects];

	CPlaneNormalMappingObject  *pPlane = new  CPlaneNormalMappingObject(pd3dDevice);
	m_ppObjects[0] = pPlane;
	m_ppObjects[0]->SetPosition(1400.0f, 20.0f, 2048.0f);

}
void  CNormalMappingShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera)
{
	OnPrepareRender(pd3dDeviceContext);
	if (m_pMaterial)
		CIlluminatedShader::UpdateShaderVariable(pd3dDeviceContext, &m_pMaterial->m_Material);
	if (m_pTexture) m_pTexture->UpdateShaderVariable(pd3dDeviceContext);


	m_ppObjects[0]->Render(pd3dDeviceContext, pCamera);
}

CTextureLightingShader::CTextureLightingShader()
{
}

CTextureLightingShader::~CTextureLightingShader()
{
	if (m_pMaterial)m_pMaterial->Release();
	if (m_pTexture)m_pTexture->Release();
}

void  CTextureLightingShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);
	CreateVertexShaderFromFile(pd3dDevice, L"NormalMapping.fx", "VS_N", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"NormalMapping.fx", "PS_N", "ps_5_0", &m_pd3dPixelShader);
}
void   CTextureLightingShader::BuildObjects(ID3D11Device *pd3dDevice, CMaterial *pMaterial, CTexture *pTexture)
{
	m_pMaterial = pMaterial;
	if (pMaterial) pMaterial->AddRef();
	m_pTexture = pTexture;
	if (pTexture) pTexture->AddRef();

	m_nObjects = 1;

	m_ppObjects = new CGameObject*[m_nObjects];
	CPlaneObject *pPlane = new CPlaneObject(pd3dDevice);
	m_ppObjects[0] = pPlane;
	m_ppObjects[0]->SetPosition(1590.0f, 20.0f, 2048.0f);

	D3DX10_MESH_32_BIT;

	//D3DX10CreateMesh();

	//ID3DX10Mesh* pMedeling;
	//pMedeling->DrawSubset();


}
void   CTextureLightingShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera)
{
	OnPrepareRender(pd3dDeviceContext);
	if (m_pMaterial)
		CIlluminatedShader::UpdateShaderVariable(pd3dDeviceContext, &m_pMaterial->m_Material);
	if (m_pTexture) m_pTexture->UpdateShaderVariable(pd3dDeviceContext);

	m_ppObjects[0]->Render(pd3dDeviceContext, pCamera);
}
//==================테셀레이션==========================

CTessellationShader::CTessellationShader()
{
}

CTessellationShader::~CTessellationShader()
{
	if (m_pMaterial)m_pMaterial->Release();
	if (m_pTexture)m_pTexture->Release();
}



void   CTessellationShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);
	CreateVertexShaderFromFile(pd3dDevice, L"NormalMapping.fx", "VS", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"NormalMapping.fx", "PS", "ps_5_0", &m_pd3dPixelShader);

	CreateHullShaderFromFile(pd3dDevice, L"NormalMapping.fx", "HS", "hs_5_0", &m_pd3dHullShader);
	CreateDomainShaderFromFile(pd3dDevice, L"NormalMapping.fx", "DS", "ds_5_0", &m_pd3dDomainShader);
}

void CShader::CreateCameraPositionBuffer(ID3D11Device *pd3dDevice)
{
	//월드 변환 행렬을 위한 상수 버퍼를 생성한다.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(D3DXVECTOR4);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbCameraPosition);
}
void CTessellationShader::BuildObjects(ID3D11Device *pd3dDevice, CMaterial *pMaterial, CTexture *pTexture)
{
	m_pMaterial = pMaterial;
	if (pMaterial) pMaterial->AddRef();
	m_pTexture = pTexture;
	if (pTexture) pTexture->AddRef();

	m_nObjects = 1;
	m_ppObjects = new CGameObject*[m_nObjects];

	CTessellationObject *pPlane = new CTessellationObject(pd3dDevice);
	m_ppObjects[0] = pPlane;
	m_ppObjects[0]->SetPosition(1200.0f, 12.0f, 2048.0f);
	CreateCameraPositionBuffer(pd3dDevice);
}
void    CTessellationShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera)
{
	OnPrepareRender(pd3dDeviceContext);
	pd3dDeviceContext->HSSetShader(m_pd3dHullShader, NULL, 0);
	pd3dDeviceContext->DSSetShader(m_pd3dDomainShader, NULL, 0);
	//pCamera->DSUpdateShaderVariables(pd3dDeviceContext); //카메라 행렬

	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbCameraPosition, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	D3DXVECTOR4 *pcbCameraPosition = (D3DXVECTOR4*)d3dMappedResource.pData;
	pcbCameraPosition->x = pCamera->GetPosition().x;
	pcbCameraPosition->y = pCamera->GetPosition().y;
	pcbCameraPosition->z = pCamera->GetPosition().z;
	pd3dDeviceContext->Unmap(m_pd3dcbCameraPosition, 0);

	pd3dDeviceContext->HSSetConstantBuffers(0x00, 1, &m_pd3dcbCameraPosition);


	m_ppObjects[0]->Render(pd3dDeviceContext, pCamera);

	pd3dDeviceContext->HSSetShader(NULL, NULL, 0);
	pd3dDeviceContext->DSSetShader(NULL, NULL, 0);
}

CDisplacementMappingShader::CDisplacementMappingShader()
{
}

CDisplacementMappingShader::~CDisplacementMappingShader()
{

}

void  CDisplacementMappingShader::CreateShader(ID3D11Device *pd3dDevice)
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



	CreateVertexShaderFromFile(pd3dDevice, L"DisplacementMapping.fx", "VS", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"DisplacementMapping.fx", "PS", "ps_5_0", &m_pd3dPixelShader);
	CreateHullShaderFromFile(pd3dDevice, L"DisplacementMapping.fx", "HS", "hs_5_0", &m_pd3dHullShader);
	CreateDomainShaderFromFile(pd3dDevice, L"DisplacementMapping.fx", "DS", "ds_5_0", &m_pd3dDomainShader);

}
void   CDisplacementMappingShader::BuildObjects(ID3D11Device *pd3dDevice, CMaterial *pMaterial, CTexture *pTexture)
{
	m_pMaterial = pMaterial;
	if (pMaterial) pMaterial->AddRef();
	m_pTexture = pTexture;
	if (pTexture) pTexture->AddRef();

	m_nObjects = 1;
	m_ppObjects = new CGameObject*[m_nObjects];

	CDisplacementMappingObject *pPlane = new  CDisplacementMappingObject(pd3dDevice);
	m_ppObjects[0] = pPlane;
	m_ppObjects[0]->SetPosition(1800.0f, 28.0f, 2048.0f);
	CreateCameraPositionBuffer(pd3dDevice);



}
void   CDisplacementMappingShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera)
{
	OnPrepareRender(pd3dDeviceContext);
	if (m_pMaterial)
		CIlluminatedShader::UpdateShaderVariable(pd3dDeviceContext, &m_pMaterial->m_Material);
	if (m_pTexture)  //텍스쳐
		m_pTexture->UpdateShaderVariable(pd3dDeviceContext);

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
	
	m_ppObjects[0]->Render(pd3dDeviceContext, pCamera);

	pd3dDeviceContext->HSSetConstantBuffers(NULL, NULL, NULL);
	pCamera->DSClearShaderVariables(pd3dDeviceContext);
	pd3dDeviceContext->HSSetShader(NULL, NULL, 0);
	pd3dDeviceContext->DSSetShader(NULL, NULL, 0);
}









CTessellationTerrainShader::CTessellationTerrainShader()
{
}

CTessellationTerrainShader::~CTessellationTerrainShader()
{
}

void   CTessellationTerrainShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};
	UINT nElements = ARRAYSIZE(d3dInputElements);


	/*CreateVertexShaderFromCompiledFile(pd3dDevice, L"Terrain_VS.fxo", d3dInputElements, nElements);
	CreatePixelShaderFromCompiledFile(pd3dDevice, L"Terrain_PS.fxo");
	CreateHullShaderFromCompiledFile(pd3dDevice, L"Terrain_HS.fxo");
	CreateDomainShaderFromCompiledFile(pd3dDevice, L"Terrain_DS.fxo");*/


	CreateVertexShaderFromFile(pd3dDevice, L"Terrain.fx", "VS", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Terrain.fx", "PS", "ps_5_0", &m_pd3dPixelShader);
	CreateHullShaderFromFile(pd3dDevice, L"Terrain.fx", "HS", "hs_5_0", &m_pd3dHullShader);
	CreateDomainShaderFromFile(pd3dDevice, L"Terrain.fx", "DS", "ds_5_0", &m_pd3dDomainShader);

}


void   CTessellationTerrainShader::BuildObjects(ID3D11Device *pd3dDevice)
{

	m_nObjects = 1;
	m_ppObjects = new CGameObject*[m_nObjects];

	CTessellationHeightMapTerrain*pTerrain = new  CTessellationHeightMapTerrain(pd3dDevice, 257, 257, D3DXVECTOR3(8.0f, 0.0f, 8.0f));
	m_ppObjects[0] = pTerrain;
	CreateCameraPositionBuffer(pd3dDevice);



	int nWidth = 257;
	int nLength = 257;

	BYTE *pHeightMapImage = new BYTE[nWidth * nLength];
	HANDLE hFile = ::CreateFile(L"HeightMap.raw", GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);
	DWORD dwBytesRead;
	::ReadFile(hFile, pHeightMapImage, (nWidth * nLength), &dwBytesRead, NULL);
	::CloseHandle(hFile);

	float* pHeightMapArray = new float[nWidth * nLength];
	for (int i = 0; i < nWidth * nLength; i++)
	{
		pHeightMapArray[i] = (pHeightMapImage[i] * 4.0f);
	}


	D3D11_TEXTURE2D_DESC d3dxTexDesc;
	d3dxTexDesc.Width = 257;
	d3dxTexDesc.Height = 257;
	d3dxTexDesc.MipLevels = 1;
	d3dxTexDesc.ArraySize = 1;
	d3dxTexDesc.Format = DXGI_FORMAT_R32_FLOAT;
	d3dxTexDesc.SampleDesc.Count = 1;
	d3dxTexDesc.SampleDesc.Quality = 0;
	d3dxTexDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dxTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	d3dxTexDesc.CPUAccessFlags = 0;
	d3dxTexDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = pHeightMapArray;
	data.SysMemPitch = 257 * sizeof(float);
	data.SysMemSlicePitch = 0;

	ID3D11Texture2D* d3dxTxtHeightMap = 0;
	(pd3dDevice->CreateTexture2D(&d3dxTexDesc, &data, &d3dxTxtHeightMap));

	ID3D11ShaderResourceView *pd3dsrvTexture;
	D3D11_SHADER_RESOURCE_VIEW_DESC d3dxSrvDesc;
	d3dxSrvDesc.Format = d3dxTexDesc.Format;
	d3dxSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	d3dxSrvDesc.Texture2D.MostDetailedMip = 0;
	d3dxSrvDesc.Texture2D.MipLevels = -1;
	pd3dDevice->CreateShaderResourceView(d3dxTxtHeightMap, &d3dxSrvDesc, &pd3dsrvTexture);


	////텍스쳐 맵핑에 사용할 샘플러 상태 객체를 생성한다.
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

	ID3D11SamplerState *pd3dDetailSamplerState = NULL;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dDetailSamplerState);


	CTexture *pTerrainTexture = new CTexture(2, 2, 0, 0);
	ID3D11ShaderResourceView *pd3dsrvBaseTexture = NULL;
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/Base_Texture.jpg"), NULL, NULL, &pd3dsrvBaseTexture, NULL);
	pTerrainTexture->SetTexture(0, pd3dsrvBaseTexture);
	pTerrainTexture->SetSampler(0, pd3dSamplerState);

	ID3D11ShaderResourceView *pd3dsrvDetailTexture = NULL;
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/Detail_Texture_7.jpg"), NULL, NULL, &pd3dsrvDetailTexture, NULL);
	pTerrainTexture->SetTexture(1, pd3dsrvDetailTexture);
	pTerrainTexture->SetSampler(1, pd3dDetailSamplerState);
	pd3dsrvDetailTexture->Release();

	m_ppObjects[0]->SetTexture(pTerrainTexture);
	pd3dsrvBaseTexture->Release();


	//텍스쳐 리소스를 생성한다.
	m_pHeightMap = new CTexture(1, 1, 5, 5);
	m_pHeightMap->SetTexture(0, pd3dsrvTexture);
	m_pHeightMap->SetSampler(0, pd3dSamplerState);
	pd3dsrvTexture->Release();
	pd3dSamplerState->Release();


}
void   CTessellationTerrainShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera)
{
	OnPrepareRender(pd3dDeviceContext);
	pd3dDeviceContext->HSSetShader(m_pd3dHullShader, NULL, 0);
	pd3dDeviceContext->DSSetShader(m_pd3dDomainShader, NULL, 0);
	pCamera->DSUpdateShaderVariables(pd3dDeviceContext); //카메라 행렬

	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbCameraPosition, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	D3DXVECTOR4 *pcbCameraPosition = (D3DXVECTOR4*)d3dMappedResource.pData;
	pcbCameraPosition->x = pCamera->GetPosition().x;
	pcbCameraPosition->y = pCamera->GetPosition().y;
	pcbCameraPosition->z = pCamera->GetPosition().z;
	pd3dDeviceContext->Unmap(m_pd3dcbCameraPosition, 0);

	pd3dDeviceContext->HSSetConstantBuffers(0x02, 1, &m_pd3dcbCameraPosition);


	m_pHeightMap->UpdateShaderVariableVS(pd3dDeviceContext);
	m_pHeightMap->UpdateShaderVariableDS(pd3dDeviceContext);
	m_pHeightMap->UpdateShaderVariable(pd3dDeviceContext);



	m_ppObjects[0]->Render(pd3dDeviceContext, pCamera);

	pd3dDeviceContext->HSSetShader(NULL, NULL, 0);
	pd3dDeviceContext->DSSetShader(NULL, NULL, 0);
}


CTestTessellationShader::CTestTessellationShader()
{
}

CTestTessellationShader::~CTestTessellationShader()
{
	if (m_pMaterial)m_pMaterial->Release();
	if (m_pTexture)m_pTexture->Release();
}

void  CTestTessellationShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);


	//CreateVertexShaderFromCompiledFile(pd3dDevice, L"Test_VS.fxo", d3dInputElements, nElements);
	//CreatePixelShaderFromCompiledFile(pd3dDevice, L"Test_PS.fxo");
	//CreateHullShaderFromCompiledFile(pd3dDevice, L"Test_HS.fxo");
	//CreateDomainShaderFromCompiledFile(pd3dDevice, L"Test_DS.fxo");



	CreateVertexShaderFromFile(pd3dDevice, L"Test.fx", "VS", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Test.fx", "PS", "ps_5_0", &m_pd3dPixelShader);
	CreateHullShaderFromFile(pd3dDevice, L"Test.fx", "HS", "hs_5_0", &m_pd3dHullShader);
	CreateDomainShaderFromFile(pd3dDevice, L"Test.fx", "DS", "ds_5_0", &m_pd3dDomainShader);



}
void   CTestTessellationShader::BuildObjects(ID3D11Device *pd3dDevice, CMaterial *pMaterial, CTexture *pTexture)
{
	m_pMaterial = pMaterial;
	if (pMaterial) pMaterial->AddRef();
	m_pTexture = pTexture;
	if (pTexture) pTexture->AddRef();

	m_nObjects = 1;
	m_ppObjects = new CGameObject*[m_nObjects];

	CTestDisplacementMappingObject *pPlane = new  CTestDisplacementMappingObject(pd3dDevice);
	m_ppObjects[0] = pPlane;
	m_ppObjects[0]->SetPosition(2000.0f, 20.0f, 2048.0f);
	CreateCameraPositionBuffer(pd3dDevice);



}
void   CTestTessellationShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera)
{
	OnPrepareRender(pd3dDeviceContext);

	pd3dDeviceContext->HSSetShader(m_pd3dHullShader, NULL, 0);
	pd3dDeviceContext->DSSetShader(m_pd3dDomainShader, NULL, 0);


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
	//카메라-투영 행렬을 도메인 셰이더에 연결한다.
	//pCamera->DSUpdateShaderVariables(pd3dDeviceContext);

	if (m_pMaterial)
		CIlluminatedShader::UpdateShaderVariable(pd3dDeviceContext, &m_pMaterial->m_Material);
	if (m_pTexture)  //텍스쳐
		m_pTexture->UpdateShaderVariable(pd3dDeviceContext);

	m_ppObjects[0]->Render(pd3dDeviceContext, pCamera);

	pd3dDeviceContext->HSSetShader(NULL, NULL, 0);
	pd3dDeviceContext->DSSetShader(NULL, NULL, 0);
}


CHumanShader::CHumanShader()
{
}

void  CHumanShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);

	//CreateVertexShaderFromCompiledFile(pd3dDevice, L"Human_VS.fxo", d3dInputElements, nElements);
	//CreatePixelShaderFromCompiledFile(pd3dDevice, L"Human_PS.fxo");



	CreateVertexShaderFromFile(pd3dDevice, L"H.fx", "VS", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"H.fx", "PS", "ps_5_0", &m_pd3dPixelShader);

}
void   CHumanShader::BuildObjects(ID3D11Device *pd3dDevice)
{

	m_nObjects = 1;
	m_ppObjects = new CGameObject*[m_nObjects];

	CHumanObject *pPlane = new  CHumanObject(pd3dDevice);
	m_ppObjects[0] = pPlane;
	m_ppObjects[0]->SetPosition(1400.0f, +60.0f, 1880.0f);
}
CHumanShader::~CHumanShader()
{
}



COtherPlayerShader::COtherPlayerShader()
{
}

void  COtherPlayerShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 3, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 4, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 5, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE", 0, DXGI_FORMAT_R32G32B32A32_UINT, 6, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);

	//CreateVertexShaderFromCompiledFile(pd3dDevice, L"Human_VS.fxo", d3dInputElements, nElements);
	//CreatePixelShaderFromCompiledFile(pd3dDevice, L"Human_PS.fxo");



	CreateVertexShaderFromFile(pd3dDevice, L"Shader.fx", "VS", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Shader.fx", "PS", "ps_5_0", &m_pd3dPixelShader);

}

// 구분값.
void   COtherPlayerShader::BuildObjects(ID3D11Device *pd3dDevice, CCharacterMesh *pWarriorMesh)
{
	string strFileName = "Data/warrior_Vertex.txt";
	string strFileName2 = "Data/Golem_Vertex.txt";

	m_fTimePos = 0.0;
	m_nObjects = 3;
	m_ppObjects = new CGameObject*[m_nObjects];



	COtherPlayerObject* pWizardObject1 = new COtherPlayerObject(pd3dDevice, strFileName);
	COtherPlayerObject* pWizardObject2 = new COtherPlayerObject(pd3dDevice, strFileName);
	COtherPlayerObject* pWizardObject3 = new COtherPlayerObject(pd3dDevice, strFileName);


	//CMonsterObject* pGolem = new CMonsterObject(pd3dDevice, strFileName2);


	//CCharacterMesh *pHumanMesh = new CCharacterMesh(pd3dDevice, strFileName);
	pWizardObject1->SetMesh(pWarriorMesh);
	pWizardObject2->SetMesh(pWarriorMesh);
	pWizardObject3->SetMesh(pWarriorMesh);





	m_ppObjects[0] = pWizardObject1;
	m_ppObjects[1] = pWizardObject2;
	m_ppObjects[2] = pWizardObject3;
	//m_ppObjects[3] = pGolem;


	m_ppObjects[0]->SetPosition(D3DXVECTOR3(200, 260.0f, 200.0f));
	//m_ppObjects[1]->SetPosition(D3DXVECTOR3(200, 260.0f, 400.0f));
	//m_ppObjects[2]->SetPosition(D3DXVECTOR3(800, 260.0f, 600.0f));


	CGameManager* pGameManager = CGameManager::GetCGameManager();
	pGameManager->m_pPlayers[1] = m_ppObjects[0];
	pGameManager->m_pPlayers[2] = m_ppObjects[1];
	pGameManager->m_pPlayers[3] = m_ppObjects[2];
	



	//m_ppObjects[3]->SetPosition(D3DXVECTOR3(400, 260.0f, 800.0f));





	//CGameManager* GameManager = CGameManager::GetCGameTimer();

	//GameManager->m_potherPlayer[GameManager->m_nMonsterNum++] = m_ppObjects[0];
	//GameManager->m_ppMonster[GameManager->m_nMonsterNum++] = m_ppObjects[1];


	//FILE *pFile = NULL;
	//_wfopen_s(&pFile, L"Data/StoneKing_AttackReady.txt", L"rt");
	//fscanf_s(pFile, "%d \n", &m_llAniTime);
	//fscanf_s(pFile, "%d \n", &m_uiBoneIndexCount);




	//m_ppResultMatrix = new D3DXMATRIX*[m_llAniTime / 10];

	//for (long long i = 0; i < m_llAniTime / 10; ++i)
	//{
	//	// 최종 행렬 -> 매 초마다 해당하는    (포인터 배열을 가지고 있는)
	//	m_ppResultMatrix[i] = new D3DXMATRIX[m_uiBoneIndexCount];
	//}


	//for (long long i = 0; i < m_llAniTime / 10; ++i)
	//{
	//	for (unsigned int j = 0; j < m_uiBoneIndexCount; ++j)
	//	{
	//		//cout << i << "  " << j << endl;
	//		fscanf_s(pFile, "%f %f %f %f  \n",
	//			&m_ppResultMatrix[i][j]._11, &m_ppResultMatrix[i][j]._12, &m_ppResultMatrix[i][j]._13, &m_ppResultMatrix[i][j]._14);
	//		fscanf_s(pFile, "%f %f %f %f  \n",
	//			&m_ppResultMatrix[i][j]._21, &m_ppResultMatrix[i][j]._22, &m_ppResultMatrix[i][j]._23, &m_ppResultMatrix[i][j]._24);
	//		fscanf_s(pFile, "%f %f %f %f  \n",
	//			&m_ppResultMatrix[i][j]._31, &m_ppResultMatrix[i][j]._32, &m_ppResultMatrix[i][j]._33, &m_ppResultMatrix[i][j]._34);
	//		fscanf_s(pFile, "%f %f %f %f  \n",
	//			&m_ppResultMatrix[i][j]._41, &m_ppResultMatrix[i][j]._42, &m_ppResultMatrix[i][j]._43, &m_ppResultMatrix[i][j]._44);
	//	}
	//}
	//::fclose(pFile);



	//// 어떻게 96개가 나오지???
	//m_pvscbResultMatrix = new VS_CB_RESULT_MATRIX*[m_llAniTime / 10];

	//for (int i = 0; i < m_llAniTime / 10; ++i)
	//	m_pvscbResultMatrix[i] = new VS_CB_RESULT_MATRIX();

	//// 각 시간마다 곱해야할 뼈 행렬들(32개?)
	//for (int i = 0; i < m_llAniTime / 10; ++i)
	//{
	//	for (int j = 0; j < m_uiBoneIndexCount; ++j)
	//	{
	//		m_pvscbResultMatrix[i]->m_d3dxmtxResult[j] = m_ppResultMatrix[i][j];
	//	}
	//}



	//CreateShaderVariables(pd3dDevice);
}
COtherPlayerShader::~COtherPlayerShader()
{
}
void COtherPlayerShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera)
{
	OnPrepareRender(pd3dDeviceContext);
	//if (m_pTexture)
	//	m_pTexture->UpdateTextureShaderVariable(pd3dDeviceContext);


	//CGameTimer* GameTimer = CGameTimer::GetCGameTimer();
	//m_fTimePos += GameTimer->GetTimeElapsed();



	//long long NowTime = m_fTimePos * 1000;
	//if (NowTime >= m_llAniTime)
	//{
	//	NowTime -= m_llAniTime;
	//	m_fTimePos = 0;
	//}
	//D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	//pd3dDeviceContext->Map(m_pd3dcbResult, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	//m_cbMapData = (VS_CB_RESULT_MATRIX *)d3dMappedResource.pData;

	//for (int i = 0; i <MaxBone; i++) //왼쪽   /////// NowTime 설정 잘할것.. 아직 못 고쳤음. NowTime = 32, i = 83
	//	m_cbMapData->m_d3dxmtxResult[i] = m_ppResultMatrix[NowTime / 10][i]; //[시간][본인덱스]
	//pd3dDeviceContext->Unmap(m_pd3dcbResult, 0);
	//if (m_pd3dcbResult != NULL)
	//	pd3dDeviceContext->VSSetConstantBuffers(VS_SLOT_RESULT_MATRIX, 1, &m_pd3dcbResult);


	//**********************************************************
	//************************************************************


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

void COtherPlayerShader::AnimateObjects(float fTimeElapsed)
{
	//서버 다른 플레이어 좌표 셋팅하는 부분
	//ClientServer *s = ClientServer::getInstangce();
	//for (auto i = 1; i < 4; ++i)
	//{
	//	if (true == s->Player[i].getPlay())
	//	{
	//		m_ppObjects[i - 1]->SetAnimationState(s->Player[i].getState());
	//		m_ppObjects[i - 1]->SetPosition(s->Player[i].getPlayerPosition());
	//		m_ppObjects[i - 1]->SetDirection(s->Player[i].getPlayerDirection());
	//		m_ppObjects[i - 1]->RenewWorldMatrix();
	//		//cout << "다른 플레이어 상태   " << m_ppObjects[i - 1]->GetAnimationStat() << endl;
	//		//cout <<" 다른 플레이어 위치  " <<m_ppObjects[i - 1]->GetPosition().x << "   " << m_ppObjects[i - 1]->GetPosition().z << endl;
	//	}
	//	else
	//	{
	//		m_ppObjects[i - 1]->SetPosition(D3DXVECTOR3(-10.0,0.0,-10.0));
	//	}
	//}
}

CCrushBoxShader::CCrushBoxShader()
{
}
CCrushBoxShader::~CCrushBoxShader()
{}


void CCrushBoxShader::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_RESULT_MATRIX);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	if (pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbResult) == S_OK)
	{
		std::cout << "success" << std::endl;

	}
}
void CCrushBoxShader::BuildObjects(ID3D11Device *pd3dDevice)
{
	//m_ppObjects[0] = pPlane;
	////pWoodObject->SetPosition(x * 600 + 200, 325.0f, (z * 400) + 450);
	//m_ppObjects[0]->SetPosition(200.0f, 325.0f, 450.0f);


	CreateShaderVariables(pd3dDevice);
	CShader::CreateShaderVariables(pd3dDevice);

	m_nObjects = 2;
	m_ppObjects = new CGameObject*[m_nObjects];

	CBoundingBoxObject *pPlane1 = new CBoundingBoxObject(pd3dDevice);
	CBoundingBoxObject *pPlane2 = new CBoundingBoxObject(pd3dDevice);

	// 골렘의 공격 반경 (140
	CBoundingCircle* pCircle1 = new CBoundingCircle(pd3dDevice, 60.0f, 20.0f, 1.0f);
	pPlane1->SetMesh(pCircle1);
	// 플레이어의 피격 반격
	CBoundingCircle* pCircle2 = new CBoundingCircle(pd3dDevice, 120.0f, 20.0f, 1.0f);
	pPlane2->SetMesh(pCircle2);

	m_ppObjects[0] = pPlane1;
	m_ppObjects[0]->SetPosition(400.0f, 300.0f, 300.0f);
	m_ppObjects[1] = pPlane2;
	m_ppObjects[1]->SetPosition(820.0f, 266.0f, 820.0f);



	m_fTimePos = 0.0;





	FILE *pFile = NULL;
	_wfopen_s(&pFile, L"Data/GolemAttack.txt", L"rt");
	fscanf_s(pFile, "%d \n", &m_llAniTime);
	fscanf_s(pFile, "%d \n", &m_uiBoneIndexCount);


	m_ppResultMatrix = new D3DXMATRIX*[m_llAniTime / 10];

	for (long long i = 0; i < m_llAniTime / 10; ++i)
	{
		// 최종 행렬 -> 매 초마다 해당하는    (포인터 배열을 가지고 있는)
		m_ppResultMatrix[i] = new D3DXMATRIX[m_uiBoneIndexCount];
	}


	for (long long i = 0; i < m_llAniTime / 10; ++i)
	{
		for (unsigned int j = 0; j < m_uiBoneIndexCount; ++j)
		{
			//cout << i << "  " << j << endl;
			fscanf_s(pFile, "%f %f %f %f  \n",
				&m_ppResultMatrix[i][j]._11, &m_ppResultMatrix[i][j]._12, &m_ppResultMatrix[i][j]._13, &m_ppResultMatrix[i][j]._14);
			fscanf_s(pFile, "%f %f %f %f  \n",
				&m_ppResultMatrix[i][j]._21, &m_ppResultMatrix[i][j]._22, &m_ppResultMatrix[i][j]._23, &m_ppResultMatrix[i][j]._24);
			fscanf_s(pFile, "%f %f %f %f  \n",
				&m_ppResultMatrix[i][j]._31, &m_ppResultMatrix[i][j]._32, &m_ppResultMatrix[i][j]._33, &m_ppResultMatrix[i][j]._34);
			fscanf_s(pFile, "%f %f %f %f  \n",
				&m_ppResultMatrix[i][j]._41, &m_ppResultMatrix[i][j]._42, &m_ppResultMatrix[i][j]._43, &m_ppResultMatrix[i][j]._44);
		}
	}
	::fclose(pFile);



	// 어떻게 96개가 나오지???
	m_pvscbResultMatrix = new VS_CB_RESULT_MATRIX*[m_llAniTime / 10];

	for (int i = 0; i < m_llAniTime / 10; ++i)
		m_pvscbResultMatrix[i] = new VS_CB_RESULT_MATRIX();

	// 각 시간마다 곱해야할 뼈 행렬들(32개?)
	for (int i = 0; i < m_llAniTime / 10; ++i)
	{
		for (int j = 0; j < m_uiBoneIndexCount; ++j)
		{
			m_pvscbResultMatrix[i]->m_d3dxmtxResult[j] = m_ppResultMatrix[i][j];
		}
	}






	//FILE* fp;
	//fopen_s(&fp, "Player_CrushData.txt", "w");
	//fprintf(fp, "%d \n", 1);
	//for (int j = 0; j < m_nObjects; j++)
	//{
	//	fprintf(fp, "%lf %lf %lf %lf %lf \n", m_ppObjects[j]->GetPosition().x, m_ppObjects[j]->GetPosition().y, m_ppObjects[j]->GetPosition().z, 490.0f, 500.0f);
	//}
	//fclose(fp);

}

void CCrushBoxShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera)
{
	OnPrepareRender(pd3dDeviceContext);
	//if (m_pTexture)
	//	m_pTexture->UpdateTextureShaderVariable(pd3dDeviceContext);


	CGameTimer* GameTimer = CGameTimer::GetCGameTimer();
	m_fTimePos += GameTimer->GetTimeElapsed();



	long long NowTime = m_fTimePos * 1000;
	if (NowTime >= m_llAniTime)
	{
		NowTime -= m_llAniTime;
		m_fTimePos = 0;
	}
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbResult, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	m_cbMapData = (VS_CB_RESULT_MATRIX *)d3dMappedResource.pData;

	for (int i = 0; i <MaxBone; i++) //왼쪽   /////// NowTime 설정 잘할것.. 아직 못 고쳤음. NowTime = 32, i = 83
		m_cbMapData->m_d3dxmtxResult[i] = m_ppResultMatrix[NowTime / 10][i]; //[시간][본인덱스]
	pd3dDeviceContext->Unmap(m_pd3dcbResult, 0);
	if (m_pd3dcbResult != NULL)
		pd3dDeviceContext->VSSetConstantBuffers(VS_SLOT_RESULT_MATRIX, 1, &m_pd3dcbResult);


	//**********************************************************
	//************************************************************


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


// 나무0번 
//int i = 0;
//for (int z = 0; z < 9; z++)
//{
//	for (int x = 0; x < 4; x++)
//	{
//		CBoundingBoxObject *pPlane = new CBoundingBoxObject(pd3dDevice);
//		m_ppObjects[i] = pPlane;
//
//		if (x == 0) //가장 처음
//			m_ppObjects[i]->SetPosition(x * 600 + 200, 325.0f, (z * 400) + 450);
//		else
//			m_ppObjects[i]->SetPosition(x * 600 + 650, 325.0f, (z * 400) + 350);
//		i++;
//	}
//}
//
//
//FILE* fp;
//fopen_s(&fp, "tree0.txt", "w");
//fprintf(fp, "%d \n", 0);
//for (int j = 0; j < m_nObjects; j++)
//{
//	fprintf(fp, "%lf %lf %lf %lf %lf \n", m_ppObjects[j]->GetPosition().x, m_ppObjects[j]->GetPosition().y, m_ppObjects[j]->GetPosition().z, 53.8551f, 51.4314f);
//}
//fclose(fp);
//

//호수01
// m_ppObjects[0]->SetPosition(740.0f, 190.0f, 1100.0f);

//캐릭터ㅏ
// 	m_ppObjects[0]->SetPosition(410.4f, 262.0f, 410.4f);