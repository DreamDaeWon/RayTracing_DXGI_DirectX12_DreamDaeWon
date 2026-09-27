#include "pch.h"
#include "Effect_Base.h"
#include "GameInstance.h"

CEffect_Base::CEffect_Base(LPDIRECT3DDEVICE9 pGraphic_Device) :
    CBlendObject(pGraphic_Device)
{
}

CEffect_Base::CEffect_Base(const CEffect_Base& rhs) :
    CBlendObject(rhs) 
{
}

HRESULT CEffect_Base::Initialize_Prototype()
{
    /*if (FAILED(__super::Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Initialize_Prototype : CEffect_Base"));
        return E_FAIL;
    }*/

    return S_OK;
}

HRESULT CEffect_Base::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Initialize : CEffect_Base"));
        return E_FAIL;
    }

    if (nullptr != pArg)
    {
        EFFECT_BASE_DESC* pEffectBaseDesc = (EFFECT_BASE_DESC *) pArg;
        m_fFrame = pEffectBaseDesc->fFrame;
        m_fMaxFrame = pEffectBaseDesc->fMaxFrame;
    }

    return S_OK;
}

_uint CEffect_Base::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);
    return 0;
}

void CEffect_Base::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CEffect_Base::Render()
{
    return S_OK;
}

void CEffect_Base::Free()
{
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBuffer_Com);

    __super::Free();
}
