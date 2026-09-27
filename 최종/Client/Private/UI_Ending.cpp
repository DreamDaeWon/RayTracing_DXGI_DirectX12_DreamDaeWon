#include "pch.h"
#include "UI_Ending.h"
#include "GameInstance.h"
#include "State.h"

CUI_Ending::CUI_Ending(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CUI_Base(pGraphic_Device)
{
}

CUI_Ending::CUI_Ending(const CUI_Base& rhs)
	:CUI_Base(rhs)
{
}

HRESULT CUI_Ending::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Ending::Initialize(void* pArg)
{


 	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(Add_Components()))
		return E_FAIL;

	D3DXMatrixIdentity(&m_ViewMatrix);

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

	m_fSizeX = 1024.f;
	m_fSizeY = 64.f;
	m_fX = g_iWinSizeX * 0.5f;
	m_fY = g_iWinSizeY * 0.5f;

	m_pTransform->Set_Scale(_float3(m_fSizeX*0.8f, m_fSizeY*0.8f , 1.f));
	m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(
		m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f,
		0.f
	));
	m_vFirstWorldMatrix = *m_pTransform->Get_WorldMatrix();

	
	return S_OK; 
}

_uint CUI_Ending::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down('P')||m_bStart)
	{
		m_bStart = true; //테스트 끝나면 이걸루
		m_fTime += fTimeDelta;
	}
	if (m_fTime > 2.f)
	{
		m_bMove = true;
	}
	if (m_bMove)
	{
		m_fTimeMove += fTimeDelta*300.f;
	}
	if (m_fTimeMove > 2000.f)
	{
		m_bTitle = true;
	}
	if (m_bTitle)
	{
		m_fTimeEnding += fTimeDelta*10.f;
	}
	if (m_fTimeEnding > 29.f)
	{
		m_fTimeEnding = 28.f;
	}
	return OBJECT_NOTHING;
}

void CUI_Ending::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);

}

HRESULT CUI_Ending::Render()
{
	if (!m_bStart)
		return S_OK;

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);


	m_pTextureCom = m_pTextureEnding;
	if (FAILED(Render_Again(512.f*1.3f,2024.f * 1.3f, m_fX, m_fY+ 2024.f * 1.2f*0.5f - m_fTimeMove, 0, 0)))
		return E_FAIL;
	Reset_First_State();
	
	if (m_bTitle)
	{
		m_pTextureCom = m_pTextureLogo;
		if (FAILED(Render_Again(g_iWinSizeX, g_iWinSizeY, m_fX, m_fY, (_uint)m_fTimeEnding, 0)))
			return E_FAIL;
		Reset_First_State();
	}



	if (FAILED(Reset_RenderState()))
		return E_FAIL;


	return S_OK;
}


HRESULT CUI_Ending::Render_Again(_float fSizeX, _float fSizeY, _float fX, _float fY, _uint iFrame, _uint iRenderState)
{
	//m_pTransform->Turn(_float3(1.f, 1.f, 0.f), 0.001f);
	m_pTransform->Set_Scale(_float3(fSizeX, fSizeY, 1.f));

	m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(
		fX - g_iWinSizeX * 0.5f,
		-fY + g_iWinSizeY * 0.5f,
		0.f
	));

	if (FAILED(m_pTransform->Bind_WorldMatrix()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture(0, (_uint)iFrame)))
		return E_FAIL;


	if (FAILED(Set_RenderState(iRenderState)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	return S_OK;
}

void CUI_Ending::Reset_First_State()
{
	m_pTransform->Set_WorldMatrix(m_vFirstWorldMatrix);
}

HRESULT CUI_Ending::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_Ending"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureEnding)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_Ending_Logo"),
		TEXT("Com_Texture1"), (CComponent**)&m_pTextureLogo)))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_Ending::Set_RenderState(_ulong lAphaRef)
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, lAphaRef);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	return S_OK;
}

HRESULT CUI_Ending::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	return S_OK;
}

CUI_Ending* CUI_Ending::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_Ending* pInstance = new CUI_Ending(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Ending"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Ending::Clone(void* pArg)
{
	CUI_Ending* pInstance = new CUI_Ending(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Ending"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Ending::Free()
{
	m_pTextureCom = nullptr;
	Safe_Release(m_pTextureEnding);
	Safe_Release(m_pTextureLogo);
	__super::Free();
}
