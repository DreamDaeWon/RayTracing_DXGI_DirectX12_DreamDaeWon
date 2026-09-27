#include "Timer_Manager.h"
#include "Timer.h"

CTimer_Manager::CTimer_Manager()
{
}

HRESULT CTimer_Manager::Add_Timer(const wstring& strTimerTag)
{
	if (nullptr != Find_Timer(strTimerTag))
	{
		MSG_BOX(TEXT("nullptr != Find_Timer(strTimerTag) : CTimer_Manager::Add_Timer"));
		return E_FAIL;
	}

	CTimer* pTimer = CTimer::Create();

	if (nullptr == pTimer)
	{
		MSG_BOX(TEXT("nullptr == pTimer : CTimer_Manager::Add_Timer"));
		return E_FAIL;
	}

	m_TimerMap.emplace(strTimerTag, pTimer);
	
	return S_OK;
}

_float CTimer_Manager::Compute_TimeDelta(const wstring& strTimerTag)
{
	CTimer* pTimer = Find_Timer(strTimerTag);
	if (nullptr == pTimer)
	{
		MSG_BOX(TEXT("nullptr == pTimer : CTimer_Manager::Compute_TimeDelta"));
		return 0.f;
	}

	return pTimer->Compute_TimeDelta();
}

CTimer* CTimer_Manager::Find_Timer(const wstring& strTimerTag)
{
	auto iter = m_TimerMap.find(strTimerTag);

	if (iter == m_TimerMap.end())
	{
		//MSG_BOX(TEXT("nullptr == pInstance : CTimer_Manager"));
		return nullptr;
	}

	return iter->second; //iter는 Pair. first에 태그 second에 CTimer*를 갖음.
}

CTimer_Manager* CTimer_Manager::Create()
{
	CTimer_Manager* pInstance = new CTimer_Manager;
	if (nullptr == pInstance)
	{
		MSG_BOX(TEXT("nullptr == pInstance : CTimer_Manager"));
		return nullptr;
	}

	return pInstance;
	//CTimer_Manager* pInstance = new CTimer_Manager;
	//return pInstance;
}

void CTimer_Manager::Free()
{
	for (auto& iter : m_TimerMap)
		Safe_Release(iter.second);
	m_TimerMap.clear();

	__super::Free();
}
