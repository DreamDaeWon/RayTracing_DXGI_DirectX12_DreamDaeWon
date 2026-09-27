#include "pch.h"
#include "Sky.h"
#include "GameInstance.h"

CSky::CSky(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CGameObject(pGraphic_Device)
{
}

CSky::CSky(const CSky& rhs) :
	CGameObject(rhs)
{
}

HRESULT CSky::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CSky"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSky::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Initialize : __super,CSky"));
		return E_FAIL;
	}
	if (nullptr != pArg)
	{
		SKY_DESC* sky_desc = (SKY_DESC * )pArg;
		if (FAILED(Add_Components(sky_desc->iLevel)))
		{
			MSG_BOX(TEXT("Failed to Add_Components : __super,CSky"));
			return E_FAIL;
		}
	}
	else
	{
		if (FAILED(Add_Components()))
		{
			MSG_BOX(TEXT("Failed to Add_Components : __super,CSky"));
			return E_FAIL;
		}
	}	

	return S_OK;
}

_uint CSky::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	return OBJECT_NOTHING;
}

void CSky::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Follow_Camera();

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CSky::Render()
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

HRESULT CSky::Add_Components()
{
	//if (LEVEL_LOADING == m_pGameInstance->Get_Level())
	//{
	//	/* For.Com_VIBuffer */
	//	if (FAILED(__super::Add_Component(LEVEL_TUTORIAL, TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	//	{
	//		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Cube : Add_Components"));
	//		return E_FAIL;
	//	}

	//	/* For.Com_Texture */
	//	if (FAILED(__super::Add_Component(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_Sky"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	//	{
	//		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Sky : Add_Components"));
	//		return E_FAIL;
	//	}
	//}
	//else if (LEVEL_LOGO == m_pGameInstance->Get_Level())
	//{
	//	/* For.Com_VIBuffer */
	//	if (FAILED(__super::Add_Component(LEVEL_SNOWBOSS, TEXT("Prototype_Component_VIBuffer_Cube_Boss"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	//	{
	//		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Cube : Add_Components"));
	//		return E_FAIL;
	//	}

	//	/* For.Com_Texture */
	//	if (FAILED(__super::Add_Component(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_Sky_Boss"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	//	{
	//		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Sky : Add_Components"));
	//		return E_FAIL;
	//	}
	//}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube_Boss"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Cube : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sky_Boss"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Sky : Add_Components"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSky::Add_Components(_uint iLevel)
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Cube : Add_Components"));
		return E_FAIL;
	}
	if (LEVEL_TUTORIAL == iLevel)
	{
		
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(iLevel, TEXT("Prototype_Component_Texture_Sky"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		{
			MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Sky : Add_Components"));
			return E_FAIL;
		}
	}
	else if (LEVEL_NORMAL1 == iLevel)
	{
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(iLevel, TEXT("Prototype_Component_Texture_Sky_Normal1"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		{
			MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Sky : Add_Components"));
			return E_FAIL;
		}
	}
	else if (LEVEL_1945 == iLevel)
	{
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(iLevel, TEXT("Prototype_Component_Texture_Sky_1945"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		{
			MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Sky : Add_Components"));
			return E_FAIL;
		}
	}
	else if (LEVEL_NORMAL2 == iLevel)
	{
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(iLevel, TEXT("Prototype_Component_Texture_Sky_Normal2"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		{
			MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Sky : Add_Components"));
			return E_FAIL;
		}
	}
	else if (LEVEL_SNOWBOSS == iLevel)
	{
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(iLevel, TEXT("Prototype_Component_Texture_Sky_Boss"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		{
			MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Sky : Add_Components"));
			return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CSky::Set_RenderState()
{
	if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DCULL_CW"));
		return E_FAIL;
	}
	if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, FALSE)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ZENABLE, F"));
		return E_FAIL;
	}
	if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ZWRITEENABLE, F"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSky::Reset_RenderState()
{
	if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DCULL_CCW"));
		return E_FAIL;
	}
	if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, TRUE)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ZENABLE, T"));
		return E_FAIL;
	}
	if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ZWRITEENABLE, T"));
		return E_FAIL;
	}
	return S_OK;
}

void CSky::Follow_Camera()
{
	_float4x4 ViewMatrix;
	if (m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix))
	{
		MSG_BOX(TEXT("Failed to GetTransform :  CSky::Follow_Camera()"));
		return;
	}
	ViewMatrix = *D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	m_pTransform->Set_State(CTransform::STATE_POSITION, *(_float3*)&ViewMatrix.m[3][0]);
}

CGameObject* CSky::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSky* pInstance = new CSky(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Faild to Created : CSky"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSky::Clone(void* pArg)
{
	CSky* pInstance = new CSky(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Faild to Cloned : CSky"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSky::Free()
{
	Safe_Release(m_pVIBuffer_Com);
	Safe_Release(m_pTextureCom);

	__super::Free();
}
