#include "pch.h"
#include "UI_Base.h"

#include"GameInstance.h"

CUI_Base::CUI_Base(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CUI_Base::CUI_Base(const CUI_Base& rhs)
	:CGameObject(rhs),
	m_ViewMatrix(rhs.m_ViewMatrix),
	m_ProjMatrix(rhs.m_ProjMatrix),
	m_fSizeX(rhs.m_fSizeX),
	m_fSizeY(rhs.m_fSizeY),
	m_fX(rhs.m_fX),
	m_fY(rhs.m_fY)
{
}

HRESULT CUI_Base::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CUI_Base"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Base::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Initialize : __super,CUI_Base"));
		return E_FAIL;
	}

	return S_OK;
}

_uint CUI_Base::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	return OBJECT_NOTHING;
}

void CUI_Base::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_Base::Render()
{

	return S_OK;
}


void CUI_Base::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	__super::Free();
}

