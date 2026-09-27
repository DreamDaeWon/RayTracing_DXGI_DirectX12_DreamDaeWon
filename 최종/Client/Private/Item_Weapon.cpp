#include "pch.h"
#include "GameInstance.h"
#include "Item_Weapon.h"
#include"Player.h"
#include "Item_Effect.h"
CItem_Weapon::CItem_Weapon(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CItem(pGraphic_Device)
{
}

CItem_Weapon::CItem_Weapon(const CItem_Weapon& rhs)
    : CItem(rhs)
{
}

HRESULT CItem_Weapon::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Initialize_Prototype : __super, CItem_Weapon"));
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CItem_Weapon::Initialize(void* pArg)
{
    _uint iWeapon = { 0 };
    if (FAILED(__super::Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Initialize : CItem_Weapon"));
        return E_FAIL;
    }
    if (pArg != nullptr)
    {
        WEAPON_ITEM_DESC* WIDesc = (WEAPON_ITEM_DESC*)pArg;
        m_iExtraBullet = WIDesc->iExtraBullet;
        m_iNowBullet = WIDesc->iNowBullet;
        iWeapon = WIDesc->iWeaponID;
        m_bPlayerDrop = WIDesc->bPlayerDrop;
    }
    if (FAILED(Add_Components()))
    {
        MSG_BOX(TEXT("Failed to Add_Components : __super, CItem_Weapon"));
        return E_FAIL;
    }
    
    m_pTransform->Set_State(CTransform::STATE_POSITION, m_vPosition);

    if (m_iExtraBullet == 0)
    {
        _uint iRandom = rand() % 5 + 1;
        m_iFrame = iRandom;
    }
    else
        m_iFrame = iWeapon;


    // 포물선을 그리기 위한 준비과정
    m_vBeforePos = m_vPosition;

    D3DXVec3Normalize(&m_vLook, &m_vLook);

    m_vLook = m_vLook * m_MoveDistance;

    m_vEndPos = _float3(m_vBeforePos.x + m_vLook.x, m_vBeforePos.y, m_vBeforePos.z + m_vLook.z);

    m_vGoPos = m_vBeforePos;
        
    // x 스케일의 값을 가져온다
    m_fScale = *m_pTransform->Get_Scale();
    m_bJump = m_bPlayerDrop;
    if(m_bJump)
    {
        Set_Parabola();
    }

    // 준수 추가 2/5
    CItem_Effect::ITEM_EFFECT_DESC ItemEffectDesc = {};
    ItemEffectDesc.vPosition = m_vGoPos;
    ItemEffectDesc.iWeaponID = m_iFrame;
    if (FAILED(m_pGameInstance->Add_Clone(g_eLevel, TEXT("Layer_ItemEffect"), TEXT("Prototype_GameObject_Item_Effect"), &ItemEffectDesc)))
    {
        MSG_BOX(TEXT("Failed to Ready_Layer_Item_Effect : CLevel_GamePlay"));
        return E_FAIL;
    }

    list<CGameObject*>* pList = m_pGameInstance->Get_List(g_eLevel, TEXT("Layer_ItemEffect"));

    if (nullptr != pList)
    {
        auto iter = pList->end();
        --iter;

        m_pItem_Effect = dynamic_cast<CItem_Effect*>((*iter));
        Safe_AddRef(m_pItem_Effect);
    }


    return S_OK;
}

_uint CItem_Weapon::Tick(_float fTimeDelta)
{
    if (m_bDead)
    {
        m_pItem_Effect->Set_Dead();
        return OBJECT_DEAD;
    }
        
    m_fTime += fTimeDelta;
    if(m_bJump)
    {
        Jump(fTimeDelta);
    }
    else if(m_bPlayerDrop)
    {
        SetUp_OnTerrain(m_fScale * 0.5f);
    }

    if (0.1f <= m_fTime) // 처음부터 판별을 하게 되면 아예 점프를 뛰지 않음
    {
        if ((m_pTransform->Get_State(CTransform::STATE_POSITION).y < m_fScale * 0.5f))
        {
            m_bJump = false;
        }
 
    }
    

	//m_pTransform->Turn(_float3(0.f, 1.f, 0.f), fTimeDelta);
    SetUp_BillBoard();

    _float4x4 WorldMatrix = *(m_pTransform->Get_WorldMatrix());
    m_pCollider_Com[COLLIDER_CUBE_AABB]->Update_Collider_Info(WorldMatrix);

    __super::Tick(fTimeDelta);

    m_pItem_Effect->Update_Position(m_pTransform->Get_State(CTransform::STATE_POSITION));

    return OBJECT_NOTHING;
}

void CItem_Weapon::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

    Item_Collision(fTimeDelta);

    m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CItem_Weapon::Render()
{
    if (FAILED(m_pTransform->Bind_WorldMatrix()))
    {
        MSG_BOX(TEXT("Failed to Bind_WorldMatrix : Render"));
        return E_FAIL;
    }
    if (FAILED(m_pTextureCom->Bind_Texture(0, m_iFrame)))
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

void CItem_Weapon::Set_Parabola()
{
    m_fEndHight = m_vEndPos.y - m_vBeforePos.y; // 도착지점의 높이와 시작지점 높이의 차를 구해줌
    m_fHeight = m_fMaxHeight - m_vBeforePos.y; // 

    m_fG = 2.f * m_fHeight / (m_fMaxTime * m_fMaxTime);

    m_fV_Y = sqrtf(2.f * m_fG * m_fHeight);

    _float b = -2.f * m_fV_Y;
    _float c = 2.f * m_fEndHight;

    m_fEndTime = (-b + sqrtf(b * b - 4.f * m_fG * c)) / (2.f * m_fG);

    m_fV_X = -(m_vBeforePos.x - m_vEndPos.x) / m_fEndTime;
    m_fV_Z = -(m_vBeforePos.z - m_vEndPos.z) / m_fEndTime;
}

void CItem_Weapon::Jump(_float fTimeDelta)
{

    m_vGoPos.x = m_vBeforePos.x + m_fV_X * m_fTime;
    m_vGoPos.y = m_vBeforePos.y + (m_fV_Y * m_fTime) - (0.5f * m_fG * m_fTime * m_fTime);
    m_vGoPos.z = m_vBeforePos.z + m_fV_Z * m_fTime;

    m_pTransform->Set_State(CTransform::STATE_POSITION, m_vGoPos);
}

HRESULT CItem_Weapon::Add_Components()
{
    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
    {
        MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Rect : Add_Components"));
        return E_FAIL;
    }

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_Weapon"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
    {
        MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Item_Weapon : Add_Components"));
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

HRESULT CItem_Weapon::Set_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 32);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    return S_OK;
}

HRESULT CItem_Weapon::Reset_RenderState()
{
    if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE)))
    {
        MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHATESTENABLE"));
        return E_FAIL;
    }
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    return S_OK;
}

void CItem_Weapon::SetUp_BillBoard()
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

void CItem_Weapon::Item_Collision(_float fTimeDelta)
{
   CCollider_Cube_AABB* pCollider = dynamic_cast<CCollider_Cube_AABB*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"))->Get_Component(TEXT("Com_Collider_Cube_AABB")));
    
   m_fFrame += fTimeDelta;
   // 나중에 UI 띄우기 야호
   if (m_pGameInstance->Collision_AABB(dynamic_cast<CCollider_Cube_AABB*>(m_pCollider_Com[COLLIDER_CUBE_AABB]), pCollider) && m_pGameInstance->Key_Down('E')&& m_fFrame>=1.f)
   {
       dynamic_cast<CPlayer*>(m_pGameInstance->Get_Object(LEVEL_STATIC,TEXT("Layer_Player")))->Set_WeaponChange(m_iFrame,m_iExtraBullet,m_iNowBullet);
	   m_bDead = true;
       m_fFrame = 0.f;
   }

}

CItem_Weapon* CItem_Weapon::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CItem_Weapon* pInstance = new CItem_Weapon(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CItem_Weapon"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CItem_Weapon::Clone(void* pArg)
{
    CItem_Weapon* pInstance = new CItem_Weapon(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Cloned : CItem_Weapon"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CItem_Weapon::Free()
{
    Safe_Release(m_pCameraTransform);
    Safe_Release(m_pItem_Effect);

    __super::Free();
}
