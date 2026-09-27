#include "pch.h"
#include "GameInstance.h"
#include "Level_GamePlay.h"
#include "Level_Loading.h"

#include "Camera_Player.h"
#include "Camera_Player_DW.h"
#include "Camera_Free.h"
#include "Player.h"
#include "MonsterArlertUI.h"
#include "QAim.h"
#include "UI_Player.h"
#include "State.h"
#include "Effect_Splat.h"
#include "Bullet.h"
#include "Object_Box.h"
#include "Terrain.h"
#include "Wall.h"
#include "UI_Player_Skill.h"
#include "UI_Boss.h"
#include "Player_Skill.h"
#include "Effect_Cloud.h"
#include "Effect_Player_Move.h"
#include "Sky.h"
#include "Weapon.h"
#include "Dummy.h"
#include "Trigger_Door.h"
#include <Item_Weapon.h>

CLevel_GamePlay::CLevel_GamePlay(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CLevel(pGraphic_Device)
{
}

HRESULT CLevel_GamePlay::Initialize()
{

	m_pGameInstance->PlayBGM(L"CrackingBattle.wav");
	m_pGameInstance->BGMVolumeDown(0.3f);

	if (FAILED(__super::Initialize()))
	{
		MSG_BOX(TEXT("Failed to Initialize : __super,CLevel_GamePlay"));
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_Terrain"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_BackGround : CLevel_GamePlay"));
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Effect : CLevel_GamePlay"));
		return E_FAIL;
	}
	
	if(FAILED(Ready_Land_Object()))
	{
		MSG_BOX(TEXT("Failed to Ready_Land_Object : CLevel_GamePlay"));
		return E_FAIL;
	}
	
	if (FAILED(Ready_Layer_UI_Player(TEXT("Layer_UI_Player"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_UI_Player : CLevel_GamePlay"));
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_QAim(TEXT("Layer_QAim"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_QAim : CLevel_GamePlay"));
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_Camera_Player(TEXT("Layer_Player"), TEXT("Layer_Camera_Player"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Player : CLevel_GamePlay"));
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_Skill(TEXT("Layer_Skill"))))
	{
		MSG_BOX(TEXT("Failed to Layer_Skill : CLevel_GamePlay"));
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Weapon(TEXT("Layer_Weapon"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Weapon : CLevel_GamePlay"));
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_Trigger()))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Collision : CLevel_GamePlay"));
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Collision(TEXT("Layer_z_CientCollision"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Collision : CLevel_GamePlay"));
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Item(TEXT("Layer_Item"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Collision : CLevel_GamePlay"));
		return E_FAIL;
	}

	m_iMonsterNum = 0;
	
	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		m_pGameInstance->StopSound(CSound_Manager::CHANNEL_BGM);

		g_eLevel = LEVEL_NORMAL1;
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pGraphic_Device, LEVEL_NORMAL1))))
			return;
	}
}

HRESULT CLevel_GamePlay::Render()
{
	if (FAILED(__super::Render()))
	{
		MSG_BOX(TEXT("Failed to Render : __super,CLevel_GamePlay"));
		return E_FAIL;
	}

	//SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));

	return S_OK;
}
HRESULT CLevel_GamePlay::Ready_Layer_Player(const wstring& strLayerTag, void* pArg)
{

	CPlayer::PLAYER_DESC* pPlayerDesc = (CPlayer::PLAYER_DESC*)pArg;
	pPlayerDesc->fSpeedPerSec = 5.f;
	pPlayerDesc->fRotationPerSec = D3DXToRadian(90.f);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_Player"), pArg)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Player : CLevel_GamePlay"));
		return E_FAIL;
	}
	
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TUTORIAL, strLayerTag, TEXT("Prototype_GameObject_PlayerHpUI"), pArg)))
	//{
	//	MSG_BOX(TEXT("Failed to Ready_Layer_Player : CLevel_GamePlay"));
	//	return E_FAIL;
	//}

	
	return S_OK;
}
HRESULT CLevel_GamePlay::Ready_Layer_Monster(const wstring& strLayerTag, void* pArg)
{
	CDummy::DUMMY_DESC Dummydesc = {};

	Dummydesc.vPos = _float3(17.f, 1.f, 20.f);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TUTORIAL, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Dummy"), &Dummydesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Monster : CLevel_GamePlay"));
		return E_FAIL;
	}
	m_iMonsterNum++;

	Dummydesc.vPos = _float3(12.f, 1.f, 25.f);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TUTORIAL, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Dummy"), &Dummydesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Monster : CLevel_GamePlay"));
		return E_FAIL;
	}

	m_iMonsterNum++;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Land_Object()
{
	CLandObject::LANDOBJECT_DESC LandObjectDesc = {};
	LandObjectDesc.pTerrainTranformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_TUTORIAL, TEXT("Layer_Terrain"), g_strTransformTag.c_str()));
	LandObjectDesc.pTerrainVIBufferCom = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_TUTORIAL, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"),&LandObjectDesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Player : CLevel_GamePlay"));
		return E_FAIL;
	}
	
	CDummy::DUMMY_DESC Dummydesc = {};

	Dummydesc.vPos = _float3(17.f, 1.f, 20.f);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TUTORIAL, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Dummy"), &Dummydesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Monster : CLevel_GamePlay"));
		return E_FAIL;
	}
	m_iMonsterNum++;
	
	Dummydesc.vPos = _float3(12.f, 1.f, 25.f);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TUTORIAL, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Dummy"), &Dummydesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Monster : CLevel_GamePlay"));
		return E_FAIL;
	}

	m_iMonsterNum++;
	
	/*if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"), &LandObjectDesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Monster : CLevel_GamePlay"));
		return E_FAIL;
	}*/

	return S_OK;
}
HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	Ready_Layer_BackGround_Parsing();

	CSky::SKY_DESC sky_desc = {};
	sky_desc.iLevel = LEVEL_TUTORIAL;
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TUTORIAL, strLayerTag, TEXT("Prototype_GameObject_Sky"), &sky_desc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_BackGround : CLevel_GamePlay"));
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround_Parsing()
{
	Load_Terrain();
	Load_Object_Box();
	Load_Wall();
	Load_GuardRail();

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Skill(const wstring& strLayerTag)
{

	CPlayer_Skill::SKILL_DESC Skdesc = {};
	Skdesc.pPlayerTransform	= dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"),g_strTransformTag));
	Skdesc.pPlayerFrameLookVec = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Get_LookPos();
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_Roll"), &Skdesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Skill : CLevel_GamePlay"));
		return E_FAIL;
	}
	CPlayer_Skill::SKILL_DESC Skdesc1 = {};
	Skdesc1.pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), g_strTransformTag));
	Skdesc1.pPlayerFrameLookVec = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Get_LookPos();

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_Barrier"), &Skdesc1)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Skill : CLevel_GamePlay"));
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const wstring& strLayerTag)
{
	CEffect_Splat::EFFECT_SPLAT_DESC EffectSplatDesc = {};
	EffectSplatDesc.fFrame = 0.f;
	EffectSplatDesc.fMaxFrame = 0.f;
	EffectSplatDesc.fRotationPerSec = D3DXToRadian(90.f);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_Effect_Splat"), TEXT("Prototype_GameObject_Effect_Splat"), &EffectSplatDesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Effect : CLevel_GamePlay"));
		return E_FAIL;
	}

	CEffect_Base::EFFECT_BASE_DESC EffectBaseDesc = {};
	EffectBaseDesc.fFrame = 0.f;
	EffectBaseDesc.fMaxFrame = 7.f;
	EffectBaseDesc.fRotationPerSec = D3DXToRadian(90.f);
	EffectBaseDesc.fSpeedPerSec = 5.f;
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_Effect_Splat_Orange"), TEXT("Prototype_GameObject_Effect_Splat_Orange"), &EffectBaseDesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Effect : CLevel_GamePlay"));
		return E_FAIL;
	}

	EffectBaseDesc.fFrame = 0.f;
	EffectBaseDesc.fMaxFrame = 0.f;
	EffectBaseDesc.fRotationPerSec = D3DXToRadian(90.f);
	EffectBaseDesc.fSpeedPerSec = 5.f;
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_Effect_Splat_Orange_Circle"), TEXT("Prototype_GameObject_Effect_Splat_Orange_Circle"), &EffectBaseDesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Effect : CLevel_GamePlay"));
		return E_FAIL;
	}

	EffectBaseDesc.fFrame = 0.f;
	EffectBaseDesc.fMaxFrame = 0.f;
	EffectBaseDesc.fRotationPerSec = D3DXToRadian(90.f);
	EffectBaseDesc.fSpeedPerSec = 5.f;
	for (_int i = 0; i < 3; ++i)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_Effect_Splat_Rambo"), TEXT("Prototype_GameObject_Effect_Splat_Rambo"), &EffectBaseDesc)))
		{
			MSG_BOX(TEXT("Failed to Ready_Layer_Effect : CLevel_GamePlay"));
			return E_FAIL;
		}
	}

	EffectBaseDesc.fFrame = 0.f;
	EffectBaseDesc.fMaxFrame = 4.f;
	EffectBaseDesc.fRotationPerSec = D3DXToRadian(90.f);
	EffectBaseDesc.fSpeedPerSec = 5.f;
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_Effect_Splat_Blue"), TEXT("Prototype_GameObject_Effect_Splat_Blue"), &EffectBaseDesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Effect : CLevel_GamePlay"));
		return E_FAIL;
	}

	EffectBaseDesc.fFrame = 0.f;
	EffectBaseDesc.fMaxFrame = 8.f;
	EffectBaseDesc.fRotationPerSec = D3DXToRadian(90.f);
	EffectBaseDesc.fSpeedPerSec = 5.f;
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_Effect_Splat_Default"), TEXT("Prototype_GameObject_Effect_Splat_Default"), &EffectBaseDesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Effect : CLevel_GamePlay"));
		return E_FAIL;
	}

	EffectBaseDesc.fFrame = 0.f;
	EffectBaseDesc.fMaxFrame = 8.f;
	EffectBaseDesc.fRotationPerSec = D3DXToRadian(90.f);
	EffectBaseDesc.fSpeedPerSec = 5.f;
	for (_int i = 0; i < 5; ++i)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_Effect_Splat_White_Circle"), TEXT("Prototype_GameObject_Effect_Splat_White_Circle"), &EffectBaseDesc)))
		{
			MSG_BOX(TEXT("Failed to Ready_Layer_Effect : CLevel_GamePlay"));
			return E_FAIL;
		}
	}
	EffectBaseDesc.fFrame = 0.f;
	EffectBaseDesc.fMaxFrame = 8.f;
	EffectBaseDesc.fRotationPerSec = D3DXToRadian(90.f);
	EffectBaseDesc.fSpeedPerSec = 5.f;
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_Effect_Splat_Sasin"), TEXT("Prototype_GameObject_Effect_Splat_Sasin"), &EffectBaseDesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Effect : CLevel_GamePlay"));
		return E_FAIL;
	}

	EffectBaseDesc.fFrame = 0.f;
	EffectBaseDesc.fMaxFrame = 8.f;
	EffectBaseDesc.fRotationPerSec = D3DXToRadian(90.f);
	EffectBaseDesc.fSpeedPerSec = 5.f;
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_Effect_Splat_Sasin_Spacle"), TEXT("Prototype_GameObject_Effect_Splat_Sasin_Spacle"), &EffectBaseDesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Effect : CLevel_GamePlay"));
		return E_FAIL;
	}


	//tEffectPlayerMoveDesc.fRotationPerSec = D3DXToRadian(90.f);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_Effect_Player_Move"), TEXT("Prototype_GameObject_Effect_Player_Move"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Effect : CLevel_GamePlay"));
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_Effect_Sparkle"), TEXT("Prototype_GameObject_Effect_Sparkle"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Effect : CLevel_GamePlay"));
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_Effect_Hit_Bullet"), TEXT("Prototype_GameObject_Effect_Hit_Bullet"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Effect : CLevel_GamePlay"));
		return E_FAIL;
	}



	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Weapon(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_Weapon_Pupa"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Weapon: CLevel_GamePlay"));
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_Weapon_Rambo"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Weapon : CLevel_GamePlay"));
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_Weapon_Sasin"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Weapon : CLevel_GamePlay"));
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_Weapon_Sparrow"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Weapon : CLevel_GamePlay"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_Weapon_SharkBlood"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Weapon : CLevel_GamePlay"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_Weapon_Atlas"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Weapon : CLevel_GamePlay"));
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI_Player(const wstring& strLayerTag)
{
	//TODO: 예은 24.01.12
	CUI_Player::UI_PLAYER_DESC  UI_Player_Desc = {};
	UI_Player_Desc.pPlayerStateCom = dynamic_cast<CState*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_State")));
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TUTORIAL, strLayerTag, TEXT("Prototype_GameObject_UI_Player"), &UI_Player_Desc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_UI_Player : CLevel_GamePlay"));
		return E_FAIL;
	}
	//TODO: 예은 스킬 24.01.19
	CUI_Player_Skill::UI_PLAYER_SKILL_DESC UI_Player_Skill_Desc = {};
	UI_Player_Skill_Desc.fRotationPerSec = D3DXToRadian(0.01f);
	UI_Player_Skill_Desc.pPlayerSp = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Get_PlayerSP();
	UI_Player_Skill_Desc.pPlayerMaxSp = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Get_PlayerMaxSP();


	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TUTORIAL, strLayerTag, TEXT("Prototype_GameObject_UI_Player_Skill"), &UI_Player_Skill_Desc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_UI_Player : CLevel_GamePlay"));
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TUTORIAL, strLayerTag, TEXT("Prototype_GameObject_UI_Player_Weapon"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_UI_Player : CLevel_GamePlay"));
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TUTORIAL, strLayerTag, TEXT("Prototype_GameObject_UI_Sparrow"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_UI_Player : CLevel_GamePlay"));
		return E_FAIL;
	}



	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_QAim(const wstring& strLayerTag)
{
	CQAim::QAIM_DESC AimDesc = {};
	AimDesc.fMouseSensor = 0.1f;
	AimDesc.fSpeedPerSec = 5.f;
	AimDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TUTORIAL, strLayerTag, TEXT("Prototype_GameObject_QAim"), &AimDesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_QAim : CLevel_GamePlay"));
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Trigger()
{
	if (FAILED(Ready_Layer_Trigger_Door(TEXT("Layer_Trigger_Door"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Trigger_Door : CLevel_GamePlay"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Trigger_Door(const wstring& strLayerTag)
{
	CTrigger_Door::TRIGGER_DESC  Trigger_Desc = {};
	Trigger_Desc.vLook = _float3(1.f, 0.f, 0.f);
	//Trigger_Desc.vPos = _float3(15.f, 0.f,29.f);
	Trigger_Desc.vPos = _float3(0.5f, 1.6f, 7.f);
	Trigger_Desc.fScale = 1.f;
	Trigger_Desc.iTexNum = 0;
	Trigger_Desc.fSpeedPerSec = 5.f;
	Trigger_Desc.fRotationPerSec = D3DXToRadian(90.f);
	Trigger_Desc.pTerrainTranformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_TUTORIAL, TEXT("Layer_Terrain"), g_strTransformTag.c_str()));
	Trigger_Desc.pTerrainVIBufferCom = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_TUTORIAL, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));


	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TUTORIAL, strLayerTag, TEXT("Prototype_GameObject_Trigger_Door"), &Trigger_Desc)))
	{
		MSG_BOX(TEXT("Failed to Prototype_GameObject_Trigger_Door : CLevel_GamePlay"));
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera_Player(const wstring& strPlayerLayerTag, const wstring& strCameraLayerTag)
{
	CCamera_Player_DW::CAMERA_PLAYER_DW_DESC CameraPlayerDesc = {};
	CameraPlayerDesc.vEye = _float3{ 0.f , 0.f, -1.f };
	CameraPlayerDesc.vAt = _float3{ 0.f , 0.f, 0.f };
	CameraPlayerDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraPlayerDesc.fFar = 1000.f;
	CameraPlayerDesc.fFovy = D3DXToRadian(90.f);
	CameraPlayerDesc.fNear = 0.1f;
	CameraPlayerDesc.fRotationPerSec = D3DXToRadian(90.f);
	CameraPlayerDesc.fSpeedPerSec = 10.f;
	CameraPlayerDesc.iLevel = LEVEL_TUTORIAL;
	CameraPlayerDesc.fMouseSensor = 0.1f;
	CameraPlayerDesc.pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")));

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, strCameraLayerTag, TEXT("Prototype_GameObject_Camera_Player_DW"), &CameraPlayerDesc)))
	{
		MSG_BOX(TEXT("Failed to Add_Clone : CLevel_GamePlay"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Collision(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_SNOWBOSS, strLayerTag, TEXT("Prototype_GameObject_ClientCollision"))))
	{
		MSG_BOX(TEXT("Failed to Prototype_GameObject_ClientCollision : CLevel_GamePlay"));
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Item(const wstring& strLayerTag)
{
	CItem_Weapon::WEAPON_ITEM_DESC ItemWeaponDesc = {};
	ItemWeaponDesc.vPos = _float3(14.f, 0.5f, 5.f);
	ItemWeaponDesc.fRotationPerSec = D3DXToRadian(90.f);
	ItemWeaponDesc.iExtraBullet = 200;
	ItemWeaponDesc.iNowBullet = 0;
	ItemWeaponDesc.iWeaponID = 1;

	if (FAILED(m_pGameInstance->Add_Clone(g_eLevel, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item_Weapon"), &ItemWeaponDesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Item_Weapon : CLevel_GamePlay"));
		return E_FAIL;
	}

	ItemWeaponDesc.vPos = _float3(15.f, 0.5f, 5.f);
	ItemWeaponDesc.iWeaponID = 2;
	ItemWeaponDesc.iExtraBullet = 300;

	if (FAILED(m_pGameInstance->Add_Clone(g_eLevel, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item_Weapon"), &ItemWeaponDesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Item_Weapon : CLevel_GamePlay"));
		return E_FAIL;
	}

	ItemWeaponDesc.vPos = _float3(16.f, 0.5f, 5.f);
	ItemWeaponDesc.iWeaponID = 3;
	ItemWeaponDesc.iExtraBullet = 25;

	if (FAILED(m_pGameInstance->Add_Clone(g_eLevel, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item_Weapon"), &ItemWeaponDesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Item_Weapon : CLevel_GamePlay"));
		return E_FAIL;
	}

	ItemWeaponDesc.vPos = _float3(14.f, 0.5f, 4.f);
	ItemWeaponDesc.iWeaponID = 4;
	ItemWeaponDesc.iExtraBullet = 85;

	if (FAILED(m_pGameInstance->Add_Clone(g_eLevel, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item_Weapon"), &ItemWeaponDesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Item_Weapon : CLevel_GamePlay"));
		return E_FAIL;
	}

	ItemWeaponDesc.vPos = _float3(16.f, 0.5f, 4.f);
	ItemWeaponDesc.iWeaponID = 5;
	ItemWeaponDesc.iExtraBullet = 42 * 9;

	if (FAILED(m_pGameInstance->Add_Clone(g_eLevel, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item_Weapon"), &ItemWeaponDesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Item_Weapon : CLevel_GamePlay"));
		return E_FAIL;
	}

	//파워
	CItem::ITEM_DESC tItemDesc = {};
	tItemDesc.vPos = _float3(20.f, 0.5f, 10.f);

	if (FAILED(m_pGameInstance->Add_Clone(g_eLevel, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item_Power"), &tItemDesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Item : CLevel_GamePlay"));
		return E_FAIL;
	}

	return S_OK;
}

void CLevel_GamePlay::Load_Object_Box()
{
	HANDLE		hFile = CreateFile(L"../Bin/Data/Object_Box.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	DWORD	dwByte(0);

	_uint iBoxNum = { 0 };
	CObject_Box::OBJ_BOX_DESC ObjBoxDesc = {};
	ObjBoxDesc.pTerrainTranformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_TUTORIAL, TEXT("Layer_Terrain"), g_strTransformTag.c_str()));
	ObjBoxDesc.pTerrainVIBufferCom = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_TUTORIAL, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));
	ObjBoxDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ObjBoxDesc.iFrame = 0;

	ReadFile(hFile, &iBoxNum, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; iBoxNum > i; ++i)
	{
		ReadFile(hFile, &ObjBoxDesc.vPosition, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &ObjBoxDesc.iFrame, sizeof(_uint), &dwByte, nullptr);

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TUTORIAL, TEXT("Layer_Box"), TEXT("Prototype_GameObject_Object_Box"), &ObjBoxDesc)))
		{
			MSG_BOX(TEXT("Failed to Add_Clone Prototype_GameObject_Object_Box: CImGUI"));
			return;
		}
	}
	CloseHandle(hFile);
}

void CLevel_GamePlay::Load_Terrain()
{
	HANDLE		hFile = CreateFile(L"../Bin/Data/Terrain.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	DWORD	dwByte(0);

	int iTerrainVerticesX(0), iTerrainVerticesZ(0), iTerrainFrame(0);

	ReadFile(hFile, &iTerrainVerticesX, sizeof(int), &dwByte, nullptr);
	ReadFile(hFile, &iTerrainVerticesZ, sizeof(int), &dwByte, nullptr);
	ReadFile(hFile, &iTerrainFrame, sizeof(int), &dwByte, nullptr);

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_VIBuffer_Terrain_Data"),
		CVIBuffer_Terrain::Create(m_pGraphic_Device, iTerrainVerticesX, iTerrainVerticesZ))))
	{
		MSG_BOX(TEXT("failed to Load_Terrain : Prototype_Component_VIBuffer_Terrain_Data"));
		return;
	}

	/* For.Prototype_Component_VIBuffer_Terrain */
	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_VIBuffer_Terrain_Data"),
		CVIBuffer_Terrain::Create(m_pGraphic_Device, iTerrainVerticesX, iTerrainVerticesZ))))
	{
		MSG_BOX(TEXT("failed to Load_Terrain : Prototype_Component_VIBuffer_Terrain_Data"));
		return;
	}*/

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain_Data"), CTerrain::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CImGUI"));
		return;
	}

	CTerrain::TERRAIN_DESC terrainDesc = {};

	terrainDesc.strVIBufferTag = TEXT("Prototype_Component_VIBuffer_Terrain_Data");
	terrainDesc.iFrame = iTerrainFrame;
	terrainDesc.iLevel = LEVEL_TUTORIAL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TUTORIAL, TEXT("Layer_Terrain"), TEXT("Prototype_GameObject_Terrain_Data"), &terrainDesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_BackGround : CImGUI"));
		return;
	}

	CloseHandle(hFile);


}

void CLevel_GamePlay::Load_Wall()
{
	HANDLE		hFile = CreateFile(L"../Bin/Data/Wall.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	DWORD	dwByte(0);
	_uint	iWallFrame(0);

	_uint iWallNum = { 0 };
	CWall::WALL_DESC wallDesc = {};
	wallDesc.strVIBufferTag = TEXT("Prototype_Component_VIBuffer_Wall_ImGUI");
	wallDesc.iLevel = LEVEL_TUTORIAL;

	ReadFile(hFile, &iWallNum, sizeof(_uint), &dwByte, nullptr);

	/* For.Prototype_Component_VIBuffer_Wall */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_VIBuffer_Wall_ImGUI"),
		CVIBuffer_Wall::Create(m_pGraphic_Device, 10, 6))))
	{
		MSG_BOX(TEXT("failed to Add_Prototype : CMainApp"));
		return;
	}

	/* For.Prototype_Component_VIBuffer_Wall */
	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_VIBuffer_Wall_ImGUI"),
		CVIBuffer_Wall::Create(m_pGraphic_Device, 10, 6))))
	{
		MSG_BOX(TEXT("failed to Add_Prototype : CMainApp"));
		return;
	}*/


	for (_uint i = 0; iWallNum > i; ++i)
	{
		ReadFile(hFile, &wallDesc.WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
		ReadFile(hFile, &wallDesc.iFrame, sizeof(_uint), &dwByte, nullptr);

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TUTORIAL, TEXT("Layer_Wall"), TEXT("Prototype_GameObject_Wall"), &wallDesc)))
		{
			MSG_BOX(TEXT("Failed to Add_Clone Prototype_GameObject_Wall: CImGUI"));
			return;
		}
	}

	CloseHandle(hFile);
}

void CLevel_GamePlay::Load_GuardRail()
{
	HANDLE		hFile = CreateFile(L"../Bin/Data/GuardRail.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	DWORD	dwByte(0);
	_uint	iWallFrame(0);

	_uint iWallNum = { 0 };
	CWall::WALL_DESC wallDesc = {};
	wallDesc.iLevel = LEVEL_TUTORIAL;
	wallDesc.strVIBufferTag = TEXT("Prototype_Component_VIBuffer_Wall_ImGUI_GuardRail");
	ReadFile(hFile, &iWallNum, sizeof(_uint), &dwByte, nullptr);

	/* For.Prototype_Component_VIBuffer_Wall */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_VIBuffer_Wall_ImGUI_GuardRail"),
		CVIBuffer_Wall::Create(m_pGraphic_Device, 10, 6))))
	{
		MSG_BOX(TEXT("failed to Add_Prototype : LoadWall"));
		return;
	}


	for (_uint i = 0; iWallNum > i; ++i)
	{
		ReadFile(hFile, &wallDesc.WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
		ReadFile(hFile, &wallDesc.iFrame, sizeof(_uint), &dwByte, nullptr);

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TUTORIAL, TEXT("Layer_Wall"), TEXT("Prototype_GameObject_Wall"), &wallDesc)))
		{
			MSG_BOX(TEXT("Failed to Add_Clone Prototype_GameObject_Wall: CImGUI"));
			return;
		}
	}

	CloseHandle(hFile);
}

CLevel_GamePlay* CLevel_GamePlay::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pGraphic_Device);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Faild to Created : CLevel_GamePlay"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{

	__super::Free(); 


}
