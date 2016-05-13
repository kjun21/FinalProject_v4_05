//-----------------------------------------------------------------------------
// File: CGameFramework.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "GameFramework.h"
#include <DXGI.h>
using namespace std;

CGameTimer* CGameTimer::Timer;
CGameManager* CGameManager::GameManager;




// Direct3D 디바이스, 스왑 체인, 디바이스 컨텍스트, 그리고 렌더 타겟 뷰 인터페이스 포인터를 NULL로 초기화한다.
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

//다음 함수는 응용 프로그램이 실행되면 호출된다는 것에 유의하라. 
bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_pDirect3D = new CDirect3DBase();

	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	//Direct3D 디바이스, 디바이스 컨텍스트, 스왑 체인 등을 생성하는 함수를 호출한다. 
	if (!m_pDirect3D->CreateDirect3DDisplay(m_hInstance, m_hWnd)) return(false);
	//렌더링할 객체(게임 월드 객체)를 생성한다. 





	//Direct3D 디바이스, 디바이스 컨텍스트, 스왑 체인 등을 생성하는 함수를 호출한다. 
	//if (!CreateDirect3DDisplay()) return(false);

	//렌더링할 객체(게임 월드 객체)를 생성한다. 
	BuildObjects();

	return(true);
}

bool CGameFramework::CreateDirect3DDisplay()
{
	//먼저, 출력 윈도우의 클라이언트 영역의 크기를 멤버 변수에 저장한다.
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

	// Direct3D 디바이스와 스왑 체인을 생성하기
	/*스왑 체인을 생성하기 위한 스왑 체인 구조체 DXGI_SWAP_CHAIN_DESC를 설정한다.
	스왑 체인의 버퍼 크기는 주 윈도우의 클라이언트 영역의 크기로 설정하고 출력 윈도우는 주 윈도우로 설정한다.
	스왑 체인의 출력 모드는 윈도우 모드로 설정한다.*/
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc)); //메모리 초기화
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

	//디바이스를 생성하기 위하여 시도할 드라이버 유형의 순서를 나타낸다.
	//디바이스 생성을 위해 시도할 순서대로 디바이스 드라이버 유형과 특성 레벨을 배열에 저장한다. 
	D3D_DRIVER_TYPE d3dDriverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	UINT nDriverTypes = sizeof(d3dDriverTypes) / sizeof(D3D_DRIVER_TYPE);

	//디바이스를 생성하기 위하여 시도할 특성 레벨의 순서를 나타낸다.
	D3D_FEATURE_LEVEL pd3dFeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	UINT nFeatureLevels = sizeof(pd3dFeatureLevels) / sizeof(D3D_FEATURE_LEVEL);

	//생성할 스왑 체인을 서술하는 구조체이다.

	/*원하는 디바이스 드라이버 유형 순서로 스왑 체인과 디바이스,
	디바이스 컨텍스트를 생성하기 위해 드라이버 유형 배열(d3dDriverTypes[])의 각 원소에 대하여
	D3D11CreateDeviceAndSwapChain() 함수 호출을 통해 디바이스와 스왑체인 생성을 시도한다.*/
	D3D_DRIVER_TYPE nd3dDriverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL nd3dFeatureLevel = D3D_FEATURE_LEVEL_11_0;
	HRESULT hResult = S_OK;

	//디바이스의 드라이버 유형과 특성 레벨을 지원하는 디바이스와 스왑 체인을 생성한다. 
	//  고수준의 디바이스 생성을 시도하고 실패하면 다음 수준의 디바이스를 생성한다.

	/*D3D11CreateDeviceAndSwapChain() 함수를 통하여 Direct3D 디바이스, 스왑 체인, 디바이스 컨텍스트를
	동시에 생성하도록 하자.
	먼저, 디바이스 생성에 필요한 디바이스 생성 플래그, 드라이버 유형, 특성 레벨(Feature Level)를 설정한다.*/
	for (UINT i = 0; i < nDriverTypes; i++)
	{
		nd3dDriverType = d3dDriverTypes[i];
		if (SUCCEEDED(hResult = D3D11CreateDeviceAndSwapChain(NULL, nd3dDriverType, NULL,
			dwCreateDeviceFlags, pd3dFeatureLevels, nFeatureLevels,
			D3D11_SDK_VERSION, &dxgiSwapChainDesc, &m_pDXGISwapChain, &m_pd3dDevice,
			&nd3dFeatureLevel, &m_pd3dDeviceContext))) break;
	}
	//디바이스가 생성되면 렌더 타겟 뷰를 생성하기 위해 CreateRenderTargetView() 함수를 호출한다. 
	if (!CreateRenderTargetDepthStencilView()) return(false);


	return(true);
}

// 이 함수는 스왑 체인의 첫 번째 후면버퍼에 대한 렌더 타겟 뷰를 생성하고 디바이스 컨텍스트에 연결한다
bool CGameFramework::CreateRenderTargetDepthStencilView()
{
	HRESULT hResult = S_OK;

	ID3D11Texture2D *pd3dBackBuffer;
	if (FAILED(hResult = m_pDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pd3dBackBuffer))) return(false);
	if (FAILED(hResult = m_pd3dDevice->CreateRenderTargetView(pd3dBackBuffer, NULL, &m_pd3dRenderTargetView))) return(false);
	if (pd3dBackBuffer) pd3dBackBuffer->Release();

	//렌더 타겟과 같은 크기의 깊이 버퍼(Depth Buffer)를 생성한다.
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

	//생성한 깊이 버퍼(Depth Buffer)에 대한 뷰를 생성한다.
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
		//마우스 캡쳐를 하고 현재 마우스 위치를 가져온다.
		SetCapture(hWnd);
		GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		//마우스 캡쳐를 해제한다.
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
			/*‘F1’ 키를 누르면 1인칭 카메라, ‘F2’ 키를 누르면 스페이스-쉽 카메라로 변경한다, ‘F3’ 키를 누르면 3인칭 카메라로 변경한다.*/
		case VK_F1:
		case VK_F2:
		case VK_F3:
			if (m_pPlayer)
			{
				//m_pPlayer->ChangeCamera(m_pd3dDevice, (wParam - VK_F1 + 1), m_GameTimer.GetTimeElapsed());
				m_pPlayer->ChangeCamera(m_pDirect3D->GetDevice(), (wParam - VK_F1 + 1), GameTimer->GetTimeElapsed());
				m_pCamera = m_pPlayer->GetCamera();
				//씬에 현재 카메라를 설정한다.
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

		cout << "윈도우 크기 변경  " << m_nWndClientWidth << "    " << m_nWndClientHeight << endl;
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

//다음 함수는 응용 프로그램이 종료될 때 호출된다는 것에 유의하라. 
//주석
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
	// 특별한 셰이더
	m_pOtherPlayerShader = new COtherPlayerShader();
	m_pOtherPlayerShader->CreateShader(m_pDirect3D->GetDevice());;
	m_pOtherPlayerShader->BuildObjects(m_pDirect3D->GetDevice(), pWarriorMesh);;

	/*지형의 xz-평면의 가운데에 플레이어가 위치하도록 한다. 플레이어의 y-좌표가 지형의 높이 보다 크고
	중력이 작용하도록 플레이어를 설정하였으므로 플레이어는 점차적으로 하강하게 된다.*/
	CHeightMapTerrain *pTerrain = m_pScene->GetTerrain();

	// 서버
	ClientServer *s = ClientServer::getInstangce();
	m_pPlayer->SetPosition(s->Player[0].getPlayerPosition());


	m_pPlayer->SetPosition(D3DXVECTOR3   // pTerrain->GetPeakHeight() + 1000.0f
		(100.0f, 266.0f, 100.f));




	//m_pPlayer->Scale(D3DXVECTOR3(0.3f, 0.3f, 0.3f));

	//m_pPlayer->SetPosition(D3DXVECTOR3(2000.0f, 0.0f, 1880.0f));
	//플레이어의 위치가 변경될 때 지형의 정보에 따라 플레이어의 위치를 변경할 수 있도록 설정한다.
	m_pPlayer->SetPlayerUpdatedContext(pTerrain);
	//카메라의 위치가 변경될 때 지형의 정보에 따라 카메라의 위치를 변경할 수 있도록 설정한다.
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
	//CShader 클래스의 정적(static) 멤버 변수로 선언된 상수 버퍼를 반환한다.
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
		
		/*키보드의 상태 정보를 반환한다. 화살표 키(‘→’, ‘←’, ‘↑’, ‘↓’)를 누르면 플레이어를 오른쪽/왼쪽(로컬 x-축), 앞/뒤(로컬 z-축)로 이동한다. ‘Page Up’과 ‘Page Down’ 키를 누르면 플레이어를 위/아래(로컬 y-축)로 이동한다.*/
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
				//서버 공격키 입력
				ClientServer *s = ClientServer::getInstangce();
				s->keyDownAttacket(dwAttack);		
			}
			else if (pKeyBuffer['W'] & 0xF0 && m_pPlayer->GetAnimationState() != ANIMATAION_CLIP_ATTACK2)
			{
				dwAttack = ATTACK02;
				//서버 공격키 입력
				//ClientServer *s = ClientServer::getInstangce();
				//s->keyDownAttacket(dwAttack);	
			}

			//서버
			//else if(attackCutState != s->Player[0].getState())
			//{
			//	s->keyUp();		
			//}
		
		}

		float cxDelta = 0.0f, cyDelta = 0.0f;
		POINT ptCursorPos;
		/*마우스를 캡쳐했으면 마우스가 얼마만큼 이동하였는 가를 계산한다. 마우스 왼쪽 또는 오른쪽 버튼이 눌러질 때의 메시지(WM_LBUTTONDOWN, WM_RBUTTONDOWN)를 처리할 때 마우스를 캡쳐하였다. 그러므로 마우스가 캡쳐된 것은 마우스 버튼이 눌려진 상태를 의미한다. 마우스를 좌우 또는 상하로 움직이면 플레이어를 x-축 또는 y-축으로 회전한다.*/

		//if (GetCapture() == m_hWnd)
		//{
		//	//마우스 커서를 화면에서 없앤다(보이지 않게 한다).
		//	SetCursor(NULL);
		//	//현재 마우스 커서의 위치를 가져온다.
		//	GetCursorPos(&ptCursorPos);
		//	//마우스 버튼이 눌린 채로 이전 위치에서 현재 마우스 커서의 위치까지 움직인 양을 구한다.
		//	cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		//	cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		//	SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		//}
		
		//플레이어를 이동하거나(dwDirection) 회전한다(cxDelta 또는 cyDelta).
		if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
		{
			if (cxDelta || cyDelta)
			{
				/*cxDelta는 y-축의 회전을 나타내고 cyDelta는 x-축의 회전을 나타낸다. 오른쪽 마우스 버튼이 눌려진 경우 cxDelta는 z-축의 회전을 나타낸다.*/
				if (pKeyBuffer[VK_RBUTTON] & 0xF0)
					m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
				else
					m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
			}
			/*플레이어를 dwDirection 방향으로 이동한다(실제로는 속도 벡터를 변경한다).
			이동 거리는 시간에 비례하도록 한다. 플레이어의 이동 속력은 (50/초)로 가정한다.
			만약 플레이어의 이동 속력이 있다면 그 값을 사용한다.*/
			//if (dwDirection) m_pPlayer->Move(dwDirection, 150.0f * m_GameTimer.GetTimeElapsed(), true);
			if (dwDirection && (m_pPlayer->GetAnimationState() != ANIMATAION_CLIP_ATTACK1
				&& m_pPlayer->GetAnimationState() != ANIMATAION_CLIP_ATTACK2))
			{
				m_pPlayer->Rotate(dwDirection, dwAttack);
				m_pPlayer->Move(dwDirection, PLAYER_SPEED * GameTimer->GetTimeElapsed(), true);
				// 180.
			}
		}
		// 어택값과 공격키 입력으로 결정한다.
		m_pPlayer->UpdateAnimation(dwDirection, dwAttack);
	}

	//플레이어를 실제로 이동하고 카메라를 갱신한다. 중력과 마찰력의 영향을 속도 벡터에 적용한다.
	//m_pPlayer->Update(GameTimer->GetTimeElapsed());
	//서버
//	ClientServer *s = ClientServer::getInstangce();
	//m_pPlayer->Update(s->time);

	// 클라
	m_pPlayer->Update(GameTimer->GetTimeElapsed(), dwDirection);
}

//  타이머에서 마지막 프레임 이후 경과된 시간을 파라메터로 전달하여 
//  CScene 클래스 객체의 AnimateObjects() 멤버 함수를 호출한다. 
void CGameFramework::AnimateObjects()
{
	ClientServer *s = ClientServer::getInstangce();
	CGameTimer* GameTimer = CGameTimer::GetCGameTimer();

	// 플레이어를 제외한 모든 오브젝트 업데이트 담당.
	if (m_pScene)
		m_pScene->AnimateObjects(GameTimer->GetTimeElapsed());

	// 이건 샘플용...
	if (m_pPlayer)
		m_pPlayer->Animate(GameTimer->GetTimeElapsed());

	//서버
	m_pOtherPlayerShader->AnimateObjects(GameTimer->GetTimeElapsed());

	//서버 플레이어 초기좌표 셋팅해주는 부분
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
	//업데이트
	AnimateObjects();

	float fClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	//if (m_pd3dRenderTargetView) m_pd3dDeviceContext->ClearRenderTargetView(m_pd3dRenderTargetView, fClearColor);
	//if (m_pd3dDepthStencilView) m_pd3dDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	if (m_pDirect3D->GetRenderTargetView()) m_pDirect3D->GetDeviceContext()-> ClearRenderTargetView(m_pDirect3D->GetRenderTargetView(), fClearColor);
	if (m_pDirect3D->GetDepthStencilView()) m_pDirect3D->GetDeviceContext()->ClearDepthStencilView(m_pDirect3D->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);




	// 카메라-투형 행렬을 정점 셰이더에 연결한다.
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


	/*렌더 타겟은 그대로 두고 깊이 버퍼를 1.0으로 지운다.
	이제 플레이어를 렌더링하면 플레이어는 무조건 그려질 것이다.*/
	m_pDirect3D->GetDeviceContext()->ClearDepthStencilView(m_pDirect3D->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	m_pDirect3D->GetSwapChain()->Present(0, 0);
	//m_pDXGISwapChain->Present(0, 0);

	GameTimer->GetFrameRate(m_pszBuffer + 12, 37);
	//m_GameTimer.GetFrameRate(m_pszBuffer + 12, 37);
	::SetWindowText(m_hWnd, m_pszBuffer);


}