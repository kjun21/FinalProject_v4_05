#pragma once
#include "stdafx.h"
#include "Shader.h"
#define WATER_HEIGHT 220

struct ClipPlaneBufferType
{
	D3DXVECTOR4 clipPlane;
};
struct ReflectionMatrix
{
	D3DXMATRIX m_d3dmtxReflection;
};

class CWaveShader : public CDisplacementMappingShader
{
protected:
	ID3D11Buffer* m_pd3dcbReflectionMatrixBuffer;
public:
	CWaveShader();

	virtual ~CWaveShader();
	void CreateConstantBuffer(ID3D11Device *pd3dDevice);
	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void BuildObjects(ID3D11Device *pd3dDevice, CMaterial *pMaterial, CTexture *pTexture);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera);
};


class CRefractionShader : public CInstancingShader
{
public:
	CRefractionShader();
	virtual ~CRefractionShader();
	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void BuildObjects(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera);
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
	
	ID3D11Buffer* m_clipPlaneBuffer;
public:
	bool CreateRenderToTexture(ID3D11Device *pd3dDevice, int textureWidth, int textureHeight);
	void CreateConstantBuffer(ID3D11Device *pd3dDevice);
	void SetRenderTarget(ID3D11DeviceContext*, CDirect3DBase* m_pDirect3D);
	void ClearRenderTarget(ID3D11DeviceContext*, CDirect3DBase* m_pDirect3D, float, float, float, float);
	static ID3D11ShaderResourceView* m_pd3dRefractionShaderResourceView;
};


class CReflectionShader : public CInstancingShader
{
public:
	CReflectionShader();
	virtual ~CReflectionShader();
	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void BuildObjects(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera);
protected:
	ID3D11Buffer *m_pd3dWaterLily01InstanceBuffer;
	ID3D11Buffer *m_pd3dWaterLily02InstanceBuffer;
	ID3D11Buffer *m_pd3dRootTreeInstanceBuffer;

	UINT m_nInstanceBufferStride;
	UINT m_nInstanceBufferOffset;
	CTexture* m_pTexture;
	CMaterial* m_pMaterial;

	//Reflection
	ID3D11Texture2D* m_renderTargetTexture;
	ID3D11RenderTargetView* m_renderTargetView;
	
	ID3D11Buffer* m_clipPlaneBuffer;
public:
	bool CreateRenderToTexture(ID3D11Device *pd3dDevice, int textureWidth, int textureHeight);
	//void CreateConstantBuffer(ID3D11Device *pd3dDevice);
	void SetRenderTarget(ID3D11DeviceContext*, CDirect3DBase* m_pDirect3D);
	void ClearRenderTarget(ID3D11DeviceContext*, CDirect3DBase* m_pDirect3D, float, float, float, float);
	static ID3D11ShaderResourceView* m_pd3dReflectionShaderResourceView;
};



class CGeneralShader : public CInstancingShader
{
public:
	CGeneralShader();
	virtual ~CGeneralShader();
	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void BuildObjects(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CDirect3DBase* m_pDirect3D, CCamera *pCamera);
protected:
	ID3D11Buffer *m_pd3dWaterLily01InstanceBuffer;
	ID3D11Buffer *m_pd3dWaterLily02InstanceBuffer;
	ID3D11Buffer *m_pd3dRootTreeInstanceBuffer;

	UINT m_nInstanceBufferStride;
	UINT m_nInstanceBufferOffset;
	CTexture* m_pTexture;

	CMaterial* m_pMaterial;
};