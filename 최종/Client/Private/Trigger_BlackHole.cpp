#include "pch.h"
#include "Trigger_BlackHole.h"
#include "GameInstance.h"
#include "AirPlane.h"
#include"Level_Loading.h"
#include"Camera_Player_DW2.h"

CTrigger_BlackHole::CTrigger_BlackHole(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CLandObject(pGraphic_Device)
{
}

CTrigger_BlackHole::CTrigger_BlackHole(const CTrigger_BlackHole& rhs) :
	CLandObject(rhs)
{
}

HRESULT CTrigger_BlackHole::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CTrigger_BlackHole"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTrigger_BlackHole::Initialize(void* pArg)
{

	if (pArg != nullptr)
	{
		TRIGGER_DESC* pDoorObjectDesc = (TRIGGER_DESC*)pArg;
		m_vBlackHoleLook = pDoorObjectDesc->vLook;
		m_vBlackHolePos = pDoorObjectDesc->vPos;
		m_fScale = pDoorObjectDesc->fScale;
		m_fGoSpeed = pDoorObjectDesc->fSpeed;
		m_iTexNum = pDoorObjectDesc->iTexNum;


		LANDOBJECT_DESC* pLandObjectDesc = (LANDOBJECT_DESC*)pArg;
		pLandObjectDesc->fSpeedPerSec = 1.f;
		pLandObjectDesc->fRotationPerSec = D3DXToRadian(1.f);

	}

	if (FAILED(__super::Initialize(pArg)))
	{

		MSG_BOX(TEXT("Failed to Initialize : __super,CTrigger_BlackHole"));
		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX(TEXT("Failed to Add_Components : __super,CTrigger_BlackHole"));
		return E_FAIL;
	}




	m_pTransform->Set_Scale(_float3(1.f,1.f,1.f));
	_float3 vScale = m_pTransform->Get_Scale();
	m_pTransform->Set_State(CTransform::STATE_LOOK, m_vBlackHoleLook * vScale.z);
	m_pTransform->Set_State(CTransform::STATE_UP, _float3(0.f, 1.f, 0.f)* vScale.y);
	_float3 vRight  = *D3DXVec3Cross(&vRight, &_float3(0.f, 1.f, 0.f), &m_pTransform->Get_State(CTransform::STATE_LOOK));
	m_pTransform->Set_State(CTransform::STATE_RIGHT, vRight * vScale.x);
	m_pTransform->Set_State(CTransform::STATE_POSITION, m_vBlackHolePos);
	m_pCollider_Com->Set_Range(2.f);
	m_pCollider_Com->Update_Collider_Info(*m_pTransform->Get_WorldMatrix()); 
	
	return S_OK;
}

_uint CTrigger_BlackHole::Tick(_float fTimeDelta)
{
	if (nullptr == m_pCamera)
	{
		m_pCamera = dynamic_cast<CCamera_Player_DW2*>(m_pGameInstance->Get_Object(LEVEL_1945, TEXT("Layer_Camera")));
		if (nullptr != m_pCamera)
			Safe_AddRef(m_pCamera);
	}
	if (m_bActive)
	{
		if(!m_bMove)
			m_pGameInstance->StopAll();

		(dynamic_cast<CAirPlane*>(m_pGameInstance->Get_Object(LEVEL_1945, TEXT("Layer_AirPlane"))))->SetCutScene(true);
		m_pGameInstance->PlaySoundW(TEXT("BullackHole_Active_Loop.wav"), CSound_Manager::SYSTEM_EFFECT3, 0.5f);

		m_bMove = true;
		m_bActive = false;

	}
	if(m_bMove)
		m_fTime += fTimeDelta;

	if (m_bMove && m_fTime > 1.f)
	{
		m_pCamera->SetCameraMode(CCamera_Player_DW2::CAMERA_PLAYER_RIGHT_VIEW);
		m_fScale += fTimeDelta * 40.f;
		m_pTransform->Set_Scale(_float3(m_fScale, m_fScale, m_fScale));
		m_pCollider_Com->Set_Range(m_pTransform->Get_Scale().x*0.19f);
		if(m_fTime>2.f)
			m_pTransform->Go_To_Dir(fTimeDelta * 5.f, _float3(0.f,0.f,-1.f));
	}
	if (m_fTime > 5.f)
	{
		//m_bMove = false;
		m_fTime = 0.f;
	}
	m_pCollider_Com->Update_Collider_Info(*m_pTransform->Get_WorldMatrix());

	__super::Tick(fTimeDelta);

	SetUp_BillBoard();
	return OBJECT_NOTHING;
}

void CTrigger_BlackHole::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);

}

HRESULT CTrigger_BlackHole::Render()
{
	if (!m_bActive&& !m_bMove)
		return S_OK;
	if (FAILED(m_pTransform->Bind_WorldMatrix()))
	{
		MSG_BOX(TEXT("Failed to Bind_WorldMatrix : Render"));
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->Bind_Texture(0, 0)))
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

void CTrigger_BlackHole::Return_Look_Position(_float3* pvLook, _float3* pvPosition)
{
	*pvLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
	*pvPosition = m_pTransform->Get_State(CTransform::STATE_POSITION);
}

_float3 CTrigger_BlackHole::Return_ViewPort_Pos()
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

HRESULT CTrigger_BlackHole::Add_Components()
{	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Cube : Add_Components"));
		return E_FAIL;
	}
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_Trigger_BlackHole"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Trigger_Door : Add_Components"));
		return E_FAIL;
	}

	//콜라이더 실험 BEGIN.
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pCollider_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Collider_Sphere : Add_Components"));
		return E_FAIL;
	}
	//콜라이더 실험 END.
	return S_OK;
}

HRESULT CTrigger_BlackHole::Set_RenderState()
{
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

HRESULT CTrigger_BlackHole::Reset_RenderState()
{

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHATESTENABLE"));
		return E_FAIL;
	}

	return S_OK;
}

void CTrigger_BlackHole::SetUp_BillBoard()
{
	CTransform* pCameraTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(g_eLevel, TEXT("Layer_Camera"), g_strTransformTag));

	if (nullptr == pCameraTransformCom)
	{
		MSG_BOX(TEXT("nullptr == pCameraTransformCom : CTrigger_BlackHole::SetUp_BillBoard()"));
		return;
	}

	const _float4x4 pCameraWorldMatrix = *pCameraTransformCom->Get_WorldMatrix();


	_float3 vScale = m_pTransform->Get_Scale();

	m_pTransform->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0]) * vScale.x);
	m_pTransform->Set_State(CTransform::STATE_LOOK, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0]) * vScale.z);

}






CTrigger_BlackHole* CTrigger_BlackHole::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTrigger_BlackHole* pInstance = new CTrigger_BlackHole(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Faild to Created : CTrigger_BlackHole"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTrigger_BlackHole::Clone(void* pArg)
{
	CTrigger_BlackHole* pInstance = new CTrigger_BlackHole(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Faild to Cloned : CTrigger_BlackHole"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrigger_BlackHole::Free()
{

	Safe_Release(m_pVIBuffer_Com);
	Safe_Release(m_pTextureCom);

	//콜라이더 실험 BEGIN
	Safe_Release(m_pCollider_Com);
	//콜라이더 실험 END.
	Safe_Release(m_pCamera);

	__super::Free();
}
