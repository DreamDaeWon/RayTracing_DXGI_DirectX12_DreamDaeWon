#include "pch.h"
#include "GameInstance.h"
#include "RedCircle.h"

CRedCircle::CRedCircle(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CLandObject(pGraphic_Device)
{
}

CRedCircle::CRedCircle(const CRedCircle& rhs) :
	CLandObject(rhs)
{
}

HRESULT CRedCircle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CRedCircle"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRedCircle::Initialize(void* pArg)
{

	if (pArg != nullptr)
	{
		// 스노우 정보 받아오기
		RED_CIRCLE_DESC* pRedCircleDesc = (RED_CIRCLE_DESC*)pArg;
		m_vBossPos = pRedCircleDesc->m_vPos;
		m_fScale = pRedCircleDesc->m_fScale;
		m_fLifeTime = pRedCircleDesc->m_fLifeTime;

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
		MSG_BOX(TEXT("Failed to Initialize : pArg == nullptr CRedCircle"));
		return E_FAIL;
	}*/

	if (FAILED(__super::Initialize(pArg)))
	{

		MSG_BOX(TEXT("Failed to Initialize : __super,CRedCircle"));
		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX(TEXT("Failed to Add_Components : __super,CRedCircle"));
		return E_FAIL;
	}

	//m_pTransform->Set_State(CTransform::STATE_RIGHT, m_pTransform->Get_State(CTransform::STATE_RIGHT) * 5);
	//m_pTransform->Set_State(CTransform::STATE_UP, m_pTransform->Get_State(CTransform::STATE_UP) * 5);
	//m_pTransform->Set_State(CTransform::STATE_LOOK, m_pTransform->Get_State(CTransform::STATE_LOOK) * 5);
	//m_pCollider_Com->Set_Scale(m_pTransform->Get_Scale());


	m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_RIGHT),5.f);

	m_pTransform->Set_State(CTransform::STATE_LOOK, m_pTransform->Get_State(CTransform::STATE_LOOK));
	m_pTransform->Set_State(CTransform::STATE_RIGHT, m_pTransform->Get_State(CTransform::STATE_RIGHT)* m_fScale);
	m_pTransform->Set_State(CTransform::STATE_UP, m_pTransform->Get_State(CTransform::STATE_UP) * m_fScale);
	m_pTransform->Set_State(CTransform::STATE_POSITION, m_vBossPos);


	return S_OK;
}

_uint CRedCircle::Tick(_float fTimeDelta)
{

	if (m_bDead)
		return OBJECT_DEAD;



	if (0.f <= m_fLifeTime)
		m_fLifeTime -= fTimeDelta;


	_float3 vPos = m_pTransform->Get_State(CTransform::STATE_POSITION);
	m_pTransform->Set_State(CTransform::STATE_POSITION,_float3(vPos.x,0.1f, vPos.z));
	//__super::SetUp_OnTerrain(0.1f);

	__super::Tick(fTimeDelta);
	return OBJECT_NOTHING;
}

void CRedCircle::Late_Tick(_float fTimeDelta)
{
	if (0.f > m_fLifeTime)
		Set_Dead();

	__super::Late_Tick(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
}

HRESULT CRedCircle::Render()
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

void CRedCircle::Return_Look_Position(_float3* pvLook, _float3* pvPosition)
{
	*pvLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
	*pvPosition = m_pTransform->Get_State(CTransform::STATE_POSITION);
}

_float3 CRedCircle::Return_ViewPort_Pos()
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

HRESULT CRedCircle::Add_Components()
{	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Rect : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Texture */
	//if (FAILED(__super::Add_Component(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_PlayerBack"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	//{
	//	MSG_BOX(TEXT("Failed to Prototype_Component_Texture_PlayerBack : Add_Components"));
	//	return E_FAIL;
	//}
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_Red_Circle"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Red_Circle : Add_Components"));
		return E_FAIL;
	}

	//콜라이더 실험 BEGIN.
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Rect"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pCollider_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Collider_Rect : Add_Components"));
		return E_FAIL;
	}
	//콜라이더 실험 END.
	return S_OK;
}

HRESULT CRedCircle::Set_RenderState()
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

	//if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE)))
	//{
	//	MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHATESTENABLE"));
	//	return E_FAIL;
	//}
	//if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 128)))
	//{
	//	MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHAREF"));
	//	return E_FAIL;
	//}
	//if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER)))
	//{
	//	MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHAFUNC"));
	//	return E_FAIL;
	//}


	return S_OK;
}

HRESULT CRedCircle::Reset_RenderState()
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

void CRedCircle::SetUp_BillBoard()
{
	//CTransform* pCameraTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_TUTORIAL, TEXT("Layer_Camera_Free"), g_strTransformTag));
	CTransform* pCameraTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(g_eLevel, TEXT("Layer_Camera_Player"), g_strTransformTag));

	if (nullptr == pCameraTransformCom)
	{
		MSG_BOX(TEXT("nullptr == pCameraTransformCom : CRedCircle::SetUp_BillBoard()"));
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

void CRedCircle::Look_Camera_Fixed_Y_Axis()
{
	//CTransform* pCameraTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_TUTORIAL, TEXT("Layer_Camera_Free"), g_strTransformTag));
	CTransform* pCameraTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(g_eLevel, TEXT("Layer_Camera_Player"), g_strTransformTag));

	if (nullptr == pCameraTransformCom)
	{
		MSG_BOX(TEXT("nullptr == pCameraTransformCom : CRedCircle::SetUp_BillBoard()"));
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


CRedCircle* CRedCircle::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CRedCircle* pInstance = new CRedCircle(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Faild to Created : CRedCircle"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRedCircle::Clone(void* pArg)
{
	CRedCircle* pInstance = new CRedCircle(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Faild to Cloned : CRedCircle"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRedCircle::Free()
{
	Safe_Release(m_pVIBuffer_Com);
	Safe_Release(m_pTextureCom);

	//콜라이더 실험 BEGIN
	Safe_Release(m_pCollider_Com);
	//콜라이더 실험 END.

	__super::Free();
}
