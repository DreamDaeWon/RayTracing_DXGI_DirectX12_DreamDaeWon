#pragma once
#include "Monster_Bullet_Base.h"

BEGIN(Client)

class CMonsterBullet final : public CMonster_Bullet_Base
{
private:
	CMonsterBullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMonsterBullet(const CMonsterBullet& rhs);
	virtual ~CMonsterBullet() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT Add_Components() override;

public:
	static CMonsterBullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override; // 이거 보스가 실행
	virtual void Free() override;
};

END