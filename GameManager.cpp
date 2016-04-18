#include "GameManager.h"


CGameManager::CGameManager()
{
	
	m_nMonsterNum = 0;
	m_ppMonster = new CGameObject*[2];
}


CGameManager::~CGameManager()
{
	/*if (m_ppGameObject)
	{
		
		delete[] m_ppGameObject;
	}*/
}
