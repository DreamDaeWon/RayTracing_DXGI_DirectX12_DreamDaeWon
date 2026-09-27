#pragma once
#include "Effect_Base.h"

#define EFFECT_SPLAT_ORANGE_LIFE_TIME 0.5f

#define RIHGT_VIEW_RIGHT 0.06f
#define RIHGT_VIEW_UP 0.02f
#define RIHGT_VIEW_BACK 0.1f

BEGIN(Client)

class CEffect_Splat_Orange final : public CEffect_Base
{
public:
	typedef struct tagEffectSplatOrangeDesc : public CEffect_Base::EFFECT_BASE_DESC {

	}EFFECT_SPLAT_ORANGE_DESC;

private:
	CEffect_Splat_Orange(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect_Splat_Orange(const CEffect_Splat_Orange& rhs);
	virtual ~CEffect_Splat_Orange() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_Life_Time() { 
		m_fLifeTime = EFFECT_SPLAT_ORANGE_LIFE_TIME;
		m_fFrame = 0.f;
	}

private:
	HRESULT Add_Components();
	HRESULT Set_RenderState();
	HRESULT Reset_RenderState();
	void SetUp_BillBoard();


private:
	_float m_fLifeTime = { 0.f }; //라이프타임. 0이상일 때만 출력. 틱마다 감소.
	CTransform* m_pCameraTransform = { nullptr };

public:
	static CEffect_Splat_Orange* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END