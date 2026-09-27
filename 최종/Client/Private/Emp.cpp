#include "pch.h"
#include "GameInstance.h"
#include "Emp.h"
#include "AirPlane.h"
#include"MoonStone.h"
#include"ShootMonster.h"
CEmp::CEmp(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CLandObject(pGraphic_Device)
{
}

CEmp::CEmp(const CEmp& rhs) :
	CLandObject(rhs)
{
}

HRESULT CEmp::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CEmp"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEmp::Initialize(void* pArg)
{

	if (pArg != nullptr)
	{
		// 스노우 정보 받아오기
		EMP_DESC* pRedCircleDesc = (EMP_DESC*)pArg;
		m_pPlaneTransform = pRedCircleDesc->pPlaneTransform;
		m_fMaxScale = pRedCircleDesc->fMaxScale;
		m_fLifeTime = pRedCircleDesc->fLifeTime;


			// 랜드 오브젝트 정보 받아오기
		LANDOBJECT_DESC* pLandObjectDesc = (LANDOBJECT_DESC*)pArg;
		pLandObjectDesc->fSpeedPerSec = 1.f;
		pLandObjectDesc->fRotationPerSec = D3DXToRadian(90.f);

	}
	/*else
	{
		MSG_BOX(TEXT("Failed to Initialize : pArg == nullptr CEmp"));
		return E_FAIL;
	}*/

	if (FAILED(__super::Initialize(pArg)))
	{

		MSG_BOX(TEXT("Failed to Initialize : __super,CEmp"));
		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX(TEXT("Failed to Add_Components : __super,CEmp"));
		return E_FAIL;
	}

	m_fCurScale = 1.f;

	m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_RIGHT),5.f);

	m_pTransform->Set_State(CTransform::STATE_LOOK, m_pTransform->Get_State(CTransform::STATE_LOOK) * m_fCurScale);
	m_pTransform->Set_State(CTransform::STATE_RIGHT, m_pTransform->Get_State(CTransform::STATE_RIGHT)* m_fCurScale);
	m_pTransform->Set_State(CTransform::STATE_UP, m_pTransform->Get_State(CTransform::STATE_UP) * m_fCurScale);
	m_pTransform->Set_State(CTransform::STATE_POSITION, m_pPlaneTransform->Get_State(CTransform::STATE_POSITION));


	return S_OK;
}

_uint CEmp::Tick(_float fTimeDelta)
{

	if (m_bDead)
		return OBJECT_DEAD;



	if (0.f <= m_fLifeTime)
	{
		if (m_bSound)
		{
			m_pGameInstance->StopSound(CSound_Manager::EFFECT);
			m_pGameInstance->PlaySoundW(TEXT("EMP.wav"), CSound_Manager::EFFECT, 0.4f);
			m_bSound = false;
		}
		m_fLifeTime -= fTimeDelta;
		m_fCurScale += fTimeDelta*40.f;
		if (m_pTransform->Get_Scale().x <= m_fMaxScale)
		{
			m_pTransform->Set_Scale(_float3(m_fCurScale, m_fCurScale, m_fCurScale));
			m_pCollider_Com->Set_Range(m_pTransform->Get_Scale().x);
		}
		else
		{
			m_bSound = true;
			m_fLifeTime = 0.f;
		}
	}

	m_pCollider_Com->Update_Collider_Info(*m_pTransform->Get_WorldMatrix());
	//__super::SetUp_OnTerrain(0.1f);

	__super::Tick(fTimeDelta);
	return OBJECT_NOTHING;
}

void CEmp::Late_Tick(_float fTimeDelta)
{
	if (0.f >= m_fLifeTime)
		Set_Dead();

	__super::Late_Tick(fTimeDelta);
	Collision_Monster();
	Collision_Monster_Bullet();
	Collision_MoonStone();

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
}

HRESULT CEmp::Render()
{
	//if (FAILED(__super::Render()))
	//{
	//	MSG_BOX(TEXT("Failed to Render : __super,CPlayer"));
	//	return E_FAIL;
	//}

	if (FAILED(m_pTransform->Bind_WorldMatrix()))
	{
		MSG_BOX(TEXT("Failed to Bind_WorldMatrix : Render"));
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->Bind_Texture(0, m_iTexNum)))
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

void CEmp::Return_Look_Position(_float3* pvLook, _float3* pvPosition)
{
	*pvLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
	*pvPosition = m_pTransform->Get_State(CTransform::STATE_POSITION);
}

_float3 CEmp::Return_ViewPort_Pos()
{
	_float3 vPosition = m_pTransform->Get_State(CTransform::STATE_POSITION);

	_float4x4 ViewMatrix, ProjectionMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjectionMatrix);

	vPosition = *D3DXVec3TransformCoord(&vPosition, &vPosition, &ViewMatrix);
	vPosition = *D3DXVec3TransformCoord(&vPosition, &vPosition, &ProjectionMatrix);

	vPosition.x = vPosition.x * g_iWinSizeX * 0.5f + g_iWinSizeX * 0.5f;
	vPosition.y = vPosition.y * g_iWinSizeY * 0.5f + g_iWinSizeY * 0.5f;

	return vPosition;
}

HRESULT CEmp::Add_Components()
{	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Rect : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_Emp"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Emp : Add_Components"));
		return E_FAIL;
	}

	//콜라이더 실험 BEGIN.
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pCollider_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Collider_Rect : Add_Components"));
		return E_FAIL;
	}
	//콜라이더 실험 END.
	return S_OK;
}

HRESULT CEmp::Set_RenderState()
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

HRESULT CEmp::Reset_RenderState()
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

void CEmp::SetUp_BillBoard()
{
	//CTransform* pCameraTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_TUTORIAL, TEXT("Layer_Camera_Free"), g_strTransformTag));
	CTransform* pCameraTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(g_eLevel, TEXT("Layer_Camera"), g_strTransformTag));

	if (nullptr == pCameraTransformCom)
	{
		MSG_BOX(TEXT("nullptr == pCameraTransformCom : CEmp::SetUp_BillBoard()"));
		return;
	}

	const _float4x4 pCameraWorldMatrix = *pCameraTransformCom->Get_WorldMatrix();


	_float3 vScale = m_pTransform->Get_Scale();

	m_pTransform->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0]) * vScale.x);
	//m_pTransform->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_UP][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_UP][0]) * vScale.y);
	m_pTransform->Set_State(CTransform::STATE_LOOK, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0]) * vScale.z);

	/*m_pTransform->Set_State(CTransform::STATE_RIGHT, *(_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0]);
	m_pTransform->Set_State(CTransform::STATE_LOOK, *(_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0]);*/
}

void CEmp::Look_Camera_Fixed_Y_Axis()
{
	//CTransform* pCameraTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_TUTORIAL, TEXT("Layer_Camera_Free"), g_strTransformTag));
	CTransform* pCameraTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(g_eLevel, TEXT("Layer_Camera"), g_strTransformTag));

	if (nullptr == pCameraTransformCom)
	{
		MSG_BOX(TEXT("nullptr == pCameraTransformCom : CEmp::SetUp_BillBoard()"));
		return;
	}

	const _float4x4 pCameraWorldMatrix = *pCameraTransformCom->Get_WorldMatrix();
	_float3 vCameraPos = pCameraTransformCom->Get_State(CTransform::STATE_POSITION);

	_float3 vScale = m_pTransform->Get_Scale();

	//_float3 vLook = vCameraPos - m_pTransform->Get_State(CTransform::STATE_POSITION);
	_float3 vLook = m_pTransform->Get_State(CTransform::STATE_POSITION) - vCameraPos;
	//vLook = *D3DXVec3Normalize(&vLook, &vLook) * vScale.z;
	_float3 vUp = { 0.f,1.f,0.f };
	vUp *= vScale.y;
	_float3 vRight = *D3DXVec3Cross(&vRight, &vUp, &vLook);
	vRight = *D3DXVec3Normalize(&vRight, &vRight) * vScale.x;
	vLook = *D3DXVec3Cross(&vLook, &vRight, &vUp);
	vLook = *D3DXVec3Normalize(&vLook, &vLook) * vScale.z;

	m_pTransform->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransform->Set_State(CTransform::STATE_UP, vUp);
	m_pTransform->Set_State(CTransform::STATE_LOOK, vLook);


}

void CEmp::Collision_Monster()
{
	if (m_bDead)
		return;

	list<CGameObject*>* pMonster = m_pGameInstance->Get_List(g_eLevel, TEXT("Layer_Monster"));
	if (nullptr == pMonster)
		return;

	for (auto iter = pMonster->begin(); iter != pMonster->end(); ++iter)
	{
		CCollider_Sphere* pMonsterColider = dynamic_cast<CCollider_Sphere*>(((*iter))->Get_Component(TEXT("Com_Collider_Sphere")));

		if (m_pGameInstance->Collision_Sphere(m_pCollider_Com, pMonsterColider))
		{
			(*iter)->Set_Dead();
		}
		
	}
	

}

void CEmp::Collision_Monster_Bullet()
{
	if (m_bDead)
		return;

	list<CGameObject*>* pMonsterBullet = m_pGameInstance->Get_List(g_eLevel, TEXT("Layer_Monster_Bullet"));
	if (nullptr == pMonsterBullet)
		return;

	for (auto iter = pMonsterBullet->begin(); iter != pMonsterBullet->end(); ++iter)
	{
		CCollider_Sphere* pMonsterBulletColider = dynamic_cast<CCollider_Sphere*>(((*iter))->Get_Component(TEXT("Com_Collider_Sphere")));
		if (nullptr == pMonsterBulletColider)
			continue;

		if (m_pGameInstance->Collision_Sphere(m_pCollider_Com, pMonsterBulletColider))
		{
			(*iter)->Set_Dead();
		}

	}
}

void CEmp::Collision_MoonStone()
{
	if (m_bDead)
		return;

	list<CGameObject*>* pMoonStone = m_pGameInstance->Get_List(g_eLevel, TEXT("Layer_MoonStone"));

	if (nullptr == pMoonStone)
		return;

	for (auto iter = pMoonStone->begin(); iter != pMoonStone->end(); ++iter)
	{
		CCollider_Sphere* pMoonstoneColider = dynamic_cast<CCollider_Sphere*>(((*iter))->Get_Component(TEXT("Com_Collider_Sphere")));

		if (m_pGameInstance->Collision_Sphere(m_pCollider_Com, pMoonstoneColider))
		{
			(*iter)->Set_Dead();
		}

	}



}


CEmp* CEmp::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEmp* pInstance = new CEmp(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Faild to Created : CEmp"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEmp::Clone(void* pArg)
{
	CEmp* pInstance = new CEmp(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Faild to Cloned : CEmp"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEmp::Free()
{
	Safe_Release(m_pVIBuffer_Com);
	Safe_Release(m_pTextureCom);

	//콜라이더 실험 BEGIN
	Safe_Release(m_pCollider_Com);
	//콜라이더 실험 END.

	__super::Free();
}
