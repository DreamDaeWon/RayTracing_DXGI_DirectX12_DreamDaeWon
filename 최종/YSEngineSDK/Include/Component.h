#pragma once

#include "Base.h"

BEGIN(Engine)

/* 컴포넌트의 공동적인 기능을 가진다.(생각보다 공통적인기능? 거의 없어  */
/* 모든 컴포넌트의 부모클래스를 정의함으로써 객체안에 컴포넌트들을 저장할때 하나의 타입(다형성)으로 저장, 관리할 수 있게 될 것이다. */

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	CComponent(LPDIRECT3DDEVICE9 pGraphic_Device);
	CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);


protected:
	LPDIRECT3DDEVICE9 m_pGraphic_Device = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };
	_bool m_bIsCloned = { false };

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END