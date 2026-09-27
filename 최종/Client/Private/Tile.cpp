#include "pch.h"
#include "Tile.h"
#include "GameInstance.h"

CTile::CTile(LPDIRECT3DDEVICE9 pGraphic_Device) :
    CGameObject(pGraphic_Device)
{
}

CTile::CTile(const CTile& rhs) :
    CGameObject(rhs)
{
}

HRESULT CTile::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTile::Initialize(void* pArg)
{
    return S_OK;
}

_uint CTile::Tick(_float fTimeDelta)
{
    return 0;
}

void CTile::Late_Tick(_float fTimeDelta)
{
}

HRESULT CTile::Render()
{
	if (FAILED(m_pTransform->Bind_WorldMatrix()))
	{
		MSG_BOX(TEXT("Failed to Bind_WorldMatrix : Render"));
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->Bind_Texture(0, (_uint)m_fFrame)))
	{
		MSG_BOX(TEXT("Failed to Bind_Texture : Render"));
		return E_FAIL;
	}
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	if (FAILED(Set_RenderState()))
	{
		MSG_BOX(TEXT("Failed to Set_RenderState : Render"));
		return E_FAIL;
	}
	if (FAILED(m_pVIBuffer_Com->Render()))
	{
		MSG_BOX(TEXT("Failed to Render : Render"));
		return E_FAIL;
	}
	if (FAILED(Reset_RenderState()))
	{
		MSG_BOX(TEXT("Failed to Reset_RenderState : Render"));
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CTile::Add_Components()
{
    return S_OK;
}

HRESULT CTile::Set_RenderState()
{
    return S_OK;
}

HRESULT CTile::Reset_RenderState()
{
    return S_OK;
}

CTile* CTile::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTile* pInstance = new CTile(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Faild to Created : CTile"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTile::Clone(void* pArg)
{
	CTile* pInstance = new CTile(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Faild to Cloned : CTile"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTile::Free()
{
    __super::Free();
}