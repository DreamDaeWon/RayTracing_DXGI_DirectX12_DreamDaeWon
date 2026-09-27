#include "pch.h"
#include "UI_AirPlane.h"
#include "GameInstance.h"
#include"AirPlane.h"
#include"Camera_Player_DW2.h"
#include"State.h"

CUI_AirPlane::CUI_AirPlane(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CUI_Base(pGraphic_Device)
{
}

CUI_AirPlane::CUI_AirPlane(const CUI_Base& rhs)
	:CUI_Base(rhs)
{
}

HRESULT CUI_AirPlane::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_AirPlane::Initialize(void* pArg)
{

 	if (FAILED(__super::Initialize(&pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		UI_AIRPLANE_DESC* AirPlaneDesc = (UI_AIRPLANE_DESC*)pArg;
		m_pPlaneState = AirPlaneDesc->pPlaneState;
		m_pPlane = AirPlaneDesc->pPlane;
	}
	
	if (FAILED(Add_Components()))
		return E_FAIL;

	D3DXMatrixIdentity(&m_ViewMatrix);

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

	m_fMaxBullet = (_float)*m_pPlane->Get_MaxBullet();
	m_fPreBullet = (_float)m_fMaxBullet;
	m_fSizeX =	256.f;
	m_fSizeY = 256.f;
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

_uint CUI_AirPlane::Tick(_float fTimeDelta)
{
	if (m_bMove)
		m_fTime += fTimeDelta;
	if (m_bCome)
		m_fTimeCome += fTimeDelta;
	if (m_bGo)
		m_fTimeGo += fTimeDelta;

	m_fCurBullet =(_float) *m_pPlane->Get_NowBullet();
	m_fGap = m_fPreBullet - m_fCurBullet;
	if (m_fGap > 0.f)
		m_bMove = true;
	else
		m_fPreBullet = m_fCurBullet;
	if (m_fPreBullet - m_fGap * m_fTime < m_fCurBullet)
	{
		m_fPreBullet = m_fCurBullet;
		m_fTime = 0.f;
	}
	m_iCurMode = dynamic_cast<CCamera_Player_DW2*>(m_pGameInstance->Get_Object(LEVEL_1945, TEXT("Layer_Camera")))->Get_Camera_Mode();
	if (m_iCurMode != m_iPreMode && m_iCurMode == CCamera_Player_DW2::CAMERA_PLAYER_RIGHT_VIEW)
	{
		m_bGo = true;
		m_iPreMode = m_iCurMode;
	}
	if (m_iCurMode != m_iPreMode && m_iCurMode == CCamera_Player_DW2::CAMERA_3VIEW)
	{
		m_bCome = true;
		m_iPreMode = m_iCurMode;
	}
	if (m_fTimeCome >= 2.f)
	{
		m_bCome = false;
		m_fTimeCome = 0.f;
	}
	if (m_fTimeGo >= 2.f)
	{
		m_bGo = false;
		m_fTimeGo = 0.f;
	}
	return OBJECT_NOTHING;
}

void CUI_AirPlane::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);

}

HRESULT CUI_AirPlane::Render()
{
	if (!g_UI)
		return S_OK;

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);
	if (m_bGo)
	{
		//哭率
		m_pTextureCom = m_pTextureSideBack;
		if (FAILED(Render_Again(434.f, 1080.f, m_fX-550.f- m_fTimeGo*500.f, m_fY, 0, 0)))
			return E_FAIL;
		//坷弗率
		if (FAILED(Render_Again(434.f, 1080.f, m_fX+550.f+ m_fTimeGo * 500.f, m_fY, 1, 0)))
			return E_FAIL;

		m_pTextureCom = m_pTextureFrame;
		if (FAILED(Render_Again(78.f, 1080.f, m_fX - 350.f- m_fTimeGo * 500.f, m_fY, 0, 0)))
			return E_FAIL;
		if (FAILED(Render_Again(78.f, 1080.f, m_fX + 350.f+ m_fTimeGo * 500.f, m_fY, 1, 0)))
			return E_FAIL;
		
		m_pTextureCom = m_pTextureSideFront;
		if (FAILED(Render_Again(224.f, 256.f, m_fX - 530.f- m_fTimeGo * 500.f, m_fY-225.f, 0, 0)))
			return E_FAIL;
		if (FAILED(Render_Again(224.f, 256.f, m_fX + 530.f+ m_fTimeGo * 500.f, m_fY - 225.f, 1, 0)))
			return E_FAIL;

	}
	else if (m_bCome)
	{
		//哭率
		m_pTextureCom = m_pTextureSideBack;
		if (FAILED(Render_Again(434.f, 1080.f, m_fX - 900.f + m_fTimeCome * 180.f, m_fY, 0, 0)))
			return E_FAIL;
		//坷弗率
		if (FAILED(Render_Again(434.f, 1080.f, m_fX + 900.f - m_fTimeCome * 180.f, m_fY, 1, 0)))
			return E_FAIL;

		m_pTextureCom = m_pTextureFrame;
		if (FAILED(Render_Again(78.f, 1080.f, m_fX - 700.f + m_fTimeCome * 180.f, m_fY, 0, 0)))
			return E_FAIL;
		if (FAILED(Render_Again(78.f, 1080.f, m_fX + 700.f - m_fTimeCome * 180.f, m_fY, 1, 0)))
			return E_FAIL;

		m_pTextureCom = m_pTextureSideFront;
		if (FAILED(Render_Again(224.f, 256.f, m_fX - 880.f + m_fTimeCome * 180.f, m_fY - 225.f, 0, 0)))
			return E_FAIL;
		if (FAILED(Render_Again(224.f, 256.f, m_fX + 880.f - m_fTimeCome * 180.f, m_fY - 225.f, 1, 0)))
			return E_FAIL;
	}
	else if (m_iCurMode == CCamera_Player_DW2::CAMERA_3VIEW)
	{
		//哭率
		m_pTextureCom = m_pTextureSideBack;
		if (FAILED(Render_Again(434.f, 1080.f, m_fX - 550.f, m_fY, 0, 0)))
			return E_FAIL;
		//坷弗率
		if (FAILED(Render_Again(434.f, 1080.f, m_fX + 550.f, m_fY, 1, 0)))
			return E_FAIL;

		m_pTextureCom = m_pTextureFrame;
		if (FAILED(Render_Again(78.f, 1080.f, m_fX - 350.f, m_fY, 0, 0)))
			return E_FAIL;
		if (FAILED(Render_Again(78.f, 1080.f, m_fX + 350.f, m_fY, 1, 0)))
			return E_FAIL;

		m_pTextureCom = m_pTextureSideFront;
		if (FAILED(Render_Again(224.f, 256.f, m_fX - 530.f, m_fY - 225.f, 0, 0)))
			return E_FAIL;
		if (FAILED(Render_Again(224.f, 256.f, m_fX + 530.f, m_fY - 225.f, 1, 0)))
			return E_FAIL;
	}
	else if (m_iCurMode == CCamera_Player_DW2::CAMERA_PLAYER_RIGHT_VIEW)
	{

	}



	m_pTextureCom = m_pTextureHp;
	if(m_pPlaneState->Get_State(CState::STATE_HP)>=1)
		if (FAILED(Render_Again(128.f, 64.f, m_fX-100.f, m_fY + 300.f, 0, 0)))
			return E_FAIL;
	if (m_pPlaneState->Get_State(CState::STATE_HP) >=2)
		if (FAILED(Render_Again(128.f, 64.f, m_fX, m_fY + 300.f, 0, 0)))
			return E_FAIL;
	if (m_pPlaneState->Get_State(CState::STATE_HP) >= 3)
		if (FAILED(Render_Again(128.f, 64.f, m_fX+ 100.f, m_fY + 300.f, 0, 0)))
			return E_FAIL;


	
	m_pTextureCom = m_pTextureMonitor;
	if (FAILED(Render_Again(256.f*1.3f, 256.f * 1.3f, m_fX+520.f, m_fY+300.f, 1, 0)))
		return E_FAIL;
	if (FAILED(Render_Again(256.f * 1.3f, 256.f * 1.3f, m_fX-520.f, m_fY+300.f, 0, 0)))
		return E_FAIL;	
	

	m_pTextureCom = m_pTextureGauge;

	if(*m_pPlane->Get_UsingSkillNum()>=1)
		if (FAILED(Render_Again(106.f * 0.4f, 67.f * 0.37f, m_fX - 532.f, m_fY + 279.f, 3, 0)))
			return E_FAIL;
	if (*m_pPlane->Get_UsingSkillNum() >= 2)
		if (FAILED(Render_Again(106.f * 0.4f, 67.f * 0.37f, m_fX - 507.f, m_fY + 279, 3, 0)))
			return E_FAIL;
	if (*m_pPlane->Get_UsingSkillNum() >= 3)
		if (FAILED(Render_Again(106.f * 0.4f, 67.f * 0.37f, m_fX - 481.f, m_fY + 279.f, 3, 0)))
			return E_FAIL;

	if (FAILED(Render_Again(284.f * 0.5f, 100.f * 0.5f, m_fX - 530.f, m_fY + 290.f, 0, 0)))
		return E_FAIL;


	if (FAILED(Render_Again((m_fPreBullet - m_fGap * m_fTime)* (290.f * 0.5f / m_fMaxBullet), 67.f * 0.5f, m_fX + 490.f + ((m_fMaxBullet-(m_fPreBullet - m_fGap * m_fTime)) * (290.f * 0.5f / m_fMaxBullet) * 0.49f), m_fY + 320.f, 1, 0)))
		return E_FAIL;


	if (FAILED(Render_Again(410.f * 0.5f, 150.f * 0.5f, m_fX + 520.f, m_fY + 300.f, 2, 0)))
		return E_FAIL;




	m_pTextureCom = m_pTextureSkill;
	if (FAILED(Render_Again(256.f*0.5f, 256.f * 0.5f, m_fX + 600.f, m_fY + 300.f, 0, 0)))
		return E_FAIL;
	if (FAILED(Render_Again(256.f * 0.5f, 256.f * 0.5f, m_fX - 600.f, m_fY + 300.f, 1, 0)))
		return E_FAIL;
	if (FAILED(Render_Again(256.f * 0.4f, 256.f * 0.4f, m_fX - 540.f, m_fY + 330.f, 2, 0)))
		return E_FAIL;
	if (0>=*m_pPlane->Get_NowBullet())
	{
		if (FAILED(Render_Again(256.f * 0.5f, 256.f * 0.5f, m_fX + 600.f, m_fY + 300.f, 3, 0)))
			return E_FAIL;
	}
	if ( m_pPlane->Get_Cool_Time(SHIFT_SKILL)- m_pPlane->Get_Cool_Time1(SHIFT_SKILL) >= 0.f )
	{
		if (FAILED(Render_Again(256.f * 0.5f, 256.f * 0.5f, m_fX - 600.f, m_fY + 300.f, 3, 0)))
			return E_FAIL;
		TCHAR	szBuf[256] = L"";
		swprintf_s(szBuf, L"%d", (_uint)(m_pPlane->Get_Cool_Time(SHIFT_SKILL) - m_pPlane->Get_Cool_Time1(SHIFT_SKILL)));
		m_pGameInstance->Render_Font(TEXT("Font_BB1"), szBuf, &_float2(m_fX - 610.f, m_fY + 290.f), D3DXCOLOR(0.f, 0.f, 0.f, 1.f));
		m_pGameInstance->Render_Font(TEXT("Font_BB"), szBuf, &_float2(m_fX - 613.f, m_fY + 287.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	}
	TCHAR	szBuf[256] = L"";
	swprintf_s(szBuf, L"%s", L"SHIFT");
	m_pGameInstance->Render_Font(TEXT("Font_gool1"), szBuf, &_float2(m_fX - 625.f, m_fY + 336.f), D3DXCOLOR(0.f, 0.f, 0.f, 0.5f));
	m_pGameInstance->Render_Font(TEXT("Font_gool"), szBuf, &_float2(m_fX - 628.f, m_fY + 339.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	if ( m_pPlane->Get_Cool_Time(SPACE_SKILL)- m_pPlane->Get_Cool_Time1(SPACE_SKILL) >=0.f)
	{
		if (FAILED(Render_Again(256.f * 0.4f, 256.f * 0.4f, m_fX - 540.f, m_fY + 330.f, 3, 0)))
			return E_FAIL;
		TCHAR	szBuf[256] = L"";
		swprintf_s(szBuf, L"%d", (_uint)(m_pPlane->Get_Cool_Time(SPACE_SKILL) - m_pPlane->Get_Cool_Time1(SPACE_SKILL)));
		m_pGameInstance->Render_Font(TEXT("Font_BB"), szBuf, &_float2(m_fX - 540.f, m_fY + 333.f), D3DXCOLOR(0.f, 0.f, 0.f, 1.f));
		m_pGameInstance->Render_Font(TEXT("Font_BB"), szBuf, &_float2(m_fX - 538.f, m_fY + 330.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	}
	TCHAR	szBuf1[256] = L"";
	swprintf_s(szBuf1, L"%s", L"SPACE");
	m_pGameInstance->Render_Font(TEXT("Font_gool1"), szBuf1, &_float2(m_fX - 512.f, m_fY + 338.f), D3DXCOLOR(0.f, 0.f, 0.f, 0.5f));
	m_pGameInstance->Render_Font(TEXT("Font_gool"), szBuf1, &_float2(m_fX - 510.f, m_fY + 335.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	return S_OK;
}

HRESULT CUI_AirPlane::Render_Again(_float fSizeX, _float fSizeY,_float fX, _float fY, _uint iFrame, _uint iRenderState)
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

HRESULT CUI_AirPlane::Reset_First_State()
{

	m_pTransform->Set_WorldMatrix(m_vFirstWorldMatrix);
	return S_OK;
}

HRESULT CUI_AirPlane::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_UI_AirPlane_HP"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureHp)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_UI_AirPlane_Gauge"),
		TEXT("Com_Texture1"), (CComponent**)&m_pTextureGauge)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_UI_AirPlane_Frame"),
		TEXT("Com_Texture2"), (CComponent**)&m_pTextureFrame)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_UI_AirPlane_Side_Back"),
		TEXT("Com_Texture3"), (CComponent**)&m_pTextureSideBack)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_UI_AirPlane_Side_Front"),
		TEXT("Com_Texture4"), (CComponent**)&m_pTextureSideFront)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_UI_AirPlane_Monitor"),
		TEXT("Com_Texture5"), (CComponent**)&m_pTextureMonitor)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_UI_AirPlane_SKill"),
		TEXT("Com_Texture6"), (CComponent**)&m_pTextureSkill)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_AirPlane::Set_RenderState(_ulong lAphaRef)
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

HRESULT CUI_AirPlane::Reset_RenderState()
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

CUI_AirPlane* CUI_AirPlane::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_AirPlane* pInstance = new CUI_AirPlane(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_AirPlane"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_AirPlane::Clone(void* pArg)
{
	CUI_AirPlane* pInstance = new CUI_AirPlane(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_AirPlane"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_AirPlane::Free()
{
	m_pTextureCom = nullptr;
	Safe_Release(m_pTextureHp);
	Safe_Release(m_pTextureGauge);
	Safe_Release(m_pTextureFrame);
	Safe_Release(m_pTextureSideBack);
	Safe_Release(m_pTextureSideFront);
	Safe_Release(m_pTextureMonitor);
	Safe_Release(m_pTextureSkill);

	__super::Free();
}
