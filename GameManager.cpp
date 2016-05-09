#include "GameManager.h"


CGameManager::CGameManager()
{
	
	m_uiMonstersNum = 10;
	m_ppMonster = new CGameObject*[m_uiMonstersNum];

	m_uiPlayersNum = 4;
	m_pPlayers = new CGameObject*[m_uiPlayersNum];

	m_uiStaticObjectNums = 50;
	m_ppStaticObject = new CGameObject*[m_uiStaticObjectNums];
}


CGameManager::~CGameManager()
{
	/*if (m_ppGameObject)
	{
		
		delete[] m_ppGameObject;
	}*/
}
