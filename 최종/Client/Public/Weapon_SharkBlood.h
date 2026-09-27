#pragma once
#include "Weapon.h"

#define SHARKBLOOD_RIHGT_VIEW_RIGHT 0.06f
#define SHARKBLOOD_RIHGT_VIEW_UP 0.02f
#define SHARKBLOOD_RIHGT_VIEW_BACK 0.1f
#define SHARKBLOOD_SHOOTING_TERM 0.15f

BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
/* 총 이미지 분석
0-6 등에 매달고 있을 때 사용하는 이미지
12-17?  조준 했을 때 반동표현하는 이미지
*/
class CWeapon_SharkBlood final: public CWeapon
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
	CWeapon_SharkBlood(LPDIRECT3DDEVICE9 pGraphic_Device);
	CWeapon_SharkBlood(const CWeapon_SharkBlood& rhs);
	virtual ~CWeapon_SharkBlood() = default;

public:
	void Shot_Bullet();
	virtual _float Collision_Bullet_Rect(class CCollider_Rect* pCollider_Rect, _float fTimeDelta) override;

private:
	HRESULT Add_Components();
	void Chase_Next_Monster(_float3 vPos,class CBullet* pBullet); //튕기기
	class CEffect_Splat_White_Circle* m_pEffect[5] {};


public:
	static CWeapon_SharkBlood* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END