#include "pch.h"
#include "Snow.h"
#include "GameInstance.h"
#include "Player.h"

CSnow::CSnow(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CLandObject(pGraphic_Device)
{
}

CSnow::CSnow(const CSnow& rhs) :
	CLandObject(rhs)
{
}

HRESULT CSnow::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CSnow"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSnow::Initialize(void* pArg)
{

	if (pArg != nullptr)
	{
		// 스노우 정보 받아오기
		SNOW_DESC* pSnowObjectDesc = (SNOW_DESC*)pArg;
		m_vBossLook = pSnowObjectDesc->m_vLook;
		m_vBossPos = pSnowObjectDesc->m_vPos;
		m_fScale = pSnowObjectDesc->m_fScale;
		m_fGoSpeed = pSnowObjectDesc->m_fSpeed;
		m_iTexNum = pSnowObjectDesc->m_iTexNum;

	/*	_float3 vLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
		_float3 vRight = *D3DXVec3Cross(&vRight,&_float3(0.f,1.f,0.f),&vLook);

		vLook = *D3DXVec3Cross(&vLook, &vRight ,&_float3(0.f, 1.f, 0.f));

		m_pTransform->Set_State(CTransform::STATE_LOOK, vRight);
		m_pTransform->Set_State(CTransform::STATE_RIGHT, -vLook);
		m_pTransform->Set_State(CTransform::STATE_UP, _float3(0.f, 1.f, 0.f));*/

		// 랜드 오브젝트 정보 받아오기
		LANDOBJECT_DESC* pLandObjectDesc = (LANDOBJECT_DESC*)pArg;
		pLandObjectDesc->fSpeedPerSec = 1.f;
		pLandObjectDesc->fRotationPerSec = D3DXToRadian(90.f);

	}
	/*else
	{
		MSG_BOX(TEXT("Failed to Initialize : pArg == nullptr CSnow"));
		return E_FAIL;
	}*/

	if (FAILED(__super::Initialize(pArg)))
	{

		MSG_BOX(TEXT("Failed to Initialize : __super,CSnow"));
		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX(TEXT("Failed to Add_Components : __super,CSnow"));
		return E_FAIL;
	}

	//m_pTransform->Set_State(CTransform::STATE_RIGHT, m_pTransform->Get_State(CTransform::STATE_RIGHT) * 5);
	//m_pTransform->Set_State(CTransform::STATE_UP, m_pTransform->Get_State(CTransform::STATE_UP) * 5);
	//m_pTransform->Set_State(CTransform::STATE_LOOK, m_pTransform->Get_State(CTransform::STATE_LOOK) * 5);
	//m_pCollider_Com->Set_Scale(m_pTransform->Get_Scale());

	_float3 vLook = m_vBossLook;
	_float3 vRight = *D3DXVec3Cross(&vRight, &_float3(0.f, 1.f, 0.f), &vLook);

	vLook = *D3DXVec3Cross(&vLook, &vRight, &_float3(0.f, 1.f, 0.f));

	_float3 vUp = _float3(0.f, 1.f, 0.f);

	D3DXVec3Normalize(&vLook, &vLook);
	D3DXVec3Normalize(&vRight, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);



	m_pTransform->Set_State(CTransform::STATE_LOOK, vRight * m_fScale);
	m_pTransform->Set_State(CTransform::STATE_RIGHT, -vLook * m_fScale);
	m_pTransform->Set_State(CTransform::STATE_UP, vUp * m_fScale);
	m_pTransform->Set_State(CTransform::STATE_POSITION, m_vBossPos);

	//m_pCollider_Com->Set_Scale(_float3(m_fScale, m_fScale, m_fScale));
	//m_pCollider_Com->Set_Range(m_fScale);
	m_pCollider_Com->Update_Collider_Info(*m_pTransform->Get_WorldMatrix());

	m_vBossLook = vRight;

	Set_Life_Time();
	m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(m_vBossPos.x, m_fScale, m_vBossPos.z));
	return S_OK;
}

_uint CSnow::Tick(_float fTimeDelta)
{
	//m_pCollider_Com->Update_Collider_Info(*m_pTransform->Get_WorldMatrix());
	////if (m_pGameInstance->Key_Pressing('K'))
	//{
	//	_float3 vRayDir, vRayPos;
	//	m_pGameInstance->Transform_PickingToLocalSpace(m_pTransform, &vRayDir, &vRayPos); //몬스터 로컬에서 마우스 레이포즈와 디렉션을 가져옴.
	//	if (true == m_pGameInstance->Collision_Rect_Ray(m_pCollider_Com, m_pTransform, vRayDir, vRayPos)) //콜라이더의 점을 몬스터의 로컬로 가져가서 계산
	//		int a = 1;//레이 필요
	//}

	//m_pTransform->(m_fGoSpeed * fTimeDelta);
	if (m_bDead)
		return OBJECT_DEAD;

	if (0.f <= m_fLifeTime)
		m_fLifeTime -= fTimeDelta;

	m_pTransform->Set_State(CTransform::STATE_POSITION,
		_float3(m_pTransform->Get_State(CTransform::STATE_POSITION).x + (m_vBossLook.x * m_fGoSpeed * fTimeDelta),
			m_pTransform->Get_State(CTransform::STATE_POSITION).y,
			m_pTransform->Get_State(CTransform::STATE_POSITION).z + m_vBossLook.z * m_fGoSpeed * fTimeDelta));


	m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_RIGHT), m_fGoSpeed * fTimeDelta);
	//__super::SetUp_OnTerrain(1.0f);

	__super::Tick(fTimeDelta);

	m_pCollider_Com->Update_Collider_Info(*m_pTransform->Get_WorldMatrix());

	return OBJECT_NOTHING;
}

void CSnow::Late_Tick(_float fTimeDelta)
{
	if (0.f > m_fLifeTime)
		Set_Dead();

	if (m_bDead)
		return;

	__super::Late_Tick(fTimeDelta);

	//SetUp_BillBoard();
	//Look_Camera_Fixed_Y_Axis();
	//Chase_Player(fTimeDelta);
	Collision_Player();
	//Collision_Box();

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CSnow::Render()
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

void CSnow::Return_Look_Position(_float3* pvLook, _float3* pvPosition)
{
	*pvLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
	*pvPosition = m_pTransform->Get_State(CTransform::STATE_POSITION);
}

_float3 CSnow::Return_ViewPort_Pos()
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

HRESULT CSnow::Add_Components()
{	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Sphere"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Sphere : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Texture */
	//if (FAILED(__super::Add_Component(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_PlayerBack"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	//{
	//	MSG_BOX(TEXT("Failed to Prototype_Component_Texture_PlayerBack : Add_Components"));
	//	return E_FAIL;
	//}
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_Snow"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Snow : Add_Components"));
		return E_FAIL;
	}

	//콜라이더 실험 BEGIN.
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pCollider_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Collider_Sphere : Add_Components"));
		return E_FAIL;
	}
	//콜라이더 실험 END.
	return S_OK;
}
//
//void CSnow::Mouse_Axis_Turn(_float fTimeDelta)
//{
//	POINT ptMouse = {};
//	GetCursorPos(&ptMouse);
//	//ScreenToClient(g_hWnd, &ptMouse);
//	//m_vCurrentMousePosition.x = (_float)ptMouse.x;
//	//m_vCurrentMousePosition.y = (_float)ptMouse.y;
//	//m_vCurrentMousePosition.z = 0.f;
//	//_float3 vRight = m_pTransform->Get_State(CTransform::STATE_RIGHT);
//	//_float3 vUp = m_pTransform->Get_State(CTransform::STATE_UP);
//	//_float3 vLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
//	//_float4x4 WorldMatrix = { vRight.x,vRight.y,vRight.z,0.f,
//	//vUp.x,vUp.y,vUp.z,0.f,
//	//vLook.x,vLook.y,vLook.z,0.f,
//	//0.f, 0.f, 0.f, 1.f
//	//};
//	//_float3 vAxis = m_vCurrentMousePosition - m_vPreMousePosition;
//	//vAxis = { vAxis.y, vAxis.x, 0.f };
//	//D3DXVec3TransformNormal(&vAxis, &vAxis, &WorldMatrix);
//	//m_pTransform->Turn(vAxis, fTimeDelta);//룩벡터와 수직
//
//
//	_long lDeltaX = ptMouse.x - m_ptMouse.x;
//	_long lDeltaY = ptMouse.y - m_ptMouse.y;
//
//	if (0.f != lDeltaX)
//	{
//		m_pTransform->Turn(_float3(0.f,1.f,0.f), fTimeDelta * lDeltaX);
//	}
//
//	if (0.f != lDeltaY)
//	{
//		m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_RIGHT), fTimeDelta * lDeltaY);
//	}
//
//	m_ptMouse = ptMouse;
//}

HRESULT CSnow::Set_RenderState()
{
	/*if (FAILED(m_pGraphic_Device->SetSamplerState(0,D3DSAMP_MINFILTER, D3DTEXF_POINT)))
	{
		MSG_BOX(TEXT("Failed to SetSamplerState : D3DSAMP_MINFILTER"));
		return E_FAIL;
	}
	if (FAILED(m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT)))
	{
		MSG_BOX(TEXT("Failed to SetSamplerState : D3DSAMP_MAGFILTER"));
		return E_FAIL;
	}
	if (FAILED(m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT)))
	{
		MSG_BOX(TEXT("Failed to SetSamplerState : D3DSAMP_MIPFILTER"));
		return E_FAIL;
	}*/
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHATESTENABLE"));
		return E_FAIL;
	}
	if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 128)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHAREF"));
		return E_FAIL;
	}
	if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHAFUNC"));
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSnow::Reset_RenderState()
{

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHATESTENABLE"));
		return E_FAIL;
	}

	/*if (FAILED(m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR)))
	{
		MSG_BOX(TEXT("Failed to SetSamplerState : D3DSAMP_MINFILTER"));
		return E_FAIL;
	}
	if (FAILED(m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR)))
	{
		MSG_BOX(TEXT("Failed to SetSamplerState : D3DSAMP_MAGFILTER"));
		return E_FAIL;
	}
	if (FAILED(m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR)))
	{
		MSG_BOX(TEXT("Failed to SetSamplerState : D3DSAMP_MIPFILTER"));
		return E_FAIL;
	}*/
	return S_OK;
}

void CSnow::SetUp_BillBoard()
{
	//CTransform* pCameraTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_TUTORIAL, TEXT("Layer_Camera_Free"), g_strTransformTag));
	CTransform* pCameraTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(g_eLevel, TEXT("Layer_Camera_Player"), g_strTransformTag));

	if (nullptr == pCameraTransformCom)
	{
		MSG_BOX(TEXT("nullptr == pCameraTransformCom : CSnow::SetUp_BillBoard()"));
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

void CSnow::Look_Camera_Fixed_Y_Axis()
{
	//CTransform* pCameraTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_TUTORIAL, TEXT("Layer_Camera_Free"), g_strTransformTag));
	CTransform* pCameraTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(g_eLevel, TEXT("Layer_Camera_Player"), g_strTransformTag));

	if (nullptr == pCameraTransformCom)
	{
		MSG_BOX(TEXT("nullptr == pCameraTransformCom : CSnow::SetUp_BillBoard()"));
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

void CSnow::Collision_Player()
{
	if (m_bDead)
		return;

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")));
	if (nullptr == pPlayer)
		return;

	CCollider_Sphere* pPlayerCollider = dynamic_cast<CCollider_Sphere*>(pPlayer->Get_Component(TEXT("Com_Collider_Sphere")));
	if (nullptr == pPlayerCollider)
		return;

	if (m_pGameInstance->Collision_Sphere(m_pCollider_Com, pPlayerCollider))
	{
		pPlayer->Set_Hit(true);
		Set_Dead();
		//m_fLifeTime = 0.f;
	}
}

void CSnow::Collision_Box()
{
	if (m_bDead)
		return;

	list<CGameObject*>* pBoxList = m_pGameInstance->Get_List(LEVEL_STATIC,TEXT("Layer_Box"));
	if (nullptr == pBoxList)
		return;

	_float3 vPosition = m_pTransform->Get_State(CTransform::STATE_POSITION);
	for (auto iter = pBoxList->begin(); iter != pBoxList->end(); ++iter)
	{
		CCollider_Cube_AABB* pColliderCubeAABB = dynamic_cast<CCollider_Cube_AABB*>((*iter)->Get_Component(TEXT("Com_Collider_Cube_AABB")));
		if (nullptr == pColliderCubeAABB)
			continue;
		if (m_pGameInstance->Collision_AABB_Dot(pColliderCubeAABB, vPosition))
			Set_Dead();
	}
	
}


CSnow* CSnow::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSnow* pInstance = new CSnow(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Faild to Created : CSnow"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSnow::Clone(void* pArg)
{
	CSnow* pInstance = new CSnow(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Faild to Cloned : CSnow"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSnow::Free()
{
	Safe_Release(m_pVIBuffer_Com);
	Safe_Release(m_pTextureCom);

	//콜라이더 실험 BEGIN
	Safe_Release(m_pCollider_Com);
	//콜라이더 실험 END.

	__super::Free();
}
