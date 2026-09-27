#include "pch.h"
#include "BackGround.h"
#include "GameInstance.h"

CBackGround::CBackGround(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CGameObject(pGraphic_Device)
{
}

CBackGround::CBackGround(const CBackGround& rhs) :
	CGameObject(rhs)
{
}

HRESULT CBackGround::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CBackGround"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBackGround::Initialize(void* pArg)
{
	BACKGROUND_DESC pBackGroundDecs = {};
	pBackGroundDecs.fSpeedPerSec = 5.f;
	pBackGroundDecs.fRotationPerSec = D3DXToRadian(90.f);

	if (FAILED(__super::Initialize(&pBackGroundDecs)))
	{
		MSG_BOX(TEXT("Failed to Initialize : __super,CBackGround"));
		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX(TEXT("Failed to Add_Components : __super,CBackGround"));
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

_uint CBackGround::Tick(_float fTimeDelta)
{
	m_fTime += fTimeDelta;
	__super::Tick(fTimeDelta);
	return 0;
}

void CBackGround::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);
}

HRESULT CBackGround::Render()
{
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);


	m_pTextureCom = m_pTextureTitleCom;
	Render_Again(m_fSizeX,m_fSizeY,m_fX,m_fY,0);
	if (m_fTime > 0.f)
		if ((_uint)(m_fTime * 2.f) % 2 == 0)
		{
			return S_OK;
		}
	m_pTextureCom = m_pTextureTextCom;
	Render_Again(512.f,256.f,m_fX,m_fY+250.f,0);




	return S_OK;
}

HRESULT CBackGround::Render_Again(_float fSizeX, _float fSizeY, _float fX, _float fY, _uint iFrame)
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

HRESULT CBackGround::Add_Components()
{	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Rect : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureTitleCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Logo : Add_Components"));
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo_Text"), TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureTextCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Logo : Add_Components"));
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CBackGround::Set_RenderState(_ulong lAphaRef)
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

HRESULT CBackGround::Reset_RenderState()
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

void CBackGround::Reset_First_State()
{
	m_pTransform->Set_WorldMatrix(m_vFirstWorldMatrix);

}

CGameObject* CBackGround::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBackGround* pInstance = new CBackGround(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBackGround"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBackGround::Clone(void* pArg)
{
	CBackGround* pInstance = new CBackGround(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Faild to Cloned : CBackGround"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBackGround::Free()
{
	m_pTextureCom = nullptr;
	Safe_Release(m_pVIBuffer_Com);
	Safe_Release(m_pTextureTitleCom);
	Safe_Release(m_pTextureTextCom);
	//Safe_Release(m_pTextureCom);

	__super::Free();
}
