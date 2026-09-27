#include "pch.h"
#include "GameInstance.h"
#include "ClientCollision.h"

CClientCollision::CClientCollision(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CGameObject(pGraphic_Device)
{
}

CClientCollision::CClientCollision(const CClientCollision& rhs) :
	CGameObject(rhs)
{
}

HRESULT CClientCollision::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize : __super,CClientCollision"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CClientCollision::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Initialize : __super,CClientCollision"));
		return E_FAIL;
	}

	return S_OK;
}

_uint CClientCollision::Tick(_float fTimeDelta)
{
	return 0;
}

void CClientCollision::Late_Tick(_float fTimeDelta)
{
	// 대원 추가
	if (g_eLevel == LEVEL_1945)
		return;
	list<CGameObject*>* pPlayerList = m_pGameInstance->Get_List(LEVEL_STATIC, TEXT("Layer_Player"));
	list<CGameObject*>* pMonsterList = m_pGameInstance->Get_List(g_eLevel, TEXT("Layer_Monster"));
	list<CGameObject*>* pMonsterBulletList = m_pGameInstance->Get_List(g_eLevel, TEXT("Layer_Monster_Bullet"));
	list<CGameObject*>* pBoxList = m_pGameInstance->Get_List(g_eLevel, TEXT("Layer_Box"));
	if (nullptr != pPlayerList && nullptr != pMonsterList)
		m_pGameInstance->Collision_AABB_List(pPlayerList, pMonsterList);
	if (nullptr != pMonsterList && nullptr != pMonsterList)
		m_pGameInstance->Collision_AABB_List(pMonsterList, pMonsterList);
	if (nullptr != pBoxList && nullptr != pMonsterBulletList)
		m_pGameInstance->Collision_AABB_Dst_Dot_List(pBoxList, pMonsterBulletList);
	if (nullptr != pMonsterList && nullptr != pBoxList)
		m_pGameInstance->Collision_AABB_List_Fixed_Dst(pMonsterList, pBoxList, fTimeDelta);
	if (nullptr != pPlayerList && nullptr != pBoxList)
		m_pGameInstance->Collision_AABB_List_Fixed_Dst(pPlayerList, pBoxList, fTimeDelta);

}


CClientCollision* CClientCollision::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CClientCollision* pInstance = new CClientCollision(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Faild to Created : CClientCollision"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CClientCollision::Clone(void* pArg)
{
	CClientCollision* pInstance = new CClientCollision(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Faild to Cloned : CClientCollision"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CClientCollision::Free()
{
	__super::Free();
}



