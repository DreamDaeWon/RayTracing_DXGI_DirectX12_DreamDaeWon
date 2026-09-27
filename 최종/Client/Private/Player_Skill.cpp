#include "pch.h"
#include "Player_Skill.h"
#include "GameInstance.h"


CPlayer_Skill::CPlayer_Skill(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CPlayer_Skill::CPlayer_Skill(const CPlayer_Skill& rhs)
	:CGameObject(rhs),
	m_eID(rhs.m_eID),
	m_fCoolTime(rhs.m_fCoolTime),
	m_fSP(rhs.m_fSP),
	m_fKeepSP(rhs.m_fKeepSP),
	m_fActTime(rhs.m_fActTime),
	m_bNoSP(rhs.m_bNoSP)
{
}

HRESULT CPlayer_Skill::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CPlayer_Skill::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		SKILL_DESC* SkillDesc = (SKILL_DESC*)pArg;
		m_pPlayerFrameLookVec = SkillDesc->pPlayerFrameLookVec;
		m_pPlayerTransform = SkillDesc->pPlayerTransform;
	}
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Initialize : __super,CBarrier"));
		return E_FAIL;
	}
	return S_OK;
}

_uint CPlayer_Skill::Tick(_float fTimeDelta)
{
	return _uint();
}

void CPlayer_Skill::Late_Tick(_float fTimeDelta)
{
}

HRESULT CPlayer_Skill::Render()
{
	return S_OK;
}

void CPlayer_Skill::Key_Input(_float fTimeDelta)
{
}

HRESULT CPlayer_Skill::Add_Components()
{
	return S_OK;
}

HRESULT CPlayer_Skill::Set_RenderState()
{
	return S_OK;
}

HRESULT CPlayer_Skill::Reset_RenderState()
{
	return S_OK;
}

void CPlayer_Skill::Free()
{
	__super::Free();


}
