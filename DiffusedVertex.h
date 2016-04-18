#pragma once
#include "stdafx.h"
class CDiffusedVertex
{
	D3DXVECTOR3 m_d3dxvPosition;
	//������ ������ ��Ÿ���� ��� ����(D3DXCOLOR ����ü)�� �����Ѵ�. 
	D3DXCOLOR m_d3dxcDiffuse;
public:
	//�����ڿ� �Ҹ��ڸ� �����Ѵ�.
	CDiffusedVertex(float x, float y, float z, D3DXCOLOR d3dxcDiffuse) 
	{
		m_d3dxvPosition = D3DXVECTOR3(x, y, z); 
		m_d3dxcDiffuse = d3dxcDiffuse; 
	}
	CDiffusedVertex(D3DXVECTOR3 d3dxvPosition, D3DXCOLOR d3dxcDiffuse) 
	{ 
		m_d3dxvPosition = d3dxvPosition; 
		m_d3dxcDiffuse = d3dxcDiffuse; 
	}
	CDiffusedVertex() 
	{ 
		m_d3dxvPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_d3dxcDiffuse = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f); 
	}
	~CDiffusedVertex() { }

};

