#pragma once
#include "LandObject.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CVIBuffer_Sphere;
class CVIBuffer_Rect;
class CCollider;
class CTexture;
END

BEGIN(Client)

class CPlayer final : public CLandObject
{
public:
	typedef struct tagPlayer_Desc : public CLandObject::LANDOBJECT_DESC {
		CTransform* pTransform_Aim = {};
	}PLAYER_DESC;


	//TODO: 예은 24.01.16 상태 추가
	enum STATE { STATE_IDLE, STATE_IDLE_WALK, 
		STATE_AIMING, STATE_AIMING_WALK,
		STATE_AIMING_ZOOM, 
		STATE_ROLL,
		STATE_DEAD, STATE_HIT, STATE_END };
private:
	enum AIMING{ AIMING_NOTHING, AIMING_SHOOT, AIMING_GUN_CHANGE, AIMING_END};
	enum WALK{WALK_STRIAGHT, WALK_BACK};
	enum SKILL_SLOT { SPACE_SKILL, SHIFT_SKILL, Q_SKILL, F_SKILL, SKILL_SLOT_END };
	enum WEAPON_SLOT { FIRST_WEAPON, SECOND_WEAPON, THIRD_WEAPON, WEAPON_SLOT_END };
	//enum PARTS{PARTS_HEAD, PARTS_BODY, PARTS_HEAD, PARTS_END};
private:
	CPlayer(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	STATE Get_CurState() { return m_eCurState; }
	
	// 대원추가 함수 마우스 포지션 반환
	_float3 Get_MousePos() { return m_vMousePos; }
	_float3* Get_LookPos() { return &m_vPlayerLook; }
	_float* Get_PlayerSP() { return &m_fSp; }
	_float* Get_PlayerMaxSP() { return &m_fMaxSp; }
	//TODO: 예은 24.01.19 스킬 쿨타임 제어, 슬롯 반환
	_float Get_Cool_Time(_uint eKey ) { return  m_pSkillCoolTime[eKey]; }
	class CPlayer_Skill* Get_Skill(_uint eKey) { return m_pSkillSlot[eKey]; }
	class CWeapon* Get_Weapon(_uint eKey) { return m_pWeaponSlot[eKey]; }
	_uint Get_CurWeapon() { return m_iSelectWeapon; }
	
	void Set_Hit(_bool bHit) { m_bHit = bHit; }
	void Set_Invincibility(_bool bInv) { m_bInvincibility= bInv; }
	void Set_InvincibilityTime(_float fInv) { m_fInvincibilityMaxTime= fInv; }
	void Set_WeaponChange(_uint iWeapon, _uint iExtraBullet, _uint iNowBullet);
	HRESULT Drop_Item(_uint iIndex);

	//void Get_Cool_Time(_Out_ _float* pSkillCoolTime) { pSkillCoolTime = m_pSkillCoolTime; }
	//void Get_Skill_Slot(_Out_ class CPlayer_Skill* pSkillSlot[]) { pSkillSlot = m_pSkillSlot;}//슬롯 반환
	//void Get_Weapon_Slot(_Out_ class CWeapon* pWeaponSlot[]) { pWeaponSlot = m_pWeaponSlot; } //슬롯 반환
	//class CWeapon* Get_Weapon_Slot() { return (CWeapon*)&m_pWeaponSlot[FIRST_WEAPON]; } //슬롯 반환

public:
	void Return_Look_Position(_float3* pvLook, _float3* pvPosition);
	void Look_At(const _float3& vAt);
	_float3 Return_ViewPort_Pos();

	void SetGameLook(_float3 _GameLook) { m_vPlayerLook = _GameLook; }

	void Collider_Update();

private:
	HRESULT Add_Components();
	HRESULT Set_RenderState();
	HRESULT Reset_RenderState();
	HRESULT Motion_Change(_float fTimDelta);
	//TODO: 예은 - 에임 쳐다보게
	void		Look_At_Aim();
	void SetUp_BillBoard();
	void SetUp_BillBoardToUP();
	void SetUp_BilBoardToAiming();

	_float3 Compute_MovePos();
	//TODO: 용수 총알 발사 함수
	void Shot_Bullet();
	//TODO: 용수 벽 충돌
	
	//대원 머지 : 혹시 몰라서 추가해둠 필요하면 주석을 풀어서 사용할 
	//void Collider_Update();
	
	void Collision(_float fTimeDelta);
	void Collision_Wall();
	void Collision_Trigger();
	void Collision_Door();
	//void Collision_Box(_float fTimeDelta);
	void Collision_Monster_Bullet(_float fTimeDelta);
	//TODO: 용수 발 먼지
	void Effect_Player_Move(_float fTimeDelta);
	//TODO: 용수 키 입력받아서 수행해야하는 기능 모아놓은 함수
	void Key_Input(_float fTimeDelta);


	//TODO: 예은 파츠 텍스쳐 출력용
	HRESULT Render_Again(_float fSizeX, _float fSizeY,_float fX, _float fY, _uint iFrame, CTexture* pTexture);

private:
	void Mouse_Axis_Turn(_float fTimeDelta);
	void Landing_Terrain();

private:
	_bool			m_bFirstAim = true;
	_bool			m_bFirstReLoad = true;
	_bool			m_bChange = false;
	_bool			m_bRollDone = true;
	_bool			m_bInvincibility = false; //무적
	_bool			m_bHit = false;
	_bool			m_bLevelChange = false;
	_uint			m_i3Shooting = 0;
	_float			m_fHitTime = { 0.f };
	CVIBuffer_Rect* m_pVIBuffer_Com = { nullptr };
	//TODO: 예은 추가
	class CState* m_pStateCom = { nullptr };
	//TODO: 용수 For. Collision
	CCollider* m_pCollider_Com[COLLIDER_END] = {};
	
	class CEffect_Player_Move* m_pEffectPlayerMove = { nullptr }; //플레이어 발걸음을 위한 클래스

	CTransform* m_pDWCameraTranformCom = { nullptr }; //카메라 트랜스폼

	//대원 추가
	_float3 m_vMousePos = {};

	_float3 m_vPlayerLook = {0.f,0.f,1.f};

	_float m_fTextureTotal = { 0.f }; // 텍스쳐 사진 로드할 때 넣어줄 값
	_float m_fTexture = { 0.f }; // 몇 번째 사진을 사용할 건지?
	_float m_fTextureDir = { 0.f }; // 텍스쳐 방향을 알려줌
	_float m_fTextureMaxPicture = { 0.f }; // 텍스쳐가 한루프를 돌 때의 몇 개의 사진이 쓰이는지
	_float m_fTextureSpeed = { 0.5f }; // 텍스쳐가 다음사진으로 넘어가는 속도


	_float4x4					m_ViewMatrix, m_ProjMatrix;
	_float						m_fOrthoBodyFrame = { 0.f }; //몸 프레임
	_float						m_fOrthoHeadFrame = { 0.f }; // 얼굴 프레임
	_float						m_fOrthoHandFrame = { 0.f }; // 팔 프레임
	_float						m_fOrthoTPSFrame = { 0.f }; // 총 교체 프레임
	_float						m_fInvincibilityTime = { 0.f };
	_float						m_fInvincibilityMaxTime = { 0.f };

	//플레이어 텍스쳐
	CTexture* m_pTextureCom = { nullptr }; //여분
	CTexture* m_pTextureIdle = { nullptr };
	CTexture* m_pTextureMove = { nullptr };
	CTexture* m_pTextureRoll = { nullptr };
	CTexture* m_pTextureDeath = { nullptr };

	CTexture* m_pTextureTPS = { nullptr }; // 총교체
	CTexture* m_pTextureTPS_Body = { nullptr }; //몸
	CTexture* m_pTextureTPS_Head = { nullptr };//얼굴
	CTexture* m_pTextureTPS_Hand = { nullptr };//얼굴
	
	_uint			m_iSelectWeapon = { THIRD_WEAPON };
	//_uint			m_iSelectedWeapon = { THIRD_WEAPON };

	/*_float3 m_vPreMousePosition = { 0.f,0.f,0.f };
	_float3 m_vCurrentMousePosition = { 0.f,0.f,0.f };*/
	POINT			m_ptMouse = {};//

	STATE							m_eCurState = { STATE_IDLE };
	STATE							m_ePreState = { STATE_END };

	AIMING							m_eCurAiming = { AIMING_NOTHING}; // 에임하면서 뭘하고 있냐
	AIMING							m_ePreAiming = { AIMING_NOTHING };
	
	WALK							m_eWalk = { WALK_STRIAGHT };
	
	_float3							m_vTargetPos = {};
	_float							m_fTime = { 0.f};
	_float							m_fTimeForfoot = { 0.f};

	_float							m_fSp = { 200.f };
	_float							m_fMaxSp = { 200.f };
	_float							m_pSkillCoolTime[SKILL_SLOT_END] = {0.f};
	class CPlayer_Skill*			m_pSkillSlot[SKILL_SLOT_END] = { nullptr };
	class CWeapon*					m_pWeaponSlot[WEAPON_SLOT_END] = { nullptr };
	class CCamera_Player_DW* m_pCamera = { nullptr };

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END