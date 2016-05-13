//-----------------------------------------------------------------------------
// File: CGameFramework.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "GameFramework.h"
#include <DXGI.h>
using namespace std;

CGameTimer* CGameTimer::Timer;
CGameManager* CGameManager::GameManager;




// Direct3D ����̽�, ���� ü��, ����̽� ���ؽ�Ʈ, �׸��� ���� Ÿ�� �� �������̽� �����͸� NULL�� �ʱ�ȭ�Ѵ�.
CGameFramework::CGameFramework()
{
	m_pd3dDevice = NULL;
	m_pDXGISwapChain = NULL;
	m_pd3dRenderTargetView = NULL;
	m_pd3dDeviceContext = NULL;

	m_pScene = NULL;
	g_pVertexShader = NULL;
	g_pPixelShader = NULL;
	m_pPlayer = NULL;
	m_pd3dDepthStencilBuffer = NULL;
	m_pd3dDepthStencilView = NULL;
	_tcscpy_s(m_pszBuffer, _T("LapProject ("));

	m_pPlayerShader = NULL;

	srand((unsigned int)time(NULL));

	m_pCamera = NULL;

}

CGameFramework::~CGameFramework()
{
}

//���� �Լ��� ���� ���α׷��� ����Ǹ� ȣ��ȴٴ� �Ϳ� �����϶�. 
bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_pDirect3D = new CDirect3DBase();

	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	//Direct3D ����̽�, ����̽� ���ؽ�Ʈ, ���� ü�� ���� �����ϴ� �Լ��� ȣ���Ѵ�. 
	if (!m_pDirect3D->CreateDirect3DDisplay(m_hInstance, m_hWnd)) return(false);
	//�������� ��ü(���� ���� ��ü)�� �����Ѵ�. 





	//Direct3D ����̽�, ����̽� ���ؽ�Ʈ, ���� ü�� ���� �����ϴ� �Լ��� ȣ���Ѵ�. 
	//if (!CreateDirect3DDisplay()) return(false);

	//�������� ��ü(���� ���� ��ü)�� �����Ѵ�. 
	BuildObjects();

	return(true);
}

bool CGameFramework::CreateDirect3DDisplay()
{
	//����, ��� �������� Ŭ���̾�Ʈ ������ ũ�⸦ ��� ������ �����Ѵ�.
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_nWndClientWidth = rcClient.right - rcClient.left;
	m_nWndClientHeight = rcClient.bottom - rcClient.top;

	//IDXGIObject


	//IDXGIFactory1* pdxgiFactory = NULL;
	//HRESULT hResult1 = CreateDXGIFactory(__uuidof(IDXGIFactory1), (void**)(&pdxgiFactory));
	//

	//int nAdapter = 0;
	//IDXGIAdapter* pdxgiAdapter;
	//vector<IDXGIAdapter* > stdvAdapters;
	//DXGI_ADAPTER_DESC dxgiAdapterDesc;
	//while (pdxgiFactory->EnumAdapters(nAdapter++, &pdxgiAdapter)
	//	!= DXGI_ERROR_NOT_FOUND)
	//{
	//	stdvAdapters.push_back(pdxgiAdapter);
	//	pdxgiAdapter->GetDesc(&dxgiAdapterDesc);
	//}

	// Direct3D ����̽��� ���� ü���� �����ϱ�
	/*���� ü���� �����ϱ� ���� ���� ü�� ����ü DXGI_SWAP_CHAIN_DESC�� �����Ѵ�.
	���� ü���� ���� ũ��� �� �������� Ŭ���̾�Ʈ ������ ũ��� �����ϰ� ��� ������� �� ������� �����Ѵ�.
	���� ü���� ��� ���� ������ ���� �����Ѵ�.*/
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc)); //�޸� �ʱ�ȭ
	dxgiSwapChainDesc.BufferCount = 1;
	dxgiSwapChainDesc.BufferDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow = m_hWnd;
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;
	dxgiSwapChainDesc.Windowed = TRUE;
	dxgiSwapChainDesc.Flags = 0;

	UINT dwCreateDeviceFlags = 0;
#ifdef _DEBUG
	dwCreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	//����̽��� �����ϱ� ���Ͽ� �õ��� ����̹� ������ ������ ��Ÿ����.
	//����̽� ������ ���� �õ��� ������� ����̽� ����̹� ������ Ư�� ������ �迭�� �����Ѵ�. 
	D3D_DRIVER_TYPE d3dDriverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	UINT nDriverTypes = sizeof(d3dDriverTypes) / sizeof(D3D_DRIVER_TYPE);

	//����̽��� �����ϱ� ���Ͽ� �õ��� Ư�� ������ ������ ��Ÿ����.
	D3D_FEATURE_LEVEL pd3dFeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	UINT nFeatureLevels = sizeof(pd3dFeatureLevels) / sizeof(D3D_FEATURE_LEVEL);

	//������ ���� ü���� �����ϴ� ����ü�̴�.

	/*���ϴ� ����̽� ����̹� ���� ������ ���� ü�ΰ� ����̽�,
	����̽� ���ؽ�Ʈ�� �����ϱ� ���� ����̹� ���� �迭(d3dDriverTypes[])�� �� ���ҿ� ���Ͽ�
	D3D11CreateDeviceAndSwapChain() �Լ� ȣ���� ���� ����̽��� ����ü�� ������ �õ��Ѵ�.*/
	D3D_DRIVER_TYPE nd3dDriverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL nd3dFeatureLevel = D3D_FEATURE_LEVEL_11_0;
	HRESULT hResult = S_OK;

	//����̽��� ����̹� ������ Ư�� ������ �����ϴ� ����̽��� ���� ü���� �����Ѵ�. 
	//  ������� ����̽� ������ �õ��ϰ� �����ϸ� ���� ������ ����̽��� �����Ѵ�.

	/*D3D11CreateDeviceAndSwapChain() �Լ��� ���Ͽ� Direct3D ����̽�, ���� ü��, ����̽� ���ؽ�Ʈ��
	���ÿ� �����ϵ��� ����.
	����, ����̽� ������ �ʿ��� ����̽� ���� �÷���, ����̹� ����, Ư�� ����(Feature Level)�� �����Ѵ�.*/
	for (UINT i = 0; i < nDriverTypes; i++)
	{
		nd3dDriverType = d3dDriverTypes[i];
		if (SUCCEEDED(hResult = D3D11CreateDeviceAndSwapChain(NULL, nd3dDriverType, NULL,
			dwCreateDeviceFlags, pd3dFeatureLevels, nFeatureLevels,
			D3D11_SDK_VERSION, &dxgiSwapChainDesc, &m_pDXGISwapChain, &m_pd3dDevice,
			&nd3dFeatureLevel, &m_pd3dDeviceContext))) break;
	}
	//����̽��� �����Ǹ� ���� Ÿ�� �並 �����ϱ� ���� CreateRenderTargetView() �Լ��� ȣ���Ѵ�. 
	if (!CreateRenderTargetDepthStencilView()) return(false);


	return(true);
}

// �� �Լ��� ���� ü���� ù ��° �ĸ���ۿ� ���� ���� Ÿ�� �並 �����ϰ� ����̽� ���ؽ�Ʈ�� �����Ѵ�
bool CGameFramework::CreateRenderTargetDepthStencilView()
{
	HRESULT hResult = S_OK;

	ID3D11Texture2D *pd3dBackBuffer;
	if (FAILED(hResult = m_pDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pd3dBackBuffer))) return(false);
	if (FAILED(hResult = m_pd3dDevice->CreateRenderTargetView(pd3dBackBuffer, NULL, &m_pd3dRenderTargetView))) return(false);
	if (pd3dBackBuffer) pd3dBackBuffer->Release();

	//���� Ÿ�ٰ� ���� ũ���� ���� ����(Depth Buffer)�� �����Ѵ�.
	D3D11_TEXTURE2D_DESC d3dDepthStencilBufferDesc;
	cout << m_nWndClientWidth << "  " << m_nWndClientHeight << endl;
	ZeroMemory(&d3dDepthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
	d3dDepthStencilBufferDesc.Width = m_nWndClientWidth;
	d3dDepthStencilBufferDesc.Height = m_nWndClientHeight;
	d3dDepthStencilBufferDesc.MipLevels = 1;
	d3dDepthStencilBufferDesc.ArraySize = 1;
	d3dDepthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dDepthStencilBufferDesc.SampleDesc.Count = 1;
	d3dDepthStencilBufferDesc.SampleDesc.Quality = 0;
	d3dDepthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dDepthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	d3dDepthStencilBufferDesc.CPUAccessFlags = 0;
	d3dDepthStencilBufferDesc.MiscFlags = 0;
	if (FAILED(hResult = m_pd3dDevice->CreateTexture2D(&d3dDepthStencilBufferDesc, NULL, &m_pd3dDepthStencilBuffer))) return(false);

	//������ ���� ����(Depth Buffer)�� ���� �並 �����Ѵ�.
	D3D11_DEPTH_STENCIL_VIEW_DESC d3dViewDesc;
	ZeroMemory(&d3dViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	d3dViewDesc.Format = d3dDepthStencilBufferDesc.Format;
	d3dViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	d3dViewDesc.Texture2D.MipSlice = 0;
	if (FAILED(hResult = m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, &d3dViewDesc, &m_pd3dDepthStencilView))) return(false);

	m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);

	return(true);
}
void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	//if (m_pScene) m_pScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);

	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		//���콺 ĸ�ĸ� �ϰ� ���� ���콺 ��ġ�� �����´�.
		SetCapture(hWnd);
		GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		//���콺 ĸ�ĸ� �����Ѵ�.
		ReleaseCapture();
		break;
	}
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pScene) m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);

	CGameTimer* GameTimer = CGameTimer::GetCGameTimer();

	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case VK_RETURN:
			break;
		case VK_F10:
			break;
			/*��F1�� Ű�� ������ 1��Ī ī�޶�, ��F2�� Ű�� ������ �����̽�-�� ī�޶�� �����Ѵ�, ��F3�� Ű�� ������ 3��Ī ī�޶�� �����Ѵ�.*/
		case VK_F1:
		case VK_F2:
		case VK_F3:
			if (m_pPlayer)
			{
				//m_pPlayer->ChangeCamera(m_pd3dDevice, (wParam - VK_F1 + 1), m_GameTimer.GetTimeElapsed());
				m_pPlayer->ChangeCamera(m_pDirect3D->GetDevice(), (wParam - VK_F1 + 1), GameTimer->GetTimeElapsed());
				m_pCamera = m_pPlayer->GetCamera();
				//���� ���� ī�޶� �����Ѵ�.
				m_pScene->SetCamera(m_pCamera);
			}
			break;

		default:
			break;
		}
		break;
	default:
		break;
	}
}

LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
	{
		m_nWndClientWidth = LOWORD(lParam);
		m_nWndClientHeight = HIWORD(lParam);

		cout << "������ ũ�� ����  " << m_nWndClientWidth << "    " << m_nWndClientHeight << endl;
	/*	m_pd3dDeviceContext->OMSetRenderTargets(0, NULL, NULL);

		if (m_pd3dRenderTargetView) m_pd3dRenderTargetView->Release();
		if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
		if (m_pd3dDepthStencilView) m_pd3dDepthStencilView->Release();

		m_pDXGISw
		
		apChain->ResizeBuffers(1, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0);*/

		m_pDirect3D->GetDeviceContext()->OMSetRenderTargets(0, NULL, NULL);

		if (m_pDirect3D->GetRenderTargetView())m_pDirect3D->GetRenderTargetView()->Release();
		if (m_pDirect3D->GetDepthStencilBuffer()) m_pDirect3D->GetDepthStencilBuffer()->Release();
		if (m_pDirect3D->GetDepthStencilView()) m_pDirect3D->GetDepthStencilView()->Release();

		m_pDirect3D->GetSwapChain()->ResizeBuffers(1, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		m_pDirect3D->CreateRenderTargetDepthStencilView(m_nWndClientWidth, m_nWndClientHeight);


		//CreateRenderTargetDepthStencilView();
		

		CCamera *pCamera = m_pPlayer->GetCamera();
		//if (pCamera) pCamera->SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);
		if (pCamera) pCamera->SetViewport(m_pDirect3D->GetDeviceContext(), 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(0);
}

//���� �Լ��� ���� ���α׷��� ����� �� ȣ��ȴٴ� �Ϳ� �����϶�. 
//�ּ�
void CGameFramework::OnDestroy()
{
	ReleaseObjects();

	m_pDirect3D->OnDestroy();
	if(m_pDirect3D)
		delete m_pDirect3D;
	//if (m_pd3dDeviceContext) m_pd3dDeviceContext->ClearState();
	//if (m_pd3dRenderTargetView) m_pd3dRenderTargetView->Release();
	//if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
	//if (m_pd3dDepthStencilView) m_pd3dDepthStencilView->Release();
	//if (m_pDXGISwapChain) m_pDXGISwapChain->Release();
	//if (m_pd3dDeviceContext) m_pd3dDeviceContext->Release();
	//if (m_pd3dDevice) m_pd3dDevice->Release();
}

void CGameFramework::BuildObjects()
{
	/*CShader::CreateShaderVariables(m_pd3dDevice);
	CIlluminatedShader::CreateShaderVariables(m_pd3dDevice);*/
	CShader::CreateShaderVariables(m_pDirect3D->GetDevice());
	CIlluminatedShader::CreateShaderVariables(m_pDirect3D->GetDevice());

	m_pSkyBoxShader = new CSkyBoxShader();
	//m_pSkyBoxShader->CreateShader(m_pd3dDevice);
	//m_pSkyBoxShader->BuildObjects(m_pd3dDevice);
	m_pSkyBoxShader->CreateShader(m_pDirect3D->GetDevice());
	m_pSkyBoxShader->BuildObjects(m_pDirect3D->GetDevice());

	string strFileName = "Data/warrior_Vertex.txt";
	CCharacterMesh *pWarriorMesh = new CCharacterMesh(m_pDirect3D->GetDevice(), strFileName);

	m_pScene = new CScene();
//	m_pScene->BuildObjects(m_pd3dDevice);
	m_pScene->BuildObjects(m_pDirect3D->GetDevice());

	m_pPlayerShader = new CPlayerShader();
	/*m_pPlayerShader->CreateShader(m_pd3dDevice);
	m_pPlayerShader->BuildObjects(m_pd3dDevice);
*/
	m_pPlayerShader->CreateShader(m_pDirect3D->GetDevice());
	m_pPlayerShader->BuildObjects(m_pDirect3D->GetDevice(), pWarriorMesh);
	
	m_pPlayer = m_pPlayerShader->GetPlayer();
	m_pPlayer->CreateBoundingBox(14.0f, 14.0f);
	// Ư���� ���̴�
	m_pOtherPlayerShader = new COtherPlayerShader();
	m_pOtherPlayerShader->CreateShader(m_pDirect3D->GetDevice());;
	m_pOtherPlayerShader->BuildObjects(m_pDirect3D->GetDevice(), pWarriorMesh);;

	/*������ xz-����� ����� �÷��̾ ��ġ�ϵ��� �Ѵ�. �÷��̾��� y-��ǥ�� ������ ���� ���� ũ��
	�߷��� �ۿ��ϵ��� �÷��̾ �����Ͽ����Ƿ� �÷��̾�� ���������� �ϰ��ϰ� �ȴ�.*/
	CHeightMapTerrain *pTerrain = m_pScene->GetTerrain();

	// ����
	ClientServer *s = ClientServer::getInstangce();
	m_pPlayer->SetPosition(s->Player[0].getPlayerPosition());


	m_pPlayer->SetPosition(D3DXVECTOR3   // pTerrain->GetPeakHeight() + 1000.0f
		(100.0f, 266.0f, 100.f));




	//m_pPlayer->Scale(D3DXVECTOR3(0.3f, 0.3f, 0.3f));

	//m_pPlayer->SetPosition(D3DXVECTOR3(2000.0f, 0.0f, 1880.0f));
	//�÷��̾��� ��ġ�� ����� �� ������ ������ ���� �÷��̾��� ��ġ�� ������ �� �ֵ��� �����Ѵ�.
	m_pPlayer->SetPlayerUpdatedContext(pTerrain);
	//ī�޶��� ��ġ�� ����� �� ������ ������ ���� ī�޶��� ��ġ�� ������ �� �ֵ��� �����Ѵ�.
	m_pPlayer->SetCameraUpdatedContext(pTerrain);

	m_pCamera = m_pPlayer->GetCamera();
	//m_pCamera->SetViewport(m_pd3dDeviceContext, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	m_pCamera->SetViewport(m_pDirect3D->GetDeviceContext(), 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	
	m_pCamera->GenerateViewMatrix();

	m_pCamera->GenerateOrthoMatrix(1200, 800);

	//m_pCamera->DSCreateShaderVariables(m_pd3dDevice);

	m_pScene->SetCamera(m_pCamera);

}

void CGameFramework::ReleaseObjects()
{
	//CShader Ŭ������ ����(static) ��� ������ ����� ��� ���۸� ��ȯ�Ѵ�.
	CShader::ReleaseShaderVariables();
	CIlluminatedShader::ReleaseShaderVariables();

	if (m_pScene) m_pScene->ReleaseObjects();
	if (m_pScene) delete m_pScene;

	if (m_pPlayerShader) m_pPlayerShader->ReleaseObjects();
	if (m_pPlayerShader) delete m_pPlayerShader;
	if (m_pOtherPlayerShader) delete m_pOtherPlayerShader;
}
void CGameFramework::ProcessInput()
{
	ClientServer *s = ClientServer::getInstangce();
	bool bProcessedByScene = false;
	if (m_pScene) bProcessedByScene = m_pScene->ProcessInput();

	CGameTimer* GameTimer = CGameTimer::GetCGameTimer();
	DWORD dwDirection = 0;
	DWORD dwAttack = 0;
	if (!bProcessedByScene)
	{
		static UCHAR pKeyBuffer[256];
		
		/*Ű������ ���� ������ ��ȯ�Ѵ�. ȭ��ǥ Ű(���桯, ���硯, ���衯, ���顯)�� ������ �÷��̾ ������/����(���� x-��), ��/��(���� z-��)�� �̵��Ѵ�. ��Page Up���� ��Page Down�� Ű�� ������ �÷��̾ ��/�Ʒ�(���� y-��)�� �̵��Ѵ�.*/
		if (GetKeyboardState(pKeyBuffer))
		{
			if (pKeyBuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
			if (pKeyBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
			if (pKeyBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
			if (pKeyBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;
			if (pKeyBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
			if (pKeyBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;

			if ((pKeyBuffer['Q'] & 0xF0)    && m_pPlayer->GetAnimationState() != ANIMATAION_CLIP_ATTACK1)
			{
				dwAttack = ATTACK01;
				//���� ����Ű �Է�
				ClientServer *s = ClientServer::getInstangce();
				s->keyDownAttacket(dwAttack);		
			}
			else if (pKeyBuffer['W'] & 0xF0 && m_pPlayer->GetAnimationState() != ANIMATAION_CLIP_ATTACK2)
			{
				dwAttack = ATTACK02;
				//���� ����Ű �Է�
				//ClientServer *s = ClientServer::getInstangce();
				//s->keyDownAttacket(dwAttack);	
			}

			//����
			//else if(attackCutState != s->Player[0].getState())
			//{
			//	s->keyUp();		
			//}
		
		}

		float cxDelta = 0.0f, cyDelta = 0.0f;
		POINT ptCursorPos;
		/*���콺�� ĸ�������� ���콺�� �󸶸�ŭ �̵��Ͽ��� ���� ����Ѵ�. ���콺 ���� �Ǵ� ������ ��ư�� ������ ���� �޽���(WM_LBUTTONDOWN, WM_RBUTTONDOWN)�� ó���� �� ���콺�� ĸ���Ͽ���. �׷��Ƿ� ���콺�� ĸ�ĵ� ���� ���콺 ��ư�� ������ ���¸� �ǹ��Ѵ�. ���콺�� �¿� �Ǵ� ���Ϸ� �����̸� �÷��̾ x-�� �Ǵ� y-������ ȸ���Ѵ�.*/

		//if (GetCapture() == m_hWnd)
		//{
		//	//���콺 Ŀ���� ȭ�鿡�� ���ش�(������ �ʰ� �Ѵ�).
		//	SetCursor(NULL);
		//	//���� ���콺 Ŀ���� ��ġ�� �����´�.
		//	GetCursorPos(&ptCursorPos);
		//	//���콺 ��ư�� ���� ä�� ���� ��ġ���� ���� ���콺 Ŀ���� ��ġ���� ������ ���� ���Ѵ�.
		//	cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		//	cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		//	SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		//}
		
		//�÷��̾ �̵��ϰų�(dwDirection) ȸ���Ѵ�(cxDelta �Ǵ� cyDelta).
		if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
		{
			if (cxDelta || cyDelta)
			{
				/*cxDelta�� y-���� ȸ���� ��Ÿ���� cyDelta�� x-���� ȸ���� ��Ÿ����. ������ ���콺 ��ư�� ������ ��� cxDelta�� z-���� ȸ���� ��Ÿ����.*/
				if (pKeyBuffer[VK_RBUTTON] & 0xF0)
					m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
				else
					m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
			}
			/*�÷��̾ dwDirection �������� �̵��Ѵ�(�����δ� �ӵ� ���͸� �����Ѵ�).
			�̵� �Ÿ��� �ð��� ����ϵ��� �Ѵ�. �÷��̾��� �̵� �ӷ��� (50/��)�� �����Ѵ�.
			���� �÷��̾��� �̵� �ӷ��� �ִٸ� �� ���� ����Ѵ�.*/
			//if (dwDirection) m_pPlayer->Move(dwDirection, 150.0f * m_GameTimer.GetTimeElapsed(), true);
			if (dwDirection && (m_pPlayer->GetAnimationState() != ANIMATAION_CLIP_ATTACK1
				&& m_pPlayer->GetAnimationState() != ANIMATAION_CLIP_ATTACK2))
			{
				m_pPlayer->Rotate(dwDirection, dwAttack);
				m_pPlayer->Move(dwDirection, PLAYER_SPEED * GameTimer->GetTimeElapsed(), true);
				// 180.
			}
		}
		// ���ð��� ����Ű �Է����� �����Ѵ�.
		m_pPlayer->UpdateAnimation(dwDirection, dwAttack);
	}

	//�÷��̾ ������ �̵��ϰ� ī�޶� �����Ѵ�. �߷°� �������� ������ �ӵ� ���Ϳ� �����Ѵ�.
	//m_pPlayer->Update(GameTimer->GetTimeElapsed());
	//����
//	ClientServer *s = ClientServer::getInstangce();
	//m_pPlayer->Update(s->time);

	// Ŭ��
	m_pPlayer->Update(GameTimer->GetTimeElapsed(), dwDirection);
}

//  Ÿ�̸ӿ��� ������ ������ ���� ����� �ð��� �Ķ���ͷ� �����Ͽ� 
//  CScene Ŭ���� ��ü�� AnimateObjects() ��� �Լ��� ȣ���Ѵ�. 
void CGameFramework::AnimateObjects()
{
	ClientServer *s = ClientServer::getInstangce();
	CGameTimer* GameTimer = CGameTimer::GetCGameTimer();

	// �÷��̾ ������ ��� ������Ʈ ������Ʈ ���.
	if (m_pScene)
		m_pScene->AnimateObjects(GameTimer->GetTimeElapsed());

	// �̰� ���ÿ�...
	if (m_pPlayer)
		m_pPlayer->Animate(GameTimer->GetTimeElapsed());

	//����
	m_pOtherPlayerShader->AnimateObjects(GameTimer->GetTimeElapsed());

	//���� �÷��̾� �ʱ���ǥ �������ִ� �κ�
	//m_pPlayer->SetPosition(s->Player[0].getPlayerPosition());


	//if (m_pPlayer)
	//	m_pPlayer->Move(D3DXVECTOR3(0.0f, 0.0f, GameTimer->GetTimeElapsed())* 200, false);
}

void CGameFramework::FrameAdvance()
{
	CGameTimer* GameTimer = CGameTimer::GetCGameTimer();
	GameTimer->Tick();



	//m_GameTimer.Tick();

	ProcessInput();
	//������Ʈ
	AnimateObjects();

	float fClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	//if (m_pd3dRenderTargetView) m_pd3dDeviceContext->ClearRenderTargetView(m_pd3dRenderTargetView, fClearColor);
	//if (m_pd3dDepthStencilView) m_pd3dDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	if (m_pDirect3D->GetRenderTargetView()) m_pDirect3D->GetDeviceContext()-> ClearRenderTargetView(m_pDirect3D->GetRenderTargetView(), fClearColor);
	if (m_pDirect3D->GetDepthStencilView()) m_pDirect3D->GetDeviceContext()->ClearDepthStencilView(m_pDirect3D->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);




	// ī�޶�-���� ����� ���� ���̴��� �����Ѵ�.
	if (m_pPlayer)
	{
		m_pPlayer->UpdateShaderVariables(m_pDirect3D->GetDeviceContext());
		m_pPlayer->GetCamera()->DSUpdateShaderVariables(m_pDirect3D->GetDeviceContext());
		
	}
	

	//CCamera *pCamera = (m_pPlayer) ? m_pPlayer->GetCamera() : NULL;

	m_pSkyBoxShader->Render(m_pDirect3D->GetDeviceContext(),                                    m_pDirect3D, m_pCamera);
	if (m_pPlayerShader) m_pPlayerShader->Render(m_pDirect3D->GetDeviceContext(), m_pDirect3D, m_pCamera);
	if (m_pOtherPlayerShader) m_pOtherPlayerShader->Render(m_pDirect3D->GetDeviceContext(), m_pDirect3D, m_pCamera);
	
	if (m_pScene) m_pScene->Render(m_pDirect3D->GetDeviceContext(),                           m_pDirect3D, m_pCamera);


	/*���� Ÿ���� �״�� �ΰ� ���� ���۸� 1.0���� �����.
	���� �÷��̾ �������ϸ� �÷��̾�� ������ �׷��� ���̴�.*/
	m_pDirect3D->GetDeviceContext()->ClearDepthStencilView(m_pDirect3D->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	m_pDirect3D->GetSwapChain()->Present(0, 0);
	//m_pDXGISwapChain->Present(0, 0);

	GameTimer->GetFrameRate(m_pszBuffer + 12, 37);
	//m_GameTimer.GetFrameRate(m_pszBuffer + 12, 37);
	::SetWindowText(m_hWnd, m_pszBuffer);


}