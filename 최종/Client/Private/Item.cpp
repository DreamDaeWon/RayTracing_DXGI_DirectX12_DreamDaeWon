#include "pch.h"
#include "GameInstance.h"
#include "Item.h"

CItem::CItem(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLandObject(pGraphic_Device)
{
}

CItem::CItem(const CItem& rhs)
	:CLandObject(rhs)
{
}

HRESULT CItem::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CItem"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CItem::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		ITEM_DESC* ItemDesc = (ITEM_DESC*)pArg;
		m_vPosition = ItemDesc->vPos;
		m_vLook = ItemDesc->vLook;
		ItemDesc->fSpeedPerSec = 2.f;
	}

	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CLandObject"));
		return E_FAIL;

	}
	return S_OK;
}

_uint CItem::Tick(_float fTimeDelta)
{
	return 0;
}

void CItem::Late_Tick(_float fTimeDelta)
{
}

HRESULT CItem::Render()
{
	return S_OK;
}

void CItem::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBuffer_Com);
	Safe_Release(m_pCollider_Com[COLLIDER_CUBE_AABB]);

	__super::Free();
}
