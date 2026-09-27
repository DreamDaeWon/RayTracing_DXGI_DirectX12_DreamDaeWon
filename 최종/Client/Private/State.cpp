#include"pch.h"
#include "State.h"

CState::CState(LPDIRECT3DDEVICE9 pGraphic_Device):
	CComponent(pGraphic_Device)
{

}

CState::CState(const CState& rhs):
	CComponent(rhs)
{
}

HRESULT CState::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CState::Initialize(void* pArg)
{
	if (pArg == nullptr)
	{
		m_vecState[STATE_CP] =1 ;
		m_vecState[STATE_MAXHP] = 1;
		m_vecState[STATE_HP] = m_vecState[STATE_MAXHP];
		m_vecState[STATE_AILMENT] = OBJECT_NOTHING;
		m_vecState[STATE_MONEY] = 0.f;
		m_vecState[STATE_DEAD] = OBJECT_NOTHING;
	}
	else
	{
		STATE_DESC* StateDesc = (STATE_DESC*)pArg;
		m_vecState[STATE_CP] = StateDesc->fCP;
		m_vecState[STATE_MAXHP] = StateDesc->fMaxHp;
		m_vecState[STATE_HP] = m_vecState[STATE_MAXHP];
		m_vecState[STATE_AILMENT] = OBJECT_NOTHING;
		m_vecState[STATE_MONEY] = 0.f;
		m_vecState[STATE_DEAD] = OBJECT_NOTHING;
	}



	return S_OK;

}

CState* CState::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CState* pInstance = new CState(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CState"));

		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent* CState::Clone(void* pArg)
{
	CState* pInstance = new CState(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CState"));

		Safe_Release(pInstance);
	}

	return pInstance;
}


void CState::Free()
{
	__super::Free();
}
