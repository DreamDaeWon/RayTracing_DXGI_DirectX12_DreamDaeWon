#include "pch.h"
#include "Splat_Default.h"
#include "GameInstance.h"
#include "Player.h"

CSplat_Default::CSplat_Default(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CEffect_Base(pGraphic_Device)
{
}

CSplat_Default::CSplat_Default(const CSplat_Default& rhs) :
	CEffect_Base(rhs)
{
}

HRESULT CSplat_Default::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CSplat_Default"));
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSplat_Default::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Initialize : CSplat_Default"));
		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX(TEXT("Failed to Add_Components ,CSplat_Default"));
		return E_FAIL;
	}

	m_pTransform->Set_Scale(_float3(0.2f, 0.2f, 0.2f));

	return S_OK;
}

_uint CSplat_Default::Tick(_float fTimeDelta)
{
	if (0.f <= m_fLifeTime)
		m_fLifeTime -= fTimeDelta;

	m_fFrame += fTimeDelta * m_fMaxFrame / EFFECT_SPLAT_DEFAULT_LIFE_TIME; // 이렇게 해야 하나의 모션이 정확히 다 보임
	if ((_int)m_fFrame > (_int)m_fMaxFrame)
		m_fFrame = 0.f;

	//if (m_pGameInstance->Key_Pressing(VK_RBUTTON) && m_pGameInstance->Key_Down(VK_LBUTTON))
	//	m_fLifeTime = EFFECT_SPLAT_LIFE_TIME;


	__super::Tick(fTimeDelta);

	return OBJECT_NOTHING;
}

void CSplat_Default::Late_Tick(_float fTimeDelta)
{
	if (m_fLifeTime < 0.f)
		return;
	//======임시 플레이어 바인딩
	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), g_strTransformTag));

	m_pTransform->Set_State(CTransform::STATE_LOOK, pPlayerTransform->Get_State(CTransform::STATE_LOOK));
	m_pTransform->Set_State(CTransform::STATE_UP, pPlayerTransform->Get_State(CTransform::STATE_UP));
	m_pTransform->Set_State(CTransform::STATE_RIGHT, pPlayerTransform->Get_State(CTransform::STATE_RIGHT));
	m_pTransform->Set_State(CTransform::STATE_POSITION, pPlayerTransform->Get_State(CTransform::STATE_POSITION));

	m_pTransform->Set_Scale(_float3(0.3f, 0.3f, 0.2f));

	SetUp_BillBoard();
	m_pTransform->Go_Right(0.097f);
	m_pTransform->Go_Up(0.02300054995135f); // 초갈의 작품 우히히~
	m_pTransform->Go_Straight(0.005f);

	//======================
	__super::Late_Tick(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CSplat_Default::Render()
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

HRESULT CSplat_Default::Add_Components()
{

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Rect : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Splat_Default"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Effect_Splat_Default : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Texture */
	/*if (FAILED(__super::Add_Component(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_Effect_Splat_Default"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Effect_Splat_Default : Add_Components"));
		return E_FAIL;
	}*/
	return S_OK;
}

HRESULT CSplat_Default::Set_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	return S_OK;
}

HRESULT CSplat_Default::Reset_RenderState()
{
	if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHATESTENABLE"));
		return E_FAIL;
	}
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	return S_OK;
}

void CSplat_Default::SetUp_BillBoard()
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
	m_pTransform->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_UP][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_UP][0]) * vScale.y);
	m_pTransform->Set_State(CTransform::STATE_LOOK, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0]) * vScale.z);

	/*m_pTransform->Set_State(CTransform::STATE_RIGHT, *(_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0]);
	m_pTransform->Set_State(CTransform::STATE_LOOK, *(_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0]);*/
}

CSplat_Default* CSplat_Default::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSplat_Default* pInstance = new CSplat_Default(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSplat_Default"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSplat_Default::Clone(void* pArg)
{
	CSplat_Default* pInstance = new CSplat_Default(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CSplat_Default"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSplat_Default::Free()
{
	Safe_Release(m_pCameraTransform);
	__super::Free();
}
