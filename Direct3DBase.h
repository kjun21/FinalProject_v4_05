#pragma once
#include "stdafx.h"
class CDirect3DBase
{
public:
	CDirect3DBase();
	~CDirect3DBase();

private:
	HINSTANCE					m_hInstance;
	HWND						m_hWnd;

	int							m_nWndClientWidth;
	int							m_nWndClientHeight;

	//디바이스 인터페이스에 대한 포인터이다. 주로 리소스를 생성하기 위하여 필요하다.
	ID3D11Device				*m_pd3dDevice;

	//스왑 체인 인터페이스에 대한 포인터이다. 주로 디스플레이를 제어하기 위하여 필요하다.
	IDXGISwapChain				*m_pDXGISwapChain;

	//렌더 타겟 뷰 인터페이스에 대한 포인터이다. 
	ID3D11RenderTargetView		*m_pd3dRenderTargetView;

	//디바이스 컨텍스트에 대한 포인터이다.주로 파이프라인 설정을 하기 위하여 필요하다.
	ID3D11DeviceContext			*m_pd3dDeviceContext;



	//다음은 프레임 레이트를 주 윈도우의 캡션에 출력하기 위한 문자열이다.
	_TCHAR						m_pszBuffer[50];



	ID3D11VertexShader*     g_pVertexShader;
	ID3D11PixelShader*      g_pPixelShader;

private:
	ID3D11Texture2D *m_pd3dDepthStencilBuffer;
	ID3D11DepthStencilView *m_pd3dDepthStencilView;
private:
	ID3D11Buffer *m_pd3dcbColor;
public:
	bool CreateRenderTargetDepthStencilView();
	bool CreateRenderTargetDepthStencilView(int nWndClientWidth, int nWndClientHeight);
	void OnDestroy();
	bool CreateDirect3DDisplay(HINSTANCE hInstance, HWND hMainWnd);

	ID3D11DeviceContext* GetDeviceContext() { return m_pd3dDeviceContext; }
	ID3D11RenderTargetView* GetRenderTargetView() { return m_pd3dRenderTargetView; }
	IDXGISwapChain* GetSwapChain() { return m_pDXGISwapChain; }
	ID3D11Device* GetDevice() { return m_pd3dDevice; }
	ID3D11DepthStencilView* GetDepthStencilView() { return m_pd3dDepthStencilView; }
	ID3D11Texture2D* GetDepthStencilBuffer() { return m_pd3dDepthStencilBuffer; }
	
	
	void SetBackBufferRenderTarget() {
		m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);
	}
	void BeginScene()
	{
		float fClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
		 m_pd3dDeviceContext->ClearRenderTargetView(m_pd3dRenderTargetView, fClearColor);
		 m_pd3dDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}
};

