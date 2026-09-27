#pragma once
#include "LandObject.h"
#include "Client_Defines.h"
#include "random"


BEGIN(Engine)
class CCollider_Cube_AABB;
class CCollider_Rect;
class CCollider;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)

class CMonster_Three_Red_Eyes final : public CLandObject
{
public:
	typedef struct tagMonster_Three_Red_Eyes_Desc : public CLandObject::LANDOBJECT_DESC {

	}MONSTER_Three_Red_Eyes_DESC;

	// 몬스터의 상태 값
	enum STATE_THREE_RED_EYES { STATE_IDLE, STATE_MOVE, STATE_ROLL, STATE_GUNBATTLE, STATE_DEATH, STATE_END };

private:
	CMonster_Three_Red_Eyes(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMonster_Three_Red_Eyes(const CMonster_Three_Red_Eyes& rhs);
	virtual ~CMonster_Three_Red_Eyes() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Return_Look_Position(_float3* pvLook, _float3* pvPosition);

private:
	void Set_State(_float fTimeDelta); // 상태값을 정해 줌
	void Do_State(_float fTimeDelta); // 상태값을 정해 줌
	void Set_Render_Texture(_float fTimeDelta); // 어떤 사진을 출력하고 어떤 숫자로 출력할 지 정해 줌
	void CheckPlayer(); // 플레이어가 인식범위에 들어왔는지?
	HRESULT Add_Components();
	HRESULT Set_RenderState();
	HRESULT Reset_RenderState();
	void SetUp_BillBoard();
	void Look_Camera_Fixed_Y_Axis();
	void Chase_Player(_float fTimeDelta, _float Min_Distance);
	_bool MousePicking(); // 마우스가 이 몬스터를 지정하면
	//TODO:용수 총알 발사함수
	void Shot_Bullet();
	//TODO:용수 총알충돌처리 테스트
	void Collision_Bullet(_float fTimeDelta);
	void Collision_Wall();

	// 준수 추가
	HRESULT Drop_Item();


private:



	_bool m_bCheckPlayer = { false }; // 몬스터가 플레이어를 인식했는지에 대한 bool값
	_float m_fCheckDestence = { 5.f }; // 몬스터가 플레이어를 인식하는 범위
	_float3 m_vMonsterLook = { 0.f, 0.f, 1.f }; // 몬스터가 게임 상에서 바라보는 룩벡터

	CTexture* m_pTextureCom = { nullptr };

	// 각각의 상태에 따른 컨테이너를 넣어 줌
	CTexture* m_pTextureIdle = { nullptr };
	CTexture* m_pTextureMove = { nullptr };
	CTexture* m_pTextureRoll = { nullptr };
	CTexture* m_pTextureGunBattle = { nullptr };
	CTexture* m_pTextureDeath = { nullptr };

	CVIBuffer_Rect* m_pVIBuffer_Com = { nullptr };
	//TODO:용수 총알충돌처리 테스트
	CCollider* m_pCollider_Com[COLLIDER_END] = { nullptr };
	CTexture* m_pTextureCollider = { nullptr };
	CTransform* m_pCameraTransform = { nullptr };

	STATE_THREE_RED_EYES m_eNowState = { STATE_IDLE }; // 현재 상태

	// 죽음 관련 변수
	_bool m_bDeadShow = { false }; // 죽는 모션을 모두 보여줬는지?

	// Idle 상태 관련 변수

	_float m_fIdleTime = { 0.f };

	// Move 관련 함수
	_float m_fMoveDistance = { 5.f };

	// 구르기 관련 변수
	_float m_fRollTime = { 0.f }; // 구르기 시간에 사용할 값
	_float m_fRollingTime = { 0.5f }; // 몇초동안 구를건지?
	_float m_fRollCoolTime = { 3.f }; // 구르기 쿨타임 값
	_float m_fRollGo = { 4.f }; // 구르는 거리
	_bool m_bTurnRight = { true };

	// 총쏘기 관련 변수
	_float m_fAttakTime = { 1.0f }; // 총쏘는 모션에서의 발사 쿨타임
	_uint m_iAttakNum = { 0 }; // 현재 총을 얼마나 발사했는지?



	// 텍스쳐 관련 변수
	_float m_fTextureTotal = { 0.f }; // 텍스쳐 사진 로드할 때 넣어줄 값
	_float m_fTexture = { 0.f }; // 몇 번째 사진을 사용할 건지?
	_float m_fTextureDir = { 0.f }; // 텍스쳐 방향을 알려줌
	_float m_fTextureMaxPicture = { 0.f }; // 텍스쳐가 한루프를 돌 때의 몇 개의 사진이 쓰이는지
	_float m_fTextureSpeed = { 0.5f }; // 텍스쳐가 다음사진으로 넘어가는 속도

	// 1초에 몇 번 돌릴건지?
	_float m_fTextureSpeed_Idle = { 1.f }; // Idle 상태일 때 텍스쳐 넘어가는 속도
	_float m_fTextureSpeed_Move = { 1.f }; // Move 상태일 때 텍스쳐 넘어가는 속도
	_float m_fTextureSpeed_Roll = { 2.f }; // Roll 상태일 때 텍스쳐 넘어가는 속도
	_float m_fTextureSpeed_GunBattle = { 0.2f }; // GunBattle 상태일 때 텍스쳐 넘어가는 속도
	_float m_fTextureSpeed_Death = { 0.5f }; //Death 상태일 때 텍스쳐 넘어가는 속도

	//몬스터 설정값
	class CState* m_pState_Com = { nullptr };


	_float m_fMoveSpeed = { 1.f }; // 이동속도
	_float m_fRollSpeed = { 3.f }; // 구르기 속도

	_float m_fTime = { 0.f }; // 시간제어


public:
	static CMonster_Three_Red_Eyes* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END