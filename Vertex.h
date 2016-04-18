#pragma once
#include "stdafx.h"
class CVertex
{
	// 정점의 위치 정보(3차원 벡터)를 저장하기 위한 멤버 변수를 선언한다.
private:
		D3DXVECTOR3 m_d3dxvPosition;
public:
	//CVertex 클래스의 생성자와 소멸자를 다음과 같이 선언한다. 
	CVertex();
	CVertex(D3DXVECTOR3 d3dxvPosition);
	~CVertex();
};

