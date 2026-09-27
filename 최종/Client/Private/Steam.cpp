#include "pch.h"
#include "GameInstance.h"
#include "Steam.h"
#include "RedRect.h"

CSteam::CSteam(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLandObject(pGraphic_Device)
{
}

CSteam::CSteam(const CSteam& rhs)
	: CLandObject(rhs)
{
}

HRESULT CSteam::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CSteam"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSteam::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Initialize : __super,CSteam"));
		return E_FAIL;
	}

	if (pArg != nullptr)
	{
		LANDOBJECT_DESC* pLandObjectDesc = (LANDOBJECT_DESC*)pArg;
		pLandObjectDesc->fSpeedPerSec = 1.f;
		pLandObjectDesc->fRotationPerSec = 1.f;
		// 준수 추가
		m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(pLandObjectDesc->vPos.x, pLandObjectDesc->vPos.y + 0.05f, pLandObjectDesc->vPos.z));
		m_iMonsterType = pLandObjectDesc->iMonsterType;
		//m_RedDelayTime = pLandObjectDesc->fdelayTime;
		m_fTerm = pLandObjectDesc->fTerm;
		m_fTime += pLandObjectDesc->fdelayTime;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX(TEXT("Failed to Add_Components : __super,CSteam"));
		return E_FAIL;
	}

	Portent();

	m_pTransform->Turn(_float3(1.f, 0.f, 0.f),D3DXToRadian(90.f));

	return S_OK;
}

_uint CSteam::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_fTime > 0.f)
		m_fTime -= fTimeDelta;
	else
	{
		m_pRedRect->Set_LifeTime();
		m_fTime = m_fTerm;
	}

	return OBJECT_NOTHING;
}

void CSteam::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CSteam::Render()
{
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

HRESULT CSteam::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Rect : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Steam"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Steam : Add_Components"));
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSteam::Set_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 32);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	return S_OK;
}

HRESULT CSteam::Reset_RenderState()
{
	if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHATESTENABLE"));
		return E_FAIL;
	}
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	return S_OK;
}

void CSteam::Portent()
{
	CRedRect::RED_RECT_DESC RedRectDesc = {};

	RedRectDesc.pTerrainTranformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(g_eLevel, TEXT("Layer_Terrain"), g_strTransformTag.c_str()));
	RedRectDesc.pTerrainVIBufferCom = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(g_eLevel, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));
	//RedRectDesc.fdelayTime = 0.f;
	//_float4x4 WorldMatrix = *m_pTransform->Get_WorldMatrix();

	RedRectDesc.vPosition = m_pTransform->Get_State(CTransform::STATE_POSITION);
	RedRectDesc.fdelayTime = m_RedDelayTime;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_NORMAL2, TEXT("Layer_RedRect"), TEXT("Prototype_GameObject_RedRect"), &RedRectDesc)))
	{
		MSG_BOX(TEXT("Failed to Add_Components ,CSteam"));
		return;
	}

	list<CGameObject*>* pList = m_pGameInstance->Get_List(LEVEL_NORMAL2, TEXT("Layer_RedRect"));

	if (nullptr != pList)
	{
		auto iter = pList->end();
		--iter;

		m_pRedRect = dynamic_cast<CRedRect*>((*iter));
		Safe_AddRef(m_pRedRect);
	}
	//m_fTime = 0.f;
}

CSteam* CSteam::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSteam* pInstance = new CSteam(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Faild to Created : CSteam"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSteam::Clone(void* pArg)
{
	CSteam* pInstance = new CSteam(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Faild to Cloned : CSteam"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSteam::Free()
{
	Safe_Release(m_pVIBuffer_Com);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRedRect);

	__super::Free();
}

