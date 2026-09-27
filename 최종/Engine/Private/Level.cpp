#include "Level.h"
#include "GameInstance.h"
CLevel::CLevel(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device), m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pGameInstance);
}

void CLevel::Free()
{


	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pGameInstance);
	
	__super::Free();
}
