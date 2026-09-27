#include "pch.h"
#include "GameInstance.h"
#include "Item_Effect.h"

CItem_Effect::CItem_Effect(LPDIRECT3DDEVICE9 pGraphic_Device) :
    CEffect_Base(pGraphic_Device)
{
}

CItem_Effect::CItem_Effect(const CItem_Effect& rhs) :
    CEffect_Base(rhs)
{
}

HRESULT CItem_Effect::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CItem_Effect"));
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CItem_Effect::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Initialize : __super,CItem_Effect"));
        return E_FAIL;
    }

    if (pArg != nullptr)
    {
        ITEM_EFFECT_DESC* ItemEffectDesc = (ITEM_EFFECT_DESC*)pArg;
        m_vPosition = ItemEffectDesc->vPosition;
        m_eWE = (WEAPON_EFFECT)ItemEffectDesc->iWeaponID;
    }

    if (FAILED(Add_Components()))
    {
        MSG_BOX(TEXT("Failed to Add_Components : __super,CItem_Effect"));
        return E_FAIL;
    }

    m_pTransform->Set_State(CTransform::STATE_POSITION, m_vPosition);

    WeaponType(m_eWE);

    return S_OK;
}

_uint CItem_Effect::Tick(_float fTimeDelta)
{
    if (m_bDead)
        return OBJECT_DEAD;

    // 스프라이트
    m_fFrame += fTimeDelta * 9.f;
    if (m_fFrame > m_fMaxFrame)
        m_fFrame = m_fMinFrame;

    SetUp_BillBoard();

    __super::Tick(fTimeDelta);

    return OBJECT_NOTHING;
}

void CItem_Effect::Late_Tick(_float fTimeDelta)
{
    if (LEVEL_LOADING == m_pGameInstance->Get_Level()/* || LEVEL_1945 == m_pGameInstance->Get_Level()*/)
        return;

    if (m_fLifeTime < 0.f)
        return;

    __super::Late_Tick(fTimeDelta);

    m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
}

HRESULT CItem_Effect::Render()
{
    if (LEVEL_LOADING == m_pGameInstance->Get_Level()/* || LEVEL_1945 == m_pGameInstance->Get_Level()*/)
        return 0;

    if (m_fLifeTime < 0.f)
        return S_OK;

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

void CItem_Effect::Update_Position(_float3 vPos)
{
    m_pTransform->Set_State(CTransform::STATE_POSITION, vPos);
}

HRESULT CItem_Effect::Add_Components()
{
    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
    {
        MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Rect : Add_Components"));
        return E_FAIL;
    }

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WeaponItemEffect"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
    {
        MSG_BOX(TEXT("Failed to Prototype_Component_Texture_WeaponItemEffect : Add_Components"));
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CItem_Effect::Set_RenderState()
{
    if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE)))
    {
        MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHABLENDENABLE"));
        return E_FAIL;
    }
    if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA)))
    {
        MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_SRCBLEND"));
        return E_FAIL;
    }
    if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA)))
    {
        MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_DESTBLEND"));
        return E_FAIL;
    }
    if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD)))
    {
        MSG_BOX(TEXT("Failed to SetRenderState : D3DBLENDOP_ADD"));
        return E_FAIL;
    }

    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    return S_OK;
}

HRESULT CItem_Effect::Reset_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE)))
    {
        MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHATESTENABLE"));
        return E_FAIL;
    }

    if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE)))
    {
        MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHABLENDENABLE"));
        return E_FAIL;
    }

    return S_OK;
}

void CItem_Effect::SetUp_BillBoard()
{
    //CTransform* pCameraTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(g_eLevel, TEXT("Layer_Camera_Player"), g_strTransformTag));
    if (nullptr == m_pCameraTransform)
    {
        m_pCameraTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera_Player"), g_strTransformTag));
        if (nullptr != m_pCameraTransform)
            Safe_AddRef(m_pCameraTransform);
    }

    if (nullptr == m_pCameraTransform)
    {
        MSG_BOX(TEXT("nullptr == m_pCameraTransformCom : CItem_Effect::SetUp_BillBoard()"));
        return;
    }

    const _float4x4 pCameraWorldMatrix = *m_pCameraTransform->Get_WorldMatrix();

    _float3 vScale = m_pTransform->Get_Scale();

    m_pTransform->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0]) * vScale.x);
    m_pTransform->Set_State(CTransform::STATE_LOOK, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0]) * vScale.z);
}

void CItem_Effect::WeaponType(WEAPON_EFFECT eWe)
{
    switch (eWe)
    {
    case Client::CItem_Effect::WE_PUPA:
        m_fFrame = 0.f;
        m_fMaxFrame = 19.f;
        break;
    case Client::CItem_Effect::WE_RAMBO:
        m_fFrame = 0.f;
        m_fMinFrame = 0.f;
        m_fMaxFrame = 19.f;
        break;
    case Client::CItem_Effect::WE_SASIN:
        m_fFrame = 60.f;
        m_fMinFrame = 60.f;
        m_fMaxFrame = 79.f;
        break;
    case Client::CItem_Effect::WE_SPARROW:
        m_fFrame = 40.f;
        m_fMinFrame = 40.f;
        m_fMaxFrame = 59.f;
        break;
    case Client::CItem_Effect::WE_SHARKBLOOD:
        m_fFrame = 20.f;
        m_fMinFrame = 20.f;
        m_fMaxFrame = 39.f;
        break;
    case Client::CItem_Effect::WE_ATLAS:
        m_fFrame = 80.f;
        m_fMinFrame = 80.f;
        m_fMaxFrame = 99.f;
        break;
    case Client::CItem_Effect::WE_END:
        break;
    default:
        break;
    }
}

CItem_Effect* CItem_Effect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CItem_Effect* pInstance = new CItem_Effect(pGraphic_Device);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Faild to Created : CItem_Effect"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CItem_Effect::Clone(void* pArg)
{
    CItem_Effect* pInstance = new CItem_Effect(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Faild to Cloned : CItem_Effect"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CItem_Effect::Free()
{
    Safe_Release(m_pCameraTransform);
    __super::Free();
}
