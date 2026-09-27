#include "pch.h"
#include "GameInstance.h"
#include "Level_SnowBoss.h"
#include "Level_Loading.h"
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
#include "Blizzard.h"

CLevel_SnowBoss::CLevel_SnowBoss(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel(pGraphic_Device)
{
}

HRESULT CLevel_SnowBoss::Initialize()
{

	//m_pGameInstance->PlayBGM(L"BattleUrgent.wav");
	//m_pGameInstance->BGMVolumeDown(0.48f);

	if (FAILED(__super::Initialize()))
	{
		MSG_BOX(TEXT("Failed to Initialize : __super,CLevel_SnowBoss"));
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_Terrain"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_BackGround : CLevel_SnowBoss"));
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_BackGround : CLevel_SnowBoss"));
		return E_FAIL;
	}
	if (FAILED(Ready_Land_Object()))
	{
		MSG_BOX(TEXT("Failed to Ready_Land_Object : CLevel_SnowBoss"));
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_UI_Player(TEXT("Layer_UI_Player"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_UI_Player : CLevel_SnowBoss"));
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_QAim(TEXT("Layer_QAim"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_QAim : CLevel_SnowBoss"));
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_Blizzard(TEXT("Layer_Blizzard"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Blizzard : CLevel_SnowBoss"));
		return E_FAIL;
	}

	

	return S_OK;
}

void CLevel_SnowBoss::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

HRESULT CLevel_SnowBoss::Render()
{
	if (FAILED(__super::Render()))
	{
		MSG_BOX(TEXT("Failed to Render : __super,CLevel_SnowBoss"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_SnowBoss::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	Ready_Layer_BackGround_Parsing();

	CSky::SKY_DESC sky_desc = {};
	sky_desc.iLevel = LEVEL_SNOWBOSS;
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_SNOWBOSS, strLayerTag, TEXT("Prototype_GameObject_Sky"),&sky_desc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_BackGround : CLevel_SnowBoss"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_SnowBoss::Ready_Layer_UI_Player(const wstring& strLayerTag)
{
	CUI_Player::UI_PLAYER_DESC  UI_Player_Desc = {};
	UI_Player_Desc.pPlayerStateCom = dynamic_cast<CState*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_State")));
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_SNOWBOSS, strLayerTag, TEXT("Prototype_GameObject_UI_Player"), &UI_Player_Desc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_UI_Player : LEVEL_SNOWBOSS"));
		return E_FAIL;
	}

	CUI_Player_Skill::UI_PLAYER_SKILL_DESC UI_Player_Skill_Desc = {};
	UI_Player_Skill_Desc.fRotationPerSec = D3DXToRadian(0.01f);
	UI_Player_Skill_Desc.pPlayerSp = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Get_PlayerSP();
	UI_Player_Skill_Desc.pPlayerMaxSp = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Get_PlayerMaxSP();


	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_SNOWBOSS, strLayerTag, TEXT("Prototype_GameObject_UI_Player_Skill"), &UI_Player_Skill_Desc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_UI_Player : LEVEL_SNOWBOSS"));
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_SNOWBOSS, strLayerTag, TEXT("Prototype_GameObject_UI_Player_Weapon"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_UI_Player : LEVEL_SNOWBOSS"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_SNOWBOSS, strLayerTag, TEXT("Prototype_GameObject_UI_Sparrow"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_UI_Player : CLevel_GamePlay"));
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_SNOWBOSS, strLayerTag, TEXT("Prototype_GameObject_UI_Ending"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_UI_Player : CLevel_GamePlay"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_SnowBoss::Ready_Layer_Effect(const wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_SnowBoss::Ready_Layer_Blizzard(const wstring& strLayerTag)
{
	CBlizzard::BLIZZARD_DESC tBlizzardDesc = {};
	tBlizzardDesc.fSpeedPerSec = 10.f;
	tBlizzardDesc.fRotationPerSec = D3DXToRadian(90.f);
	tBlizzardDesc.fScale = 0.2f;

	for (size_t i = 0; i < 500; i++)
	{
		tBlizzardDesc.vPos = _float3((rand() % 7000) * 0.01f - 10.f, (rand() % 3000) * 0.01f,(rand() % 7000) * 0.01f - 10.f);

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_SNOWBOSS, strLayerTag, TEXT("Prototype_GameObject_Blizzard"), &tBlizzardDesc)))
		{
			MSG_BOX(TEXT("Failed to Prototype_GameObject_Blizzard : CLevel_SnowBoss"));
			return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CLevel_SnowBoss::Ready_Layer_QAim(const wstring& strLayerTag)
{
	CQAim::QAIM_DESC AimDesc = {};
	AimDesc.fMouseSensor = 0.1f;
	AimDesc.fSpeedPerSec = 5.f;
	AimDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_SNOWBOSS, strLayerTag, TEXT("Prototype_GameObject_QAim"), &AimDesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_QAim : CLevel_SnowBoss"));
		return E_FAIL;
	}
	return S_OK;
}
//
//HRESULT CLevel_SnowBoss::Ready_Layer_Skill(const wstring& strLayerTag)
//{
//	CPlayer_Skill::SKILL_DESC Skdesc = {};
//	Skdesc.pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_SNOWBOSS, TEXT("Layer_Player"), g_strTransformTag));
//	Skdesc.pPlayerFrameLookVec = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Object(LEVEL_SNOWBOSS, TEXT("Layer_Player")))->Get_LookPos();
//	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_SNOWBOSS, strLayerTag, TEXT("Prototype_GameObject_Roll"), &Skdesc)))
//	{
//		MSG_BOX(TEXT("Failed to Ready_Layer_Skill : LEVEL_SNOWBOSS"));
//		return E_FAIL;
//	}
//	CPlayer_Skill::SKILL_DESC Skdesc1 = {};
//	Skdesc1.pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_SNOWBOSS, TEXT("Layer_Player"), g_strTransformTag));
//	Skdesc1.pPlayerFrameLookVec = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Object(LEVEL_SNOWBOSS, TEXT("Layer_Player")))->Get_LookPos();
//
//	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_SNOWBOSS, strLayerTag, TEXT("Prototype_GameObject_Barrier"), &Skdesc1)))
//	{
//		MSG_BOX(TEXT("Failed to Ready_Layer_Skill : LEVEL_SNOWBOSS"));
//		return E_FAIL;
//	}
//	return S_OK;
//}

HRESULT CLevel_SnowBoss::Ready_Layer_Camera_Free(const wstring& strLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC pCameraFreeDesc = {};

	pCameraFreeDesc.vEye = _float3{ 0.f , 10.f, -10.f };
	pCameraFreeDesc.vAt = _float3{ 0.f , 0.f, 0.f };
	pCameraFreeDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	pCameraFreeDesc.fFar = 1000.f;
	pCameraFreeDesc.fFovy = D3DXToRadian(60.f);
	pCameraFreeDesc.fMouseSensor = 1.f;
	pCameraFreeDesc.fNear = 0.1f;
	pCameraFreeDesc.fRotationPerSec = D3DXToRadian(90.f);
	pCameraFreeDesc.fSpeedPerSec = 10.f;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_SNOWBOSS, strLayerTag, TEXT("Prototype_GameObject_Camera_Free_Boss"), &pCameraFreeDesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Camera_Free : LEVEL_SNOWBOSS"));
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CLevel_SnowBoss::Ready_Layer_BackGround_Parsing()
{
	Load_Terrain();
	Load_Wall();
	Load_WallTerrain();

	return S_OK;
}

HRESULT CLevel_SnowBoss::Ready_Layer_Monster(const wstring& strLayerTag, void* pArg)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_SNOWBOSS, strLayerTag, TEXT("Prototype_GameObject_Boss_Yeti"), pArg)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Monster : CLevel_SnowBoss"));
		return E_FAIL;
	}
	m_iMonsterNum++;
	if (FAILED(Ready_Layer_UI_Boss(TEXT("Layer_UI_Boss"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_UI_Boss : CLevel_SnowBoss"));
		return E_FAIL;
	}
	return S_OK;

}


HRESULT CLevel_SnowBoss::Ready_Land_Object()
{
	CLandObject::LANDOBJECT_DESC LandObjectDesc = {};
	LandObjectDesc.pTerrainTranformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_SNOWBOSS, TEXT("Layer_Terrain"), g_strTransformTag.c_str()));
	LandObjectDesc.pTerrainVIBufferCom = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_SNOWBOSS, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));

	/*if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"), &LandObjectDesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Player : CLevel_SnowBoss"));
		return E_FAIL;
	}*/

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")));
	pPlayer->Set_Terrain(&LandObjectDesc);
	pPlayer->Set_Pos(_float3(10.f, 0.f, 5.f));

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"), &LandObjectDesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Monster : CLevel_SnowBoss"));
		return E_FAIL;
	}

	return S_OK;
}


HRESULT CLevel_SnowBoss::Ready_Layer_UI_Boss(const wstring& strLayerTag, void* pArg)
{

	CUI_Boss::UI_BOSS_DESC UIBossDesc = {};
	UIBossDesc.pBossStateCom = dynamic_cast<CState*>(m_pGameInstance->Get_Component(LEVEL_SNOWBOSS, TEXT("Layer_Monster"), TEXT("Com_State"), --m_iMonsterNum));
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_SNOWBOSS, strLayerTag, TEXT("Prototype_GameObject_UI_Boss"), &UIBossDesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Monster : CLevel_SnowBoss"));
		return E_FAIL;
	}
	m_iMonsterNum++;
	return S_OK;
}

//HRESULT CLevel_SnowBoss::Ready_Layer_QAim(const wstring& strLayerTag)
//{
//	CQAim::QAIM_DESC AimDesc = {};
//	AimDesc.fMouseSensor = 0.1f;
//	AimDesc.fSpeedPerSec = 5.f;
//	AimDesc.fRotationPerSec = D3DXToRadian(90.0f);
//
//	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_SNOWBOSS, strLayerTag, TEXT("Prototype_GameObject_QAim"), &AimDesc)))
//	{
//		MSG_BOX(TEXT("Failed to Ready_Layer_QAim : CLevel_GamePlay"));
//		return E_FAIL;
//	}
//
//	return S_OK;
//}

//HRESULT CLevel_SnowBoss::Ready_Layer_Effect(const wstring& strLayerTag)
//{
//	CEffect_Splat::EFFECT_SPLAT_DESC EffectSplatDesc = {};
//	EffectSplatDesc.fFrame = 0.f;
//	EffectSplatDesc.fMaxFrame = 0.f;
//	EffectSplatDesc.fRotationPerSec = D3DXToRadian(90.f);
//
//	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_Effect_Splat"), TEXT("Prototype_GameObject_Effect_Splat"), &EffectSplatDesc)))
//	{
//		MSG_BOX(TEXT("Failed to Ready_Layer_Effect : CLevel_GamePlay"));
//		return E_FAIL;
//	}
//
//	CEffect_Base::EFFECT_BASE_DESC EffectBaseDesc = {};
//	EffectBaseDesc.fFrame = 0.f;
//	EffectBaseDesc.fMaxFrame = 7.f;
//	EffectBaseDesc.fRotationPerSec = D3DXToRadian(90.f);
//	EffectBaseDesc.fSpeedPerSec = 5.f;
//	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_Effect_Splat_Orange"), TEXT("Prototype_GameObject_Effect_Splat_Orange"), &EffectBaseDesc)))
//	{
//		MSG_BOX(TEXT("Failed to Ready_Layer_Effect : CLevel_GamePlay"));
//		return E_FAIL;
//	}
//
//
//	EffectBaseDesc.fFrame = 0.f;
//	EffectBaseDesc.fMaxFrame = 4.f;
//	EffectBaseDesc.fRotationPerSec = D3DXToRadian(90.f);
//	EffectBaseDesc.fSpeedPerSec = 5.f;
//	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_Effect_Splat_Blue"), TEXT("Prototype_GameObject_Effect_Splat_Blue"), &EffectBaseDesc)))
//	{
//		MSG_BOX(TEXT("Failed to Ready_Layer_Effect : CLevel_GamePlay"));
//		return E_FAIL;
//	}
//
//	EffectBaseDesc.fFrame = 0.f;
//	EffectBaseDesc.fMaxFrame = 8.f;
//	EffectBaseDesc.fRotationPerSec = D3DXToRadian(90.f);
//	EffectBaseDesc.fSpeedPerSec = 5.f;
//	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_Effect_Splat_Default"), TEXT("Prototype_GameObject_Effect_Splat_Default"), &EffectBaseDesc)))
//	{
//		MSG_BOX(TEXT("Failed to Ready_Layer_Effect : CLevel_GamePlay"));
//		return E_FAIL;
//	}
//
//
//	//tEffectPlayerMoveDesc.fRotationPerSec = D3DXToRadian(90.f);
//
//	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_Effect_Player_Move"), TEXT("Prototype_GameObject_Effect_Player_Move"))))
//	//{
//	//	MSG_BOX(TEXT("Failed to Ready_Layer_Effect : CLevel_GamePlay"));
//	//	return E_FAIL;
//	//}
//
//	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_Effect_Sparkle"), TEXT("Prototype_GameObject_Effect_Sparkle"))))
//	{
//		MSG_BOX(TEXT("Failed to Ready_Layer_Effect : CLevel_GamePlay"));
//		return E_FAIL;
//	}
//
//	/*if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_Effect_Hit_Bullet"), TEXT("Prototype_GameObject_Effect_Hit_Bullet"))))
//	{
//		MSG_BOX(TEXT("Failed to Ready_Layer_Effect : CLevel_GamePlay"));
//		return E_FAIL;
//	}*/
//
//	return S_OK;
//}

void CLevel_SnowBoss::Load_Terrain()
{
	HANDLE		hFile = CreateFile(L"../Bin/Data/BossTerrain.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	DWORD	dwByte(0);

	int iTerrainVerticesX(0), iTerrainVerticesZ(0), iTerrainFrame(0);

	ReadFile(hFile, &iTerrainVerticesX, sizeof(int), &dwByte, nullptr);
	ReadFile(hFile, &iTerrainVerticesZ, sizeof(int), &dwByte, nullptr);
	ReadFile(hFile, &iTerrainFrame, sizeof(int), &dwByte, nullptr);

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_VIBuffer_Terrain_Data_Boss"),
		CVIBuffer_Terrain::Create(m_pGraphic_Device, iTerrainVerticesX, iTerrainVerticesZ))))
	{
		MSG_BOX(TEXT("failed to Load_Terrain : Prototype_Component_VIBuffer_Terrain_Data"));
		return;
	}

	///* For.Prototype_Component_VIBuffer_Terrain */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_VIBuffer_Terrain_Data_Boss"),
	//	CVIBuffer_Terrain::Create(m_pGraphic_Device, iTerrainVerticesX, iTerrainVerticesZ))))
	//{
	//	MSG_BOX(TEXT("failed to Load_Terrain : Prototype_Component_VIBuffer_Terrain_Data"));
	//	return;
	//}

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain_Data_Boss"), CTerrain::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CImGUI"));
		return;
	}

	CTerrain::TERRAIN_DESC terrainDesc = {};

	terrainDesc.strVIBufferTag = TEXT("Prototype_Component_VIBuffer_Terrain_Data_Boss");
	terrainDesc.iFrame = iTerrainFrame;
	terrainDesc.iLevel = LEVEL_SNOWBOSS;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_SNOWBOSS, TEXT("Layer_Terrain"), TEXT("Prototype_GameObject_Terrain_Data_Boss"), &terrainDesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_BackGround : CImGUI"));
		return;
	}

	CloseHandle(hFile);
}

void CLevel_SnowBoss::Load_Wall()
{
	HANDLE		hFile = CreateFile(L"../Bin/Data/BossWall.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	DWORD	dwByte(0);
	_uint	iWallFrame(0);

	_uint iWallNum = { 0 };
	CWall::WALL_DESC wallDesc = {};
	wallDesc.iLevel = LEVEL_SNOWBOSS;
	wallDesc.strVIBufferTag = TEXT("Prototype_Component_VIBuffer_Wall_ImGUI_Boss");
	ReadFile(hFile, &iWallNum, sizeof(_uint), &dwByte, nullptr);

	/* For.Prototype_Component_VIBuffer_Wall */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_VIBuffer_Wall_ImGUI_Boss"),
		CVIBuffer_Wall::Create(m_pGraphic_Device, 3, 6))))
	{
		MSG_BOX(TEXT("failed to Add_Prototype : LoadWall"));
		return;
	}

	/* For.Prototype_Component_VIBuffer_Wall */
	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_VIBuffer_Wall_ImGUI_Boss"),
		CVIBuffer_Wall::Create(m_pGraphic_Device, 3, 47))))
	{
		MSG_BOX(TEXT("failed to Add_Prototype : LoadWall"));
		return;
	}*/

	for (_uint i = 0; iWallNum > i; ++i)
	{
		ReadFile(hFile, &wallDesc.WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
		ReadFile(hFile, &wallDesc.iFrame, sizeof(_uint), &dwByte, nullptr);
		
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_SNOWBOSS, TEXT("Layer_Wall"), TEXT("Prototype_GameObject_Wall_Boss"), &wallDesc)))
		{
			MSG_BOX(TEXT("Failed to Add_Clone Prototype_GameObject_Wall: CImGUI"));
			return;
		}
	}

	CloseHandle(hFile);
}

void CLevel_SnowBoss::Load_WallTerrain()
{
	HANDLE		hFile = CreateFile(L"../Bin/Data/WallTerrain.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	DWORD	dwByte(0);
	_uint	iWallFrame(0);

	_uint iWallNum = { 0 };
	CWall::WALL_DESC wallDesc = {};
	wallDesc.strVIBufferTag = TEXT("Prototype_Component_VIBuffer_Wall_Terrain_Boss");
	wallDesc.iLevel = LEVEL_SNOWBOSS;
	ReadFile(hFile, &iWallNum, sizeof(_uint), &dwByte, nullptr);

	/* For.Prototype_Component_VIBuffer_Wall */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_VIBuffer_Wall_Terrain_Boss"),
		CVIBuffer_Wall::Create(m_pGraphic_Device, 11, 51))))
	{
		MSG_BOX(TEXT("failed to Add_Prototype : Load_WallTerrain"));
		return;
	}

	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_VIBuffer_Wall_Terrain_Boss"),
		CVIBuffer_Wall::Create(m_pGraphic_Device, 11, 51))))
	{
		MSG_BOX(TEXT("failed to Add_Prototype : Load_WallTerrain"));
		return;
	}*/

	for (_uint i = 0; iWallNum > i; ++i)
	{
		ReadFile(hFile, &wallDesc.WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
		ReadFile(hFile, &wallDesc.iFrame, sizeof(_uint), &dwByte, nullptr);

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_SNOWBOSS, TEXT("Layer_Wall"), TEXT("Prototype_GameObject_Wall_Terrain_Boss"), &wallDesc)))
		{
			MSG_BOX(TEXT("Failed to Add_Clone Prototype_GameObject_WallTerrain: CImGUI"));
			return;
		}
	}

	CloseHandle(hFile);
}

CLevel_SnowBoss* CLevel_SnowBoss::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_SnowBoss* pInstance = new CLevel_SnowBoss(pGraphic_Device);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Faild to Created : CLevel_SnowBoss"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_SnowBoss::Free()
{
	__super::Free();
}
