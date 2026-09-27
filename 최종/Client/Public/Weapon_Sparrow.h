#pragma once
#include "Weapon.h"

#define SPARROW_RIHGT_VIEW_RIGHT 0.06f
#define SPARROW_RIHGT_VIEW_UP 0.02f
#define SPARROW_RIHGT_VIEW_BACK 0.1f
#define SPARROW_SHOOTING_TERM 1.f

BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
/* 총 이미지 분석
0-6 등에 매달고 있을 때 사용하는 이미지
12-17?  조준 했을 때 반동표현하는 이미지
*/
class CWeapon_Sparrow final: public CWeapon
{
//public:
//	typedef struct tagEffectSplatDesc : public CEffect_Base::EFFECT_BASE_DESC {
//
//	}EFFECT_SPLAT_DESC;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Clone_Bullet() override;

private:
	CWeapon_Sparrow(LPDIRECT3DDEVICE9 pGraphic_Device);
	CWeapon_Sparrow(const CWeapon_Sparrow& rhs);
	virtual _uint Tick(_float fTimeDelta) override;
	virtual ~CWeapon_Sparrow() = default;

public:
	void Shot_Bullet();

private:
	HRESULT Add_Components();
	class CEffect_Splat_Blue* m_pEffect = { nullptr };

public:
	static CWeapon_Sparrow* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END