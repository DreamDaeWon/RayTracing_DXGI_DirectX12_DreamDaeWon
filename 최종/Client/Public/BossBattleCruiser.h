#pragma once
#include "LandObject.h"
#include "Client_Defines.h"
#include "random"


BEGIN(Engine)
class CCollider_Cube_AABB;
class CCollider_Rect;
class CCollider;
class CVIBuffer_AirPlane;
class CTexture;
END

BEGIN(Client)


class CBossBattleCruiser final : public CLandObject
{
public:

	typedef struct tagMonster_Boss_BattleCruiser_Desc : public CLandObject::LANDOBJECT_DESC {
		_float fScale;
	}BOSS_BATTLE_CRUISER_DESC;

	// 보스의 상태 값 패턴을 추가할 경우 무조건 IDLE 앞에 두도록 하자
	enum STATE_BOSS_BATTLECRUISER { STATE_LAYSER, STATE_SKILL_SHOOT_CIRCLE, 
		STATE_IDLE, STATE_SPON, STATE_MOVE, STATE_DEATH, STATE_END };

	_uint Get_Boss_State() { return m_eNowState; }


private:
	CBossBattleCruiser(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBossBattleCruiser(const CBossBattleCruiser& rhs);
	virtual ~CBossBattleCruiser() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Return_Look_Position(_float3* pvLook, _float3* pvPosition);

private:
	void CheckPlayer(); // 플레이어가 인식범위에 들어왔는지?
	HRESULT Add_Components();
	HRESULT Set_RenderState();
	HRESULT Reset_RenderState();
	void Chase_Player(_float fTimeDelta, _float Min_Distance);
	_bool MousePicking(); // 마우스가 이 몬스터를 지정하면
	//TODO:용수 총알 발사함수
	void Shot_Bullet(_float3 vLook);
	//TODO:용수 총알충돌처리 테스트
	void Collision_Bullet(_float fTimeDelta);
	void Collision_Wall();
	void Collision_Player();
	void Collider_Billboarding();

	void PlusRedRect();

private:
	void Do_State(_float fTimeDelta); // 총괄

	void Spon(_float fTimeDelta); // 시네마틱 등장신

	void Idle(_float fTimeDelta); // 대기상태 아무것도 안함

	// 내가 지정해준 위치로 이동(time, 어디로, 얼마동안)
	_bool Move(_float fTimeDelta, _float3 _MovePos, _float _MoveTime); 

	void Layser(_float fTimeDelta); // 레이저 발사 스킬

	void CircleShoot(_float fTimeDelta); // 원 모양으로 발사 스킬

	void Death(_float fTimeDelta); // 죽음 모션 보여주기


private:

	_float m_fScale = { 1.f };
	_float3 m_vStartPos = { 0.f,0.f,0.f }; // 시작 지점 X,Y,Z
	_float3 m_vNormalPos = {25.f,0.f,45.f}; // 기본 위치


	// 스폰 관련 변수
	enum ESTATESPON {SPON_SIZE_UP, SPON_MOVE_GO, SPON_MOVE_UP, SPON_END}; // 스폰 할 때 사용 할 enum값
	ESTATESPON m_eSpon = SPON_SIZE_UP; // 현재 스폰함수 안에서의 상태



	// 움직임 관련변수
	_bool m_bSetMoveTarget = { false }; // 움직일 거리를 계산했는지?
	_float3 m_vMovePos = {}; // 움직일 거리


	// 총쏘기 관련변수
	_float m_fShootTime = {0.5f}; //몇 초에 한번씩 총을 발사할 건지?
	_uint m_iShootNumber = {10}; // 몇 번이나 총알을 쏠건지?
	_uint m_iShootNumberPlus = {0}; // 몇 번이나 총알을 쐈는지?

	// 레이저 관련변수
	_float m_fLateLayserShoot = {3.f}; // 얼마나 뒤에 레이저를 발사할건지? (빨간색경고 네모 라이프타임 값)
	_float3 m_vRedRectPos = {}; //빨간 색 렉트를 생성할 때 넣어줄 위치 값
	_float3 m_vRedRectScale = {33.f, 1.f, 3.f}; // 빨간 색 렉트를 생성할 때 넣어줄 스케일 값
	_float3 m_vBossLayserPos = {}; // 보스가 레이져를 쏘러갈 때의 위치
	_float3 m_vLayserPos = {}; // 레이저의 생성 위치
	_uint m_iLayserDir = {}; // 레이저 쏠 방향
	_bool m_bGoShootLayser = { false }; //레이저 쏘러 갔는지?
	_bool m_bGoPlayerShootLayser = { false }; // 플레이어한테 레이저 쏘러 갔는지?
	_bool m_bShootingLayser = { false }; // 레이저를 쏘는 중인지?
	_bool m_bShootLayser = {false}; // 레이저를 쐇는지?
	class CRedRect* m_pRedRect = { nullptr }; // 빨간네모



	_bool m_bCheckPlayer = { false }; // 몬스터가 플레이어를 인식했는지에 대한 bool값

	_float m_fCheckDestence = { 10.f }; // 몬스터가 플레이어를 인식하는 범위
	_float3 m_vMonsterLook = { 0.f, 0.f, 1.f }; // 몬스터가 게임 상에서 바라보는 룩벡터

	CTexture* m_pTextureCom = { nullptr };

	CVIBuffer_AirPlane* m_pVIBuffer_Com = { nullptr };
	//TODO:용수 총알충돌처리 테스트
	CCollider* m_pCollider_Com[COLLIDER_END] = { nullptr };
	CTexture* m_pTextureCollider = { nullptr };
	CTransform* m_pCameraTransform = { nullptr };

	STATE_BOSS_BATTLECRUISER m_eNowState = { STATE_SPON }; // 현재 상태 처음 상태 SPON 고정

	// 죽음 관련 변수
	_bool m_bDeadShow = { false }; // 죽는 모션을 모두 보여줬는지?

	// Idle 상태 관련 변수

	_float m_fIdleDynamic = { 0.2f };
	_float m_fIdleTime = { 2.f };
	_float m_fIdleAngle = { 0.f };

	// Move 관련 함수
	_float m_fMoveDistance = { 5.f };


	//몬스터 설정값
	class CState* m_pState_Com = { nullptr };
	_float m_fTime = { 0.f }; // 시간제어


public:
	static CBossBattleCruiser* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END