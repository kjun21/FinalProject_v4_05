#pragma once
#include "stdafx.h"
#include "Shader.h"

struct ClipPlaneBufferType
{
	D3DXVECTOR4 clipPlane;
};

class CWaveShader : public CDisplacementMappingShader
{
public:
	CWaveShader();

	virtual ~CWaveShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void BuildObjects(ID3D11Device *pd3dDevice, CMaterial *pMaterial, CTexture *pTexture);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, ID3D11DepthStencilView *pd3dDepthStencilView, CCamera *pCamera);
};


class CRefractionShader : public CInstancingShader
{
public:
	CRefractionShader();
	virtual ~CRefractionShader();
	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void BuildObjects(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, ID3D11DepthStencilView *pd3dDepthStencilView, CCamera *pCamera);
protected:
	ID3D11Buffer *m_pd3dWaterLily01InstanceBuffer;
	ID3D11Buffer *m_pd3dWaterLily02InstanceBuffer;
	ID3D11Buffer *m_pd3dRootTreeInstanceBuffer;



	UINT m_nInstanceBufferStride;
	UINT m_nInstanceBufferOffset;
	CTexture* m_pTexture;

	CMaterial* m_pMaterial;

	//±¼Àý
	ID3D11Texture2D* m_renderTargetTexture;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11ShaderResourceView* m_shaderResourceView;
	ID3D11Buffer* m_clipPlaneBuffer;
public:
	bool CreateRenderToTexture(ID3D11Device *pd3dDevice, int textureWidth, int textureHeight);
	void CreateConstantBuffer(ID3D11Device *pd3dDevice);
	void SetRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*);
	void ClearRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*, float, float, float, float);
};


class CReflectionnShader : public CInstancingShader
{
public:
	CReflectionnShader();
	virtual ~CReflectionnShader();
	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void BuildObjects(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, ID3D11DepthStencilView *pd3dDepthStencilView, CCamera *pCamera);
protected:
	ID3D11Buffer *m_pd3dWaterLily01InstanceBuffer;
	ID3D11Buffer *m_pd3dWaterLily02InstanceBuffer;
	ID3D11Buffer *m_pd3dRootTreeInstanceBuffer;



	UINT m_nInstanceBufferStride;
	UINT m_nInstanceBufferOffset;
	CTexture* m_pTexture;

	CMaterial* m_pMaterial;
};