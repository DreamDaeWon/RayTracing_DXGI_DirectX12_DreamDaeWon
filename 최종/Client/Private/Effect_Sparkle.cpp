#include "pch.h"
#include "Effect_Sparkle.h"
#include "GameInstance.h"
#include "Player.h"

CEffect_Sparkle::CEffect_Sparkle(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CEffect_Base(pGraphic_Device)
{
}

CEffect_Sparkle::CEffect_Sparkle(const CEffect_Sparkle& rhs) :
	CEffect_Base(rhs)
{
}

HRESULT CEffect_Sparkle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CEffect_Sparkle"));
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CEffect_Sparkle::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Initialize : CEffect_Sparkle"));
		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX(TEXT("Failed to Add_Components ,CEffect_Sparkle"));
		return E_FAIL;
	}

	m_pTransform->Set_Scale(_float3(0.2f, 0.2f, 0.2f));
	return S_OK;
}

_uint CEffect_Sparkle::Tick(_float fTimeDelta)
{
	if (0.f <= m_fLifeTime)
		m_fLifeTime -= fTimeDelta;

	m_fFrame += fTimeDelta;
	if (m_fFrame > m_fMaxFrame)
		m_fFrame = 0.f;

	if (0.1f > m_fLifeTime)
		m_fFrame = 1.f;
	else
		m_fFrame = 0.f;
	//if (m_pGameInstance->Key_Pressing(VK_RBUTTON) && m_pGameInstance->Key_Down(VK_LBUTTON))
	//	m_fLifeTime = EFFECT_SPLAT_LIFE_TIME;


	__super::Tick(fTimeDelta);

	return OBJECT_NOTHING;
}

void CEffect_Sparkle::Late_Tick(_float fTimeDelta)
{
	if (m_fLifeTime < 0.f)
		return;
	//======임시 플레이어 바인딩
	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), g_strTransformTag));
	_float3 vPlayerRight = pPlayerTransform->Get_State(CTransform::STATE_RIGHT);
	_float3 vPlayerUp = pPlayerTransform->Get_State(CTransform::STATE_UP);
	_float3 vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);
	_float3 vPlayerPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	m_pTransform->Set_State(CTransform::STATE_POSITION, vPlayerPosition + vPlayerRight * 0.6f + vPlayerUp * 0.1f - vPlayerLook * 0.02f);
	SetUp_BillBoard();
	m_pTransform->Set_State(CTransform::STATE_RIGHT, -m_pTransform->Get_State(CTransform::STATE_RIGHT));
	//======================
	__super::Late_Tick(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
}

HRESULT CEffect_Sparkle::Render()
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

HRESULT CEffect_Sparkle::Add_Components()
{

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Rect : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Sparkle"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Effect_Sparkle : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Texture */
	/*if (FAILED(__super::Add_Component(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_Effect_Sparkle"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Effect_Sparkle : Add_Components"));
		return E_FAIL;
	}*/
	return S_OK;
}

HRESULT CEffect_Sparkle::Set_RenderState()
{
	
	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	//m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	//m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 20);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	//m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	return S_OK;
}

HRESULT CEffect_Sparkle::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	//m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	//m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	return S_OK;
}

void CEffect_Sparkle::SetUp_BillBoard()
{
	//CTransform* pCameraTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_TUTORIAL, TEXT("Layer_Camera_Free"), g_strTransformTag));
	//CTransform* pCameraTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(g_eLevel, TEXT("Layer_Camera_Player"), g_strTransformTag));

	if (nullptr == m_pCameraTransform)
	{
		m_pCameraTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera_Player"), g_strTransformTag));
		if (nullptr != m_pCameraTransform)
			Safe_AddRef(m_pCameraTransform);
	}

	if (nullptr == m_pCameraTransform)
	{
		MSG_BOX(TEXT("nullptr == m_pCameraTransformCom : CMonster::SetUp_BillBoard()"));
		return;
	}

	const _float4x4 pCameraWorldMatrix = *m_pCameraTransform->Get_WorldMatrix();


	_float3 vScale = m_pTransform->Get_Scale();

	m_pTransform->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0]) * vScale.x);
	//m_pTransform->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_UP][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_UP][0]) * vScale.y);
	m_pTransform->Set_State(CTransform::STATE_LOOK, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0]) * vScale.z);

	/*m_pTransform->Set_State(CTransform::STATE_RIGHT, *(_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0]);
	m_pTransform->Set_State(CTransform::STATE_LOOK, *(_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0]);*/
}

CEffect_Sparkle* CEffect_Sparkle::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_Sparkle* pInstance = new CEffect_Sparkle(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEffect_Sparkle"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Sparkle::Clone(void* pArg)
{
	CEffect_Sparkle* pInstance = new CEffect_Sparkle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEffect_Sparkle"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Sparkle::Free()
{
	Safe_Release(m_pCameraTransform);
	__super::Free();
}
