#include "LandObject.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CCollider;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)

class CBossYeti final : public CLandObject
{
public:
	typedef struct tagBoss_Yeti_Desc : public CLandObject::LANDOBJECT_DESC {

	}Boss_Yeti_DESC;

	// 몬스터의 상태 값
	enum STATE_BOSS_YETI { STATE_IDLE, STATE_SKILL_READY, STATE_ROLL, STATE_GOBOTTOM ,STATE_JUMP_ATTAK, STATE_JUMP_ATTAK_END, STATE_ICE_SPLINTER, STATE_DEATH, STATE_SCENE_READY, STATE_SCENE, STATE_SCENE_END, STATE_END };

private:
	CBossYeti(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBossYeti(const CBossYeti& rhs);
	virtual ~CBossYeti() = default;

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
	_bool MousePicking();
	//TODO:용수 총알충돌처리 테스트
	void Collision_Bullet(_float fTimeDelta);
	void Collision_Wall();
	//void Collision_Player();

	// 스킬 구현
	//자르반 궁을 위한 함수들
	void Skill_Jump_Attak(_float fTimeDelta); // 자르반 궁

	//구르기 함수
	void Skill_Roll_Attak(_float fTimeDelta);

	//파고 들어서 구 흩뿌리기
	void Skill_GoBottom(_float fTimeDelta);

	//충격파 얼음가시 솟구치기
	void Skill_UpIceSplinter(_float fTimeDelta);


	// 컷씬을 위한 함수
	void Set_Parabola(); // 역탄도 공식을 이용한 포물선 함수
	void Cut_Scene(_float fTimeDelta); // 컷씬 행동
	void Move_Scene(); // 점프 포물선 그리는 함수

private:
	// 이전틱의 시간값
	_float m_fTimeBeforeTimeDelta = { 0.f };// 이전틱의 시간값


	// 포물선 공식에 필요한 변수
	_float3 m_vBeforePos = {50.f,1000.f,50.f}; // 스킬 시작했을 때의 초기지점

	_float3 m_vBefore_Jump_Pos = {};
	_float3 m_vEndPos = {25.f, m_fScale.y * 0.5f, 25.f}; // 스킬의 목적지 지점
	_float3 m_NowPos = {}; // 현재 위치
	_float m_fV_X = {}; // X축으로의 속도
	_float m_fV_Y = {}; // Y축으로의 속도
	_float m_fV_Z = {}; // Z축으로의 속도

	_float m_fG = {}; // Y축으로의 중력가속도
	_float m_fEndTime = {}; // 도착지점까지 도달 시간
	_float m_fMaxHeight = { 1100.0f };// 최대 높이
	_float m_fHeight = {}; // 최대 높이Y- 시작지점높이의 Y
	_float m_fEndHight = {}; // 도착지점 높이 Y - 시작지점 높이 Y
	_float m_fTime = { 0.f }; // 흐르는 시간
	_float m_fMaxTime = { 3.f }; // 최대높이 까지 가는 시간


	// 점프공격
	_float3 m_vGoPos = {}; // 포물선을 그릴 때 포물선 공식을 계속 더해줄 값
	_float m_fJumpDistance = { 8.f }; // 점프할 거리
	_float m_fNowJumpDistance = { 0.f }; // 현재까지 점프한 거리

private: // 컷신을 위한 변수
	_bool m_bCutScene = { false }; //컷씬을 보여줬는지?

	_bool m_bShowEndCutScene = { false }; // 엔딩 컷씬을 보여주는 중인지?

	_bool m_bEndMusic = { false }; // 엔딩음악을 틀었는지?


private: // 스킬 사용을 위한 변수들
	// 스킬 준비 시간
	_float m_fSkillReadytime = { 1.5f };


	// 자르반 궁을 위한 변수
	//_float3 m_vBeforePos = {}; // 자르반 스킬 시작했을 때의 초기지점
	_float m_fJump_Attak_Speed = { 1.f }; // 자르반 속도
	_float m_fJump_Attak_Speed_final = { 1.f }; // 자르반 속도
	_float m_fJump_Attak_Time = { 2.5f };
	_float3 m_vJump_Attak_Final_Pos = {}; // 최종 도착지점

	// 구르기 스킬을 위한 변수
	_float3 m_vSetRollDir = {}; // 구르는 방향벡터
	_float m_fRollSpeed = { 25.f }; // 구르기 속도
	_float m_fSnowTime = { 0.f }; // 눈 던지는 시간

	// 들어갔다 나오기 스킬
	_float m_fGoBottomTime = { 0.f }; // 들어가는 시간.
	_float m_fGoBottomSpeed = { 15.f }; // 들어가는 속도
	_uint m_iBottomNum = { 2 }; // 몇 번 땅에 들어갈껀지?
	_uint m_iBottomNumPlus = { 0 }; // 지금 몇번이나 땅에 들어갔는지?
	_uint m_iGoBottom = { 0 }; // 스킬의 일련의 과정
	_bool m_bSnowBottom = { false }; //공을 던졌는지?
	_bool m_bGoBottom = { false }; // 아래로 파고 들고 있는 중인지??
	_bool m_bSoundUp = {false}; // 올라올 때의 소리를 냈는지?

	//아이스 가시 스킬
	_bool m_bIceSplinterSkill = { false };

	list<class CRedCircle*> m_RedCirclelist; //  빨간 원들을 관리해줄 벡터
private:

	//_bool m_bCheckPlayer = { false }; // 몬스터가 플레이어를 인식했는지에 대한 bool값
	//_float m_fCheckDestence = { 20.f }; // 몬스터가 플레이어를 인식하는 범위
	_float3 m_vMonsterLook = { 0.f, 0.f, 1.f }; // 몬스터가 게임 상에서 바라보는 룩벡터

	CTexture* m_pTextureCom = { nullptr };

	// 현재 단계의 카메라 객체
	class CCamera_Player_DW* m_pCamera = { nullptr };
	CTransform* m_pDWCameraTransform = { nullptr };


	// 각각의 상태에 따른 컨테이너를 넣어 줌
	CTexture* m_pTextureIdle = { nullptr };
	CTexture* m_pTextureSkillReady = { nullptr };
	CTexture* m_pTextureRoll = { nullptr };
	CTexture* m_pTextureJumpAttak = { nullptr };
	CTexture* m_pTextureJumpAttakEnd = { nullptr };
	CTexture* m_pTextureDeath = { nullptr };
	CTexture* m_pTextureGoBottom = { nullptr };
	CTexture* m_pTextureUpBottom = { nullptr };


	CVIBuffer_Rect* m_pVIBuffer_Com = { nullptr };
	//TODO:용수 총알충돌처리 테스트
	CCollider* m_pCollider_Com[COLLIDER_END] = {nullptr};
	CTexture* m_pTextureCollider = { nullptr };

	STATE_BOSS_YETI m_eNowState = { STATE_IDLE }; // 현재 상태

	// 죽음 관련 변수
	_bool m_bDeadShow = { false }; // 죽는 모션을 모두 보여줬는지?


	// 구르기 관련 변수
	_float m_fRollTime = { 0.f }; // 구르기 시간에 사용할 값
	_float m_fRollCoolTime = { 3.f }; // 구르기 쿨타임 값
	_float m_fRollGo = { 2.0f }; // 총 구르기 시간
	_bool m_bTurnRight = { true };

	// 텍스쳐 관련 변수
	_float m_fTextureTotal = { 0.f }; // 텍스쳐 사진 로드할 때 넣어줄 값
	_float m_fTexture = { 0.f }; // 몇 번째 사진을 사용할 건지?
	_float m_fTextureDir = { 0.f }; // 텍스쳐 방향을 알려줌
	_float m_fTextureMaxPicture = { 0.f }; // 텍스쳐가 한루프를 돌 때의 몇 개의 사진이 쓰이는지
	_float m_fTextureSpeed = { 0.5f }; // 텍스쳐가 다음사진으로 넘어가는 속도

	// 1초에 몇 번 돌릴건지?
	_float m_fTextureSpeed_Idle = { 1.f }; // Idle 상태일 때 텍스쳐 넘어가는 속도
	_float m_fTextureSpeed_SkillReady = { 0.25f }; // Move 상태일 때 텍스쳐 넘어가는 속도
	_float m_fTextureSpeed_Roll = { 1.f }; // Roll 상태일 때 텍스쳐 넘어가는 속도
	_float m_fTextureSpeed_JumpAttak = { 0.25f }; // JumpAttak 상태일 때 텍스쳐 넘어가는 속도
	_float m_fTextureSpeed_JumpAttakEnd = { 0.5f }; // JumpAttakEnd 상태일 때 텍스쳐 넘어가는 속도
	_float m_fTextureSpeed_SkillSplinter = { 1.0f }; // SkillSplinter 상태일 때 텍스쳐 넘어가는 속도
	_float m_fTextureSpeed_Death = { 0.13f }; //Death 상태일 때 텍스쳐 넘어가는 속도

	//몬스터 설정값
	class CState* m_pState_Com = { nullptr };

	_float3 m_fScale = {5.f,5.f,1.f}; // 보스의 크기
	


	//_float m_fTime = { 0.f }; // 시간제어


public:
	static CBossYeti* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END