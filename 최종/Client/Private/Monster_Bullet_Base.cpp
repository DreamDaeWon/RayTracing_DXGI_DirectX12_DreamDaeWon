#include "pch.h"
#include "Monster_Bullet_Base.h"
#include "GameInstance.h"

CMonster_Bullet_Base::CMonster_Bullet_Base(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CLandObject(pGraphic_Device)
{
}

CMonster_Bullet_Base::CMonster_Bullet_Base(const CMonster_Bullet_Base& rhs) :
	CLandObject(rhs), 
	m_fSpeed(rhs.m_fSpeed),
	m_vBossPos(rhs.m_vBossPos),
	m_fScale(rhs.m_fScale),
	m_fLifeTime(rhs.m_fLifeTime),
	m_iTexNum(rhs.m_iTexNum),
	m_vLook(rhs.m_vLook)
{
}

HRESULT CMonster_Bullet_Base::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CMonsterBullet"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonster_Bullet_Base::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		//정보 받아오기
		MONSTER_BULLET_BASE_DESC* pMonster_Bullet_Base_Desc = (MONSTER_BULLET_BASE_DESC*)pArg;

		m_vBossPos = pMonster_Bullet_Base_Desc->m_vPos;
		m_fScale = pMonster_Bullet_Base_Desc->m_fScale;
		m_fLifeTime = pMonster_Bullet_Base_Desc->m_fLifeTime;
		m_vLook = pMonster_Bullet_Base_Desc->m_vLook;
		m_fSpeed = pMonster_Bullet_Base_Desc->m_fSpeed;

		D3DXVec3Normalize(&m_vLook, &m_vLook);

		// 랜드 오브젝트 정보 받아오기
		LANDOBJECT_DESC* pLandObjectDesc = (LANDOBJECT_DESC*)pArg;
		pLandObjectDesc->fSpeedPerSec = 1.f;
		pLandObjectDesc->fRotationPerSec = D3DXToRadian(90.f);

	}

	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Initialize : CMonster_Bullet_Base"));
		return E_FAIL;
	}

	return S_OK;
}

_uint CMonster_Bullet_Base::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	return OBJECT_NOTHING;
}

void CMonster_Bullet_Base::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CMonster_Bullet_Base::Render()
{
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

void CMonster_Bullet_Base::Return_Look_Position(_float3* pvLook, _float3* pvPosition)
{
	*pvLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
	*pvPosition = m_pTransform->Get_State(CTransform::STATE_POSITION);
}

_float3 CMonster_Bullet_Base::Return_ViewPort_Pos()
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

void CMonster_Bullet_Base::SetUp_BillBoard()
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
		MSG_BOX(TEXT("nullptr == m_pCameraTransformCom : CRed_Monster_Bullet::SetUp_BillBoard()"));
		return;
	}

	_float3 vScale = m_pTransform->Get_Scale();

	if (g_eLevel == LEVEL_1945)
	{
		
		CTransform* m_pCamera1945 = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_1945, TEXT("Layer_Camera"), g_strTransformTag));
		if (nullptr != m_pCamera1945)

		_float3 vScale = m_pTransform->Get_Scale();
		m_pTransform->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize((_float3*)&(m_pCamera1945->Get_State(CTransform::STATE_UP)), (_float3*)&(m_pCamera1945->Get_State(CTransform::STATE_UP) * vScale.y)));
		m_pTransform->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize((_float3*)&(m_pCamera1945->Get_State(CTransform::STATE_RIGHT)), (_float3*)&(m_pCamera1945->Get_State(CTransform::STATE_RIGHT) * vScale.x)));
		m_pTransform->Set_State(CTransform::STATE_LOOK, *D3DXVec3Normalize((_float3*)&(m_pCamera1945->Get_State(CTransform::STATE_LOOK)), (_float3*)&(m_pCamera1945->Get_State(CTransform::STATE_LOOK) * vScale.z)));
		
		return;
	}

	const _float4x4 pCameraWorldMatrix = *m_pCameraTransform->Get_WorldMatrix();


	m_pTransform->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_UP][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_UP][0]) * vScale.y);
	m_pTransform->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0]) * vScale.x);
	m_pTransform->Set_State(CTransform::STATE_LOOK, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0]) * vScale.z);

}

HRESULT CMonster_Bullet_Base::Set_RenderState()
{	
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
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	return S_OK;
}

HRESULT CMonster_Bullet_Base::Reset_RenderState()
{
	if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHATESTENABLE"));
		return E_FAIL;
	}

	return S_OK;
}

void CMonster_Bullet_Base::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBuffer_Com);
	Safe_Release(m_pCollider_Com);
	Safe_Release(m_pCameraTransform);

	__super::Free();
}