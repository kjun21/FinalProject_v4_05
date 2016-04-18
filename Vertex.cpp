#include "Vertex.h"

	//CVertex 클래스의 생성자와 소멸자를 다음과 같이 선언한다. 
CVertex::CVertex()
{
	m_d3dxvPosition = D3DXVECTOR3(0, 0, 0);
}
CVertex::CVertex(D3DXVECTOR3 d3dxvPosition) { m_d3dxvPosition = d3dxvPosition; }
CVertex::~CVertex() { }


