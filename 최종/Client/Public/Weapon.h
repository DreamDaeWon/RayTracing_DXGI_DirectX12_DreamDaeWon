#pragma once
#include "GameObject.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
class CCollider_Rect;
END

BEGIN(Client)
/* 총 이미지 분석
0-6 등에 매달고 있을 때 사용하는 이미지
12-17?  조준 했을 때 반동표현하는 이미지
*/
class CWeapon abstract : public CGameObject
{
	//public:
	//	typedef struct tagEffectSplatDesc : public CEffect_Base::EFFECT_BASE_DESC {
	//
	//	}EFFECT_SPLAT_DESC;

public:
	typedef struct tagWeaponBulletInfo
	{
		_float fCP = { 0.f };
		_uint iNowBulletNum = { 0 };
		_uint iMaxBulletNum = { 0 };
		_uint iExtraBulletNum = { 0 };
	}WEAPON_BULLET_INFO;

	/*typedef struct tagBulletCollisionInfo
	{
		_float fCP = { 0.f };
	}BULLET_COLLISION_INFO;*/

public:
	enum STATE { STATE_IDLE, STATE_AIMING, STATE_SHOOTING, STATE_END };
	enum WEAPONID {
		WEAPON_PUPA, WEAPON_RAMBO, WEAPON_SASIN,
		WEAPON_SPARROW, WEAPON_SHARKBLOOD, WEAPON_ATLAS,
		WEAPON_EXODUS, WEAPON_FLAMESHAKER, WEAPON_GUNGNIR,
		WEAPON_PALGYE, WEAPON_TSURAR, WEAPON_VINOPAINTER
	};

protected:

	enum BULLETID { BULLET_RIFLE, BULLET_SHOTGUN, BULLET_SASIN, BULLET_SHARKBLOOD };
	enum WEAPON_SLOT { FIRST_WEAPON, SECOND_WEAPON, THIRD_WEAPON, WEAPON_SLOT_END };
	enum WEAPON_SHOOTING{ SHOOTING_AUTO, SHOOTING_SEMIAUTO, SHOOTING_BURST, SHOOTING_END};
	// SEMIAUTO: 단발
	// AUTO: 연사
	// BURST: 3점사
protected:
	CWeapon(LPDIRECT3DDEVICE9 pGraphic_Device);
	CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Shot_Bullet() = 0;
	void Set_Idle();
	void Set_Frame(_float fFrame) { m_fFrame = fFrame; }
	_float3 Get_Pos() { return m_pTransform->Get_State(CTransform::STATE_POSITION); }
	_uint Get_ID() { return m_eID; } // 총 enum타입 반환
	_uint Get_BulletID() { return m_eBulletID; }
	_float Get_ShootingTerm() { return m_fShootingTerm; }
	_uint Get_ShootingWay() { return m_eShooting_Way; }
	
	void Set_Activate(_bool bAct) { m_bActivate = bAct; }
	_float Get_Gauge() { return m_fGauge; }
	_float Get_ReboundSize() { return m_fReboundSize; }
	_float Get_MaxGauge() { return m_fMaxGauge; }
	_bool IsThatEx() { return m_bEx; }
	void Set_State(STATE eState) { m_eCurState = eState; }
	void Set_SlotIndex(_uint iIndx) { m_iSlotIndex = iIndx; }
	void Set_PlayerTexture(_float fTex) { m_fTextureDir = fTex; }
	void Plus_ExBullet(_uint iPlusBullet) { m_iExtraBulletNum += iPlusBullet; }
	void Plus_ExBullet() { m_iExtraBulletNum += m_iMaxBulletNum; }
	void Plus_GunGauge(_float fPlusGauge);
	
	void Reset() { m_iNowBulletNum = m_iMaxBulletNum; m_iExtraBulletNum = m_iMaxBulletNum; };
	WEAPON_BULLET_INFO Get_Weapon_Bullet_Info()
	{
		WEAPON_BULLET_INFO tWeaponBulletInfo = {};
		tWeaponBulletInfo.fCP = m_fCP;
		tWeaponBulletInfo.iNowBulletNum = m_iNowBulletNum;
		tWeaponBulletInfo.iMaxBulletNum = m_iMaxBulletNum;
		tWeaponBulletInfo.iExtraBulletNum = m_iExtraBulletNum;
		return tWeaponBulletInfo;
	}
	void Set_Weapon_Bullet_Info(_uint iNowBullet, _uint iExtraBullet)
	{
		m_iNowBulletNum = iNowBullet;
		m_iExtraBulletNum = iExtraBullet;
	}
	void Set_QAimSize(_uint iQAimSize) { m_iQAimSize = iQAimSize; }
	_uint Get_QAimSize() { return m_iQAimSize; }
	void Reload(); //재장전 함수
	
	//void Collision_Bullet_Rect(class CCollider* pCollider, _float fTimeDelta);
	//콜라이더 렉트 포인터 넘겨주면 이 총에 있는 모든 불릿과의 충돌처리를 해줌?
	//충돌된 총알의 총 대미지를 반환
	virtual _float Collision_Bullet_Rect(class CCollider_Rect* pCollider_Rect, _float fTimeDelta);
	virtual void Collision_Bullet_Rect_For_Box(class CCollider_Rect* pCollider_Rect, _float fTimeDelta);

	void Get_World_Mouse_Ray_Shot_Grouping(_Out_ _float3* vRayDir, _Out_ _float3* vRayPos); //탄착군을 고려한 마우스 레이를 반환

protected:
	virtual HRESULT Add_Components() = 0;
	virtual HRESULT Clone_Bullet()=0;

	HRESULT Set_RenderState();
	HRESULT Reset_RenderState();

	void SetUp_BillBoard();
	void SetUp_BillBoard_Camera();

	void Motion_Change();


protected:
	_bool					m_bActivate = { false };
	_bool					m_bEx = { false };
	_bool					m_bReLoad = { false };
	_uint					m_iCameraMode = { 0 };
	_float				m_fGauge = { 0.f }; //
	_float				m_fReboundSize = {0.f};
	_float				m_fMaxGauge = { 200.f };//Initialize에서 반드시 정해줘야함
	WEAPON_SHOOTING m_eShooting_Way = { SHOOTING_AUTO };
	WEAPONID   m_eID = {};//Initialize에서 반드시 정해줘야함
	_float m_fTerm = { 0.f }; //총 연사 속도
	BULLETID		m_eBulletID = {};
	_float m_fShootingTerm = { 0.f }; //Initialize에서 반드시 정해줘야함

	_uint				m_iSlotIndex = { WEAPON_SLOT_END };
	_float			m_fTextureDir = { 0.f };

	_float3			m_vScale = {};

	CTexture* m_pTextureCom = { nullptr }; // 일반 모드
	CTexture* m_pTextureCom1 = { nullptr }; // Ex 모드
	CVIBuffer_Rect* m_pVIBuffer_Com = { nullptr };

	class CCamera_Player_DW* m_pCamera = { nullptr };
	CTransform* m_pCameraTransform = { nullptr };

	_float m_fFrame = { 0.f };
	
	STATE m_eCurState = { STATE_IDLE };
	STATE m_ePreState = { STATE_END };

	_uint m_iQAimSize = { 32 }; //큐 에임 사이즈(탄착군을 위한 변수) 총마다 큐 에임 다르게 해서 그 안의 공간에 총알이 박히게 하기 위함


protected:
	//총알 30개 리스트 들고 있기.
	_float m_fCP = { 0.f }; //공격력. 현재 장착하고 있는 총의 공격력을 플레이어에 세팅 해줌.
	_uint m_iNowBulletNum = { 0 }; //현재 탄창에서의 남은 총알 개수 1~MaxBulletNum, 0일때는 발사 안됨.
	_uint m_iMaxBulletNum = { 0 }; // 한 탄창의 최대 개수
	_uint m_iExtraBulletNum = { 0 }; // 현재 탄창을 제외한 남은 총알 개수

	vector<class CBullet*> m_vecBullet = {}; //랜덤엑세스를 위한 벡터 컨테이너

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END

