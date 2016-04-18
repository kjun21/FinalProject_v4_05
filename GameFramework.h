#pragma once

#include "Timer.h"
#include "Scene.h"
#include "Player.h"
#include "AirplanePlayer.h"
#include "AirplaneMesh.h"
#include "GameManager.h"


class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	//����̽�, ���� ü��, ����̽� ���ؽ�Ʈ, ����̽��� ���õ� �並 �����ϴ� �Լ��̴�.
	//bool CreateRenderTargetView();
	//void SetViewport();
	
	bool CreateDirect3DDisplay();
    
	//�������� �޽�, ��ü�� �����ϰ� �Ҹ��ϴ� �Լ��̴�. 
    void BuildObjects();
    void ReleaseObjects();

	//�����ӿ�ũ�� �ٽ�(����� �Է�, �ִϸ��̼�, ������)�� �����ϴ� �Լ��̴�. 
    void ProcessInput();
    void AnimateObjects();
    void FrameAdvance();

	//�������� �޽���(Ű����, ���콺 �Է�)�� ó���ϴ� �Լ��̴�. 
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

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

	//������ ���� �����ӿ�ũ���� ����� Ÿ�̸��̴�.
	CGameTimer					m_GameTimer;

	//������ ������ ���(Scene)�� �����ϴ� ��ü�� ���� �����͸� ��Ÿ����.
	CScene						*m_pScene;

	//������ ������ ����Ʈ�� �� �������� ĸ�ǿ� ����ϱ� ���� ���ڿ��̴�.
	_TCHAR						m_pszBuffer[50];



	CPlayer *m_pPlayer;

	ID3D11VertexShader*     g_pVertexShader;
	ID3D11PixelShader*      g_pPixelShader;

private:
	ID3D11Buffer *m_pd3dcbColor;


	//���������� ���콺 ��ư�� Ŭ���� ���� ���콺 Ŀ���� ��ġ�̴�.
	POINT	m_ptOldCursorPos;


private:
	ID3D11Texture2D *m_pd3dDepthStencilBuffer;
	ID3D11DepthStencilView *m_pd3dDepthStencilView;
public:
	bool CreateRenderTargetDepthStencilView();
public:
	CPlayerShader *m_pPlayerShader;
	CSkyBoxShader* m_pSkyBoxShader;


private:
	CCamera *m_pCamera;

};
