#pragma once
#include "Monster_Bullet_Base.h"

BEGIN(Client)

class CShootMonsterBullet final : public CMonster_Bullet_Base
{
public:
	enum SHOOTMONSTER_MONSTER_BULLET { STATE_IDLE, STATE_DEATH, STATE_END };

private:
	CShootMonsterBullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	CShootMonsterBullet(const CShootMonsterBullet& rhs);
	virtual ~CShootMonsterBullet() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void Do_State(_float fTimeDelta); // 상태값을 정해 줌

	virtual HRESULT Add_Components() override;

private:
	SHOOTMONSTER_MONSTER_BULLET m_eState = { STATE_IDLE };

	// 죽음 관련 변수
	_bool m_bDeadShow = { false }; // 죽는 모션을 모두 보여줬는지?

	// 시간
	_float m_fTime = { 0.f };


public:
	static CShootMonsterBullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override; // 이거 보스가 실행
	virtual void Free() override;
};

END
