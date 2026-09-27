#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

class CClientCollision final : public CGameObject
{
private:
	CClientCollision(LPDIRECT3DDEVICE9 pGraphic_Device);
	CClientCollision(const CClientCollision& rhs);
	virtual ~CClientCollision() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;


public:
	static CClientCollision* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

