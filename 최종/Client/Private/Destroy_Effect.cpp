#include "pch.h"
#include "GameInstance.h"
#include "Destroy_Effect.h"
#include "Boss_Destroy_Effect.h"
#include "Trigger_BlackHole.h"

_float g_EffectSound2 = 0.5f;

CDestroy_Effect::CDestroy_Effect(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CEffect_Base(pGraphic_Device)
{
}

CDestroy_Effect::CDestroy_Effect(const CDestroy_Effect& rhs) :
	CEffect_Base(rhs)
{
}

HRESULT CDestroy_Effect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CItem_Effect"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDestroy_Effect::Initialize(void* pArg)
{
    m_pGameInstance->StopSound(CSound_Manager::EFFECT);
    m_pGameInstance->PlaySoundW(L"Mine_Explode.wav", CSound_Manager::EFFECT, g_EffectSound2);

    if (FAILED(__super::Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Initialize : __super,CItem_Effect"));
        return E_FAIL;
    }

    if (pArg != nullptr)
    {
        DESTROY_EFFECT_DESC* DestroyEffectDesc = (DESTROY_EFFECT_DESC*)pArg;
        m_vPosition = DestroyEffectDesc->vPosition;
        m_fScale = DestroyEffectDesc->fScale;
    }

    if (FAILED(Add_Components()))
    {
        MSG_BOX(TEXT("Failed to Add_Components : __super,CItem_Effect"));
        return E_FAIL;
    }

    m_pTransform->Set_State(CTransform::STATE_POSITION, m_vPosition);
    m_pTransform->Set_Scale(_float3(m_fScale, m_fScale, m_fScale));

    return S_OK;
}

_uint CDestroy_Effect::Tick(_float fTimeDelta)
{
    if (m_bDead)
        return OBJECT_DEAD;

    // 스프라이트
    m_fFrame += fTimeDelta * 20.f;
    if (m_fFrame > m_fMaxFrame)
    {
        Set_Dead();
        CTrigger_BlackHole* pTrigger = (dynamic_cast<CTrigger_BlackHole*>(m_pGameInstance->Get_Object(LEVEL_1945, TEXT("Layer_Trigger_BlackHole"))));
        pTrigger->Set_Active();
    }
    

    if (11 == (_uint)m_fFrame)
        Second_Explosion(1);

    if (31 == (_uint)m_fFrame)
        Second_Explosion(2);

    SetUp_BillBoard();

    __super::Tick(fTimeDelta);

    return OBJECT_NOTHING;
}

void CDestroy_Effect::Late_Tick(_float fTimeDelta)
{
    if (LEVEL_LOADING == m_pGameInstance->Get_Level()/* || LEVEL_1945 == m_pGameInstance->Get_Level()*/)
        return;


    __super::Late_Tick(fTimeDelta);

    m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
}

HRESULT CDestroy_Effect::Render()
{
    if (LEVEL_LOADING == m_pGameInstance->Get_Level()/* || LEVEL_1945 == m_pGameInstance->Get_Level()*/)
        return 0;

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

HRESULT CDestroy_Effect::Add_Components()
{
    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
    {
        MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Rect : Add_Components"));
        return E_FAIL;
    }

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_1945, TEXT("Prototype_Component_Texture_Effect_Destroy"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
    {
        MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Effect_Destroy : Add_Components"));
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CDestroy_Effect::Set_RenderState()
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

HRESULT CDestroy_Effect::Reset_RenderState()
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

void CDestroy_Effect::SetUp_BillBoard()
{
    //CTransform* pCameraTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(g_eLevel, TEXT("Layer_Camera_Player"), g_strTransformTag));
    if (nullptr == m_pCameraTransform)
    {
        m_pCameraTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_1945, TEXT("Layer_Camera"), g_strTransformTag));
        if (nullptr != m_pCameraTransform)
            Safe_AddRef(m_pCameraTransform);
    }

    if (nullptr == m_pCameraTransform)
    {
        MSG_BOX(TEXT("nullptr == m_pCameraTransformCom : CDestroy_Effect::SetUp_BillBoard()"));
        return;
    }

    const _float4x4 pCameraWorldMatrix = *m_pCameraTransform->Get_WorldMatrix();

    _float3 vScale = m_pTransform->Get_Scale();

    m_pTransform->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0]) * vScale.x);
    m_pTransform->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_UP][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_UP][0]) * vScale.y);
    m_pTransform->Set_State(CTransform::STATE_LOOK, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0]) * vScale.z);
}

void CDestroy_Effect::Second_Explosion(_uint iCount)
{
    CBoss_Destroy_Effect::BOSS_DESTROY_EFFECT_DESC BossDestroyEffectDesc = {};
    BossDestroyEffectDesc.vPosition.x = m_pTransform->Get_State(CTransform::STATE_POSITION).x + (_float)(rand() % 20 + 1) - 10.f;
    BossDestroyEffectDesc.vPosition.y = m_pTransform->Get_State(CTransform::STATE_POSITION).y + (_float)(rand() % 10 + 1) - 5.f;
    BossDestroyEffectDesc.vPosition.z = m_pTransform->Get_State(CTransform::STATE_POSITION).z + (_float)(rand() % 20 + 1) - 15.f;
    BossDestroyEffectDesc.fMaxFrame = 89.f;

    for (_uint i = 0; i < iCount; ++i)
    {
        BossDestroyEffectDesc.fScale = (_float)(rand() % 10) + 1.f + 5.f;
        if (FAILED(m_pGameInstance->Add_Clone(g_eLevel, TEXT("Layer_DestroyEffect"), TEXT("Prototype_GameObject_Boss_Destroy_Effect"), &BossDestroyEffectDesc)))
        {
            MSG_BOX(TEXT("Failed to Ready_Layer_Boss_Destroy_Effect : CLevel_GamePlay"));
            return;
        }
    }
}

CDestroy_Effect* CDestroy_Effect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CDestroy_Effect* pInstance = new CDestroy_Effect(pGraphic_Device);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Faild to Created : CDestroy_Effect"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDestroy_Effect::Clone(void* pArg)
{
    CDestroy_Effect* pInstance = new CDestroy_Effect(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Faild to Cloned : CDestroy_Effect"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDestroy_Effect::Free()
{
    Safe_Release(m_pCameraTransform);
    __super::Free();
}
