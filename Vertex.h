#pragma once
#include "stdafx.h"
class CVertex
{
	// ������ ��ġ ����(3���� ����)�� �����ϱ� ���� ��� ������ �����Ѵ�.
private:
		D3DXVECTOR3 m_d3dxvPosition;
public:
	//CVertex Ŭ������ �����ڿ� �Ҹ��ڸ� ������ ���� �����Ѵ�. 
	CVertex();
	CVertex(D3DXVECTOR3 d3dxvPosition);
	~CVertex();
};

