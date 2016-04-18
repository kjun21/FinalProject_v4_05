#pragma once
#include "stdafx.h"
#include "Object.h"
class CRotatingObject : public CGameObject
{
public:
	CRotatingObject(int nMeshes = 1);
	~CRotatingObject();
	void Animate(float fTimeElapsed);

protected:
	//자전 속도와 회전축 벡터를 나타내는 멤버 변수를 선언한다.
	float m_fRotationSpeed;
	D3DXVECTOR3 m_d3dxvRotationAxis;

public:
	//자전 속도와 회전축 벡터를 설정하는 함수이다.
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(D3DXVECTOR3 d3dxvRotationAxis) { m_d3dxvRotationAxis = d3dxvRotationAxis; }
};

