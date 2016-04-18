//-----------------------------------------------------------------------------
// File: CGameTimer.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Timer.h"
using namespace std;
CGameTimer::CGameTimer()
{
	//���� Ÿ�̸Ӱ� ������
	if (QueryPerformanceFrequency((LARGE_INTEGER *)&m_PerformanceFrequency)) 
    { 
		m_bHardwareHasPerformanceCounter = TRUE; // PerformanceCounter�� �ִ�.
		QueryPerformanceCounter((LARGE_INTEGER *)&m_nLastTime); 
		m_fTimeScale = 1.0f / m_PerformanceFrequency;
	} 
    else 
    { 
		m_bHardwareHasPerformanceCounter = FALSE;
		m_nLastTime	= ::timeGetTime(); 
		m_fTimeScale = 0.001f;	//�ð� ���� 0.001��
    }

    m_nSampleCount = 0;
	m_nCurrentFrameRate = 0;
	m_FramePerSecond = 0;
	m_fFPSTimeElapsed = 0.0f;
}

CGameTimer::~CGameTimer()
{
}

void CGameTimer::Tick(float fLockFPS)
{
    float fTimeElapsed; 

	//���� ī���Ϳ� ���� ���ļ��� ����Ͽ� ���� �ð��� �����Ѵ�.
	if (m_bHardwareHasPerformanceCounter) 
    {
		QueryPerformanceCounter((LARGE_INTEGER *)&m_nCurrentTime); //���� �ð�
	} 
    else 
    {
		m_nCurrentTime = ::timeGetTime();
	} 

	
	fTimeElapsed = (m_nCurrentTime - m_nLastTime) * m_fTimeScale; //������ Ÿ��
	//cout << "���� �ð� : " <<1/ fTimeElapsed << endl;
    if (fLockFPS > 0.0f)
    {
		//�� �Լ��� �Ķ����(fLockFPS)�� 0���� ũ�� �� �ð���ŭ ȣ���� �Լ��� ��ٸ��� �Ѵ�.
        while (fTimeElapsed < (1.0f / fLockFPS))
        {
	        if (m_bHardwareHasPerformanceCounter) 
            {
		        QueryPerformanceCounter((LARGE_INTEGER *)&m_nCurrentTime);
	        } 
            else 
            {
				m_nCurrentTime = ::timeGetTime();
			
	        } 
			//���������� �� �Լ��� ȣ���� ���� ����� �ð��� ����Ѵ�.
	        fTimeElapsed = (m_nCurrentTime - m_nLastTime) * m_fTimeScale;
        }
    } 
	//���� �ð��� m_nLastTime�� �����Ѵ�.
	m_nLastTime = m_nCurrentTime;
	//Sleep(2000);
	/* ������ ������ ó�� �ð��� ���� ������ ó�� �ð��� ���̰� 
	1�ʺ��� ������ ���� ������ ó�� �ð��� m_fFrameTime[0]�� �����Ѵ�. */
    if (fabsf(fTimeElapsed - m_fTimeElapsed) < 1.0f)
    {
		//cout << "��" << endl;
        // Wrap FIFO frame time buffer.
        memmove(&m_fFrameTime[1], m_fFrameTime, (MAX_SAMPLE_COUNT - 1) * sizeof(float));
		
		m_fFrameTime[0] = fTimeElapsed;
        if (m_nSampleCount < MAX_SAMPLE_COUNT)
			m_nSampleCount++;
		
    }

	//�ʴ� ������ ���� 1 ������Ű�� ���� ������ ó�� �ð��� �����Ͽ� �����Ѵ�.
	m_FramePerSecond++;
	//cout << "�ʴ� ������ ��"<<m_FramePerSecond << endl;

	m_fFPSTimeElapsed += fTimeElapsed;
	//cout << "������ ����Ʈ ��� �ҿ� �ð�" << m_fFPSTimeElapsed << endl;
	if (m_fFPSTimeElapsed > 1.0f) //�� 1�� ������
    { 
		m_nCurrentFrameRate	= m_FramePerSecond;
		//cout << "���� FPS :"<<m_nCurrentFrameRate << endl;
		m_FramePerSecond = 0;
		m_fFPSTimeElapsed = 0.0f;
	} 

    //������ ������ ó�� �ð��� ����� ���Ͽ� ������ ó�� �ð��� ���Ѵ�.
    m_fTimeElapsed = 0.0f;
    for (ULONG i = 0; i < m_nSampleCount; i++)
		m_fTimeElapsed += m_fFrameTime[i];
    if (m_nSampleCount > 0) 
		m_fTimeElapsed /= m_nSampleCount;

	/*cout << "fps" << m_nCurrentFrameRate << endl;
	cout << "elapsed" << 1/m_fTimeElapsed << endl;*/
}

unsigned long CGameTimer::GetFrameRate(LPTSTR lpszString, int nCharacters) const
{
	//���� ������ ����Ʈ�� ���ڿ��� ��ȯ�Ͽ� lpszString ���ۿ� ���� �� FPS���� �����Ѵ�.
    if (lpszString)
    {
        _itow_s(m_nCurrentFrameRate, lpszString, nCharacters, 10);
        wcscat_s(lpszString, nCharacters, _T(" FPS)"));
    } 

		//cout << "������ : " << m_nCurrentFrameRate << endl;
		return(m_nCurrentFrameRate);
}

float CGameTimer::GetTimeElapsed() const
{
    return(m_fTimeElapsed);
}
