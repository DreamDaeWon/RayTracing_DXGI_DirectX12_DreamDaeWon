#pragma once

#include "Component.h"
#include"Client_Defines.h"


/*
24.01.12 예은 작업
STATE_MAXHP: 최대 체력
STATE_HP: 체력
STATE_CP: 공격력
STATE_AILMENT: 상태 이상 (감전, 화상)
STATE_MONEY: 플레이어-돈/ 몬스터-
STATE_DEAD: 죽음

Client_Defines: #define 참고

STATE_DESC: 정의는 어디서 해주고 clone으로 들어오는가?
이 컴포넌트를 가지는 add_components()함수에서 직접 정의
player참고
=> 	class CState* m_pState_Com = { nullptr }; 생성

Initialize: 
=> 체력추가에 대해 pArg가 없는 경우: STATE_DEAD, MONEY, AILMENT를 제외한 모든 것을 1로 초기화




*/

BEGIN(Engine)
class CComponent;
END


BEGIN(Client)

class CState final : public CComponent
{
public:
	enum STATE { STATE_MAXHP, STATE_HP, STATE_CP, STATE_AILMENT, 
						  STATE_MONEY, STATE_DEAD, STATE_END };
	typedef struct tagStateDesc {
		_float	fMaxHp = 0.f;
		_float	fCP = 0.f;
	}STATE_DESC; 



public:
	_float	Get_State(STATE eState) { return m_vecState[eState]; }
	void		Set_State(STATE eState, _float fValue) { m_vecState[eState] = fValue; }

private:
	CState(LPDIRECT3DDEVICE9 pGraphic_Device);
	CState(const CState& rhs);
	virtual ~CState() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
public:
	_bool		Set_Hp(_float fCP) {
		m_vecState[STATE_HP] += fCP; 
		if (m_vecState[STATE_HP] <= 0)	
			return m_vecState[STATE_DEAD] = OBJECT_DEAD; //죽음
		
		return OBJECT_NOTHING; //안죽음
	}
	void	Set_Ailment(_float fObject_State)
	{
		m_vecState[STATE_AILMENT] = fObject_State;
	}
private:
	_float m_vecState[STATE_END] = {};

public:
	static CState* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

