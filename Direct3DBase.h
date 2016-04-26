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

	//����̽� �������̽��� ���� �������̴�. �ַ� ���ҽ��� �����ϱ� ���Ͽ� �ʿ��ϴ�.
	ID3D11Device				*m_pd3dDevice;

	//���� ü�� �������̽��� ���� �������̴�. �ַ� ���÷��̸� �����ϱ� ���Ͽ� �ʿ��ϴ�.
	IDXGISwapChain				*m_pDXGISwapChain;

	//���� Ÿ�� �� �������̽��� ���� �������̴�. 
	ID3D11RenderTargetView		*m_pd3dRenderTargetView;

	//����̽� ���ؽ�Ʈ�� ���� �������̴�.�ַ� ���������� ������ �ϱ� ���Ͽ� �ʿ��ϴ�.
	ID3D11DeviceContext			*m_pd3dDeviceContext;



	//������ ������ ����Ʈ�� �� �������� ĸ�ǿ� ����ϱ� ���� ���ڿ��̴�.
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

