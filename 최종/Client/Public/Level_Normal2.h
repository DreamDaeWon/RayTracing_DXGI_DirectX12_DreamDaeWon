#pragma once
#include "Level.h"
#include "Client_Defines.h"


BEGIN(Client)

class CLevel_Normal2 final : public CLevel
{
public:
	enum SUMMON { SUMMON_NOTHING, SUMMON1, SUMMON2, SUMMON_TERM, SUMMON3, SUMMON4, SUMMON5, SUMMON6, SUMMON_END };

private:
	CLevel_Normal2(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel_Normal2() = default;


public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround_Parsing();
	HRESULT Ready_Layer_UI_Player(const wstring& strLayerTag);
	HRESULT Ready_Layer_Monster(const wstring& strLayerTag, void* pArg = nullptr);
	HRESULT Ready_Land_Object();
	HRESULT Ready_Layer_Trigger();
	HRESULT Ready_Layer_Trigger_Door(const wstring& strLayerTag);
	HRESULT Ready_Layer_Trigger_Summon(const wstring& strLayerTag);
	HRESULT Ready_Layer_Trigger_Camera(const wstring& strLayerTag);

	HRESULT Ready_Layer_QAim(const wstring& strLayerTag);
	HRESULT Ready_Layer_Effect(const wstring& strLayerTag);

	void Summon();
	void Camera();
	void Col_Player_Summon();
	void Col_Player_Camera();
	void Check_Monster_Num();
	void Load_Terrain();
	void Load_Wall();
	void Load_GuardRail();
	void Load_Object();	
	void Load_Monster(const wstring& strLayerTag, _uint iFileNum);

	void Load_Steam1(const wstring& strLayerTag);
	void Load_Steam2(const wstring& strLayerTag);
	void Load_Steam3(const wstring& strLayerTag);
	void Load_Steam4_1(const wstring& strLayerTag);
	void Load_Steam4_2(const wstring& strLayerTag);
	void Load_Steam5(const wstring& strLayerTag);
	void Load_Steam6(const wstring& strLayerTag);

private:

	_bool m_bActive = { false };
	SUMMON m_eCurPettern = { SUMMON_NOTHING };
	SUMMON m_ePrePettern = { SUMMON_NOTHING };
	_uint m_iMonsterNum = { 0 };
	_int m_iGoalMonsterNum = { 0 };

public:
	static CLevel_Normal2* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
};

END