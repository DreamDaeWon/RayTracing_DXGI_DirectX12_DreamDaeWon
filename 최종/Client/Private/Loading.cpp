#include "pch.h"
#include "Loading.h"
#include "GameInstance.h"
#include"Loader.h"
CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CGameObject(pGraphic_Device)
{
}

CLoading::CLoading(const CLoading& rhs) :
	CGameObject(rhs)
{
}

HRESULT CLoading::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CLoading"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoading::Initialize(void* pArg)
{
	LOADING_DESC pBackGroundDecs = {};
	pBackGroundDecs.fSpeedPerSec = 5.f;
	pBackGroundDecs.fRotationPerSec = D3DXToRadian(90.f);
	if (FAILED(__super::Initialize(&pBackGroundDecs)))
	{
		MSG_BOX(TEXT("Failed to Initialize : __super,CLoading"));
		return E_FAIL;
	}
	if (pArg != nullptr)
	{
		LOADING_DESC* pLoading_desc = (LOADING_DESC*)pArg;
		m_pLoader = pLoading_desc->pLoader;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX(TEXT("Failed to Add_Components : __super,CLoading"));
		return E_FAIL;
	}


	D3DXMatrixIdentity(&m_ViewMatrix);

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);


	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;
	m_fX = g_iWinSizeX * 0.5f ;
	m_fY = g_iWinSizeY * 0.5f;

	m_pTransform->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(
		m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f,
		0.f
	));
	m_vFirstWorldMatrix = *m_pTransform->Get_WorldMatrix();

	return S_OK;
}

_uint CLoading::Tick(_float fTimeDelta)
{

	m_fTime += fTimeDelta;
	m_fTexture += fTimeDelta*6.f;
	if (m_fTexture > 63.f)
		m_fTexture = 16.f;



	__super::Tick(fTimeDelta);
	return 0;
}

void CLoading::Late_Tick(_float fTimeDelta)
{

	__super::Late_Tick(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);
}

HRESULT CLoading::Render()
{

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);


	m_pTextureCom = m_pTextureBackCom;
	Render_Again(m_fSizeX,m_fSizeY,m_fX,m_fY,0);

	//m_pTextureCom = m_pTextureBarCom;
	//Render_Again(278.f,16.f,m_fX,m_fY+250.f,0);

	TCHAR	szBuf[256] = L"LOADING";
	m_pGameInstance->Render_Font(TEXT("Font_Default"), szBuf, &_float2(m_fX - 550.f, m_fY + 110.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	m_pTextureCom = m_pTextureGattoCom;
	Render_Again(128.f, 128.f,m_fX-500.f,m_fY+250.f,(_uint)m_fTexture);



	return S_OK;
}

HRESULT CLoading::Render_Again(_float fSizeX, _float fSizeY, _float fX, _float fY, _uint iFrame)
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
	if (FAILED(Set_RenderState(10.f)))
		return E_FAIL;
	if (FAILED(m_pVIBuffer_Com->Render()))
		return E_FAIL;
	if (FAILED(Reset_RenderState()))
		return E_FAIL;
	Reset_First_State();

	return S_OK;
}

HRESULT CLoading::Add_Components()
{	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Rect : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Back"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureBackCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Logo : Add_Components"));
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Move"), TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureGattoCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Logo : Add_Components"));
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Bar"), TEXT("Com_Texture2"), reinterpret_cast<CComponent**>(&m_pTextureBarCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Logo : Add_Components"));
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CLoading::Set_RenderState(_ulong lAphaRef)
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

HRESULT CLoading::Reset_RenderState()
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

void CLoading::Reset_First_State()
{
	m_pTransform->Set_WorldMatrix(m_vFirstWorldMatrix);

}

CGameObject* CLoading::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLoading* pInstance = new CLoading(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLoading"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLoading::Clone(void* pArg)
{
	CLoading* pInstance = new CLoading(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Faild to Cloned : CLoading"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoading::Free()
{
	m_pTextureCom = nullptr;
	Safe_Release(m_pVIBuffer_Com);
	Safe_Release(m_pTextureBackCom);
	Safe_Release(m_pTextureGattoCom);
	Safe_Release(m_pTextureBarCom);


	__super::Free();
}
