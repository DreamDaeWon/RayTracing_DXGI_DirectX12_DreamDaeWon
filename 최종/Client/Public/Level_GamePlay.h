#pragma once
#include "Level.h"
#include "Client_Defines.h"


BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
private:
	CLevel_GamePlay(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel_GamePlay() = default;


public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_Player(const wstring& strLayerTag, void* pArg = nullptr);
	HRESULT Ready_Layer_Monster(const wstring& strLayerTag, void* pArg = nullptr);
	HRESULT Ready_Land_Object();
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround_Parsing();
	HRESULT Ready_Layer_Skill(const wstring& strLayerTag);
	HRESULT Ready_Layer_Effect(const wstring& strLayerTag);
	HRESULT Ready_Layer_Weapon(const wstring& strLayerTag); //TODO: 용수 웨폰생성용
	HRESULT Ready_Layer_UI_Player(const wstring& strLayerTag);
	HRESULT Ready_Layer_QAim(const wstring& strLayerTag);
	HRESULT Ready_Layer_Trigger();
	HRESULT Ready_Layer_Trigger_Door(const wstring& strLayerTag);
	HRESULT Ready_Layer_Camera_Player(const wstring& strPlayerLayerTag, const wstring& strCameraLayerTag);
	HRESULT Ready_Layer_Collision(const wstring& strLayerTag);
	HRESULT Ready_Layer_Item(const wstring& strLayerTag);

	void Load_Object_Box();
	void Load_Terrain();
	void Load_Wall();
	void Load_GuardRail();

private:
	_uint m_iMonsterNum = { 0 };
//5private:
	//LPDIRECT3DDEVICE9 m_pGraphic_Device = { nullptr };

public:
	static CLevel_GamePlay* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
};

END