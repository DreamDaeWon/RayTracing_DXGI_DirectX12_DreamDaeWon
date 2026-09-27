#include "pch.h"
#include "Wall.h"
#include "GameInstance.h"
#include <Player.h>
#include <Weapon.h>
#include "Monster_Bullet_Base.h"

CWall::CWall(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CWall::CWall(const CWall& rhs)
	: CGameObject(rhs)
{
}

HRESULT CWall::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CWall"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CWall::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Initialize : __super,CWall"));
		return E_FAIL;
	}

	if (nullptr != pArg)
	{
		tagWall_DESC* pWallDesc = (tagWall_DESC*)pArg;

		m_pTransform->Set_WorldMatrix(pWallDesc->WorldMatrix);
		m_iFrame = pWallDesc->iFrame;
		////pArg
		//if (FAILED(Add_Components(pWallDesc->strVIBufferTag)))
		//{
		//	MSG_BOX(TEXT("Failed to Add_Components : __super,CWall"));
		//	return E_FAIL;
		//}

		if (FAILED(Add_Components(pWallDesc->strVIBufferTag,  pWallDesc->iLevel)))
		{
			MSG_BOX(TEXT("Failed to Add_Components : __super,CWall"));
			return E_FAIL;
		}
	}

	m_pCollider_Com->Set_Scale(_float3(5.f, 9.f, 1.f));

	if (0 == m_iFrame&&g_eLevel==LEVEL_TUTORIAL)
	{
		m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(m_pTransform->Get_State(CTransform::STATE_POSITION).x, m_pTransform->Get_State(CTransform::STATE_POSITION).y - 8.8f, m_pTransform->Get_State(CTransform::STATE_POSITION).z));
	}

	m_fFrame = m_iFrame;

	return S_OK;
}

_uint CWall::Tick(_float fTimeDelta)
{
	if (LEVEL_LOADING == m_pGameInstance->Get_Level())// || LEVEL_1945 == m_pGameInstance->Get_Level())
		return 0;

	if (m_bDead)
		return 1;

	if (8 == m_iFrame)
	{
		if (0 > m_fY)
		{
			m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(m_pTransform->Get_State(CTransform::STATE_POSITION).x, m_fY, m_pTransform->Get_State(CTransform::STATE_POSITION).z));
			m_fY += 0.025f;
		}
	}

	if (10 == m_iFrame)
	{
		m_fFrame += fTimeDelta * 200.f;
		if (m_fFrame > 37.f)
			m_fFrame = 10.f;
	}

	__super::Tick(fTimeDelta);

	Setting_Collider_Info();
 	

	return 0;
}

void CWall::Late_Tick(_float fTimeDelta)
{
	if (LEVEL_LOADING == m_pGameInstance->Get_Level())// || LEVEL_1945 == m_pGameInstance->Get_Level())
		return;

	Collision_Bullet(fTimeDelta);
	Collision_Monster_Bullet(fTimeDelta);

	__super::Late_Tick(fTimeDelta);

	if (8 == m_iFrame || 10 <= m_iFrame)
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
	else
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CWall::Render()
{
	_float4x4 ViewMatrix, ProjectionMatrix;

	if (FAILED(m_pTransform->Bind_WorldMatrix()))
	{
		MSG_BOX(TEXT("Failed to Bind_WorldMatrix : __super,CWall"));
		return E_FAIL;
	}

	if (LEVEL_1945 == m_pGameInstance->Get_Level())
	{
		if (FAILED(m_pTextureCom->Bind_Texture(0, (_uint)m_fFrame)))
		{
			MSG_BOX(TEXT("Failed to Bind_Texture : __super,CWall"));
			return E_FAIL;
		}
	}
	else
	{
		if (FAILED(m_pTextureCom->Bind_Texture(0, m_iFrame)))
		{
			MSG_BOX(TEXT("Failed to Bind_Texture : __super,CWall"));
			return E_FAIL;
		}
	}

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//if (LEVEL_SNOWBOSS == m_pGameInstance->Get_Level())
		//m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	Set_RenderState();

	if (FAILED(m_pVIBuffer_Com->Render()))
	{
		MSG_BOX(TEXT("Failed to Render : __super,CWall"));
		return E_FAIL;
	}

	Reset_RenderState();

	return S_OK;
}


void CWall::Collision_Bullet(_float fTimeDelta)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")));
	if (nullptr == pPlayer)
		return;

	CWeapon* pWeapons[3] = { pPlayer->Get_Weapon(0),pPlayer->Get_Weapon(1) ,pPlayer->Get_Weapon(2) };

	for (size_t i = 0; i < 3; i++)
	{
		if (nullptr == pWeapons[i])
			continue;
		_float fTotalDamage = pWeapons[i]->Collision_Bullet_Rect((m_pCollider_Com), fTimeDelta);
	}

	return;
}

void CWall::Collision_Monster_Bullet(_float fTimeDelta)
{
	list<CGameObject*>* pMonsterBulletList = m_pGameInstance->Get_List(g_eLevel, TEXT("Layer_Monster_Bullet"));
	if (nullptr == pMonsterBulletList)
		return;

	for (auto iter = pMonsterBulletList->begin(); iter != pMonsterBulletList->end(); ++iter)
	{
		//레이(몬스터 불릿) 정보와 플레이어 콜라이더 렉트
		_float3 vRayDir(0.f, 0.f, 0.f);
		_float3 vRayPos(0.f, 0.f, 0.f);
		_float fLength(0.f);
		(dynamic_cast<CMonster_Bullet_Base*>(*iter))->Get_RayPos_RayDir_Length(fTimeDelta ,&vRayPos, &vRayDir, &fLength);
		if (m_pGameInstance->Collision_Rect_Ray_Same_Space(m_pCollider_Com, vRayDir, vRayPos, fLength))
		{
			(*iter)->Set_Dead();
		}
	}
}

HRESULT CWall::Add_Components(const wstring& strVIBufferTag)
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, strVIBufferTag, TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Wall : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Wall"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Wall : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Rect"), TEXT("Com_Collider_Rect"), reinterpret_cast<CComponent**>(&m_pCollider_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Collider_Rect : Add_Components"));
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Collider"), TEXT("Com_Texture_Collider"), reinterpret_cast<CComponent**>(&m_pTextureCollider))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Collider : Add_Components"));
		return E_FAIL;
	}

	
	return S_OK;
}

HRESULT CWall::Add_Components(const wstring& strVIBufferTag, _uint iLevel)
{
	
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(iLevel, strVIBufferTag, TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Wall : Add_Components"));
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Wall"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Wall : Add_Components"));
		return E_FAIL;
	}
	//if (iLevel == LEVEL_TUTORIAL)
	//{
	//	/* For.Com_Texture */
	//	
	//}
	//else if (iLevel == LEVEL_SNOWBOSS)
	//{
	//	/* For.Com_Texture */
	//	if (FAILED(__super::Add_Component(iLevel, TEXT("Prototype_Component_Texture_Wall_Boss"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	//	{
	//		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Wall : Add_Components"));
	//		return E_FAIL;
	//	}
	//}
	//

	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Rect"), TEXT("Com_Collider_Rect"), reinterpret_cast<CComponent**>(&m_pCollider_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Collider_Rect : Add_Components"));
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Collider"), TEXT("Com_Texture_Collider"), reinterpret_cast<CComponent**>(&m_pTextureCollider))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Collider : Add_Components"));
		return E_FAIL;
	}

	
	return S_OK;
}

HRESULT CWall::Set_RenderState()
{
	if (8 == m_iFrame || 10 <= m_iFrame)
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
	}
	else
	{

		if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE)))
		{
			MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHATESTENABLE"));
			return E_FAIL;
		}
		if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200))) // 테스트 할 알파값
		{
			MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHAREF"));
			return E_FAIL;
		}
		if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER)))
		{
			MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHAFUNC"));
			return E_FAIL;
		}
	}

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	return S_OK;
}
HRESULT CWall::Reset_RenderState()
{
	if (8 == m_iFrame)
	{
		if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE)))
		{
			MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHABLENDENABLE"));
			return E_FAIL;
		}
	}
	else
	{
		if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE)))
		{
			MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHATESTENABLE"));
			return E_FAIL;
		}
	}

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	return S_OK;
}

void CWall::Landing_Object(const wstring& strObjectLayerTag)
{
}

void CWall::Setting_Collider_Info()
{
	_float4x4 WorldMatrix = *m_pTransform->Get_WorldMatrix();
	
	//벽의 위치를 받아옴.
	m_pCollider_Com->Update_Collider_Info(WorldMatrix);
	m_pCollider_Com->Turn(_float3(0.f, 1.f, 0.f), 1.f);
	_float3 vScale = m_pCollider_Com->Get_Scale();
	_float3 vRight = m_pCollider_Com->Get_State(CCollider::STATE_RIGHT);
	_float3 vPos = m_pCollider_Com->Get_State(CCollider::STATE_POSITION);
	vPos -= vRight * 0.5f;
	vPos.y += vScale.y * 0.5f;
	m_pCollider_Com->Set_State(CCollider::STATE_POSITION, vPos);
	m_pCollider_Com->Update_Collider_Info(*m_pCollider_Com->Get_WorldMatrix());
	
}

CWall* CWall::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CWall* pInstance = new CWall(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWall"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWall::Clone(void* pArg)
{
	CWall* pInstance = new CWall(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWall"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWall::Free()
{
	Safe_Release(m_pVIBuffer_Com);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pCollider_Com);
	Safe_Release(m_pTextureCollider);

	__super::Free();
}
