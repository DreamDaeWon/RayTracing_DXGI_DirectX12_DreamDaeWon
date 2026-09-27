#include "pch.h"
#include "Effect_Cloud.h"
#include "GameInstance.h"
#include "Player.h"

CEffect_Cloud::CEffect_Cloud(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CEffect_Base(pGraphic_Device)
{
}

CEffect_Cloud::CEffect_Cloud(const CEffect_Cloud& rhs) :
	CEffect_Base(rhs)
{
}

HRESULT CEffect_Cloud::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CEffect_Cloud"));
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CEffect_Cloud::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Initialize : CEffect_Cloud"));
		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX(TEXT("Failed to Add_Components ,CEffect_Cloud"));
		return E_FAIL;
	}

	m_fMaxFrame = 4.f;

	m_pTransform->Set_Scale(_float3(0.3f, 0.3f, 0.3f));
	return S_OK;
}

_uint CEffect_Cloud::Tick(_float fTimeDelta)
{
	if (LEVEL_LOADING == m_pGameInstance->Get_Level()|| LEVEL_1945 == m_pGameInstance->Get_Level())
		return 0;

	if (0.f <= m_fLifeTime)
		m_fLifeTime -= fTimeDelta;

	m_fFrame += fTimeDelta;
	if (m_fFrame > m_fMaxFrame)
		m_fFrame = 0.f;

	//if (m_pGameInstance->Key_Pressing(VK_RBUTTON) && m_pGameInstance->Key_Down(VK_LBUTTON))
	//	m_fLifeTime = EFFECT_SPLAT_LIFE_TIME;
	SetUp_BillBoard();
	_float3 vPos = m_pTransform->Get_State(CTransform::STATE_POSITION);
	vPos.y += 1.5f * fTimeDelta;
	Set_Position(vPos);

	__super::Tick(fTimeDelta);

	return OBJECT_NOTHING;
}

void CEffect_Cloud::Late_Tick(_float fTimeDelta)
{
	if (LEVEL_LOADING == m_pGameInstance->Get_Level() || LEVEL_1945 == m_pGameInstance->Get_Level())
		return ;

	if (m_fLifeTime < 0.f)
		return;
	//======임시 플레이어 바인딩
	/*CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_TUTORIAL, TEXT("Layer_Player"), g_strTransformTag));
	_float3 vPlayerUp = pPlayerTransform->Get_State(CTransform::STATE_UP);
	_float3 vPlayerPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	m_pTransform->Set_State(CTransform::STATE_POSITION, vPlayerPosition - vPlayerUp * 0.4f);
	SetUp_BillBoard();*/
	//======================
	__super::Late_Tick(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
}

HRESULT CEffect_Cloud::Render()
{
	if (LEVEL_LOADING == m_pGameInstance->Get_Level() || LEVEL_1945 == m_pGameInstance->Get_Level())
		return S_OK;

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

HRESULT CEffect_Cloud::Add_Components()
{

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Rect : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Player_Move"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Effect_Player_Move : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Texture */
	/*if (FAILED(__super::Add_Component(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_Effect_Player_Move"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Effect_Player_Move : Add_Components"));
		return E_FAIL;
	}*/
	return S_OK;
}

HRESULT CEffect_Cloud::Set_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 32);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	/*if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE)))
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
	}*/

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	return S_OK;
}

HRESULT CEffect_Cloud::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHATESTENABLE"));
		return E_FAIL;
	}

	/*if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHABLENDENABLE"));
		return E_FAIL;
	}*/

	return S_OK;
}

void CEffect_Cloud::SetUp_BillBoard()
{
	//CTransform* pCameraTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_TUTORIAL, TEXT("Layer_Camera_Free"), g_strTransformTag));
	//CTransform* pCameraTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(g_eLevel, TEXT("Layer_Camera_Player"), g_strTransformTag));

	if (nullptr == m_pCameraTransformCom)
	{
		m_pCameraTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera_Player"), g_strTransformTag));
		if (nullptr != m_pCameraTransformCom)
			Safe_AddRef(m_pCameraTransformCom);
	}


	if (nullptr == m_pCameraTransformCom)
	{
		MSG_BOX(TEXT("nullptr == m_pCameraTransformCom : CEffect_Cloud::SetUp_BillBoard()"));
		return;
	}

	const _float4x4 pCameraWorldMatrix = *m_pCameraTransformCom->Get_WorldMatrix();


	_float3 vScale = m_pTransform->Get_Scale();

	m_pTransform->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0]) * vScale.x);
	//m_pTransform->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_UP][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_UP][0]) * vScale.y);
	m_pTransform->Set_State(CTransform::STATE_LOOK, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0]) * vScale.z);

	/*m_pTransform->Set_State(CTransform::STATE_RIGHT, *(_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0]);
	m_pTransform->Set_State(CTransform::STATE_LOOK, *(_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0]);*/
}

CEffect_Cloud* CEffect_Cloud::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_Cloud* pInstance = new CEffect_Cloud(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEffect_Cloud"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Cloud::Clone(void* pArg)
{
	CEffect_Cloud* pInstance = new CEffect_Cloud(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEffect_Cloud"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Cloud::Free()
{
	Safe_Release(m_pCameraTransformCom);

	__super::Free();
}
