#pragma once
#include "Item.h"

BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
class CCollider;
END

BEGIN(Client)

class CItem_Weapon final : public CItem
{
public:
	typedef struct tagWeaponItem :public CItem::ITEM_DESC
	{
		_uint iNowBullet = { 0 };
		_uint iExtraBullet = { 0 };
		_uint iWeaponID = { 0 };
		_bool bPlayerDrop = { false };
	}WEAPON_ITEM_DESC;
	enum ITEM_WEAPON { ITEM_PUPA, ITEM_RAMBO, ITEM_SASIN, ITEM_SPARROW, ITEM_SHARKBLOOD, ITEM_ATLAS, ITEM_WEAPON_END };

private:
	CItem_Weapon(LPDIRECT3DDEVICE9 pGraphic_Device);
	CItem_Weapon(const CItem_Weapon& rhs);
	virtual ~CItem_Weapon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void Set_Parabola(); // 역탄도 공식을 이용한 포물선 함수
	void Jump(_float fTimeDelta); // 포물선 그리면서 날아가는 함수

	HRESULT Add_Components();
	HRESULT Set_RenderState();
	HRESULT Reset_RenderState();

	void SetUp_BillBoard();
	void Item_Collision(_float fTimeDelta);

private:
	// 포물선 공식에 필요한 변수
	_float3 m_vGoPos = {};
	_float3 m_vBeforePos = {}; // 스킬 시작했을 때의 초기지점
	_float3 m_vEndPos = {}; // 스킬의 목적지 지점
	_float3 m_NowPos = {}; // 현재 위치
	_float m_fV_X = {}; // X축으로의 속도
	_float m_fV_Y = {}; // Y축으로의 속도
	_float m_fV_Z = {}; // Z축으로의 속도

	_float m_fG = {}; // Y축으로의 중력가속도
	_float m_fEndTime = {}; // 도착지점까지 도달 시간
	_float m_fMaxHeight = { 1.f };// 최대 높이
	_float m_fHeight = {}; // 최대 높이Y- 시작지점높이의 Y
	_float m_fEndHight = {}; // 도착지점 높이 Y - 시작지점 높이 Y
	_float m_fTime = { 0.f }; // 흐르는 시간
	_float m_fMaxTime = { 0.2f }; // 최대높이 까지 가는 시간
	_bool m_bJump = { false }; // 점프를 뛸지 말지 알려주는 불변수
	_bool m_bPlayerDrop = { false };// 플레이어가 던진건지 아닌지 알려주는 불변수

	_float m_fScale = { 0.f }; // 크기

	_float m_MoveDistance = { 0.5f }; // 얼마나 앞에까지 포물선을 그리면 갈껀지?


	_float	m_fFrame= { 0.f };
	_uint			m_iFrame = { 0 };
	_uint			m_iNowBullet = {0};
	_uint			m_iExtraBullet = {0};

	CTransform* m_pCameraTransform = { nullptr };
	class CItem_Effect* m_pItem_Effect = { nullptr };

public:
	static CItem_Weapon* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

