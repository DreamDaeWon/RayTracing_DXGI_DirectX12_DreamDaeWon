#include "pch.h"
#include "Effect_Orange_Circle.h"
#include "GameInstance.h"
#include "Player.h"

CEffect_Orange_Circle::CEffect_Orange_Circle(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CEffect_Base(pGraphic_Device)
{
}

CEffect_Orange_Circle::CEffect_Orange_Circle(const CEffect_Orange_Circle& rhs) :
	CEffect_Base(rhs)
{
}

HRESULT CEffect_Orange_Circle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CEffect_Orange_Circle"));
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CEffect_Orange_Circle::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Initialize : CEffect_Orange_Circle"));
		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX(TEXT("Failed to Add_Components ,CEffect_Orange_Circle"));
		return E_FAIL;
	}

	m_pTransform->Set_Scale(_float3(0.2f, 0.2f, 0.2f));

	return S_OK;
}

_uint CEffect_Orange_Circle::Tick(_float fTimeDelta)
{
	if (0.f <= m_fLifeTime)
		m_fLifeTime -= fTimeDelta;

	m_pTransform->Set_Scale(_float3(EFFECT_SPLAT_ORANGE_CIRCLE_LIFE_TIME - m_fLifeTime, EFFECT_SPLAT_ORANGE_CIRCLE_LIFE_TIME - m_fLifeTime, 0.2f));

	m_fFrame += fTimeDelta * m_fMaxFrame / EFFECT_SPLAT_ORANGE_CIRCLE_LIFE_TIME; // 이렇게 해야 하나의 모션이 정확히 다 보임
	if ((_int)m_fFrame > (_int)m_fMaxFrame)
		m_fFrame = 0.f;

	//if (m_pGameInstance->Key_Pressing(VK_RBUTTON) && m_pGameInstance->Key_Down(VK_LBUTTON))
	//	m_fLifeTime = EFFECT_SPLAT_LIFE_TIME;


	__super::Tick(fTimeDelta);

	return OBJECT_NOTHING;
}

void CEffect_Orange_Circle::Late_Tick(_float fTimeDelta)
{
	if (m_fLifeTime < 0.f)
		return;
	//======임시 플레이어 바인딩
	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), g_strTransformTag));

	m_pTransform->Set_State(CTransform::STATE_LOOK, pPlayerTransform->Get_State(CTransform::STATE_LOOK));
	m_pTransform->Set_State(CTransform::STATE_UP, pPlayerTransform->Get_State(CTransform::STATE_UP));
	m_pTransform->Set_State(CTransform::STATE_RIGHT, pPlayerTransform->Get_State(CTransform::STATE_RIGHT));
	m_pTransform->Set_State(CTransform::STATE_POSITION, pPlayerTransform->Get_State(CTransform::STATE_POSITION));

	m_pTransform->Set_Scale(_float3((EFFECT_SPLAT_ORANGE_CIRCLE_LIFE_TIME - m_fLifeTime) * m_fScale, (EFFECT_SPLAT_ORANGE_CIRCLE_LIFE_TIME - m_fLifeTime) * m_fScale, 0.2f));

	SetUp_BillBoard();
	m_pTransform->Go_Right(0.125f);
	m_pTransform->Go_Up(0.01f);
	m_pTransform->Go_Straight(0.005f);

	//======================
	__super::Late_Tick(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CEffect_Orange_Circle::Render()
{
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

HRESULT CEffect_Orange_Circle::Add_Components()
{

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Rect : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Splat_Orange_Circle"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Effect_Splat_Orange_Circle : Add_Components"));
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CEffect_Orange_Circle::Set_RenderState()
{

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	return S_OK;
}

HRESULT CEffect_Orange_Circle::Reset_RenderState()
{
	if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHATESTENABLE"));
		return E_FAIL;
	}
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	return S_OK;
}

void CEffect_Orange_Circle::SetUp_BillBoard()
{
	//CTransform* pCameraTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Camera_Free"), g_strTransformTag));
	CTransform* pCameraTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera_Player"), g_strTransformTag));

	if (nullptr == pCameraTransformCom)
	{
		MSG_BOX(TEXT("nullptr == pCameraTransformCom : CMonster::SetUp_BillBoard()"));
		return;
	}

	const _float4x4 pCameraWorldMatrix = *pCameraTransformCom->Get_WorldMatrix();


	_float3 vScale = m_pTransform->Get_Scale();

	m_pTransform->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0]) * vScale.x);
	m_pTransform->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_UP][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_UP][0]) * vScale.y);
	m_pTransform->Set_State(CTransform::STATE_LOOK, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0]) * vScale.z);

	/*m_pTransform->Set_State(CTransform::STATE_RIGHT, *(_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0]);
	m_pTransform->Set_State(CTransform::STATE_LOOK, *(_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0]);*/
}

CEffect_Orange_Circle* CEffect_Orange_Circle::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_Orange_Circle* pInstance = new CEffect_Orange_Circle(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEffect_Orange_Circle"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Orange_Circle::Clone(void* pArg)
{
	CEffect_Orange_Circle* pInstance = new CEffect_Orange_Circle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEffect_Orange_Circle"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Orange_Circle::Free()
{
	__super::Free();
}
