#include "pch.h"
#include "UI_Sparrow.h"
#include "GameInstance.h"
#include"Player.h"
#include"State.h"

CUI_Sparrow::CUI_Sparrow(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CUI_Base(pGraphic_Device)
{
}

CUI_Sparrow::CUI_Sparrow(const CUI_Base& rhs)
	:CUI_Base(rhs)
{
}

HRESULT CUI_Sparrow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Sparrow::Initialize(void* pArg)
{

	CTransform::TRANSFORM_DESC transform = {};
	transform.fRotationPerSec = D3DXToRadian(10.f);


 	if (FAILED(__super::Initialize(&transform)))
		return E_FAIL;

	

	if (FAILED(Add_Components()))
		return E_FAIL;

	D3DXMatrixIdentity(&m_ViewMatrix);

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);


	m_fSizeX = 1024.f;
	m_fSizeY = 1024.f;
	m_fX = g_iWinSizeX * 0.5f;
	m_fY = g_iWinSizeY * 0.5f;

	m_pTransform->Set_Scale(_float3(m_fSizeX, m_fSizeY , 1.f));
	m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(
		m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f,
		0.f
	));
	m_vFirstWorldMatrix = *m_pTransform->Get_WorldMatrix();
	return S_OK; 
}

_uint CUI_Sparrow::Tick(_float fTimeDelta)
{
	if (dynamic_cast<CPlayer*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Get_CurState() == CPlayer::STATE_AIMING_ZOOM)
		m_fTime += fTimeDelta * 5.f;
	else
	{
		m_fTime = 0.f;
		m_bOnce = false;
	}
	if (m_fTime > 7.f)
	{
		m_fTime = 3.f;
		m_bOnce = true;
	}
	return OBJECT_NOTHING;
}

void CUI_Sparrow::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);

}

HRESULT CUI_Sparrow::Render()
{

	if (!g_UI)
		return S_OK;

	if (m_fTime == 0.f)
		return S_OK;

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	m_pTextureCom = m_pTextureZoomUI1;
	if (FAILED(Render_Again(m_fSizeX * 1.5f, m_fSizeY * 1.5f, m_fX, m_fY, m_fTime, 0)))
		return E_FAIL;
	m_pTextureCom = m_pTextureZoomUI0;
	if (FAILED(Render_Again(m_fSizeX , m_fSizeY ,m_fX, m_fY, m_fTime, 0)))
		return E_FAIL;
	if (m_fTime > 1.f)
	{
		m_pTextureCom = m_pTextureZoomUI2;
		if (FAILED(Render_Again(256.f, 64.f, m_fX, m_fY + 170.f, m_fTime, 0)))
			return E_FAIL;
	}
	if (m_fTime > 1.6f)
	{
		m_pTextureCom = m_pTextureZoomUI3;
		if (FAILED(Render_Again(256.f, 64.f, m_fX, m_fY + 120.f, m_fTime, 0)))
			return E_FAIL;
	}
	if (m_fTime > 2.1f)
	{
		m_pTextureCom = m_pTextureZoomUI4;
		if (FAILED(Render_Again(128.f, 64.f, m_fX, m_fY + 90.f, m_fTime, 0)))
			return E_FAIL;
	}

	if (!m_bOnce)
	{
		m_pTextureCom = m_pTextureZoomUI5;
		m_pTransform->Turn(_float3(0.f, 0.f, 1.f), m_fTime);
		if (FAILED(Render_Again(m_fSizeX * 0.5f, m_fSizeY * 0.5f, m_fX, m_fY, m_fTime, 0)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Sparrow::Render_Again(_float fSizeX, _float fSizeY,_float fX, _float fY, _uint iFrame, _uint iRenderState)
{
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

	if (FAILED(Reset_RenderState()))
		return E_FAIL;

	if (FAILED(Reset_First_State()))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_Sparrow::Reset_First_State()
{

	m_pTransform->Set_WorldMatrix(m_vFirstWorldMatrix);
	return S_OK;
}

HRESULT CUI_Sparrow::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_UI_Sparrow0"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureZoomUI0)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_UI_Sparrow1"),
		TEXT("Com_Texture1"), (CComponent**)&m_pTextureZoomUI1)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_UI_Sparrow2"),
		TEXT("Com_Texture2"), (CComponent**)&m_pTextureZoomUI2)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_UI_Sparrow3"),
		TEXT("Com_Texture3"), (CComponent**)&m_pTextureZoomUI3)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_UI_Sparrow4"),
		TEXT("Com_Texture4"), (CComponent**)&m_pTextureZoomUI4)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_UI_Sparrow5"),
		TEXT("Com_Texture5"), (CComponent**)&m_pTextureZoomUI5)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Sparrow::Set_RenderState(_ulong lAphaRef)
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

HRESULT CUI_Sparrow::Reset_RenderState()
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

CUI_Sparrow* CUI_Sparrow::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_Sparrow* pInstance = new CUI_Sparrow(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Sparrow"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Sparrow::Clone(void* pArg)
{
	CUI_Sparrow* pInstance = new CUI_Sparrow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Sparrow"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Sparrow::Free()
{
	m_pTextureCom = nullptr;
	Safe_Release(m_pTextureZoomUI0);
	Safe_Release(m_pTextureZoomUI1);
	Safe_Release(m_pTextureZoomUI2);
	Safe_Release(m_pTextureZoomUI3);
	Safe_Release(m_pTextureZoomUI4);
	Safe_Release(m_pTextureZoomUI5);

	__super::Free();
}
