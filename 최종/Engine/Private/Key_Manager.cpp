#include "Key_Manager.h"

CKey_Manager::CKey_Manager()
{
}

HRESULT CKey_Manager::Initialize()
{
	ZeroMemory(m_bKeyState, sizeof(m_bKeyState));
	return S_OK;
}

_bool CKey_Manager::Key_Pressing(_uint _iKey)
{
	if (GetAsyncKeyState(_iKey) & 0x8000)
		return true;

	return false;
}

_bool CKey_Manager::Key_Down(_uint _iKey)
{

	// 이전에 눌림이 없음		&&	현재 눌렀음
	if (!m_bKeyState[_iKey] && (GetAsyncKeyState(_iKey) & 0x8000))
	{
		m_bKeyState[_iKey] = !m_bKeyState[_iKey];
		return true;
	}

	if (m_bKeyState[_iKey] && !(GetAsyncKeyState(_iKey) & 0x8000))
	{
		m_bKeyState[_iKey] = !m_bKeyState[_iKey];
	}

	return false;
}

_bool CKey_Manager::Key_Up(_uint _iKey)
{

	// 이전에 눌렀음		&&	현재 누르지 않음
	if (m_bKeyState[_iKey] && !(GetAsyncKeyState(_iKey) & 0x8000))
	{
		m_bKeyState[_iKey] = !m_bKeyState[_iKey];
		return true;
	}

	if (!m_bKeyState[_iKey] && (GetAsyncKeyState(_iKey) & 0x8000))
	{
		m_bKeyState[_iKey] = !m_bKeyState[_iKey];
	}

	return false;
}

CKey_Manager* CKey_Manager::Create()
{
	CKey_Manager* pInstance = new CKey_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CKey_Manager"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKey_Manager::Free()
{
	__super::Free();
}
