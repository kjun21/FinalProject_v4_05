#pragma once
#include "stdafx.h"
#include "Shader.h"
#include "GameManager.h"
class CUiShader : public CTexturedShader
{
public:
	CUiShader();
	~CUiShader();
	virtual void BuildObjects(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera);
	virtual void CreateShaderVariables(ID3D11Device * pd3dDevice);
	void CreateDisabledDepthStencilState(ID3D11Device * pd3dDevice);
	virtual void AnimateObjects(float fTimeElapsed);
private:
	int m_nScreenWidth, m_nScreenHeight;
	int m_nBitmapWidth, m_nBitmapHeight;
	int m_nPreviousPosX, m_nPreviousPosY;
	CTexture *m_pTexture;


	ID3D11DepthStencilState* m_pd3dDisabledDepthStencilState;


};

