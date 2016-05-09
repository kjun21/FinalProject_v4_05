#pragma once
#include "Camera.h"
#include "Object.h"

struct VS_CB_WORLD_MATRIX2
{
	D3DXMATRIX m_d3dxmtxWorld;
};

struct VS_CB_RESULT_MATRIX2
{
	D3DXMATRIX m_d3dxmtxResult[200];
};



class CPlayer : public CGameObject
{
protected:
	// �÷��̾� Ŭ������ ī�޶� ������ �ִ�.
	CCamera *m_pCamera;
	bool  m_bPlayerState;
	bool m_bDamageCheck;
public:
	//�÷��̾ �̵��ϴ� �Լ��̴�.
	void Move(ULONG nDirection, float fDistance, bool bVelocity = false);
	void Move(const D3DXVECTOR3& d3dxvShift, DWORD dwDirection,  bool bVelocity = false);
	void Move(float fxOffset = 0.0f, float fyOffset = 0.0f, float fzOffset = 0.0f);
	//�÷��̾ ȸ���ϴ� �Լ��̴�.
	void Rotate(float x, float y, float z);
	void Rotate(DWORD dwDirection, DWORD dwAttack);
	//�÷��̾��� ��ġ�� ȸ�� ������ ��� �ð��� ���� �����ϴ� �Լ��̴�.
	void Update(float fTimeElapsed, DWORD dwDirection);
	//�÷��̾��� ��ġ�� �ٲ� ������ ȣ��Ǵ� �Լ��� �� �Լ����� ����ϴ� ������ �����ϴ� �Լ��̴�.
	virtual void OnPlayerUpdated(float fTimeElapsed);
	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }
	/*�÷��̾��� ��ġ�� d3dxvPosition ��ġ�� �����Ѵ�.
	d3dxvPosition ���Ϳ��� ���� �÷��̾��� ��ġ ���͸� ����
	���� �÷��̾��� ��ġ���� d3dxvPosition ���������� ���� ���Ͱ� �ȴ�.
	���� �÷��̾��� ��ġ���� �� ���� ���� ��ŭ �̵��Ѵ�.*/
	void SetPosition(const D3DXVECTOR3& d3dxvPosition) { Move((d3dxvPosition - m_d3dxvPosition), false); }
	
	// �浹 ���� �Լ�
public:

	void CreateBoundingBox(float fX, float fY);
	bool CollideAABB(D3DXVECTOR3 d3dxvPosition);

	float CalculateDistance(D3DXVECTOR3 d3dxvinputPosition);
	bool CalculateCollisionRange(D3DXVECTOR3 d3dxvPlayerPosition);
	float CalculateAttackRange(float fAttackRadius, float fBeAttackedRadius);
	void CollisionCheck();
	float GetBeAttackedRadius() { return m_fBeAttackedRadius; }
	float GetAttackRadius(UINT uiAttackNum) { return m_fAttackRadius[uiAttackNum]; }
	virtual bool GetDamageCheck() { return m_bDamageCheck; }
	virtual void ISDamagedByPlayer() { cout << "�÷��̾�, ���ظ� ����!! "<< endl;  m_bDamageCheck = true; }
	virtual void ISPossibleDamageByPlayer() { cout << "�÷��̾�, �ٽ� ���ظ� ������ ����" << endl; m_bDamageCheck = false; }

	virtual void Die() {
		cout << "�÷��̾� ����" << endl;
		m_nAnimationState = ANIMATAION_CLIP_DEATH;
		//m_bPlayerState = MONSTER_STATE_DYING;
	}
public:
	CPlayer(int nMeshes = 1);
	virtual ~CPlayer();
	//�÷��̾��� ���� ī�޶� �����ϰ� ��ȯ�ϴ� ��� �Լ��� �����Ѵ�.
	void SetCamera(CCamera *pCamera) { m_pCamera = pCamera; }
	CCamera *GetCamera() { return(m_pCamera); }
	//�÷��̾��� ��� ���۸� �����ϰ� �����ϴ� ��� �Լ��� �����Ѵ�.
	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext);


protected:
	float m_fAttackRadius[2];
	float m_fBeAttackedRadius;


	//�÷��̾��� ��ġ ����, x-��(Right), y-��(Up), z-��(Look) �����̴�.
	D3DXVECTOR3 m_d3dxvPosition;
	D3DXVECTOR3 m_d3dxvRight;
	D3DXVECTOR3 m_d3dxvUp;
	D3DXVECTOR3 m_d3dxvLook;

	AnimationClip* m_AnimationClip;
	UINT m_nAnimationState;
	ID3D11Buffer	 *m_pd3dcbAnimation;
	D3DXMATRIX **m_ppResultMatrix;
	VS_CB_RESULT_MATRIX2 *m_cbMapData;

public:
	void SetAnimationClip(AnimationClip* animationClip) { m_AnimationClip = animationClip; }
	void UpdateAnimation(DWORD dwDirection, DWORD dwAttack);
	UINT GetAnimationState() { return m_nAnimationState; }
protected:
	//�÷��̾ ���� x-��(Right), y-��(Up), z-��(Look)���� �󸶸�ŭ ȸ���ߴ°��� ��Ÿ����.
	float m_fPitch;
	float m_fYaw;
	float m_fRoll;

	//�÷��̾��� �̵� �ӵ��� ��Ÿ���� �����̴�.
	D3DXVECTOR3 m_d3dxvVelocity;
	//�÷��̾ �ۿ��ϴ� �߷��� ��Ÿ���� �����̴�.
	D3DXVECTOR3 m_d3dxvGravity;
	//xz-��鿡�� (�� ������ ����) �÷��̾��� �̵� �ӷ��� �ִ밪�� ��Ÿ����.
	float m_fMaxVelocityXZ;
	//y-�� �������� (�� ������ ����) �÷��̾��� �̵� �ӷ��� �ִ밪�� ��Ÿ����.
	float m_fMaxVelocityY;
	//�÷��̾ �ۿ��ϴ� �������� ��Ÿ����.
	float m_fFriction;

	//�÷��̾��� ��ġ�� �ٲ� ������ ȣ��Ǵ� OnPlayerUpdated() �Լ����� ����ϴ� �������̴�.
	LPVOID m_pPlayerUpdatedContext;
	//ī�޶��� ��ġ�� �ٲ� ������ ȣ��Ǵ� OnCameraUpdated() �Լ����� ����ϴ� �������̴�.
	LPVOID m_pCameraUpdatedContext;

	////�÷��̾ ���� ������ ī�޶��̴�.
	//CCamera *m_pCamera;

public:
	D3DXVECTOR3 GetPosition() { return(m_d3dxvPosition); }
	D3DXVECTOR3 GetLookVector() { return(m_d3dxvLook); }
	D3DXVECTOR3 GetUpVector() { return(m_d3dxvUp); }
	D3DXVECTOR3 GetRightVector() { return(m_d3dxvRight); }

	void SetFriction(float fFriction) { m_fFriction = fFriction; }
	void SetGravity(const D3DXVECTOR3& d3dxvGravity) { m_d3dxvGravity = d3dxvGravity; }
	void SetMaxVelocityXZ(float fMaxVelocity) { m_fMaxVelocityXZ = fMaxVelocity; }
	void SetMaxVelocityY(float fMaxVelocity) { m_fMaxVelocityY = fMaxVelocity; }
	void SetVelocity(const D3DXVECTOR3& d3dxvVelocity) { m_d3dxvVelocity = d3dxvVelocity; }

	

	const D3DXVECTOR3& GetVelocity() const { return(m_d3dxvVelocity); }
	float GetYaw() const { return(m_fYaw); }
	float GetPitch() const { return(m_fPitch); }
	float GetRoll() const { return(m_fRoll); }





	//ī�޶��� ��ġ�� �ٲ� ������ ȣ��Ǵ� �Լ��� �� �Լ����� ����ϴ� ������ �����ϴ� �Լ��̴�.
	virtual void OnCameraUpdated(float fTimeElapsed);
	void SetCameraUpdatedContext(LPVOID pContext) { m_pCameraUpdatedContext = pContext; }

	//ī�޶� ������ �� ȣ��Ǵ� �Լ��̴�.
	CCamera *OnChangeCamera(ID3D11Device *pd3dDevice, DWORD nNewCameraMode, DWORD nCurrentCameraMode);

	virtual void ChangeCamera(ID3D11Device *pd3dDevice, DWORD nNewCameraMode, float fTimeElapsed);
	//�÷��̾��� ��ġ�� ȸ�������κ��� ���� ��ȯ ����� �����ϴ� �Լ��̴�.
	virtual void OnPrepareRender();
	//�÷��̾��� ī�޶� 3��Ī ī�޶��� �� �÷��̾� �޽��� �������Ѵ�.
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);

	virtual  void Animate(float fTimeElapsed);

	virtual void CheckMove(float fTime)
	{
		m_d3dxmtxWorld._43 += fTime;
	}
protected:
	int m_nDirectionState;
	D3DXVECTOR3 m_d3dxvDirection;
	D3DXVECTOR3 m_d3dxvPreDir;
	float CalculateRotation(D3DXVECTOR3 d3dxvInputDir);
};


class CTerrainPlayer : public CPlayer
{
public:
	CTerrainPlayer(ID3D11Device *pd3dDevice, int nMeshes = 1);
	virtual ~CTerrainPlayer(){}

	virtual void ChangeCamera(ID3D11Device *pd3dDevice, DWORD nNewCameraMode, float fTimeElapsed);
	virtual void OnPlayerUpdated(float fTimeElapsed);
	virtual void OnCameraUpdated(float fTimeElapsed);

	virtual  void Animate(float fTimeElapsed);

	virtual void CheckMove(float fTime)
	{
		m_d3dxvPosition.z += fTime;
	}

};
