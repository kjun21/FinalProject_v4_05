// LabProject03.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "LabProject08.h"
#include "GameFramework.h"



#define MAX_LOADSTRING 100
ClientServer *s = ClientServer::getInstangce();

HINSTANCE						ghAppInstance;								
TCHAR							szTitle[MAX_LOADSTRING];					
TCHAR							szWindowClass[MAX_LOADSTRING];			

CGameFramework					gGameFramework;      

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);


int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	::LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	::LoadString(hInstance, IDC_LABPROJECT03, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// // 응용프로그램 초기화를 수행합니다.
	if (!InitInstance (hInstance, nCmdShow)) 
		return(FALSE);

	
	hAccelTable = ::LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LABPROJECT03));

	while (1) 
	{
		//PeekMessage() 메세지가 있으면 꺼내고, TRUE를 반환한다.
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{
			if (msg.message == WM_QUIT) 
				break;
			if (!::TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		} 
		//메세지가 없으면 FALSE를 반환하여
		// GameFramework.FrameAdvance() 함수를 호출하여 게임 프로그램이 CPU를 사용할 수 있도록 해야 한다.
		else 
		{
			gGameFramework.FrameAdvance();
		}
	}
	//메세제 루프가 종료되면 프레임 워크 객체를 소멸시킨다.
	gGameFramework.OnDestroy();

	return((int)msg.wParam);
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LABPROJECT03));
	wcex.hCursor		= ::LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_LABPROJECT03);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= ::LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	
	return ::RegisterClassEx(&wcex);
	LPVOID;

}

//다음은 주 윈도우를 생성하고 화면에 보이도록 하는 함수이다.
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

	ghAppInstance = hInstance; 
	s->setHINSTANCE(hInstance);

	RECT rc = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
	AdjustWindowRect(&rc, dwStyle, FALSE);
	HWND hMainWnd = CreateWindow(szWindowClass, szTitle, dwStyle, 0,0,
		                                                        rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
	if (!hMainWnd) return(FALSE);

	// 프로그램의 주 윈도우가 생성되면 CGameFramework 
	// 클래스의 OnCreate() 함수를 호출하여 프레임워크 객체를 초기화하도록 한다.
	s->setSocketHWND(hMainWnd);
	//서버 초기화 부분
	//s->socketInit();

	gGameFramework.OnCreate(hInstance, hMainWnd);

	::ShowWindow(hMainWnd, nCmdShow);
	::UpdateWindow(hMainWnd);


	return(TRUE);
}

// 다음은 윈도우 클래스를 등록할 때 설정한 윈도우 프로시져 함수이다
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
        case WM_SIZE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MOUSEMOVE:
        case WM_KEYDOWN:
        case WM_KEYUP:
			gGameFramework.OnProcessingWindowMessage(hWnd, message, wParam, lParam);
			break;
		case WM_COMMAND:
			wmId = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			switch (wmId)
			{
				case IDM_ABOUT:
					::DialogBox(ghAppInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
					break;
				case IDM_EXIT:
					::DestroyWindow(hWnd);
					break;
				default:
					return(::DefWindowProc(hWnd, message, wParam, lParam));
			}
			break;
		case WM_PAINT:
			hdc = ::BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;
		//AsyncSelcet Model Server Data Read
		case WM_SOCKET:
		{
			if (WSAGETSELECTERROR(lParam)) {
				closesocket((SOCKET)wParam);
				break;
			}
			switch (WSAGETSELECTEVENT(lParam)) {
			case FD_READ:
				s->readPacket();
				//	InvalidateRect(hWnd, NULL, TRUE);
				break;
			case FD_CLOSE:
				closesocket((SOCKET)wParam);
				break;
			}
		}
			break;
		case WM_DESTROY:
			::PostQuitMessage(0);
			break;
		default:
			return(::DefWindowProc(hWnd, message, wParam, lParam));
	}
	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
		case WM_INITDIALOG:
			return((INT_PTR)TRUE);
		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				::EndDialog(hDlg, LOWORD(wParam));
				return((INT_PTR)TRUE);
			}
			break;
	}
	return((INT_PTR)FALSE);
}
