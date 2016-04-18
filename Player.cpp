
#include "Player.h"
#include "GameFramework.h"
using namespace std;

CPlayer::CPlayer(int nMeshes) : CGameObject(nMeshes)
{
	m_pCamera = NULL;

	m_d3dxvPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_d3dxvRight = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_d3dxvUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_d3dxvLook = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	m_d3dxvVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_d3dxvGravity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fMaxVelocityXZ = 0.0f;
	m_fMaxVelocityY = 0.0f;
	m_fFriction = 0.0f;

	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;

	m_pPlayerUpdatedContext = NULL;
	m_pCameraUpdatedContext = NULL;
	m_d3dxvDirection = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	m_d3dxvPreDir = D3DXVECTOR3(0.0f, 0.0f, 0.99998f);
	m_nObjectType = TYPE_PLAYER;
}


CPlayer::~CPlayer()
{
	if (m_pCamera) delete m_pCamera;
}
void CPlayer::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	//���� ��ȯ ����� ���� ��� ���۸� �����Ѵ�.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_RESULT_MATRIX);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbAnimation) == S_OK)
		std::cout << "success" << std::endl;
	
}
void CPlayer::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext)
{
	//�÷��̾��� ���� ī�޶��� UpdateShaderVariables() ��� �Լ��� ȣ���Ѵ�.
	if (m_pCamera) m_pCamera->UpdateShaderVariables(pd3dDeviceContext);
}
/*�÷��̾��� ��ġ�� �����ϴ� �Լ��̴�.�÷��̾��� ��ġ�� �⺻������ ����ڰ�
�÷��̾ �̵��ϱ� ���� Ű���带 ���� �� ����ȴ�.�÷��̾��� �̵� ����(dwDirection)�� ���� �÷��̾ fDistance ��ŭ �̵��Ѵ�.
*/

void CPlayer::Rotate(DWORD dwDirection, DWORD dwAttack)
{
	D3DXVECTOR3 d3dxvDirection = D3DXVECTOR3(0, 0, 0);
	if (dwDirection)
	{
		if (dwDirection & DIR_FORWARD)
			d3dxvDirection += D3DXVECTOR3(0.0f, 0.0f, 0.99998f);
		if (dwDirection & DIR_BACKWARD)
			d3dxvDirection -= D3DXVECTOR3(0.0f, 0.0f, 0.99999f);
		if (dwDirection & DIR_RIGHT)
			d3dxvDirection += D3DXVECTOR3(0.99998f, 0.0f, 0.0f);
		if (dwDirection & DIR_LEFT)
			d3dxvDirection -= D3DXVECTOR3(0.99999f, 0.0f, 0.0f);

		//if (dwDirection & DIR_FORWARD)
		//	d3dxvDirection += D3DXVECTOR3(0.0f, 0.0f, 1.0f);
		//if (dwDirection & DIR_BACKWARD)
		//	d3dxvDirection -= D3DXVECTOR3(0.0f, 0.0f, 1.0f);
		//if (dwDirection & DIR_RIGHT)
		//	d3dxvDirection += D3DXVECTOR3(1.0f, 0.0f, 0.0f);
		//if (dwDirection & DIR_LEFT)
		//	d3dxvDirection -= D3DXVECTOR3(1.0f, 0.0f, 0.0f);


		
		// �밢������ �̵��� ���� �� �ڼ��� �״�� �����ϰ� �ʹ�.
		//cout << "���� ����  " << m_d3dxvPreDir.x << "  " << m_d3dxvPreDir.z << endl;
		//cout << "�Է� ���� " << d3dxvDirection.x << "  " << d3dxvDirection.z << endl;
	
		// ���� �Է� ����� ���� �Է� ������ �ٸ���
		// ������ �ٲ� ���� �ν��ϰ� ȸ���Ѵ�.
		// ��� �ϸ�ɱ��.
		if (m_d3dxvPreDir != d3dxvDirection)
		{

			float fAngle = CalculateRotation(d3dxvDirection);
		
		}
		// ���� ����� �Է��� ���� ��� �� �Է��� ��������Ѵ�.
		else if (m_d3dxvPreDir == d3dxvDirection)
		{
			
			D3DXVec3Normalize(&d3dxvDirection, &d3dxvDirection);
			m_d3dxvLook = d3dxvDirection;

		}
		m_d3dxvPreDir = d3dxvDirection;
		cout << m_d3dxvLook.x << "  " << m_d3dxvLook.y << "  " << m_d3dxvLook.z << endl;
		//���� Ű�Է� �޴� �κ�
		/*ClientServer *s = ClientServer::getInstangce();
		s->keyDown(m_d3dxvLook);*/
	
	}
}

float CPlayer::CalculateRotation(D3DXVECTOR3 d3dxvInputDir)
{
	D3DXVECTOR3 d3dxvAxis;
	// ���ο� ����
	D3DXVec3Normalize(&d3dxvInputDir, &d3dxvInputDir);

	// ���� �����̴� ����.
	m_d3dxvDirection = m_d3dxvLook;


	D3DXVec3Cross(&d3dxvAxis, &m_d3dxvDirection, &d3dxvInputDir);
	float fAngle = D3DXVec3Dot(&m_d3dxvDirection, &d3dxvInputDir);

	//m_d3dxvDirection = d3dxvInputDir;
	float a = (float)acos((double)fAngle);
	float b = D3DXToDegree(a);
	//cout << "ȸ���� ���� : " << fAngle << endl;

	D3DXMATRIX mtxRotate;
	D3DXMatrixRotationAxis(&mtxRotate, &d3dxvAxis, fAngle);

	//D3DXMatrixRotationAxis(&mtxRotate, &d3dxvAxis, a);
	D3DXVec3TransformNormal(&m_d3dxvLook, &m_d3dxvLook, &mtxRotate);
	D3DXVec3TransformNormal(&m_d3dxvRight, &m_d3dxvRight, &mtxRotate);

	D3DXVec3Normalize(&m_d3dxvLook, &m_d3dxvLook);
	D3DXVec3Cross(&m_d3dxvRight, &m_d3dxvUp, &m_d3dxvLook);
	D3DXVec3Normalize(&m_d3dxvRight, &m_d3dxvRight);
	D3DXVec3Cross(&m_d3dxvUp, &m_d3dxvLook, &m_d3dxvRight);
	D3DXVec3Normalize(&m_d3dxvUp, &m_d3dxvUp);

	return b;
}

void CPlayer::UpdateAnimation(DWORD dwDirection, DWORD dwAttack)
{
	if (((dwDirection & DIR_FORWARD) || (dwDirection & DIR_BACKWARD) ||
		(dwDirection & DIR_RIGHT) || (dwDirection & DIR_LEFT))
		&& m_nAnimationState != ANIMATAION_CLIP_ATTACK1
		&& m_nAnimationState != ANIMATAION_CLIP_ATTACK2)
		m_nAnimationState = ANIMATAION_CLIP_RUN;

	else if (dwDirection == 0 && dwAttack == 0
		&& m_nAnimationState != ANIMATAION_CLIP_ATTACK1 &&  m_nAnimationState != ANIMATAION_CLIP_ATTACK2)
	{
		m_nAnimationState = ANIMATAION_CLIP_IDLE;
		// cout << m_nAnimationState << endl;
	}
	else if (dwAttack == ATTACK01 || m_nAnimationState == ANIMATAION_CLIP_ATTACK1)
	{
		//m_nAnimationState = ANIMATAION_CLIP_IDLE;
		m_nAnimationState = ANIMATAION_CLIP_ATTACK1;
	}
	else if (dwAttack == ATTACK02 || m_nAnimationState == ANIMATAION_CLIP_ATTACK2)
	{
		//m_nAnimationState = ANIMATAION_CLIP_IDLE;
		m_nAnimationState = ANIMATAION_CLIP_ATTACK2;
	}
}

void CPlayer::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity)
{
	
	//m_ppMeshes[0];
	if (dwDirection)
	{
		D3DXVECTOR3 d3dxvShift = D3DXVECTOR3(0, 0, 0);
		D3DXVECTOR3 d3dxvDirection = D3DXVECTOR3(0, 0, 0);
		//ȭ��ǥ Ű ���衯�� ������ ���� z-�� �������� �̵�(����)�Ѵ�. ���顯�� ������ �ݴ� �������� �̵��Ѵ�.

		//cout << m_d3dxmtxWorld._31 << "  " << m_d3dxmtxWorld._33 << endl;
		
		d3dxvShift += m_d3dxvLook * fDistance;


		/*	 if (dwDirection & DIR_FORWARD)
		d3dxvShift += m_d3dxvLook * fDistance;
		if (dwDirection & DIR_BACKWARD)
		d3dxvShift += m_d3dxvLook * fDistance;
		if (dwDirection & DIR_RIGHT)
		d3dxvShift += m_d3dxvLook * fDistance;
		if (dwDirection & DIR_LEFT)
		d3dxvShift += m_d3dxvLook * fDistance;*/



		//if (dwDirection & DIR_FORWARD)
		//{
		//	d3dxvShift += m_d3dxvLook * fDistance;
		//	//d3dxvDirection += m_d3dxvLook;
		//}
		//	
		//if (dwDirection & DIR_BACKWARD)
		//{
		//	d3dxvShift -= m_d3dxvLook * fDistance;
		//	//d3dxvDirection -= m_d3dxvLook;
		//}
		//
		////ȭ��ǥ Ű ���桯�� ������ ���� x-�� �������� �̵��Ѵ�. ���硯�� ������ �ݴ� �������� �̵��Ѵ�.
		//if (dwDirection & DIR_RIGHT)
		//{
		//	d3dxvShift += m_d3dxvRight * fDistance;
		//	//d3dxvDirection += m_d3dxvRight;
		//	//d3dxvShift += m_d3dxvDirection * fDistance;


		///*	if (m_nDirectionState == DIR_RIGHT)
		//	{
		//		d3dxvShift += m_d3dxvDirection * fDistance;
		//	}

		//	else
		//	{
		//		m_nDirectionState = DIR_RIGHT;
		//		CalculateRotation(D3DXVECTOR3(1.0f, 0.0f, 0.0f));
		//	}*/
		//
		//}
		//
		//if (dwDirection & DIR_LEFT)
		//{
		//	d3dxvShift -= m_d3dxvRight * fDistance;
		//	//d3dxvDirection -= m_d3dxvRight;

		//}





		////��Page Up���� ������ ���� y-�� �������� �̵��Ѵ�. ��Page Down���� ������ �ݴ� �������� �̵��Ѵ�.

		//if (dwDirection & DIR_UP) d3dxvShift += m_d3dxvUp * fDistance;
		//if (dwDirection & DIR_DOWN) d3dxvShift -= m_d3dxvUp * fDistance;



		//�÷��̾ ���� ��ġ ���Ϳ��� d3dxvShift ���� ��ŭ �̵��Ѵ�.
		Move(d3dxvShift, bUpdateVelocity);

	}
}



void CPlayer::Move(const D3DXVECTOR3& d3dxvShift, bool bUpdateVelocity)
{
	ClientServer *s = ClientServer::getInstangce();
	//bUpdateVelocity�� ���̸� �÷��̾ �̵����� �ʰ� �ӵ� ���͸� �����Ѵ�.
	// �̵��ӵ� ������ְ� �Ʒ����� ������ �����ش�.
	if (bUpdateVelocity)
	{
		m_d3dxvVelocity += d3dxvShift;

	}
	//*********************************
	// ���� �����ִ°�
	else
	{

		//�÷��̾ ���� ��ġ ���Ϳ��� d3dxvShift ���� ��ŭ �̵��Ѵ�.
		D3DXVECTOR3 d3dxvPosition = m_d3dxvPosition + d3dxvShift;
		m_d3dxvPosition = d3dxvPosition;

		////���� �÷��̾� ��ġ ���濡 ���� ��ǥ �ٲ��ִ� �κ�
		//m_d3dxvPosition = s->Player[0].getPlayerPosition();
		//�÷��̾��� ��ġ�� ����Ǿ����Ƿ� ī�޶��� ��ġ�� d3dxvShift ���� ��ŭ �̵��Ѵ�.
		m_pCamera->Move(d3dxvShift);
	}
}

//�÷��̾ ���� x-��, y-��, z-���� �߽����� ȸ���Ѵ�.
void CPlayer::Rotate(float x, float y, float z)
{
	D3DXMATRIX mtxRotate;
	DWORD nCurrentCameraMode = m_pCamera->GetMode();

	//1��Ī ī�޶� �Ǵ� 3��Ī ī�޶��� ��� �÷��̾��� ȸ���� �ణ�� ������ ������.
	if ((nCurrentCameraMode == FIRST_PERSON_CAMERA) || (nCurrentCameraMode == THIRD_PERSON_CAMERA))
	{
		/*���� x-���� �߽����� ȸ���ϴ� ���� ���� �յڷ� ���̴� ���ۿ� �ش��Ѵ�. �׷��Ƿ� x-���� �߽����� ȸ���ϴ� ������ -89.0~+89.0�� ���̷� �����Ѵ�. x�� ������ m_fPitch���� ���� ȸ���ϴ� �����̹Ƿ� x��ŭ ȸ���� ���� Pitch�� +89�� ���� ũ�ų� -89�� ���� ������ m_fPitch�� +89�� �Ǵ� -89���� �ǵ��� ȸ������(x)�� �����Ѵ�.*/
		if (x != 0.0f)
		{
			m_fPitch += x;
			if (m_fPitch > +89.0f) { x -= (m_fPitch - 89.0f); m_fPitch = +89.0f; }
			if (m_fPitch < -89.0f) { x -= (m_fPitch + 89.0f); m_fPitch = -89.0f; }
		}
		//���� y-���� �߽����� ȸ���ϴ� ���� ������ ������ ���̹Ƿ� ȸ�� ������ ������ ����.
		if (y != 0.0f)
		{
			m_fYaw += y;
			if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
			if (m_fYaw < 0.0f) m_fYaw += 360.0f;
		}
		/*���� z-���� �߽����� ȸ���ϴ� ���� ������ �¿�� ����̴� ���̹Ƿ� ȸ�� ������ -20.0~+20.0�� ���̷� ���ѵȴ�. z�� ������ m_fRoll���� ���� ȸ���ϴ� �����̹Ƿ� z��ŭ ȸ���� ���� m_fRoll�� +20�� ���� ũ�ų� -20������ ������ m_fRoll�� +20�� �Ǵ� -20���� �ǵ��� ȸ������(z)�� �����Ѵ�.*/
		if (z != 0.0f)
		{
			m_fRoll += z;
			if (m_fRoll > +20.0f) { z -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
			if (m_fRoll < -20.0f) { z -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }
		}
		//ī�޶� x, y, z ��ŭ ȸ���Ѵ�. �÷��̾ ȸ���ϸ� ī�޶� ȸ���ϰ� �ȴ�.


		//**********************************ī�޶� ȸ���ϴ°��� ���� ���� �ּ�ó����.
		m_pCamera->Rotate(x, y, z);

		/*�÷��̾ ȸ���Ѵ�. 1��Ī ī�޶� �Ǵ� 3��Ī ī�޶󿡼� �÷��̾��� ȸ���� ���� y-�࿡���� �Ͼ��. �÷��̾��� ���� y-��(Up ����)�� �������� ���� z-��(Look ����)�� ���� x-��(Right ����)�� ȸ����Ų��. �⺻������ Up ���͸� �������� ȸ���ϴ� ���� �÷��̾ �ȹٷ� ���ִ� ���� �����Ѵٴ� �ǹ��̴�.*/
		if (y != 0.0f)
		{
			D3DXMatrixRotationAxis(&mtxRotate, &m_d3dxvUp, (float)D3DXToRadian(y));
			D3DXVec3TransformNormal(&m_d3dxvLook, &m_d3dxvLook, &mtxRotate);
			D3DXVec3TransformNormal(&m_d3dxvRight, &m_d3dxvRight, &mtxRotate);
		}
	}
	else if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
		/*�����̽�-�� ī�޶󿡼� �÷��̾��� ȸ���� ȸ�� ������ ������ ����. �׸��� ��� ���� �߽����� ȸ���� �� �� �ִ�.*/
		//**********************************ī�޶� ȸ���ϴ°��� ���� ���� �ּ�ó����.
		m_pCamera->Rotate(x, y, z);
		if (x != 0.0f)
		{
			D3DXMatrixRotationAxis(&mtxRotate, &m_d3dxvRight, (float)D3DXToRadian(x));
			D3DXVec3TransformNormal(&m_d3dxvLook, &m_d3dxvLook, &mtxRotate);
			D3DXVec3TransformNormal(&m_d3dxvUp, &m_d3dxvUp, &mtxRotate);
		}
		if (y != 0.0f)
		{
			D3DXMatrixRotationAxis(&mtxRotate, &m_d3dxvUp, (float)D3DXToRadian(y));
			D3DXVec3TransformNormal(&m_d3dxvLook, &m_d3dxvLook, &mtxRotate);
			D3DXVec3TransformNormal(&m_d3dxvRight, &m_d3dxvRight, &mtxRotate);
		}
		if (z != 0.0f)
		{
			D3DXMatrixRotationAxis(&mtxRotate, &m_d3dxvLook, (float)D3DXToRadian(z));
			D3DXVec3TransformNormal(&m_d3dxvUp, &m_d3dxvUp, &mtxRotate);
			D3DXVec3TransformNormal(&m_d3dxvRight, &m_d3dxvRight, &mtxRotate);
		}
	}

	/*ȸ������ ���� �÷��̾��� ���� x-��, y-��, z-���� ���� �������� ���� �� �����Ƿ� z-��(LookAt ����)�� �������� �Ͽ� ���� �����ϰ� �������Ͱ� �ǵ��� �Ѵ�.*/
	D3DXVec3Normalize(&m_d3dxvLook, &m_d3dxvLook);
	D3DXVec3Cross(&m_d3dxvRight, &m_d3dxvUp, &m_d3dxvLook);
	D3DXVec3Normalize(&m_d3dxvRight, &m_d3dxvRight);
	D3DXVec3Cross(&m_d3dxvUp, &m_d3dxvLook, &m_d3dxvRight);
	D3DXVec3Normalize(&m_d3dxvUp, &m_d3dxvUp);
}

void CPlayer::Update(float fTimeElapsed)
{
	/*�÷��̾��� �ӵ� ���͸� �߷� ���Ϳ� ���Ѵ�. �߷� ���Ϳ� fTimeElapsed�� ���ϴ� ���� �߷��� �ð��� ����ϵ��� �����Ѵٴ� �ǹ��̴�.*/
	m_d3dxvVelocity += m_d3dxvGravity * fTimeElapsed;
	/*�÷��̾��� �ӵ� ������ XZ-������ ũ�⸦ ���Ѵ�. �̰��� XZ-����� �ִ� �ӷº��� ũ�� �ӵ� ������ x�� z-���� ������ �����Ѵ�.*/
	float fLength = sqrtf(m_d3dxvVelocity.x * m_d3dxvVelocity.x + m_d3dxvVelocity.z * m_d3dxvVelocity.z);
	float fMaxVelocityXZ = m_fMaxVelocityXZ * fTimeElapsed;
	if (fLength > fMaxVelocityXZ)
	{
		m_d3dxvVelocity.x *= (fMaxVelocityXZ / fLength);
		m_d3dxvVelocity.z *= (fMaxVelocityXZ / fLength);
	}
	/*�÷��̾��� �ӵ� ������ Y-������ ũ�⸦ ���Ѵ�. �̰��� Y �� ������ �ִ� �ӷº��� ũ�� �ӵ� ������ y-���� ������ �����Ѵ�.*/
	fLength = sqrtf(m_d3dxvVelocity.y * m_d3dxvVelocity.y);
	float fMaxVelocityY = m_fMaxVelocityY * fTimeElapsed;
	if (fLength > fMaxVelocityY)
		m_d3dxvVelocity.y *= (fMaxVelocityY / fLength);

	//****************************************************
	//****************************************************
	//****************************************************
	//�÷��̾ �ӵ� ���� ��ŭ ������ �̵��Ѵ�(ī�޶� �̵��� ���̴�). 
	// ���� ������Ʈ�� �Ǵ� ���̴�...
	Move(m_d3dxvVelocity, false);

	/*�÷��̾��� ��ġ�� ����� �� �߰��� ������ �۾��� �����Ѵ�. ���� ���, 
	�÷��̾��� ��ġ�� ����Ǿ����� �÷��̾� ��ü���� ����(Terrain)�� ������ ����. 
	�÷��̾��� ���ο� ��ġ�� ��ȿ�� ��ġ�� �ƴ� ���� �ְ� �Ǵ� �÷��̾��� �浹 �˻� ���� ������ �ʿ䰡 �ִ�. 
	�̷��� ��Ȳ���� �÷��̾��� ��ġ�� ��ȿ�� ��ġ�� �ٽ� ������ �� �ִ�.*/
	if (m_pPlayerUpdatedContext) OnPlayerUpdated(fTimeElapsed);

	DWORD nCurrentCameraMode = m_pCamera->GetMode();
	//�÷��̾��� ��ġ�� ����Ǿ����Ƿ� ī�޶��� ���¸� �����Ѵ�.



	// ī�޶� ������Ʈ
	if (nCurrentCameraMode == THIRD_PERSON_CAMERA) 
		m_pCamera->Update(m_d3dxvPosition, fTimeElapsed);


	//ī�޶��� ��ġ�� ����� �� �߰��� ������ �۾��� �����Ѵ�. 
	if (m_pCameraUpdatedContext) OnCameraUpdated(fTimeElapsed); 



	//ī�޶� 3��Ī ī�޶��̸� ī�޶� ����� �÷��̾� ��ġ�� �ٶ󺸵��� �Ѵ�.
	if (nCurrentCameraMode == THIRD_PERSON_CAMERA) 
		m_pCamera->SetLookAt(m_d3dxvPosition);


	//ī�޶��� ī�޶� ��ȯ ����� �ٽ� �����Ѵ�.
	m_pCamera->RegenerateViewMatrix();

	/*�÷��̾��� �ӵ� ���Ͱ� ������ ������ ������ �Ǿ�� �Ѵٸ� ���� ���͸� �����Ѵ�. �ӵ� ������ �ݴ� ���� ���͸� ���ϰ� ���� ���ͷ� �����. ���� ����� �ð��� ����ϵ��� �Ͽ� �������� ���Ѵ�. ���� ���Ϳ� �������� ���Ͽ� ���� ���͸� ���Ѵ�. �ӵ� ���Ϳ� ���� ���͸� ���Ͽ� �ӵ� ���͸� ���δ�. �������� �ӷº��� ũ�� �ӷ��� 0�� �� ���̴�.*/
	D3DXVECTOR3 d3dxvDeceleration = -m_d3dxvVelocity;
	D3DXVec3Normalize(&d3dxvDeceleration, &d3dxvDeceleration);
	fLength = D3DXVec3Length(&m_d3dxvVelocity);
	float fDeceleration = (m_fFriction * fTimeElapsed);
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_d3dxvVelocity += d3dxvDeceleration * fDeceleration;
}

CCamera*  CPlayer::OnChangeCamera(ID3D11Device *pd3dDevice, DWORD nNewCameraMode, DWORD nCurrentCameraMode)
{
	CCamera *pNewCamera = NULL;
	//���ο� ī�޶��� ��忡 ���� ī�޶� ���� �����Ѵ�.
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		pNewCamera = new CFirstPersonCamera(m_pCamera);
		break;
	case THIRD_PERSON_CAMERA:
		pNewCamera = new CThirdPersonCamera(m_pCamera);
		break;
	case SPACESHIP_CAMERA:
		pNewCamera = new CSpaceShipCamera(m_pCamera);
		break;
	}
	/*���� ī�޶��� ��尡 �����̽�-�� ����� ī�޶��̰� ���ο� ī�޶� 1��Ī �Ǵ� 3��Ī ī�޶��̸� �÷��̾��� Up ���͸� ������ǥ���� y-�� ���� ����(0, 1, 0)�� �ǵ��� �Ѵ�. ��, �ȹٷ� ������ �Ѵ�. �׸��� �����̽�-�� ī�޶��� ��� �÷��̾��� �̵����� ������ ����. Ư��, y-�� ������ �������� �����Ӵ�. �׷��Ƿ� �÷��̾��� ��ġ�� ����(��ġ ������ y-��ǥ�� 0���� ũ��)�� �� �� �ִ�. �̶� ���ο� ī�޶� 1��Ī �Ǵ� 3��Ī ī�޶��̸� �÷��̾��� ��ġ�� ������ �Ǿ�� �Ѵ�. �׷��Ƿ� �÷��̾��� Right ���Ϳ� Look ������ y ���� 0���� �����. ���� �÷��̾��� Right ���Ϳ� Look ���ʹ� �������Ͱ� �ƴϹǷ� ����ȭ�Ѵ�.*/
	if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
		m_d3dxvUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		m_d3dxvRight.y = 0.0f;
		m_d3dxvLook.y = 0.0f;
		D3DXVec3Normalize(&m_d3dxvRight, &m_d3dxvRight);
		D3DXVec3Normalize(&m_d3dxvLook, &m_d3dxvLook);
		m_fPitch = 0.0f;
		m_fRoll = 0.0f;
		/*Look ���Ϳ� ������ǥ���� z-��(0, 0, 1)�� �̷�� ����(����=cos)�� ����Ͽ� �÷��̾��� y-���� ȸ�� ���� m_fYaw�� �����Ѵ�.*/
		m_fYaw = (float)D3DXToDegree(acosf(D3DXVec3Dot(&D3DXVECTOR3(0.0f, 0.0f, 1.0f), &m_d3dxvLook)));
		if (m_d3dxvLook.x < 0.0f) m_fYaw = -m_fYaw;
	}
	else if ((nNewCameraMode == SPACESHIP_CAMERA) && m_pCamera)
	{
		/*���ο� ī�޶��� ��尡 �����̽�-�� ����� ī�޶��̰� ���� ī�޶� ��尡 1��Ī �Ǵ� 3��Ī ī�޶��̸� �÷��̾��� ���� ���� ���� ī�޶��� ���� ��� ���� �����.*/
		m_d3dxvRight = m_pCamera->GetRightVector();
		m_d3dxvUp = m_pCamera->GetUpVector();
		m_d3dxvLook = m_pCamera->GetLookVector();
	}

	if (pNewCamera)
	{
		//���� ī�޶� ������ ���ο� ī�޶� ���� ���̴� ������ �����Ѵ�.
		if (!m_pCamera)
		{
			pNewCamera->CreateShaderVariables(pd3dDevice);
			pNewCamera->DSCreateShaderVariables(pd3dDevice);
		}
		pNewCamera->SetMode(nNewCameraMode);
		//���� ī�޶� ����ϴ� �÷��̾� ��ü�� �����Ѵ�.
		pNewCamera->SetPlayer(this);
	}

	if (m_pCamera) delete m_pCamera;

	return(pNewCamera);
}
void CPlayer::OnPlayerUpdated(float fTimeElapsed)
{
}

void CPlayer::OnCameraUpdated(float fTimeElapsed)
{
}
void CPlayer::ChangeCamera(ID3D11Device *pd3dDevice, DWORD nNewCameraMode, float fTimeElapsed)
{
}

/*�÷��̾��� ��ġ�� ȸ�������κ��� ���� ��ȯ ����� �����ϴ� �Լ��̴�.
�÷��̾��� Right ���Ͱ� ���� ��ȯ ����� ù ��° �� ����,
Up ���Ͱ� �� ��° �� ����,
Look ���Ͱ� �� ��° �� ����, �÷��̾��� ��ġ ���Ͱ� �� ��° �� ���Ͱ� �ȴ�.*/
void CPlayer::OnPrepareRender()
{
	m_d3dxmtxWorld._11 = m_d3dxvRight.x;
	m_d3dxmtxWorld._12 = m_d3dxvRight.y;
	m_d3dxmtxWorld._13 = m_d3dxvRight.z;
	m_d3dxmtxWorld._21 = m_d3dxvUp.x;
	m_d3dxmtxWorld._22 = m_d3dxvUp.y;
	m_d3dxmtxWorld._23 = m_d3dxvUp.z;
	m_d3dxmtxWorld._31 = m_d3dxvLook.x;
	m_d3dxmtxWorld._32 = m_d3dxvLook.y;
	m_d3dxmtxWorld._33 = m_d3dxvLook.z;

	m_d3dxmtxWorld._41 = m_d3dxvPosition.x;
	m_d3dxmtxWorld._42 = m_d3dxvPosition.y;
	m_d3dxmtxWorld._43 = m_d3dxvPosition.z;


	

	//m_d3dxmtxWorld = m_d3dmtxScale * m_d3dxmtxWorld;

}

void CPlayer::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	for (int i = 0; i < 5; i++)
	{
		if (m_AnimationClip[i].m_nAnimationState == m_nAnimationState)
		{
			CGameTimer* GameTimer = CGameTimer::GetCGameTimer();
			m_AnimationClip[i].m_fTimePos += GameTimer->GetTimeElapsed();


			m_AnimationClip[i].llNowTime = m_AnimationClip[i].m_fTimePos * 1000;
			if (m_AnimationClip[i].llNowTime >= m_AnimationClip[i].m_llAniTime)
			{
				m_AnimationClip[i].llNowTime -= m_AnimationClip[i].m_llAniTime;
				m_AnimationClip[i].m_fTimePos = 0;

				// 
				if (m_nAnimationState == ANIMATAION_CLIP_ATTACK1
					|| m_nAnimationState == ANIMATAION_CLIP_ATTACK2
					|| m_nAnimationState == ANIMATAION_CLIP_DEATH)
					m_nAnimationState = ANIMATAION_CLIP_IDLE;
			}

			D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
			pd3dDeviceContext->Map(m_pd3dcbAnimation, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
			m_cbMapData = (VS_CB_RESULT_MATRIX2 *)d3dMappedResource.pData;
			for (int j = 0; j < MaxBone; j++) //����
				m_cbMapData->m_d3dxmtxResult[j] = m_AnimationClip[i].m_ppResultMatrix[m_AnimationClip[i].llNowTime / 10][j];
			pd3dDeviceContext->Unmap(m_pd3dcbAnimation, 0);


			if (m_pd3dcbAnimation != NULL)
				pd3dDeviceContext->VSSetConstantBuffers(VS_SLOT_RESULT_MATRIX, 1, &m_pd3dcbAnimation);
		}
	}
	CGameObject::Render(pd3dDeviceContext, pCamera);
}

CTerrainPlayer::CTerrainPlayer(ID3D11Device *pd3dDevice, int nMeshes) : CPlayer(nMeshes)
{
	//CGameObject::Scale(D3DXVECTOR3(0.3, 0.3, 0.3));

	/*CCharacterMesh *pHumanMesh = new CCharacterMesh(pd3dDevice, strFileName);
	pHumanMesh->CreateChileMesh(pd3dDevice);
	SetMesh(pHumanMesh, 0);
	Scale(&D3DXVECTOR3(0.1, 0.1, 0.1));
	CGameObject::Rotate(-90.0, 0.0, 0.0);
	CGameObject::SetPosition(300.0, 0.0, 800.0);*/
	//Rotate(-90.0, 0.0, 0.0);
	//Scale(&D3DXVECTOR3(0.3, 0.3, 0.3));


	//string strFileName = "Data/walk02Box001";
	//CCharacterMesh *pHumanMesh = new CCharacterMesh(pd3dDevice, strFileName);
	//SetMesh(pHumanMesh, 0);
	//CGameObject::Scale(D3DXVECTOR3(0.3, 0.3, 0.3));


}

void CTerrainPlayer::ChangeCamera(ID3D11Device *pd3dDevice, DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return;
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		SetFriction(250.0f);
		//1��Ī ī�޶��� �� �÷��̾ y-�� �������� �߷��� �ۿ��Ѵ�.
		SetGravity(D3DXVECTOR3(0.0f, -300.0f, 0.0f));
		SetMaxVelocityXZ(300.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(pd3dDevice, FIRST_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(D3DXVECTOR3(0.0f, 20.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 50000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->DSCreateShaderVariables(pd3dDevice);
		break;
	case SPACESHIP_CAMERA:
		SetFriction(125.0f);
		//�����̽� �� ī�޶��� �� �÷��̾ �߷��� �ۿ����� �ʴ´�.
		SetGravity(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(300.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(pd3dDevice, SPACESHIP_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 50000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->DSCreateShaderVariables(pd3dDevice);
		break;
	case THIRD_PERSON_CAMERA:
		SetFriction(250.0f);
		//3��Ī ī�޶��� �� �÷��̾ y-�� �������� �߷��� �ۿ��Ѵ�.
		SetGravity(D3DXVECTOR3(0.0f, -300.0f, 0.0f));
		SetMaxVelocityXZ(300.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(pd3dDevice, THIRD_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.25f);
		m_pCamera->SetOffset(D3DXVECTOR3(0.0f, 400.0f, -150.0f)); // ���� ������  20 -50   // 250.0f, -150.0f
		m_pCamera->GenerateProjectionMatrix(1.01f, 50000.0f, ASPECT_RATIO, 60.0f);

		m_pCamera->DSCreateShaderVariables(pd3dDevice);
		break;
	default:
		break;
	}
	Update(fTimeElapsed);
}

void CTerrainPlayer::OnPlayerUpdated(float fTimeElapsed)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)m_pPlayerUpdatedContext;
	D3DXVECTOR3 d3dxvScale = pTerrain->GetScale();
	D3DXVECTOR3 d3dxvPlayerPosition = GetPosition();
	int z = (int)(d3dxvPlayerPosition.z / d3dxvScale.z);
	bool bReverseQuad = ((z % 2) != 0);
	/*���� �ʿ��� �÷��̾��� ���� ��ġ (x, z)�� y ���� ���Ѵ�.
	�׸��� �÷��̾� �޽��� ���̰� 12�̰� �÷��̾���
	�߽��� ������ü�� ����̹Ƿ� y ���� �޽��� ������ ������ ���ϸ� �÷��̾��� ��ġ�� �ȴ�.*/
	float fHeight = pTerrain->GetHeight(d3dxvPlayerPosition.x, d3dxvPlayerPosition.z, bReverseQuad) + 6.0f;
	/*�÷��̾��� �ӵ� ������ y-���� �����̸�(���� ���, �߷��� ����Ǵ� ���) �÷��̾��� ��ġ ������ y-���� ���� �۾����� �ȴ�. �̶� �÷��̾��� ���� ��ġ�� y ���� ������ ����(������ ������ ���� + 6)���� ������ �÷��̾ ���ӿ� �ְ� �ǹǷ� �÷��̾��� �ӵ� ������ y ���� 0���� ����� �÷��̾��� ��ġ ������ y-���� ������ ���̷� �����Ѵ�. �׷��� �÷��̾�� ���� ���� �ְ� �ȴ�.*/
	//	cout << m_d3dxvPosition.x << "  " << m_d3dxvPosition.z << endl;



	if (d3dxvPlayerPosition.y < fHeight)
	{
		D3DXVECTOR3 d3dxvPlayerVelocity = GetVelocity();
		d3dxvPlayerVelocity.y = 0.0f;
		SetVelocity(d3dxvPlayerVelocity);
		d3dxvPlayerPosition.y = fHeight;
		SetPosition(d3dxvPlayerPosition);
	}
}

void CTerrainPlayer::OnCameraUpdated(float fTimeElapsed)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)m_pCameraUpdatedContext;
	D3DXVECTOR3 d3dxvScale = pTerrain->GetScale();
	CCamera *pCamera = GetCamera();
	D3DXVECTOR3 d3dxvCameraPosition = pCamera->GetPosition();
	int z = (int)(d3dxvCameraPosition.z / d3dxvScale.z);
	bool bReverseQuad = ((z % 2) != 0);
	/*���� �ʿ��� ī�޶��� ���� ��ġ (x, z)�� ����(y ��)�� ���Ѵ�. �� ���� ī�޶��� ��ġ�� �ش��ϴ� ������ ���� ���� ������ ī�޶� ���ӿ� �ְ� �ȴ�. �̷��� �Ǹ� <�׸� 4>�� ���ʰ� ���� ������ �׷����� �ʴ� ��찡 �߻��Ѵ�(ī�޶� ���� �ȿ� �����Ƿ� ���ε� ������ �ٲ��). �̷��� ��찡 �߻����� �ʵ��� ī�޶��� ��ġ�� �ּҰ��� (������ ���� + 5)�� �����Ѵ�. ī�޶��� ��ġ�� �ּҰ��� ������ ��� ��ġ���� ī�޶� ���� �Ʒ��� ��ġ���� �ʵ��� �����Ѵ�.*/
	float fHeight = pTerrain->GetHeight(d3dxvCameraPosition.x, d3dxvCameraPosition.z, bReverseQuad) + 5.0f;
	if (d3dxvCameraPosition.y < fHeight)
	{
		d3dxvCameraPosition.y = fHeight;
		pCamera->SetPosition(d3dxvCameraPosition);
		//3��Ī ī�޶��� ��� ī�޶��� y-��ġ�� ����Ǿ����Ƿ� ī�޶� �÷��̾ �ٶ󺸵��� �Ѵ�.
		if (pCamera->GetMode() == THIRD_PERSON_CAMERA)
		{
			CThirdPersonCamera *p3rdPersonCamera = (CThirdPersonCamera *)pCamera;
			p3rdPersonCamera->SetLookAt(GetPosition());
		}
	}
}

void CPlayer::Animate(float fTimeElapsed)
{

}
void CTerrainPlayer::Animate(float fTimeElapsed)
{
	//Scale(D3DXVECTOR3(0.3, 0.3, 0.3));
	//���⼭�� �ȵȴ� �ϴ���.

	//cout << "�÷��̾� ȣǮ" << endl;
	//cout << "������ ��ǥ??" << m_d3dxvPosition.x << "   " << m_d3dxvPosition.y<< "  " << m_d3dxvPosition.z << endl;



	// �̵��ȴ�.
	//m_d3dxvPosition += D3DXVECTOR3(0.0f, 0.0f, fTimeElapsed * 100);

	// �ȵȴ�.
	//m_d3dxmtxWorld._43 += fTimeElapsed * 10;
}