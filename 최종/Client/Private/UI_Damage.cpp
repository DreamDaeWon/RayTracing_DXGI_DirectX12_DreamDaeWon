#include "pch.h"
#include "UI_Damage.h"
#include "GameInstance.h"

CUI_Damage::CUI_Damage(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CUI_Base(pGraphic_Device)
{
}

CUI_Damage::CUI_Damage(const CUI_Base& rhs) :
	CUI_Base(rhs)
{
}

HRESULT CUI_Damage::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CUI_Base"));
		return E_FAIL;
	}

	m_fX = 640;
	m_fY = 360;
	m_fSizeX = 0;
	m_fSizeY = 0;
	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);
	
    return S_OK;
}

HRESULT CUI_Damage::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Initialize : __super,CUI_Base"));
		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX(TEXT("Failed to Add_Components ,CWeapon"));
		return E_FAIL;
	}

	m_iCP = 0;
	m_bDead = true;

	D3DXMatrixIdentity(&m_ViewMatrix);

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

	m_fSizeX = 64.f;
	m_fSizeY = 64.f;
	m_fX = g_iWinSizeX * 0.5f;
	m_fY = g_iWinSizeY * 0.5f;

	m_pTransform->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(
		m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f,
		0.f
	));

    return S_OK;
}

_uint CUI_Damage::Tick(_float fTimeDelta)
{
	if (m_bRealDead && 0.f >= m_fLifeTime)
		return OBJECT_DEAD;

	if (m_bDead)
		return OBJECT_NOTHING;

	if (0 < m_fLifeTime)
		m_fLifeTime -= fTimeDelta;

	/*if (24.f <= m_fSizeX)
	{
		m_fSizeX -= 24.f * fTimeDelta;
		m_fSizeY -= 24.f * fTimeDelta;
		m_pTransform->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	}*/
	
		

	__super::Tick(fTimeDelta);

    return OBJECT_NOTHING;
}

void CUI_Damage::Late_Tick(_float fTimeDelta)
{
	if (m_bDead)
		return;

	if (0 >= m_fLifeTime)
		m_bDead = true;

	__super::Late_Tick(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Damage::Render()
{
	if (!g_UI)
		return S_OK;




	if (m_bDead)
		return S_OK;

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	if (FAILED(Set_RenderState()))
		return E_FAIL;

	//===========렌더를 한다. 1의 자리 10의 자리 100의 자리==============
	_uint iThousands(0), iHundreds(0), iTens(0), iUnits(0);
	iThousands = m_iCP / 1000;
	iHundreds = (m_iCP - iThousands * 1000) / 100; //200 일때 2
	iTens = (m_iCP - iThousands * 1000 - iHundreds * 100 ) / 10;
	iUnits = m_iCP % 10;
	


	

	if (1 <= m_iCP)
	{
		m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

		if (FAILED(m_pTransform->Bind_WorldMatrix()))
			return E_FAIL;

		if (FAILED(m_pTextureCom->Bind_Texture(0, iUnits)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}

	if (10 <= m_iCP)
	{
		m_fX -= 20.f * m_fRatio;
		m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

		if (FAILED(m_pTransform->Bind_WorldMatrix()))
			return E_FAIL;

		if (FAILED(m_pTextureCom->Bind_Texture(0, iTens)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}

	if (100 <= m_iCP)
	{
		m_fX -= 20.f * m_fRatio;
		m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

		if (FAILED(m_pTransform->Bind_WorldMatrix()))
			return E_FAIL;

		if (FAILED(m_pTextureCom->Bind_Texture(0, iHundreds)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}

	if (1000 <= m_iCP)
	{
		m_fX -= 20.f * m_fRatio;
		m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

		if (FAILED(m_pTransform->Bind_WorldMatrix()))
			return E_FAIL;

		if (FAILED(m_pTextureCom->Bind_Texture(0, iThousands)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}




	//==================
	
	if (FAILED(Reset_RenderState()))
		return E_FAIL;

    return S_OK;
}

void CUI_Damage::Hit_Damage(_float fCP)
{
	if (!m_bDead)//만약 살아있었으면 즉 지워지지 않았으면 더 해서 출력해주기.
		m_iCP += _uint(fCP);
	else
		m_iCP = _uint(fCP);
	Set_Life_Time();
}

void CUI_Damage::Set_Life_Time()
{
	//뷰제트를 사용해야함.
	//뷰제트가 작아 > 사이즈가 크다
	//뷰제트가 커 > 사이즈가 작아
	//반비례 관계야.
	//뷰제트로 나눈다 >> 이게 반비례
	//어디서부터 작게하고 어디서 가장 클지를 설정해야함.
	//100이 가장 작게 24.f로
	//10에서 가장 큰데 48.f로

	m_fLifeTime = 1.f; 
	if (30.f <= m_fViewZ)
	{
		m_fSizeX = 9.6f;
		m_fSizeY = 9.6f;
		m_fRatio = 0.2f;
	}
	else if (10.f >= m_fViewZ)
	{
		m_fSizeX = 48.f;
		m_fSizeY = 48.f;
		m_fRatio = 1.f;
	}
	else
	{
		// 뷰제트가 1000이면 1/1000 >> 뷰제트가 100이상일때는 반절 1/2 >> 1/10
		//범위를 10 ~ 100 >> 1 ~ 0.5
		// 30 - m_fViewZ >> 20 ~ 0
		// 나누기 60 >> 0.5 ~ 0
		// 더하기 0.5 >> 1 ~ 0.1
		m_fRatio = 30 - m_fViewZ;
		m_fRatio /= 25.f;
		m_fRatio += 0.2f;
		m_fSizeX = 48.f * m_fRatio;
		m_fSizeY = 48.f * m_fRatio;
	}

 	m_pTransform->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_bDead = false;
}

void CUI_Damage::Update_Position(_float3 vViewPos, _float fViewZ)
{
	m_fX = vViewPos.x;
	m_fY = g_iWinSizeY - vViewPos.y - 5.f;
	m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(
		m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f,
		0.f
	));
	m_fViewZ = fViewZ;
}

void CUI_Damage::Set_RealDead()
{
	m_bRealDead = true;
	m_fLifeTime = 2.f;
}

HRESULT CUI_Damage::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Damage"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSG_BOX(TEXT("Failed to Add Component : CUI_Damage"));
		return E_FAIL;
	}
		

    return S_OK;
}

HRESULT CUI_Damage::Set_RenderState(_ulong lAphaRef)
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

HRESULT CUI_Damage::Reset_RenderState()
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

CUI_Damage* CUI_Damage::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_Damage* pInstance = new CUI_Damage(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Damage"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Damage::Clone(void* pArg)
{
	CUI_Damage* pInstance = new CUI_Damage(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Sparrow"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Damage::Free()
{
	__super::Free();
}
