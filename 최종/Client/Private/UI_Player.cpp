#include "pch.h"
#include "UI_Player.h"
#include "GameInstance.h"

#include"State.h"

CUI_Player::CUI_Player(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CUI_Base(pGraphic_Device)
{
}

CUI_Player::CUI_Player(const CUI_Base& rhs)
	:CUI_Base(rhs),
	m_bGattoHeart(false),
	m_bGoneHeart1(false),
	m_bGoneHeart2(false),
	m_bGoneHeart3(false),
	m_bEnd(false),
	m_fHeartFrame(12.f),
	m_fHeartDeadFrame(0.f),
	m_fGattoFaceFrame(0.f)
{
}

HRESULT CUI_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Player::Initialize(void* pArg)
{



 	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//TODO: 예은 추가 - 반드시 받아야함 => 안받으면 게임 UI의 이유가...
	if (pArg == nullptr)
		return E_FAIL;
	UI_PLAYER_DESC* PD = (UI_PLAYER_DESC*)pArg;
	m_pPlayerStateCom = PD->pPlayerStateCom;
	Safe_AddRef(m_pPlayerStateCom);


	if (FAILED(Add_Components()))
		return E_FAIL;

	D3DXMatrixIdentity(&m_ViewMatrix);

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);


	m_fSizeX = 512.f;
	m_fSizeY = 512.f;
	m_fX = 370.f;
	m_fY = 100.f;


	m_fHeartSizeX = 128.f*0.9f;
	m_fHeartSizeY = 128.f * 0.9f;



	m_pTransform->Set_Scale(_float3(m_fSizeX, m_fSizeY , 1.f));
	m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(
		m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f,
		0.f
	));

	return S_OK; 
}

_uint CUI_Player::Tick(_float fTimeDelta)
{
	return OBJECT_NOTHING;
}

void CUI_Player::Late_Tick(_float fTimeDelta)
{
	if (!m_bEnd)
	{
		m_fHeartFrame += 8.f * fTimeDelta;
		m_fHeartDeadFrame += 12.f * fTimeDelta * 1.2f;
		m_fGattoFaceFrame += 8.f * fTimeDelta*0.5f;
	}
	else
	{
		m_fHeartFrame = 12.f;
		m_fHeartDeadFrame = 0.f;
		m_fGattoFaceFrame = 0.f;
	}

	if (m_fHeartFrame > 19.f)
		m_fHeartFrame = 12.f;

	if (m_fHeartDeadFrame > 20.f)
		m_fHeartDeadFrame = 20.f;
	else if (m_fHeartDeadFrame > 12.f)
		m_fHeartDeadFrame = 0.f;



	if (m_fGattoFaceFrame > 8.f)
		m_fGattoFaceFrame = 0.f;



	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);

}

HRESULT CUI_Player::Render()
{

	if (!g_UI)
		return S_OK;


	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	//하트 - 배경
	m_pTransform->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	if (FAILED(Render_Again(m_fX,m_fY,0,20, UI_HEARTBACK)))
		return E_FAIL;
	// 가토 - 배경
	m_pTransform->Set_Scale(_float3(256.f*0.9f, 256.f * 1.2f * 0.9f, 1.f));
	if (FAILED(Render_Again(m_fX-250.f+5.f, m_fY, 0, 20, UI_GATTOBACK)))
		return E_FAIL;



	//하트
	m_pTransform->Set_Scale(_float3(m_fHeartSizeX, m_fHeartSizeY, 1.f));
	if (m_pPlayerStateCom->Get_State(CState::STATE_HP) >= 1.f)
	{
		//하트 1
		if (FAILED(Render_Again(m_fX - 100.f, m_fY, (_uint)m_fHeartFrame, 20, UI_HEART)))
			return E_FAIL;
		m_bGoneHeart1 = false;

		if (m_pPlayerStateCom->Get_State(CState::STATE_HP) >= 2.f)
		{
			//하트 2
			
			if (FAILED(Render_Again(m_fX - 50.f, m_fY, (_uint)m_fHeartFrame, 20, UI_HEART)))
				return E_FAIL;
			m_bGoneHeart2 = false;

			if (m_pPlayerStateCom->Get_State(CState::STATE_HP) >= 3.f)
			{
				//하트3
				if (FAILED(Render_Again(m_fX, m_fY, (_uint)m_fHeartFrame, 20, UI_HEART)))
					return E_FAIL;
				m_bGoneHeart3 = false;
				m_bGattoHeart = false;
			}
			else if(!m_bGoneHeart3)
			{
				m_fHeartDeadFrame = 0.f;
				m_bGoneHeart3 = true;
				m_bGattoHeart = true;
			}
			if (m_bGoneHeart3)
			{
				if (FAILED(Render_Again(m_fX, m_fY, (_uint)m_fHeartDeadFrame, 20, UI_HEART)))
					return E_FAIL;
				if (m_fHeartDeadFrame > 11.f)
				{
					m_bGattoHeart = false;
					m_fHeartDeadFrame = 20.f;
				}

			}
		}
		else if (!m_bGoneHeart2)
		{
			m_fHeartDeadFrame = 0.f;
			m_bGoneHeart2 = true;
			m_bGattoHeart = true;

		}

		if (m_bGoneHeart2)
		{
			if (FAILED(Render_Again(m_fX - 50.f, m_fY, (_uint)m_fHeartDeadFrame, 20, UI_HEART)))
				return E_FAIL;
			if (m_fHeartDeadFrame > 11.f)
			{
				m_bGattoHeart = false;
				m_fHeartDeadFrame = 20.f;
			}

		}
		
	}
	else if (!m_bGoneHeart1)
	{
		m_fHeartDeadFrame = 0.f;
		m_bGoneHeart1 = true;
		m_bGattoHeart = true;
	}
	if (m_bGoneHeart1)
	{
		if (FAILED(Render_Again(m_fX-100.f, m_fY, (_uint)m_fHeartDeadFrame, 20, UI_HEART)))
			return E_FAIL;

		if (m_fHeartDeadFrame > 11.f)
		{
			m_fHeartDeadFrame = 20.f;
			m_bEnd = true;
		}

	}

	if(m_bGattoHeart == true)
 		m_fGattoFaceFrame = 8.f;


	//가토 얼굴
	m_pTransform->Set_Scale(_float3(256.f*0.7f, 256.f * 0.7f, 1.f));
	if (FAILED(Render_Again(m_fX - 250.f, m_fY-8.f, (_uint)m_fGattoFaceFrame, 20, UI_GATTOFACE)))
		return E_FAIL;



	if (FAILED(Reset_RenderState()))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_Player::Render_Again(_float fX, _float fY, _uint iFrame, _uint iRenderState, UI e_UIId)
{

	m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(
		fX - g_iWinSizeX * 0.5f,
		-fY + g_iWinSizeY * 0.5f,
		0.f
	));
	if (FAILED(m_pTransform->Bind_WorldMatrix()))
		return E_FAIL;
	switch (e_UIId)
	{
	case UI_HEARTBACK:
		if (FAILED(m_pTextureCom->Bind_Texture(0, (_uint)iFrame)))
			return E_FAIL;
		break;
	case UI_HEART:
		if (FAILED(m_pTextureHeartCom->Bind_Texture(0, (_uint)iFrame)))
			return E_FAIL;
		break;
	case UI_GATTOBACK:
		if (FAILED(m_pTextureGattoBackCom->Bind_Texture(0, (_uint)iFrame)))
			return E_FAIL;
		break;
	case UI_GATTOFACE:
		if (FAILED(m_pTextureGattoFaceCom->Bind_Texture(0, (_uint)iFrame)))
			return E_FAIL;
		break;
	case UI_END:
		break;
	}

	if (FAILED(Set_RenderState(iRenderState)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Player::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_UI"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	//TODO: 예은 추가

	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_Hp_UI"),
		TEXT("Com_Texture1"), (CComponent**)&m_pTextureHeartCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_Hp_Gatto_Back"),
		TEXT("Com_Texture2"), (CComponent**)&m_pTextureGattoBackCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_Hp_Gatto"),
		TEXT("Com_Texture3"), (CComponent**)&m_pTextureGattoFaceCom)))
		return E_FAIL;



	return S_OK;
}

HRESULT CUI_Player::Set_RenderState(_ulong lAphaRef)
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

HRESULT CUI_Player::Reset_RenderState()
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

CUI_Player* CUI_Player::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_Player* pInstance = new CUI_Player(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Player"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Player::Clone(void* pArg)
{
	CUI_Player* pInstance = new CUI_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Player"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Player::Free()
{
	__super::Free();

	Safe_Release(m_pTextureHeartCom);
	Safe_Release(m_pTextureGattoBackCom);
	Safe_Release(m_pTextureGattoFaceCom);
	Safe_Release(m_pPlayerStateCom); 

}
