#include "pch.h"
#include "GameInstance.h"
#include "Level_Normal2.h"
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
#include"Weapon.h"
#include "Trigger_Door.h"
#include "Trigger_Summon.h"
#include "Trigger_Camera.h"

CLevel_Normal2::CLevel_Normal2(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel(pGraphic_Device)
{
}

HRESULT CLevel_Normal2::Initialize()
{
	m_pGameInstance->PlayBGM(L"It'sNotGod.wav");
	m_pGameInstance->BGMVolumeDown(0.46f);


	//It'sNotGod.wav
	if (FAILED(__super::Initialize()))
	{
		MSG_BOX(TEXT("Failed to Initialize : __super,CLevel_Normal2"));
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_Terrain"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_BackGround : CLevel_Normal2"));
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_BackGround : CLevel_Normal2"));
		return E_FAIL;
	}
	if (FAILED(Ready_Land_Object()))
	{
		MSG_BOX(TEXT("Failed to Ready_Land_Object : CLevel_Normal2"));
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_UI_Player(TEXT("Layer_UI_Player"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_UI_Player : CLevel_Normal2"));
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_Trigger()))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Collision : CLevel_GamePlay"));
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_QAim(TEXT("Layer_QAim"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_QAim : CLevel_Normal2"));
		return E_FAIL;
	}
	
	return S_OK;
}

void CLevel_Normal2::Tick(_float fTimeDelta)
{
	Summon();
	Camera();
	__super::Tick(fTimeDelta);
	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		m_pGameInstance->StopSound(CSound_Manager::CHANNEL_BGM);

		g_eLevel = LEVEL_SNOWBOSS;
		/*if (FAILED(Next_Level()))
			return;*/
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pGraphic_Device, LEVEL_SNOWBOSS))))
			return;
	}
}

HRESULT CLevel_Normal2::Render()
{
	if (FAILED(__super::Render()))
	{
		MSG_BOX(TEXT("Failed to Render : __super,CLevel_Normal2"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Normal2::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	Ready_Layer_BackGround_Parsing();

	CSky::SKY_DESC sky_desc = {};
	sky_desc.iLevel = LEVEL_NORMAL2;
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_NORMAL2, strLayerTag, TEXT("Prototype_GameObject_Sky"),&sky_desc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_BackGround : CLevel_Normal2"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Normal2::Ready_Layer_UI_Player(const wstring& strLayerTag)
{
	CUI_Player::UI_PLAYER_DESC  UI_Player_Desc = {};
	UI_Player_Desc.pPlayerStateCom = dynamic_cast<CState*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_State")));
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_NORMAL2, strLayerTag, TEXT("Prototype_GameObject_UI_Player"), &UI_Player_Desc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_UI_Player : LEVEL_NORMAL2"));
		return E_FAIL;
	}

	CUI_Player_Skill::UI_PLAYER_SKILL_DESC UI_Player_Skill_Desc = {};
	UI_Player_Skill_Desc.fRotationPerSec = D3DXToRadian(0.01f);
	UI_Player_Skill_Desc.pPlayerSp = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Get_PlayerSP();
	UI_Player_Skill_Desc.pPlayerMaxSp = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Get_PlayerMaxSP();


	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_NORMAL2, strLayerTag, TEXT("Prototype_GameObject_UI_Player_Skill"), &UI_Player_Skill_Desc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_UI_Player : LEVEL_NORMAL2"));
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_NORMAL2, strLayerTag, TEXT("Prototype_GameObject_UI_Player_Weapon"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_UI_Player : LEVEL_NORMAL2"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_NORMAL2, strLayerTag, TEXT("Prototype_GameObject_UI_Sparrow"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_UI_Player : CLevel_GamePlay"));
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_Normal2::Ready_Layer_Effect(const wstring& strLayerTag)
{
	return S_OK;
}

void CLevel_Normal2::Summon()
{
	Col_Player_Summon();

	if (m_eCurPettern != m_ePrePettern)
	{
		switch (m_eCurPettern)
		{
		case SUMMON_NOTHING:
			break;
		case SUMMON1:
			Load_Monster(TEXT("Layer_Monster"), 0);
			m_pGameInstance->PlaySoundW(TEXT("chimera_pattern3_beam_00.wav"), CSound_Manager::MONSTER5, 0.2f);
			Load_GuardRail();
			m_bActive = true;
			break;
		case SUMMON2:
			Load_Monster(TEXT("Layer_Monster"), 1);
			m_pGameInstance->PlaySoundW(TEXT("chimera_pattern3_beam_00.wav"), CSound_Manager::MONSTER5, 0.2f);
			m_bActive = true;
			break;
		case SUMMON_TERM:
			{
				list<CGameObject*>* pListWall = m_pGameInstance->Get_List(LEVEL_NORMAL2, TEXT("Layer_Wall"));
				auto iter = pListWall->end();
				--iter;
				Safe_Release(*iter);
				iter = pListWall->erase(iter);
				--iter;
				Safe_Release(*iter);
				iter = pListWall->erase(iter);
				--iter;
				Safe_Release(*iter);
				iter = pListWall->erase(iter);
			}
			m_bActive = false;

			break;
		case SUMMON_END:
			break;
		}
		m_ePrePettern = m_eCurPettern;
	}



	if (m_eCurPettern != SUMMON_NOTHING && m_bActive)
		Check_Monster_Num();

}

void CLevel_Normal2::Camera()
{
	Col_Player_Camera();

}

void CLevel_Normal2::Col_Player_Summon()
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")));
	if (nullptr == pPlayer)
		return;
	list<CGameObject*>* pSummon = m_pGameInstance->Get_List(LEVEL_NORMAL2, TEXT("Layer_Trigger_Summon"));
	if (nullptr == pSummon)
		return;
	CCollider_Sphere* pPlayerCollider = dynamic_cast<CCollider_Sphere*>(pPlayer->Get_Component(TEXT("Com_Collider_Sphere")));
	if (nullptr == pPlayerCollider)
		return;
	for (auto iter = pSummon->begin(); iter != pSummon->end(); ++iter)
	{
		CCollider_Sphere* pSummonCollider = dynamic_cast<CCollider_Sphere*>(((*iter))->Get_Component(TEXT("Com_Collider_Sphere")));
		CTrigger_Summon* pTrigger = (dynamic_cast<CTrigger_Summon*>(*iter));
		if (m_pGameInstance->Collision_Sphere(pPlayerCollider, pSummonCollider) && pTrigger->Get_Active())
		{
			CTrigger_Summon::TRIGGERID TriggerID = pTrigger->Get_TriggerID();
			if (TriggerID == CTrigger_Summon::TRIGGERID1)
			{
				m_eCurPettern = SUMMON1;
				pTrigger->Set_Active();
			}
			else if (TriggerID == CTrigger_Summon::TRIGGERID2)
			{
				m_eCurPettern = SUMMON3;
				pTrigger->Set_Active();

			}
		}
	}
}

void CLevel_Normal2::Col_Player_Camera()
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")));
	if (nullptr == pPlayer)
		return;
	list<CGameObject*>* pCameraTri = m_pGameInstance->Get_List(LEVEL_NORMAL2, TEXT("Layer_Trigger_Camera"));
	if (nullptr == pCameraTri)
		return;
	CCollider_Sphere* pPlayerCollider = dynamic_cast<CCollider_Sphere*>(pPlayer->Get_Component(TEXT("Com_Collider_Sphere")));
	if (nullptr == pPlayerCollider)
		return;
	for (auto iter = pCameraTri->begin(); iter != pCameraTri->end(); ++iter)
	{
		CCollider_Sphere* pCameraTriCollider = dynamic_cast<CCollider_Sphere*>(((*iter))->Get_Component(TEXT("Com_Collider_Sphere")));
		CTrigger_Camera* pTrigger = (dynamic_cast<CTrigger_Camera*>(*iter));
		if (m_pGameInstance->Collision_Sphere(pPlayerCollider, pCameraTriCollider) && pTrigger->Get_Active())
		{
			CTrigger_Camera::CAMERAMODE CameraMode = pTrigger->Get_CameraMode();
			if (CameraMode == CTrigger_Camera::CAMERAMODE1)
			{
				pTrigger->Set_Active();
				CCamera_Player_DW* pPlayer = dynamic_cast<CCamera_Player_DW*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Camera_Player")));
				pPlayer->Set_CameraTrapLevelUp(true);
			}
			else if (CameraMode == CTrigger_Camera::CAMERAMODE2)
			{
				pTrigger->Set_Active();
				CCamera_Player_DW* pPlayer = dynamic_cast<CCamera_Player_DW*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Camera_Player")));
				pPlayer->Set_CameraTrapLevelUp(false);
			}
		}
	}
}

void CLevel_Normal2::Check_Monster_Num()
{
	m_iMonsterNum = 0;
	list<CGameObject*>* pMonster = m_pGameInstance->Get_List(LEVEL_NORMAL2, TEXT("Layer_Monster"));
	if (nullptr == pMonster)
		return;
	for (auto iter = pMonster->begin(); iter != pMonster->end(); ++iter)
	{
		if ((*iter)->Get_Dead())
			++m_iMonsterNum;
	}
	if (m_eCurPettern == SUMMON1 && m_iMonsterNum == m_iGoalMonsterNum)
	{
		m_eCurPettern = SUMMON2;
		m_bActive = false;

	}
	else if (m_eCurPettern == SUMMON2 && m_iMonsterNum == m_iGoalMonsterNum)
	{
		m_eCurPettern = SUMMON_TERM;
		m_bActive = false;
	}
}

HRESULT CLevel_Normal2::Ready_Layer_QAim(const wstring& strLayerTag)
{
	CQAim::QAIM_DESC AimDesc = {};
	AimDesc.fMouseSensor = 0.1f;
	AimDesc.fSpeedPerSec = 5.f;
	AimDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_NORMAL2, strLayerTag, TEXT("Prototype_GameObject_QAim"), &AimDesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_QAim : CLevel_Normal2"));
		return E_FAIL;
	}
	return S_OK;
}


HRESULT CLevel_Normal2::Ready_Layer_BackGround_Parsing()
{
	Load_Terrain();
	Load_Wall();
	Load_Object();

	return S_OK;
}

HRESULT CLevel_Normal2::Ready_Layer_Monster(const wstring& strLayerTag, void* pArg)
{
	Load_Steam1(strLayerTag);
	Load_Steam2(strLayerTag);
	Load_Steam3(strLayerTag);
	Load_Steam4_1(strLayerTag);
	Load_Steam4_2(strLayerTag);
	Load_Steam5(strLayerTag);
	Load_Steam6(strLayerTag);
	
	return S_OK;
}


HRESULT CLevel_Normal2::Ready_Land_Object()
{
	CLandObject::LANDOBJECT_DESC LandObjectDesc = {};
	LandObjectDesc.pTerrainTranformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_NORMAL2, TEXT("Layer_Terrain"), g_strTransformTag.c_str()));
	LandObjectDesc.pTerrainVIBufferCom = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_NORMAL2, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")));
	pPlayer->Set_Terrain(&LandObjectDesc);
	pPlayer->Set_Pos(_float3(4.5f, 0.f, 4.f));


	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"), &LandObjectDesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Monster : CLevel_Normal2"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Normal2::Ready_Layer_Trigger()
{
	if (FAILED(Ready_Layer_Trigger_Door(TEXT("Layer_Trigger_Door"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Trigger_Door : CLevel_GamePlay"));
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_Trigger_Summon(TEXT("Layer_Trigger_Summon"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Trigger_Door : CLevel_GamePlay"));
		return E_FAIL;

	}
	if (FAILED(Ready_Layer_Trigger_Camera(TEXT("Layer_Trigger_Camera"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Trigger_Door : CLevel_GamePlay"));
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_Normal2::Ready_Layer_Trigger_Door(const wstring& strLayerTag)
{
	CTrigger_Door::TRIGGER_DESC  Trigger_Desc = {};
	Trigger_Desc.vLook = _float3(0.f, 0.f, -1.f);
	Trigger_Desc.vPos = _float3(7.f, 1.6f, 49.f);
	Trigger_Desc.fScale = 5.f;
	Trigger_Desc.iTexNum = 1;
	Trigger_Desc.fSpeedPerSec = 5.f;
	Trigger_Desc.fRotationPerSec = D3DXToRadian(90.f);
	Trigger_Desc.pTerrainTranformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_NORMAL2, TEXT("Layer_Terrain"), g_strTransformTag.c_str()));
	Trigger_Desc.pTerrainVIBufferCom = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_NORMAL2, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));


	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_NORMAL2, strLayerTag, TEXT("Prototype_GameObject_Trigger_Door"), &Trigger_Desc)))
	{
		MSG_BOX(TEXT("Failed to Prototype_GameObject_Trigger_Door : CLevel_GamePlay"));
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_Normal2::Ready_Layer_Trigger_Summon(const wstring& strLayerTag)
{
	CTrigger_Summon::TRIGGER_DESC  Trigger_Desc = {};
	Trigger_Desc.vPos = _float3(4.5f, 0.f, 9.f);
	Trigger_Desc.fScale = 5.f;
	Trigger_Desc.eTriggerID = CTrigger_Summon::TRIGGERID1;
	Trigger_Desc.iTexNum = 0;
	Trigger_Desc.fSpeedPerSec = 5.f;
	Trigger_Desc.fRotationPerSec = D3DXToRadian(90.f);
	Trigger_Desc.pTerrainTranformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_NORMAL2, TEXT("Layer_Terrain"), g_strTransformTag.c_str()));
	Trigger_Desc.pTerrainVIBufferCom = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_NORMAL2, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));


	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_NORMAL2, strLayerTag, TEXT("Prototype_GameObject_Trigger_Summon"), &Trigger_Desc)))
	{
		MSG_BOX(TEXT("Failed to Prototype_GameObject_Trigger_Door : CLevel_GamePlay"));
		return E_FAIL;
	}


	//Trigger_Desc.vPos = _float3(36.f, 0.f, 36.f);
	//Trigger_Desc.eTriggerID = CTrigger_Summon::TRIGGERID2;

	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_NORMAL2, strLayerTag, TEXT("Prototype_GameObject_Trigger_Summon"), &Trigger_Desc)))
	//{
	//	MSG_BOX(TEXT("Failed to Prototype_GameObject_Trigger_Door : CLevel_GamePlay"));
	//	return E_FAIL;
	//}



	return S_OK;
}

HRESULT CLevel_Normal2::Ready_Layer_Trigger_Camera(const wstring& strLayerTag)
{
	CTrigger_Camera::TRIGGER_DESC  Trigger_Desc = {};
	Trigger_Desc.vPos = _float3(4.f, 0.f, 19.f);
	Trigger_Desc.fScale = 5.f;
	Trigger_Desc.iTexNum = 0;
	Trigger_Desc.eCameraMode = CTrigger_Camera::CAMERAMODE1;
	Trigger_Desc.fSpeedPerSec = 5.f;
	Trigger_Desc.fRotationPerSec = D3DXToRadian(90.f);
	Trigger_Desc.pTerrainTranformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_NORMAL2, TEXT("Layer_Terrain"), g_strTransformTag.c_str()));
	Trigger_Desc.pTerrainVIBufferCom = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_NORMAL2, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));


	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_NORMAL2, strLayerTag, TEXT("Prototype_GameObject_Trigger_Camera"), &Trigger_Desc)))
	{
		MSG_BOX(TEXT("Failed to Prototype_GameObject_Trigger_Door : CLevel_GamePlay"));
		return E_FAIL;
	}

	Trigger_Desc.vPos = _float3(10.f, 0.f, 37.f);
	Trigger_Desc.eCameraMode = CTrigger_Camera::CAMERAMODE2;
	
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_NORMAL2, strLayerTag, TEXT("Prototype_GameObject_Trigger_Camera"), &Trigger_Desc)))
	{
		MSG_BOX(TEXT("Failed to Prototype_GameObject_Trigger_Door : CLevel_GamePlay"));
		return E_FAIL;
	}

	return S_OK;
}




void CLevel_Normal2::Load_Terrain()
{
	HANDLE		hFile = CreateFile(L"../Bin/Data/Level2_Terrain.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	DWORD	dwByte(0);

	int iTerrainVerticesX(0), iTerrainVerticesZ(0), iTerrainFrame(0);

	ReadFile(hFile, &iTerrainVerticesX, sizeof(int), &dwByte, nullptr);
	ReadFile(hFile, &iTerrainVerticesZ, sizeof(int), &dwByte, nullptr);
	ReadFile(hFile, &iTerrainFrame, sizeof(int), &dwByte, nullptr);

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_VIBuffer_Terrain_Data_Normal2"),
		CVIBuffer_Terrain::Create(m_pGraphic_Device, iTerrainVerticesX, iTerrainVerticesZ))))
	{
		MSG_BOX(TEXT("failed to Load_Terrain : Prototype_Component_VIBuffer_Terrain_Data"));
		return;
	}

	
	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain_Normal2"), CTerrain::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CImGUI"));
		return;
	}

	CTerrain::TERRAIN_DESC terrainDesc = {};

	terrainDesc.strVIBufferTag = TEXT("Prototype_Component_VIBuffer_Terrain_Data_Normal2");
	terrainDesc.iFrame = iTerrainFrame;
	terrainDesc.iLevel = LEVEL_NORMAL2;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_NORMAL2, TEXT("Layer_Terrain"), TEXT("Prototype_GameObject_Terrain_Normal2"), &terrainDesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_BackGround : CImGUI"));
		return;
	}

	CloseHandle(hFile);
}

void CLevel_Normal2::Load_Wall()
{
	HANDLE		hFile = CreateFile(L"../Bin/Data/Level2_Wall.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	DWORD	dwByte(0);
	_uint	iWallFrame(0);

	_uint iWallNum = { 0 };
	CWall::WALL_DESC wallDesc = {};
	wallDesc.iLevel = LEVEL_NORMAL2;
	wallDesc.strVIBufferTag = TEXT("Prototype_Component_VIBuffer_Wall_ImGUI_Normal2");
	ReadFile(hFile, &iWallNum, sizeof(_uint), &dwByte, nullptr);

	/* For.Prototype_Component_VIBuffer_Wall */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_VIBuffer_Wall_ImGUI_Normal2"),
		CVIBuffer_Wall::Create(m_pGraphic_Device, 16, 6))))
	{
		MSG_BOX(TEXT("failed to Add_Prototype : LoadWall"));
		return;
	}


	for (_uint i = 0; iWallNum > i; ++i)
	{
		ReadFile(hFile, &wallDesc.WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
		ReadFile(hFile, &wallDesc.iFrame, sizeof(_uint), &dwByte, nullptr);
		
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_NORMAL2, TEXT("Layer_Wall"), TEXT("Prototype_GameObject_Wall"), &wallDesc)))
		{
			MSG_BOX(TEXT("Failed to Add_Clone Prototype_GameObject_Wall: CImGUI"));
			return;
		}
	}

	CloseHandle(hFile);
}

void CLevel_Normal2::Load_GuardRail()
{
	HANDLE		hFile = CreateFile(L"../Bin/Data/Level2_GuardRail_1.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	DWORD	dwByte(0);
	_uint	iWallFrame(0);

	_uint iWallNum = { 0 };
	CWall::WALL_DESC wallDesc = {};
	wallDesc.iLevel = LEVEL_NORMAL2;
	wallDesc.strVIBufferTag = TEXT("Prototype_Component_VIBuffer_Wall_ImGUI_Normal2_GuardRail");
	ReadFile(hFile, &iWallNum, sizeof(_uint), &dwByte, nullptr);

	/* For.Prototype_Component_VIBuffer_Wall */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_VIBuffer_Wall_ImGUI_Normal2_GuardRail"),
		CVIBuffer_Wall::Create(m_pGraphic_Device, 10, 6))))
	{
		MSG_BOX(TEXT("failed to Add_Prototype : LoadWall"));
		return;
	}


	for (_uint i = 0; iWallNum > i; ++i)
	{
		ReadFile(hFile, &wallDesc.WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
		ReadFile(hFile, &wallDesc.iFrame, sizeof(_uint), &dwByte, nullptr);

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_NORMAL2, TEXT("Layer_Wall"), TEXT("Prototype_GameObject_Wall"), &wallDesc)))
		{
			MSG_BOX(TEXT("Failed to Add_Clone Prototype_GameObject_Wall: CImGUI"));
			return;
		}
	}

	CloseHandle(hFile);
}

void CLevel_Normal2::Load_Object()
{
	HANDLE		hFile = CreateFile(L"../Bin/Data/Level2_Box.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	DWORD	dwByte(0);

	_uint iBoxNum = { 0 };
	CObject_Box::OBJ_BOX_DESC ObjBoxDesc = {};
	ObjBoxDesc.pTerrainTranformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_NORMAL2, TEXT("Layer_Terrain"), g_strTransformTag.c_str()));
	ObjBoxDesc.pTerrainVIBufferCom = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_NORMAL2, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));
	ObjBoxDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ObjBoxDesc.iFrame = 0;

	ReadFile(hFile, &iBoxNum, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; iBoxNum > i; ++i)
	{
		ReadFile(hFile, &ObjBoxDesc.vPosition, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &ObjBoxDesc.iFrame, sizeof(_uint), &dwByte, nullptr);

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_NORMAL2, TEXT("Layer_Box"), TEXT("Prototype_GameObject_Object_Box"), &ObjBoxDesc)))
		{
			MSG_BOX(TEXT("Failed to Add_Clone Prototype_GameObject_Object_Box: CImGUI"));
			return;
		}
	}
	CloseHandle(hFile);
}
void CLevel_Normal2::Load_Steam1(const wstring& strLayerTag)
{
	HANDLE		hFile = CreateFile(L"../Bin/Data/Steam1.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	DWORD	dwByte(0);

	_uint iMonsterNum = { 0 };
	CLandObject::LANDOBJECT_DESC LandObjectDesc = {};
	LandObjectDesc.pTerrainTranformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_NORMAL2, TEXT("Layer_Terrain"), g_strTransformTag.c_str()));
	LandObjectDesc.pTerrainVIBufferCom = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_NORMAL2, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));
	LandObjectDesc.fRotationPerSec = 1.f;
	LandObjectDesc.fdelayTime = 0.f;
	ReadFile(hFile, &iMonsterNum, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; iMonsterNum > i; ++i)
	{
		ReadFile(hFile, &LandObjectDesc.iMonsterType, sizeof(_uint), &dwByte, nullptr);

		if (4 == LandObjectDesc.iMonsterType)
		{
			ReadFile(hFile, &LandObjectDesc.vPos, sizeof(_float3), &dwByte, nullptr);

			LandObjectDesc.fdelayTime += 0.175f;

			if (FAILED(m_pGameInstance->Add_Clone(LEVEL_NORMAL2, TEXT("Layer_Steam"), TEXT("Prototype_GameObject_Steam"), &LandObjectDesc)))
			{
				MSG_BOX(TEXT("Failed to Add_Clone Prototype_GameObject_Monster: CImGUI"));
				return;
			}
			m_iMonsterNum++;
		}

	}

	CloseHandle(hFile);
}

void CLevel_Normal2::Load_Steam2(const wstring& strLayerTag)
{
	HANDLE		hFile = CreateFile(L"../Bin/Data/Steam2.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	DWORD	dwByte(0);

	_uint iMonsterNum = { 0 };
	CLandObject::LANDOBJECT_DESC LandObjectDesc = {};
	LandObjectDesc.pTerrainTranformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_NORMAL2, TEXT("Layer_Terrain"), g_strTransformTag.c_str()));
	LandObjectDesc.pTerrainVIBufferCom = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_NORMAL2, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));
	LandObjectDesc.fRotationPerSec = 1.f;
	LandObjectDesc.fdelayTime = 0.f;
	LandObjectDesc.fTerm = 2.f;
	ReadFile(hFile, &iMonsterNum, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; iMonsterNum > i; ++i)
	{
		ReadFile(hFile, &LandObjectDesc.iMonsterType, sizeof(_uint), &dwByte, nullptr);

		if (4 == LandObjectDesc.iMonsterType)
		{
			ReadFile(hFile, &LandObjectDesc.vPos, sizeof(_float3), &dwByte, nullptr);

			if(6 == i)
				LandObjectDesc.fdelayTime += 1.f;

			if (FAILED(m_pGameInstance->Add_Clone(LEVEL_NORMAL2, TEXT("Layer_Steam"), TEXT("Prototype_GameObject_Steam"), &LandObjectDesc)))
			{
				MSG_BOX(TEXT("Failed to Add_Clone Prototype_GameObject_Monster: CImGUI"));
				return;
			}
			m_iMonsterNum++;
		}

	}

	CloseHandle(hFile);
}

void CLevel_Normal2::Load_Steam3(const wstring& strLayerTag)
{
	HANDLE		hFile = CreateFile(L"../Bin/Data/Steam3.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	DWORD	dwByte(0);

	_uint iMonsterNum = { 0 };
	CLandObject::LANDOBJECT_DESC LandObjectDesc = {};
	LandObjectDesc.pTerrainTranformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_NORMAL2, TEXT("Layer_Terrain"), g_strTransformTag.c_str()));
	LandObjectDesc.pTerrainVIBufferCom = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_NORMAL2, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));
	LandObjectDesc.fRotationPerSec = 1.f;
	LandObjectDesc.fdelayTime = 0.f;
	ReadFile(hFile, &iMonsterNum, sizeof(_uint), &dwByte, nullptr);

	LandObjectDesc.fdelayTime += 2.1f;

	LandObjectDesc.fTerm = 6.f;

	for (_uint i = 0; iMonsterNum - 12 > i; ++i)
	{
		ReadFile(hFile, &LandObjectDesc.iMonsterType, sizeof(_uint), &dwByte, nullptr);

		ReadFile(hFile, &LandObjectDesc.vPos, sizeof(_float3), &dwByte, nullptr);

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_NORMAL2, TEXT("Layer_Steam"), TEXT("Prototype_GameObject_Steam"), &LandObjectDesc)))
		{
			MSG_BOX(TEXT("Failed to Add_Clone Prototype_GameObject_Monster: CImGUI"));
			return;
		}

		LandObjectDesc.fdelayTime += 0.175f;
		m_iMonsterNum++;
	}

	LandObjectDesc.fdelayTime += 1.5f;

	for (_uint i = 12; iMonsterNum > i; ++i)
	{
		ReadFile(hFile, &LandObjectDesc.iMonsterType, sizeof(_uint), &dwByte, nullptr);

		ReadFile(hFile, &LandObjectDesc.vPos, sizeof(_float3), &dwByte, nullptr);

		if (18 == i)
			LandObjectDesc.fdelayTime -= 1.f;

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_NORMAL2, TEXT("Layer_Steam"), TEXT("Prototype_GameObject_Steam"), &LandObjectDesc)))
		{
			MSG_BOX(TEXT("Failed to Add_Clone Prototype_GameObject_Monster: CImGUI"));
			return;
		}
		m_iMonsterNum++;

	}

	CloseHandle(hFile);
}

void CLevel_Normal2::Load_Steam4_1(const wstring& strLayerTag)
{
	HANDLE		hFile = CreateFile(L"../Bin/Data/Steam4.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	DWORD	dwByte(0);

	_uint iMonsterNum = { 0 };
	CLandObject::LANDOBJECT_DESC LandObjectDesc = {};
	LandObjectDesc.pTerrainTranformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_NORMAL2, TEXT("Layer_Terrain"), g_strTransformTag.c_str()));
	LandObjectDesc.pTerrainVIBufferCom = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_NORMAL2, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));
	LandObjectDesc.fRotationPerSec = 1.f;
	LandObjectDesc.fdelayTime = 0.f;
	LandObjectDesc.fTerm = 5.3f;
	ReadFile(hFile, &iMonsterNum, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; iMonsterNum > i; ++i)
	{
		ReadFile(hFile, &LandObjectDesc.iMonsterType, sizeof(_uint), &dwByte, nullptr);

		if (4 == LandObjectDesc.iMonsterType)
		{
			ReadFile(hFile, &LandObjectDesc.vPos, sizeof(_float3), &dwByte, nullptr);

			LandObjectDesc.fdelayTime += 0.15f;

			if (FAILED(m_pGameInstance->Add_Clone(LEVEL_NORMAL2, TEXT("Layer_Steam"), TEXT("Prototype_GameObject_Steam"), &LandObjectDesc)))
			{
				MSG_BOX(TEXT("Failed to Add_Clone Prototype_GameObject_Monster: CImGUI"));
				return;
			}
			m_iMonsterNum++;
		}

	}

	CloseHandle(hFile);
}

void CLevel_Normal2::Load_Steam4_2(const wstring& strLayerTag)
{
	HANDLE		hFile = CreateFile(L"../Bin/Data/Steam4_2.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	DWORD	dwByte(0);

	_uint iMonsterNum = { 0 };
	CLandObject::LANDOBJECT_DESC LandObjectDesc = {};
	LandObjectDesc.pTerrainTranformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_NORMAL2, TEXT("Layer_Terrain"), g_strTransformTag.c_str()));
	LandObjectDesc.pTerrainVIBufferCom = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_NORMAL2, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));
	LandObjectDesc.fRotationPerSec = 1.f;
	LandObjectDesc.fdelayTime = 1.8f;
	LandObjectDesc.fTerm = 5.3f;
	ReadFile(hFile, &iMonsterNum, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; iMonsterNum > i; ++i)
	{
		ReadFile(hFile, &LandObjectDesc.iMonsterType, sizeof(_uint), &dwByte, nullptr);

		if (4 == LandObjectDesc.iMonsterType)
		{
			ReadFile(hFile, &LandObjectDesc.vPos, sizeof(_float3), &dwByte, nullptr);

			LandObjectDesc.fdelayTime += 0.15f;

			if (FAILED(m_pGameInstance->Add_Clone(LEVEL_NORMAL2, TEXT("Layer_Steam"), TEXT("Prototype_GameObject_Steam"), &LandObjectDesc)))
			{
				MSG_BOX(TEXT("Failed to Add_Clone Prototype_GameObject_Monster: CImGUI"));
				return;
			}
			m_iMonsterNum++;
		}

	}

	CloseHandle(hFile);
}

void CLevel_Normal2::Load_Steam5(const wstring& strLayerTag)
{
	HANDLE		hFile = CreateFile(L"../Bin/Data/Steam5.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	DWORD	dwByte(0);

	_uint iMonsterNum = { 0 };
	CLandObject::LANDOBJECT_DESC LandObjectDesc = {};
	LandObjectDesc.pTerrainTranformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_NORMAL2, TEXT("Layer_Terrain"), g_strTransformTag.c_str()));
	LandObjectDesc.pTerrainVIBufferCom = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_NORMAL2, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));
	LandObjectDesc.fRotationPerSec = 1.f;
	LandObjectDesc.fdelayTime = 3.6f;
	LandObjectDesc.fTerm = 5.3f;
	ReadFile(hFile, &iMonsterNum, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; iMonsterNum > i; ++i)
	{
		ReadFile(hFile, &LandObjectDesc.iMonsterType, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &LandObjectDesc.vPos, sizeof(_float3), &dwByte, nullptr);

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_NORMAL2, TEXT("Layer_Steam"), TEXT("Prototype_GameObject_Steam"), &LandObjectDesc)))
		{
			MSG_BOX(TEXT("Failed to Add_Clone Prototype_GameObject_Monster: CImGUI"));
			return;
		}
		m_iMonsterNum++;

	}

	CloseHandle(hFile);
}

void CLevel_Normal2::Load_Steam6(const wstring& strLayerTag)
{
	HANDLE		hFile = CreateFile(L"../Bin/Data/Steam6.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	DWORD	dwByte(0);

	_uint iMonsterNum = { 0 };
	CLandObject::LANDOBJECT_DESC LandObjectDesc = {};
	LandObjectDesc.pTerrainTranformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_NORMAL2, TEXT("Layer_Terrain"), g_strTransformTag.c_str()));
	LandObjectDesc.pTerrainVIBufferCom = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_NORMAL2, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));
	LandObjectDesc.fRotationPerSec = 1.f;
	LandObjectDesc.fdelayTime = 5.1f;
	LandObjectDesc.fTerm = 5.3f;
	ReadFile(hFile, &iMonsterNum, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; iMonsterNum > i; ++i)
	{
		ReadFile(hFile, &LandObjectDesc.iMonsterType, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &LandObjectDesc.vPos, sizeof(_float3), &dwByte, nullptr);

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_NORMAL2, TEXT("Layer_Steam"), TEXT("Prototype_GameObject_Steam"), &LandObjectDesc)))
		{
			MSG_BOX(TEXT("Failed to Add_Clone Prototype_GameObject_Monster: CImGUI"));
			return;
		}
		m_iMonsterNum++;

	}

	CloseHandle(hFile);
}
void CLevel_Normal2::Load_Monster(const wstring& strLayerTag, _uint iFileNum)
{
	TCHAR	szBuf[256] = L"";
	swprintf_s(szBuf, L"../Bin/Data/Level2_Monster%d.dat", iFileNum);
	HANDLE		hFile = CreateFile(szBuf, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	DWORD	dwByte(0);

	_uint iMonsterNum = { 0 };
	CLandObject::LANDOBJECT_DESC LandObjectDesc = {};
	LandObjectDesc.pTerrainTranformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_NORMAL2, TEXT("Layer_Terrain"), g_strTransformTag.c_str()));
	LandObjectDesc.pTerrainVIBufferCom = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_NORMAL2, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));

	ReadFile(hFile, &iMonsterNum, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; iMonsterNum > i; ++i)
	{
		ReadFile(hFile, &LandObjectDesc.iMonsterType, sizeof(_uint), &dwByte, nullptr);

		if (0 == LandObjectDesc.iMonsterType)
		{
			ReadFile(hFile, &LandObjectDesc.vPos, sizeof(_float3), &dwByte, nullptr);

			if (FAILED(m_pGameInstance->Add_Clone(LEVEL_NORMAL2, strLayerTag, TEXT("Prototype_GameObject_Monster_Three_Red_Eyes"), &LandObjectDesc)))
			{
				MSG_BOX(TEXT("Failed to Add_Clone Prototype_GameObject_Monster: CImGUI"));
				return;
			}
			m_iGoalMonsterNum++;
		}
		else if (1 == LandObjectDesc.iMonsterType)
		{
			ReadFile(hFile, &LandObjectDesc.vPos, sizeof(_float3), &dwByte, nullptr);

			if (FAILED(m_pGameInstance->Add_Clone(LEVEL_NORMAL2, strLayerTag, TEXT("Prototype_GameObject_Monster_Red_Fly"), &LandObjectDesc)))
			{
				MSG_BOX(TEXT("Failed to Add_Clone Prototype_GameObject_Monster: CImGUI"));
				return;
			}
			m_iGoalMonsterNum++;
		}
		else if (2 == LandObjectDesc.iMonsterType)
		{
			ReadFile(hFile, &LandObjectDesc.vPos, sizeof(_float3), &dwByte, nullptr);

			if (FAILED(m_pGameInstance->Add_Clone(LEVEL_NORMAL2, strLayerTag, TEXT("Prototype_GameObject_Monster_White_Fly"), &LandObjectDesc)))
			{
				MSG_BOX(TEXT("Failed to Add_Clone Prototype_GameObject_Monster: CImGUI"));
				return;
			}
			m_iGoalMonsterNum++;
		}
		else if (3 == LandObjectDesc.iMonsterType)
		{
			ReadFile(hFile, &LandObjectDesc.vPos, sizeof(_float3), &dwByte, nullptr);

			if (FAILED(m_pGameInstance->Add_Clone(LEVEL_NORMAL2, strLayerTag, TEXT("Prototype_GameObject_Monster_Jump_PinkSlime"), &LandObjectDesc)))
			{
				MSG_BOX(TEXT("Failed to Add_Clone Prototype_GameObject_Monster: CImGUI"));
				return;
			}
			m_iGoalMonsterNum++;
		}
		else
		{
			return;
		}

	}

	CloseHandle(hFile);
}
CLevel_Normal2* CLevel_Normal2::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_Normal2* pInstance = new CLevel_Normal2(pGraphic_Device);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Faild to Created : CLevel_Normal2"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Normal2::Free()
{
	__super::Free();
}
