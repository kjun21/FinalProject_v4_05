#pragma once
#include "stdafx.h"
class CDiffusedVertex
{
	D3DXVECTOR3 m_d3dxvPosition;
	//정점의 색상을 나타내는 멤버 변수(D3DXCOLOR 구조체)를 선언한다. 
	D3DXCOLOR m_d3dxcDiffuse;
public:
	//생성자와 소멸자를 선언한다.
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

