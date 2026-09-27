#pragma once
#include "Effect_Base.h"

BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)

class CItem_Effect final : public CEffect_Base
{
public:
	enum WEAPON_EFFECT { WE_PUPA, WE_RAMBO, WE_SASIN, WE_SPARROW, WE_SHARKBLOOD, WE_ATLAS, WE_END};

public:
	typedef struct tagItemEffect_Desc : public CEffect_Base::EFFECT_BASE_DESC {
		_float3 vPosition = { 0.f,0.f,0.f };
		_uint iWeaponID = { 0 };
	}ITEM_EFFECT_DESC;

private:
	CItem_Effect(LPDIRECT3DDEVICE9 pGraphic_Device);
	CItem_Effect(const CItem_Effect& rhs);
	virtual ~CItem_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Update_Position(_float3 vPos);

private:
	HRESULT Add_Components();
	HRESULT Set_RenderState();
	HRESULT Reset_RenderState();
	void SetUp_BillBoard();
	void WeaponType(WEAPON_EFFECT eWE);

private:
	_float m_fLifeTime = { 0.f }; //라이프타임. 0이상일 때만 출력. 틱마다 감소.
	_float3 m_vPosition = { 0.f, 0.f, 0.f };
	WEAPON_EFFECT m_eWE = { WE_END };
	_float m_fMinFrame = { 0.f };

	CTransform* m_pCameraTransform = { nullptr };

public:
	static CItem_Effect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END