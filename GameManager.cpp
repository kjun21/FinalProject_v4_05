#include "GameManager.h"


CGameManager::CGameManager()
{
	
	m_uiMonstersNum = 5;
	m_ppMonster = new CGameObject*[m_uiMonstersNum];

	m_uiPlayersNum = 4;
	m_pPlayers = new CGameObject*[m_uiPlayersNum];
}


CGameManager::~CGameManager()
{
	/*if (m_ppGameObject)
	{
		
		delete[] m_ppGameObject;
	}*/
}
