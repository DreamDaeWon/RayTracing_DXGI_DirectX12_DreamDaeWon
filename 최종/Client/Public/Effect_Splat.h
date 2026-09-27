#pragma once
#include "Effect_Base.h"

#define EFFECT_SPLAT_LIFE_TIME 0.2f

#define RIHGT_VIEW_RIGHT 0.06f
#define RIHGT_VIEW_UP 0.02f
#define RIHGT_VIEW_BACK 0.1f

BEGIN(Client)

class CEffect_Splat final: public CEffect_Base
{
public:
	typedef struct tagEffectSplatDesc : public CEffect_Base::EFFECT_BASE_DESC {

	}EFFECT_SPLAT_DESC;

private:
	CEffect_Splat(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect_Splat(const CEffect_Splat& rhs);
	virtual ~CEffect_Splat() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_Life_Time() { m_fLifeTime = EFFECT_SPLAT_LIFE_TIME; }

private:
	HRESULT Add_Components();
	HRESULT Set_RenderState();
	HRESULT Reset_RenderState();
	void SetUp_BillBoard();


private:
	_float m_fLifeTime = { 0.f }; //라이프타임. 0이상일 때만 출력. 틱마다 감소.
	CTransform* m_pCameraTransform = { nullptr };

public:
	static CEffect_Splat* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END