#include "pch.h"
#include "GameInstance.h"
#include "Item_Power.h"
#include "Player.h"
#include "Weapon.h"

CItem_Power::CItem_Power(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CItem(pGraphic_Device)
{
}

CItem_Power::CItem_Power(const CItem_Power& rhs)
    : CItem(rhs)
{
}

HRESULT CItem_Power::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Initialize_Prototype : __super, CItem_Power"));
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CItem_Power::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Initialize : CItem_Power"));
        return E_FAIL;
    }

    if (FAILED(Add_Components()))
    {
        MSG_BOX(TEXT("Failed to Add_Components : __super, CItem_Power"));
        return E_FAIL;
    }

    m_fMaxFrame = 5.f;

    m_pTransform->Set_State(CTransform::STATE_POSITION, m_vPosition);
    m_pTransform->Set_Scale(_float3(1.f, 1.f, 1.f));
    //m_pTransform->Set_State(CTransform::STATE_LOOK, _float3(0.f, 0.f, -1.f));

    return S_OK;
}

_uint CItem_Power::Tick(_float fTimeDelta)
{
    if (m_bDead)
    {
        m_fLifeTime -= fTimeDelta;
        return OBJECT_NOTHING;
    }
       

    _float4x4 WorldMatrix = *(m_pTransform->Get_WorldMatrix());
    m_pCollider_Com[COLLIDER_CUBE_AABB]->Update_Collider_Info(WorldMatrix);

    // 스프라이트
    m_fFrame += fTimeDelta * POWER_SPRITE_SPEED;
    if (m_fFrame > m_fMaxFrame)
        m_fFrame = 0.f;

    __super::Tick(fTimeDelta);

    return OBJECT_NOTHING;
}

void CItem_Power::Late_Tick(_float fTimeDelta)
{
    if (m_bDead)
    {
        if (0.f > m_fLifeTime)
        {
            //m_pGameInstance->StopSound(CSound_Manager::EFFECT);
            //m_pGameInstance->PlaySoundW(TEXT("chimera_pattern3_beam_00.wav"), CSound_Manager::EFFECT, 0.2f);
            m_bDead = false;
        }
          
        return;
    }
       

    __super::Late_Tick(fTimeDelta);

    Item_Collision();
    SetUp_BillBoard();

    m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CItem_Power::Render()
{
    if (m_bDead)
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

HRESULT CItem_Power::Add_Components()
{
    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
    {
        MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Rect : Add_Components"));
        return E_FAIL;
    }

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_1945"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
    {
        MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Item_1945 : Add_Components"));
        return E_FAIL;
    }

    /* For. Com_Collider*/
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube_AABB"), TEXT("Com_Collider_Cube_AABB"), reinterpret_cast<CComponent**>(&m_pCollider_Com[COLLIDER_CUBE_AABB]))))
    {
        MSG_BOX(TEXT("Failed to Prototype_Component_Collider_Cube_AABB : Add_Components"));
        return E_FAIL;
    }
    return S_OK;
}

HRESULT CItem_Power::Set_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    return S_OK;
}

HRESULT CItem_Power::Reset_RenderState()
{
    if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE)))
    {
        MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHATESTENABLE"));
        return E_FAIL;
    }
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    return S_OK;
}

void CItem_Power::Item_Collision()
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")));
    if (nullptr == pPlayer)
        return;
    CCollider_Cube_AABB* pCollider = dynamic_cast<CCollider_Cube_AABB*>(pPlayer->Get_Component(TEXT("Com_Collider_Cube_AABB")));
    if (nullptr == pCollider)
        return;
    if (m_pGameInstance->Collision_AABB(dynamic_cast<CCollider_Cube_AABB*>(m_pCollider_Com[COLLIDER_CUBE_AABB]), pCollider))
    {
        CWeapon* pWeapon = pPlayer->Get_Weapon(pPlayer->Get_CurWeapon());
        if (nullptr == pWeapon)
            return;
        pWeapon->Plus_GunGauge(pWeapon->Get_MaxGauge());
        Set_Life_Time();
        m_pGameInstance->StopSound(CSound_Manager::EFFECT);
        m_pGameInstance->PlaySoundW(TEXT("GetReinforceItem.wav"), CSound_Manager::EFFECT, 0.2f);
    }
}

void CItem_Power::SetUp_BillBoard()
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
        MSG_BOX(TEXT("nullptr == m_pCameraTransformCom : CPlayer::SetUp_BillBoard()"));
        return;
    }

    _float4x4 pCameraWorldMatrix = *m_pCameraTransform->Get_WorldMatrix();


    _float3 vScale = m_pTransform->Get_Scale();

    m_pTransform->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0]) * vScale.x);
    m_pTransform->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_UP][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_UP][0]) * vScale.y);
    m_pTransform->Set_State(CTransform::STATE_LOOK, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0]) * vScale.z);
}


CItem_Power* CItem_Power::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CItem_Power* pInstance = new CItem_Power(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CItem_Power"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CItem_Power::Clone(void* pArg)
{
    CItem_Power* pInstance = new CItem_Power(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Cloned : CItem_Power"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CItem_Power::Free()
{
    Safe_Release(m_pCameraTransform);

    __super::Free();
}
