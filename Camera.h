#pragma once
#include "stdafx.h"
#include "Mesh.h"

#define FIRST_PERSON_CAMERA	0x01
#define SPACESHIP_CAMERA	 0x02
#define THIRD_PERSON_CAMERA	0x03

//������ ������ ũ��� ��Ⱦ��(Aspect Ratio)�� ��Ÿ���� ����� ������ ���� �����Ѵ�.
#define ASPECT_RATIO	(float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))

struct VS_CB_CAMERA
{
	D3DXMATRIX m_d3dxmtxView;
	D3DXMATRIX m_d3dxmtxProjection;
};


struct DS_CB_CAMERA
{
	D3DXMATRIX m_d3dxmtxView;
	D3DXMATRIX m_d3dxmtxProjection;
};


class CPlayer;

class CCamera
{
protected:
	ID3D11Buffer *m_pd3dcbCameraInDS;
public:
	void DSUpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext);
	void DSCreateShaderVariables(ID3D11Device *pd3dDevice);
	void DSClearShaderVariables(ID3D11DeviceContext *pd3dDeviceContext);


protected:
	//ī�޶� ��ȯ ��İ� ���� ��ȯ ����� ��Ÿ���� ��� ������ �����Ѵ�.
	D3DXMATRIX m_d3dxmtxView;
	D3DXMATRIX m_d3dxmtxProjection;

	//��-��Ʈ�� ��Ÿ���� ��� ������ �����Ѵ�.
	D3D11_VIEWPORT m_d3dViewport;

	//ī�޶� ��ȯ ��İ� ���� ��ȯ ����� ���� ��� ���� �������̽� �����͸� �����Ѵ�.
	ID3D11Buffer *m_pd3dcbCamera;

	//ī�޶� ����� �÷��̾� ��ü�� ���� �����͸� �����Ѵ�.
	CPlayer *m_pPlayer;
public:
	//CCamera Ŭ������ �⺻ �����ڸ� ������ ���� �����Ѵ�.
	CCamera(CCamera *pCamera);
	virtual ~CCamera();


public:
	void SetPlayer(CPlayer *pPlayer) { m_pPlayer = pPlayer; }

	//��-��Ʈ�� �����ϴ� ��� �Լ��� �����Ѵ�.
	void SetViewport(ID3D11DeviceContext *pd3dDeviceContext, DWORD xStart, DWORD yStart, DWORD nWidth, DWORD nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f);

	//ī�޶� ��ȯ ��İ� ���� ��ȯ ����� �����ϴ� ��� �Լ��� �����Ѵ�.
	void GenerateViewMatrix();
	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle);

	/*ī�޶� ������ ȸ���� �ϰ� �Ǹ� ������ �Ǽ������� ����Ȯ�� ������ 
	ī�޶��� ���� x-��(Right), y-��(Up), z-��(LookAt)�� ���� �������� ���� �� �ִ�. 
	ī�޶��� ���� x-��(Right), y-��(Up), z-��(LookAt)�� ���� �����ϵ��� ������ش�.*/
	void RegenerateViewMatrix();

	//��� ���۸� �����ϰ� ������ �����ϴ� ��� �Լ��� �����Ѵ�.
	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext);



protected:
	//ī�޶��� ��ġ(������ǥ��) �����̴�.
	D3DXVECTOR3 m_d3dxvPosition;
	/* ī�޶��� ���� x - ��(Right), y - ��(Up), z - ��(Look)�� ��Ÿ���� �����̴�.*/
	D3DXVECTOR3 m_d3dxvRight;
	D3DXVECTOR3 m_d3dxvUp;
	D3DXVECTOR3 m_d3dxvLook;

	//ī�޶� x-��, z-��, y-������ �󸶸�ŭ ȸ���ߴ� ���� ��Ÿ���� �����̴�.
	float m_fPitch;
	float m_fRoll;
	float m_fYaw;

	//ī�޶��� ����(1��Ī ī�޶�, �����̽�-�� ī�޶�, 3��Ī ī�޶�)�� ��Ÿ����.
	DWORD m_nMode;

	//ī�޶� �ٶ󺸴� ��(������ǥ��)�� ��Ÿ���� �����̴�.
	D3DXVECTOR3 m_d3dxvLookAtWorld;

	//�÷��̾�� ī�޶��� �������� ��Ÿ���� �����̴�. �ַ� 3��Ī ī�޶󿡼� ���ȴ�.
	D3DXVECTOR3 m_d3dxvOffset;
	//�÷��̾ ȸ���� �� �󸶸�ŭ�� �ð��� ������Ų �� ī�޶� ȸ����ų ���ΰ��� ��Ÿ����.
	float m_fTimeLag;


public:
	void SetMode(DWORD nMode) { m_nMode = nMode; }
	DWORD GetMode() { return(m_nMode); }

	CPlayer *GetPlayer() { return(m_pPlayer); }

	D3D11_VIEWPORT GetViewport() { return(m_d3dViewport); }

	D3DXMATRIX GetViewMatrix() { return(m_d3dxmtxView); }
	D3DXMATRIX GetProjectionMatrix() { return(m_d3dxmtxProjection); }
	ID3D11Buffer *GetCameraConstantBuffer() { return(m_pd3dcbCamera); }

	void SetPosition(D3DXVECTOR3 d3dxvPosition) { m_d3dxvPosition = d3dxvPosition; }
	D3DXVECTOR3& GetPosition() { return(m_d3dxvPosition); }

	void SetLookAtPosition(D3DXVECTOR3 d3dxvLookAtWorld) { m_d3dxvLookAtWorld = d3dxvLookAtWorld; }
	D3DXVECTOR3& GetLookAtPosition() { return(m_d3dxvLookAtWorld); }

	D3DXVECTOR3& GetRightVector() { return(m_d3dxvRight); }
	D3DXVECTOR3& GetUpVector() { return(m_d3dxvUp); }
	D3DXVECTOR3& GetLookVector() { return(m_d3dxvLook); }

	float& GetPitch() { return(m_fPitch); }
	float& GetRoll() { return(m_fRoll); }
	float& GetYaw() { return(m_fYaw); }

	void SetOffset(D3DXVECTOR3 d3dxvOffset) 
	{ m_d3dxvOffset = d3dxvOffset; m_d3dxvPosition += d3dxvOffset; }
	D3DXVECTOR3& GetOffset() { return(m_d3dxvOffset); }

	void SetTimeLag(float fTimeLag) { m_fTimeLag = fTimeLag; }
	float GetTimeLag() { return(m_fTimeLag); }

	//ī�޶� d3dxvShift ��ŭ �̵��ϴ� �����Լ��̴�.
	virtual void Move(const D3DXVECTOR3& d3dxvShift) { m_d3dxvPosition += d3dxvShift; }
	//ī�޶� x-��, y-��, z-������ ȸ���ϴ� �����Լ��̴�.
	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f) { }
	//ī�޶��� �̵�, ȸ���� ���� ī�޶��� ������ �����ϴ� �����Լ��̴�.
	
	
	virtual void Update(D3DXVECTOR3& d3dxvLookAt, float fTimeElapsed){}
	/*3��Ī ī�޶󿡼� ī�޶� �ٶ󺸴� ������ �����ϴ� �����Լ��̴�. �Ϲ������� �÷��̾ �ٶ󺸵��� �����Ѵ�.*/
	virtual void SetLookAt(D3DXVECTOR3& vLookAt) { }




	protected:
		//����ü�� 6�� ���(���� ��ǥ��)�� ��Ÿ����.
		D3DXPLANE m_d3dxFrustumPlanes[6];

public:
	//����ü�� 6�� ����� ����Ѵ�.
	void CalculateFrustumPlanes();
	//�ٿ�� �ڽ��� ����ü�� ������ ���Եǰų� �Ϻζ� ���ԵǴ� ���� �˻��Ѵ�.
	bool IsInFrustum(D3DXVECTOR3& d3dxvMinimum, D3DXVECTOR3& d3dxvMaximum);
	bool IsInFrustum(AABB *pAABB);


};


class CFirstPersonCamera : public CCamera
{
public:
	CFirstPersonCamera(CCamera *pCamera);
	virtual ~CFirstPersonCamera() { }

	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
};

class CThirdPersonCamera : public CCamera
{
public:
	CThirdPersonCamera(CCamera *pCamera);
	virtual ~CThirdPersonCamera() { }

	virtual void Update(D3DXVECTOR3& d3dxvLookAt, float fTimeElapsed);
	virtual void SetLookAt(D3DXVECTOR3& vLookAt);
};

class CSpaceShipCamera : public CCamera
{
public:
	CSpaceShipCamera(CCamera *pCamera);
	virtual ~CSpaceShipCamera() { }

	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
};