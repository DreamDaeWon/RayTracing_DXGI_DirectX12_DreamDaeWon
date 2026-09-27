#pragma once
#include "Monster_Bullet_Base.h"

BEGIN(Client)

class CRed_Monster_Bullet final : public CMonster_Bullet_Base
{
public:
	enum RED_MONSTER_BULLET { STATE_IDLE, STATE_TRACKING, STATE_DEATH, STATE_END };

private:
	CRed_Monster_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	CRed_Monster_Bullet(const CRed_Monster_Bullet& rhs);
	virtual ~CRed_Monster_Bullet() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void Do_State(_float fTimeDelta); // 상태값을 정해 줌
	void Set_Render_Texture(_float fTimeDelta); // 어떤 사진을 출력하고 어떤 숫자로 출력할 지 정해 줌
	void CheckPlayer(); // 플레이어가 인식범위에 들어왔는지?

	virtual HRESULT Add_Components() override;

private:
	RED_MONSTER_BULLET m_eState = { STATE_TRACKING };

	CTexture* m_pTextureIdle = { nullptr };
	CTexture* m_pTextureDeath = { nullptr };

	// 죽음 관련 변수
	_bool m_bDeadShow = { false }; // 죽는 모션을 모두 보여줬는지?

	// 텍스쳐 관련 변수
	_float m_fTextureTotal = { 0.f }; // 텍스쳐 사진 로드할 때 넣어줄 값
	_float m_fTexture = { 0.f }; // 몇 번째 사진을 사용할 건지?
	_float m_fTextureMaxPicture = { 0.f }; // 텍스쳐가 한루프를 돌 때의 몇 개의 사진이 쓰이는지
	_float m_fTextureSpeed = { 0.5f }; // 텍스쳐가 다음사진으로 넘어가는 속도

	// 1초에 몇 번 돌릴건지?
	_float m_fTextureSpeed_Idle = { 1.f }; // Idle 상태일 때 텍스쳐 넘어가는 속도
	_float m_fTextureSpeed_Death = { 0.5f }; //Death 상태일 때 텍스쳐 넘어가는 속도

	_float m_fDistance = { 2.0f }; // 얼마나 가까워지면 추적을 멈출껀지?

	// 시간
	_float m_fTime = { 0.f };

public:
	static CRed_Monster_Bullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override; // 이거 보스가 실행
	virtual void Free() override;
};

END