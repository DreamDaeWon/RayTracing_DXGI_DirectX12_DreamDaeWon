#include "pch.h"
#include "UI_Player_Weapon.h"
#include "GameInstance.h"
#include"Player.h"

#include"Weapon.h"

CUI_Player_Weapon::CUI_Player_Weapon(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CUI_Base(pGraphic_Device)
{
}

CUI_Player_Weapon::CUI_Player_Weapon(const CUI_Base& rhs)
	:CUI_Base(rhs)
{
}

HRESULT CUI_Player_Weapon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Player_Weapon::Initialize(void* pArg)
{



 	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	D3DXMatrixIdentity(&m_ViewMatrix);

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);


	m_fSizeX = 512.f;
	m_fSizeY = 256.f;
	m_fX = 1080.f;
	m_fY = 610.f;



	m_pTransform->Set_Scale(_float3(m_fSizeX*0.8f, m_fSizeY*0.8f , 1.f));
	m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(
		m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f,
		0.f
	));
	m_WorldMatrix =* m_pTransform->Get_WorldMatrix();
	return S_OK; 
}

_uint CUI_Player_Weapon::Tick(_float fTimeDelta)
{
	m_fFrame += 2.f * fTimeDelta * 2.f;
	if (m_fFrame > 3.f)
		m_fFrame = 0.f;


	Get_State();

	return OBJECT_NOTHING;
}

void CUI_Player_Weapon::Late_Tick(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);

}

HRESULT CUI_Player_Weapon::Render()
{

	if (!g_UI)
		return S_OK;

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	//메인 back

	_float fGauge = 0.f;
	_float fMaxGauge = 0.f;
	m_pTextureCom = m_pTextureWeaponBackCom;
	if (FAILED(Render_Again(m_fSizeX * 0.8f, m_fSizeY * 0.8f, m_fX, m_fY + 7.f, 0, 0)))
		return E_FAIL;
	if (m_pWeaponSlot[m_iSelectWeapon] != nullptr)
	{
		fGauge = m_pWeaponSlot[m_iSelectWeapon]->Get_Gauge();
		fMaxGauge = m_pWeaponSlot[m_iSelectWeapon]->Get_MaxGauge();
		//메인 총 Icon
		if (m_pWeaponSlot[m_iSelectWeapon]->IsThatEx())
			m_pTextureCom = m_pTextureWeaponExCom;
		else
			m_pTextureCom = m_pTextureWeaponCom;
		if (FAILED(Render_Again(m_fSizeX * 0.4f, m_fSizeY * 0.8f, m_fX + 100.f, m_fY, m_pWeaponSlot[m_iSelectWeapon]->Get_ID(), 20)))
			return E_FAIL;
		//메인 게이지 back
		m_pTextureCom = m_pTextureMainCom;
		if (FAILED(Render_Again(279.f, 16.f * 1.2f, m_fX - 105.f, m_fY + 30.f, 0, 20)))
			return E_FAIL;
		//메인 게이지 게이지
		if (FAILED(Render_Again(fGauge *(279.f / fMaxGauge), 16.f*1.2f, m_fX - 105.f + ((fMaxGauge- fGauge)*(279.f/ fMaxGauge)*0.49f), m_fY + 30.f, 1, 20)))
			return E_FAIL;
		//메인 총 Bullet Icon
		m_pTextureCom = m_pTextureBulletIcon;
		if (FAILED(Render_Again(m_fSizeX * 0.125f * 0.8f, m_fSizeY * 0.24f * 0.8f, m_fX - 80.f, m_fY + 55.f, m_pWeaponSlot[m_iSelectWeapon]->Get_BulletID(), 20)))
			return E_FAIL;
		if (m_iSelectWeapon == THIRD_WEAPON)
		{
			//메인 총알 개수 - Font_gool2
			TCHAR	szBuf[256] = L"";
			swprintf_s(szBuf, L"%d/∞", m_pWeaponSlot[m_iSelectWeapon]->Get_Weapon_Bullet_Info().iNowBulletNum);
			m_pGameInstance->Render_Font(TEXT("Font_gool2"), szBuf, &_float2(m_fX - 65.f, m_fY + 45.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

		}
		else
		{
			//메인 총알 개수 - Font_gool2
			TCHAR	szBuf[256] = L"";
			swprintf_s(szBuf, L"%d/%d", m_pWeaponSlot[m_iSelectWeapon]->Get_Weapon_Bullet_Info().iNowBulletNum, m_pWeaponSlot[m_iSelectWeapon]->Get_Weapon_Bullet_Info().iExtraBulletNum);
			m_pGameInstance->Render_Font(TEXT("Font_gool2"), szBuf, &_float2(m_fX - 65.f, m_fY + 45.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

		}
		//메인 게이지 frame		
		m_pTextureCom = m_pTextureMainCom;
		if (FAILED(Render_Again(279.f, 16.f * 1.2f, m_fX - 105.f, m_fY + 30.f, 2, 20)))
			return E_FAIL;
		if (m_pWeaponSlot[m_iSelectWeapon]->IsThatEx())
		{
			//메인 dot
			m_pTextureCom = m_pTextureDotCom;
			if (FAILED(Render_Again(m_fSizeX * 0.52f, m_fSizeY * 0.5f, m_fX - 108.f, m_fY - 45.f, (_uint)m_fFrame, 20)))
				return E_FAIL;
		}
		

	}
	


	//미니1 back
	m_pTextureCom = m_pTextureWeaponBackCom;
	if (FAILED(Render_Again(128.f * 0.8f, 128.f * 0.8f, m_fX + 150.f, m_fY - 260.f, 1, 20)))
		return E_FAIL;
	if (nullptr != m_pWeaponSlot[FIRST_WEAPON])
	{

		fGauge = m_pWeaponSlot[FIRST_WEAPON]->Get_Gauge();
		fMaxGauge = m_pWeaponSlot[FIRST_WEAPON]->Get_MaxGauge();
		if (FIRST_WEAPON == m_iSelectWeapon)
		{
			//미니1 back_check
			m_pTextureCom = m_pTextureMiniCheckCom;
			if (FAILED(Render_Again(64.f * 0.8f, 64.f * 0.8f, m_fX + 150.f, m_fY - 260.f, 0, 20)))
				return E_FAIL;
			//미니1 테두리_check
			m_pTextureCom = m_pTextureMiniSlotCheckCom;
			if (FAILED(Render_Again(128.f * 0.8f, 128.f * 0.8f, m_fX + 150.f, m_fY - 260.f, 1, 20)))
				return E_FAIL;
			m_pGameInstance->Render_Font(TEXT("Font_Comic_Shark"), TEXT("1"), &_float2(m_fX + 158.f, m_fY - 285.f), D3DXCOLOR(0.f, 0.f, 0.f, 1.f));

		}
		else
		{
			//미니1 테두리_check
			m_pTextureCom = m_pTextureMiniSlotCheckCom;
			if (FAILED(Render_Again(128.f * 0.8f, 128.f * 0.8f, m_fX + 150.f, m_fY - 260.f, 0, 20)))
				return E_FAIL;

			m_pGameInstance->Render_Font(TEXT("Font_Comic_Shark"), TEXT("1"), &_float2(m_fX + 158.f, m_fY - 285.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));


		}
		//미니1 총 Icon
		if (m_pWeaponSlot[FIRST_WEAPON]->IsThatEx())
			m_pTextureCom = m_pTextureWeaponExCom;
		else
			m_pTextureCom = m_pTextureWeaponCom;
		if (FAILED(Render_Again(m_fSizeX * 0.2f, m_fSizeY * 0.4f, m_fX + 150.f, m_fY - 260.f, m_pWeaponSlot[FIRST_WEAPON]->Get_ID(), 20)))
			return E_FAIL;
		//미니1 게이지 back
		m_pTextureCom = m_pTextureMiniCom;
		if (FAILED(Render_Again(76.f, 6.f * 1.2f, m_fX + 150.f, m_fY - 225.f, 0, 20)))
			return E_FAIL;
		//미니1 게이지 bar
		if (FAILED(Render_Again(fGauge * (76.f / fMaxGauge), 6.f * 1.2f, m_fX + 150.f + ((fMaxGauge - fGauge) * (76.f / fMaxGauge) * 0.49f), m_fY - 225.f, 1, 20)))
			return E_FAIL;
		//미니1 게이지 frame
		if (FAILED(Render_Again(76.f, 6.f * 1.2f, m_fX + 150.f, m_fY - 225.f, 2, 20)))
			return E_FAIL;

		//총알 정보
		TCHAR	szBuf[256] = L"";
		swprintf_s(szBuf, L"%d/%d", m_pWeaponSlot[FIRST_WEAPON]->Get_Weapon_Bullet_Info().iNowBulletNum, m_pWeaponSlot[FIRST_WEAPON]->Get_Weapon_Bullet_Info().iExtraBulletNum);
		m_pGameInstance->Render_Font(TEXT("Font_Comic_Shark1"), szBuf, &_float2(m_fX + 133.5f, m_fY - 244.5f), D3DXCOLOR(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font_Comic_Shark1"), szBuf, &_float2(m_fX + 135.f, m_fY - 245.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
		


	}
	
	//미니2 back
	m_pTextureCom = m_pTextureWeaponBackCom;
	if (FAILED(Render_Again(128.f * 0.8f, 128.f * 0.8f, m_fX + 150.f, m_fY - 190.f, 1, 20)))
		return E_FAIL;
	if (nullptr != m_pWeaponSlot[SECOND_WEAPON])
	{

		fGauge = m_pWeaponSlot[SECOND_WEAPON]->Get_Gauge();
		fMaxGauge = m_pWeaponSlot[SECOND_WEAPON]->Get_MaxGauge();
		if (SECOND_WEAPON == m_iSelectWeapon)
		{
			//미니2 back_check
			m_pTextureCom = m_pTextureMiniCheckCom;
			if (FAILED(Render_Again(64.f * 0.8f, 64.f * 0.8f, m_fX + 150.f, m_fY - 190.f, 0, 20)))
				return E_FAIL;

			//미니2 테두리_check
			m_pTextureCom = m_pTextureMiniSlotCheckCom;
			if (FAILED(Render_Again(128.f * 0.8f, 128.f * 0.8f, m_fX + 150.f, m_fY - 190.f, 1, 20)))
				return E_FAIL;

			m_pGameInstance->Render_Font(TEXT("Font_Comic_Shark"), TEXT("2"), &_float2(m_fX + 158.f, m_fY - 215.f), D3DXCOLOR(0.f, 0.f, 0.f, 1.f));

		}
		else
		{
			//미니2 테두리_check
			m_pTextureCom = m_pTextureMiniSlotCheckCom;
			if (FAILED(Render_Again(128.f * 0.8f, 128.f * 0.8f, m_fX + 150.f, m_fY - 190.f, 0, 20)))
				return E_FAIL;
			m_pGameInstance->Render_Font(TEXT("Font_Comic_Shark"), TEXT("2"), &_float2(m_fX + 158.f, m_fY - 215.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

		}
		//미니2 총 Icon
		if (m_pWeaponSlot[SECOND_WEAPON]->IsThatEx())
			m_pTextureCom = m_pTextureWeaponExCom;
		else
			m_pTextureCom = m_pTextureWeaponCom;
		if (FAILED(Render_Again(m_fSizeX * 0.2f, m_fSizeY * 0.4f, m_fX + 150.f, m_fY - 190.f, m_pWeaponSlot[SECOND_WEAPON]->Get_ID(), 20)))
			return E_FAIL;
		//미니2 게이지 back
		m_pTextureCom = m_pTextureMiniCom;
		if (FAILED(Render_Again(76.f, 6.f * 1.2f, m_fX + 150.f, m_fY - 155.f, 0, 20)))
			return E_FAIL;
		//미니2 게이지 bar
		if (FAILED(Render_Again(fGauge * (76.f / fMaxGauge), 6.f * 1.2f, m_fX + 150.f + ((fMaxGauge - fGauge) * (76.f / fMaxGauge) * 0.49f), m_fY - 155.f, 1, 20)))
			return E_FAIL;
		//미니2 게이지 frame
		if (FAILED(Render_Again(76.f, 6.f * 1.2f, m_fX + 150.f, m_fY - 155.f, 2, 20)))
			return E_FAIL;
		//총알 정보
		
		TCHAR	szBuf[256] = L"";
		swprintf_s(szBuf, L"%d/%d", m_pWeaponSlot[SECOND_WEAPON]->Get_Weapon_Bullet_Info().iNowBulletNum, m_pWeaponSlot[SECOND_WEAPON]->Get_Weapon_Bullet_Info().iExtraBulletNum);
		m_pGameInstance->Render_Font(TEXT("Font_Comic_Shark1"), szBuf, &_float2(m_fX + 133.5f, m_fY - 174.5f), D3DXCOLOR(0.f, 0.f, 0.f, 1.f));
		m_pGameInstance->Render_Font(TEXT("Font_Comic_Shark1"), szBuf, &_float2 (m_fX + 135.f, m_fY - 175.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	}
	//미니3 back
	m_pTextureCom = m_pTextureWeaponBackCom;
	if (FAILED(Render_Again(128.f * 0.8f, 128.f * 0.8f, m_fX + 150.f, m_fY - 120.f, 1, 20)))
		return E_FAIL;
	if (nullptr != m_pWeaponSlot[THIRD_WEAPON])
	{

		fGauge = m_pWeaponSlot[THIRD_WEAPON]->Get_Gauge();
		fMaxGauge = m_pWeaponSlot[THIRD_WEAPON]->Get_MaxGauge();
		if (THIRD_WEAPON == m_iSelectWeapon)
		{
			//미니3 back_check
			m_pTextureCom = m_pTextureMiniCheckCom;
			if (FAILED(Render_Again(64.f * 0.8f, 64.f * 0.8f, m_fX + 150.f, m_fY - 120.f, 0, 20)))
				return E_FAIL;

			//미니3 테두리_check
			m_pTextureCom = m_pTextureMiniSlotCheckCom;
			if (FAILED(Render_Again(128.f * 0.8f, 128.f * 0.8f, m_fX + 150.f, m_fY - 120.f, 1, 20)))
				return E_FAIL;
			m_pGameInstance->Render_Font(TEXT("Font_Comic_Shark"), TEXT("3"), &_float2(m_fX + 158.f, m_fY - 146.f), D3DXCOLOR(0.f, 0.f, 0.f, 1.f));

		}
		else
		{
			//미니3 테두리_check
			m_pTextureCom = m_pTextureMiniSlotCheckCom;
			if (FAILED(Render_Again(128.f * 0.8f, 128.f * 0.8f, m_fX + 150.f, m_fY - 120.f, 0, 20)))
				return E_FAIL;
			m_pGameInstance->Render_Font(TEXT("Font_Comic_Shark"), TEXT("3"), &_float2(m_fX + 158.f, m_fY - 146.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

		}
		//미니3 총 Icon
		if (m_pWeaponSlot[THIRD_WEAPON]->IsThatEx())
			m_pTextureCom = m_pTextureWeaponExCom;
		else
			m_pTextureCom = m_pTextureWeaponCom;\
		if (FAILED(Render_Again(m_fSizeX * 0.2f, m_fSizeY * 0.4f, m_fX + 150.f, m_fY - 120.f, m_pWeaponSlot[THIRD_WEAPON]->Get_ID(), 20)))
			return E_FAIL;
		//미니3 게이지 back
		m_pTextureCom = m_pTextureMiniCom;
		if (FAILED(Render_Again(76.f, 6.f * 1.2f, m_fX + 150.f, m_fY - 85.f, 0, 20)))
			return E_FAIL;
		//미니3 게이지 bar
		if (FAILED(Render_Again(fGauge * (76.f / fMaxGauge), 6.f * 1.2f, m_fX + 150.f + ((fMaxGauge - fGauge) * (76.f / fMaxGauge) * 0.49f), m_fY - 85.f, 1, 20)))
			return E_FAIL;
		//미니3 게이지 frame
		if (FAILED(Render_Again(76.f, 6.f * 1.2f, m_fX + 150.f, m_fY - 85.f, 2, 20)))
			return E_FAIL;
		//총알 정보
		TCHAR	szBuf[256] = L"";
		swprintf_s(szBuf, L"%d/∞", m_pWeaponSlot[THIRD_WEAPON]->Get_Weapon_Bullet_Info().iNowBulletNum);
		m_pGameInstance->Render_Font(TEXT("Font_Comic_Shark1"), szBuf, &_float2(m_fX + 128.5f, m_fY - 104.5f), D3DXCOLOR(0.f, 0.f, 0.f, 1.f));
		m_pGameInstance->Render_Font(TEXT("Font_Comic_Shark1"), szBuf, &_float2(m_fX + 130.f, m_fY - 105.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	}


	if (Reset_RenderState())
		return E_FAIL;


	return S_OK;
}

void CUI_Player_Weapon::Get_State()
{
	for (_uint i = 0; i < WEAPON_SLOT_END; i++)
	{
		m_pWeaponSlot[i] = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Get_Weapon(i);
	}
	m_iSelectWeapon = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Get_CurWeapon();

}

HRESULT CUI_Player_Weapon::Render_Again(_float fSizeX, _float fSizeY, _float fX, _float fY, _uint iFrame, _uint iRenderState)
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

	Reset_First_State();
	return S_OK;
}

void CUI_Player_Weapon::Reset_First_State()
{
	m_pTransform->Set_WorldMatrix(m_WorldMatrix);
}

HRESULT CUI_Player_Weapon::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */

	//TODO: 예은 추가

	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_UI_Weapon_Icon"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureWeaponCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_UI_WeaponEx_Icon"),
		TEXT("Com_Texture1"), (CComponent**)&m_pTextureWeaponExCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_UI_Weapon_Main"),
		TEXT("Com_Texture2"), (CComponent**)&m_pTextureMainCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_UI_Weapon_Mini"),
		TEXT("Com_Texture3"), (CComponent**)&m_pTextureMiniCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_UI_Weapon_Back_Dot"),
		TEXT("Com_Texture4"), (CComponent**)&m_pTextureDotCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_UI_Weapon_Mini_Check"),
		TEXT("Com_Texture5"), (CComponent**)&m_pTextureMiniCheckCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_UI_Weapon_Mini_Check1"),
		TEXT("Com_Texture6"), (CComponent**)&m_pTextureMiniSlotCheckCom)))
		return E_FAIL;	
	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_UI_Weapon_Bulllet_Icon"),
		TEXT("Com_Texture7"), (CComponent**)&m_pTextureBulletIcon)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_UI_Weapon_Back"),
		TEXT("Com_Texture8"), (CComponent**)&m_pTextureWeaponBackCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Player_Weapon::Set_RenderState(_ulong lAphaRef)
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

HRESULT CUI_Player_Weapon::Reset_RenderState()
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

CUI_Player_Weapon* CUI_Player_Weapon::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_Player_Weapon* pInstance = new CUI_Player_Weapon(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Player"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Player_Weapon::Clone(void* pArg)
{
	CUI_Player_Weapon* pInstance = new CUI_Player_Weapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Player"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Player_Weapon::Free()
{
	m_pTextureCom = nullptr;
	Safe_Release(m_pTextureWeaponBackCom);
	Safe_Release(m_pTextureWeaponCom);
	Safe_Release(m_pTextureWeaponExCom);
	Safe_Release(m_pTextureMainCom);
	Safe_Release(m_pTextureMiniCom);
	Safe_Release(m_pTextureDotCom);
	Safe_Release(m_pTextureMiniCheckCom);
	Safe_Release(m_pTextureMiniSlotCheckCom);
	Safe_Release(m_pTextureBulletIcon);
	__super::Free();

}
