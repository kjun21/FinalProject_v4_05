#pragma once
//-----------------------------------------------------------------------------
// File: CGameTimer.h
//-----------------------------------------------------------------------------

const ULONG MAX_SAMPLE_COUNT = 50; // 50ȸ�� ������ ó���ð��� �����Ͽ� ����Ѵ�.

class CGameTimer
{
private:
	static CGameTimer* Timer;
public:
	static CGameTimer* GetCGameTimer()
	{
		if (!Timer)
			Timer = new CGameTimer();
		return Timer;
	}
public:
	CGameTimer();
	virtual ~CGameTimer();

	//Ÿ�̸� �ð��� �����Ѵ�.
	void Tick(float fLockFPS = 0.0f);
	//������ ����Ʈ�� ��ȯ�Ѵ�.
	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0) const;
	// �������� ��� ��� �ð��� ��ȯ�Ѵ�.
	float GetTimeElapsed() const;

private:
	bool							m_bHardwareHasPerformanceCounter;   // ��ǻ�Ͱ� Performance Counter�� ������ �ִ� ��
	float							m_fTimeScale;						// Scale Counter�� ��
	float							m_fTimeElapsed;						// ������ ������ ���� ������ �ð�
	__int64							m_nCurrentTime;						// ������ �ð�
	__int64							m_nLastTime;						// ������ �������� �ð�
	__int64							m_PerformanceFrequency;				// ��ǻ���� Performance Frequency

	float							m_fFrameTime[MAX_SAMPLE_COUNT]; // ������ �ð��� �����ϱ� ���� �迭
	ULONG							m_nSampleCount;  // ������ ������ Ƚ��

	unsigned long					m_nCurrentFrameRate;				// ������ ������ ����Ʈ
	unsigned long					m_FramePerSecond;					//�ʴ� ������ ��
	float							m_fFPSTimeElapsed;					// ������ ����Ʈ ��� �ҿ� �ð�
};