#include "pch.h"
#include "Loader.h"

#include <process.h> //쓰레드를 사용하기 위한 헤더

#include "Monster_Jump_PinkSlime.h"
#include "Monster_Three_Red_Eyes.h"
#include "MonsterArlertUI.h"
#include "Camera_Player.h"
#include "White_Monster_Bullet.h"
#include "Red_Monster_Bullet.h"
#include "Monster_White_Fly.h"
#include "Monster_Red_Fly.h"
#include "Dummy.h"
#include "Steam.h"
#include "RedRect.h"
#include "SteamAttack.h"
#include "Camera_Player_DW.h"
#include "Camera_Player_DW2.h"
#include "GameInstance.h"
#include "Camera_Free.h"
#include "MonsterBullet.h"
#include "IceSplinter.h"
#include "EarthQuake.h"
#include "BackGround.h"
#include "PlayerHpUI.h"
#include "RedCircle.h"
#include "BossYeti.h"
#include "Terrain.h"
#include "Player.h"
#include "Explosion.h"
#include "Snow.h"
#include "QAim.h"
#include "QAim2.h"
#include "Sky.h"
#include "UI_Player.h"
#include "Barrier.h"
#include "Roll.h"
#include "State.h"
#include "Effect_Splat.h"
#include "Effect_Splat_White_Circle.h"
#include "Effect_Splat_Orange.h"
#include "Effect_Splat_Rambo.h"
#include "Effect_Orange_Circle.h"
#include "Effect_Splat_Sasin.h"
#include "Effect_Sasin_Spacle.h"
#include "Effect_Splat_Blue.h"
#include "Splat_Default.h"
#include "Effect_Sparkle.h"
#include "Bullet.h"
#include "Weapon_Pupa.h"
#include "Weapon_Rambo.h"
#include "Weapon_Sasin.h"
#include "Weapon_Sparrow.h"
#include "Weapon_SharkBlood.h"
#include "Weapon_Atlas.h"
#include "Object_Box.h"
#include "Wall.h"
#include "UI_Player_Skill.h"
#include "UI_Player_Weapon.h"
#include "UI_Boss.h"
#include "Effect_Cloud.h"
#include "Effect_Player_Move.h"
#include "Effect_Hit_Bullet.h"
#include "Item_Weapon.h"
#include "Item_Hamburger.h"
#include "UI_Sparrow.h"
#include "AirPlane.h"
#include "ShootMonster.h"
#include "MoonStone.h"
#include "ClientCollision.h"
#include "TrailRenderer.h"
#include "UI_AirPlane.h"
#include "Comet.h"
#include "AirPlaneTurretBody.h"
#include "AirPlaneTurretMouse.h"
#include "ShootMonsterBullet.h"
#include "Emp.h"
#include "UI_Damage.h"
#include "Trigger_Door.h"
#include "Trigger_BlackHole.h"
#include "Trigger_Summon.h"
#include "Trigger_Camera.h"
#include "SpawnEffect.h" 
#include "Item_1945.h"	 
#include "BossLayser.h"
#include "BossBattleCruiser.h"
#include "Item_Effect.h" 
#include "Item_Power.h"
#include "Blizzard.h"
#include "Destroy_Effect.h"
#include "Boss_Destroy_Effect.h"
#include"UI_Ending.h"
_uint APIENTRY Loading_Main(void* pArg)
{
	/* 로더에게 지정된 레벨을 준비해라*/
	CLoader* pLoader = (CLoader*)pArg;
	if (nullptr == pLoader)
		return 1;

	if (FAILED(pLoader->Start()))
	{
		MSG_BOX(TEXT("Failed to Start : pLoader"));
		return 1;
	}

	return 0;
}

CLoader::CLoader(LPDIRECT3DDEVICE9 pGraphic_Device) : 
	m_pGraphic_Device(pGraphic_Device),
	m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_Critical_Section);
	/* 스레드를 생성한다. */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Loading_Main, this, 0, nullptr);
	if (0 == m_hThread)
	{
		MSG_BOX(TEXT("Failed to _beginthreadex : CLoader"));
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CLoader::Start()
{
	EnterCriticalSection(&m_Critical_Section);

	HRESULT hr = { 0 };

	switch (m_eNextLevelID)
	{
	case Client::LEVEL_STATIC:
		break;
	case Client::LEVEL_LOADING:
		break;
	case Client::LEVEL_LOGO:
		hr = Loading_For_Logo();
		break;
	case Client::LEVEL_TUTORIAL:
		hr = Loading_For_GamePlay();
		break;
	case Client::LEVEL_NORMAL1:
		hr = Loading_For_Normal1();
		break;
	case Client::LEVEL_1945:
		hr = Loading_For_1945();
		break;
	case Client::LEVEL_NORMAL2:
		hr = Loading_For_Normal2();
		break;
	case Client::LEVEL_SNOWBOSS:
		hr = Loading_For_SnowBoss();
		break;
	case Client::LEVEL_END:
		break;
	default:
		break;
	}

	if (FAILED(hr))
	{
		MSG_BOX(TEXT("Faild to Start : CLoader"));
		LeaveCriticalSection(&m_Critical_Section);
		return E_FAIL;
	}
		

	LeaveCriticalSection(&m_Critical_Section);

	return S_OK;
}

HRESULT CLoader::Loading_For_Logo() // 로고 레벨에서 사용할 자워들 준비
{

	m_strLoadingText = TEXT("텍스쳐(레벨별 원형)를(을) 로딩 중 입니다.");
	/* For.Prototype_Component_Texture_Logo */
 	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"), 
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Title/title.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo_Text"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Title/title_Space.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("객체의 원형를(을) 로딩 중 입니다.");
	/* For.Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	m_strLoadingText = TEXT("로딩이 완료되었습니다.");
	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay()
{
	m_strLoadingText = TEXT("텍스쳐(레벨별 원형)를(을) 로딩 중 입니다.");

#pragma region Texture
#pragma region Texture_Trigger_Door
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_Trigger_Door"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXCUBE, TEXT("../Bin/Resource/Object/Trigger/Trigger_Door%d.dds"),3))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_Trigger_Door_In"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Object/Trigger/Trigger_Door1.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

#pragma endregion Texture_Trigger_Door

#pragma region Texture_Terrain
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Terrain/Tile%d.png"), 19))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Texture_Terrain

#pragma region Texture_Dummy

	/* For.Prototype_Component_Texture_Pink_Slime_Monster_Idle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_Dummy"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/Dummy/Dummy.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

#pragma endregion Texture_Dummy
#pragma region Texture_Sky
	///* For.Prototype_Component_Texture_Sky */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_Sky"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXCUBE, TEXT("../Bin/Resource/Texture/SkyBox/Sky_%d.dds"),4))))
	//{
	//	MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
	//	return E_FAIL;
	//}
	/* For.Prototype_Component_Texture_Sky */
 	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXCUBE, TEXT("../Bin/Resource/Map/Sky/Sky.dds")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Texture_Sky

#pragma region Texture_UI

#pragma region Texture_UI_Collider
	/* For.Prototype_Component_Texture_Collider */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_Collider"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Collider/ColliderTexture%d.png"),3))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Texture_UI_Collider

#pragma region Texture_UI_Heart

	/* For.Prototype_Component_Texture_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_UI"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/HP/HeartBack.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	//TO DO: 예은 추가
	/* For.Prototype_Component_Texture_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_Hp_UI"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/HP/hp_UI_%d.png"),21))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype - Prototype_Component_Texture_Hp_UI : CLoader"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_Hp_Gatto"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/HP/GattoIcon_%d.png"), 9))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype - Prototype_Component_Texture_Hp_Gatto : CLoader"));
		return E_FAIL;
	}	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_Hp_Gatto_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/HP/GattoIconBack.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype - Prototype_Component_Texture_Hp_Gatto_Back : CLoader"));
		return E_FAIL;
	}

#pragma endregion Texture_UI_Heart
	
#pragma region Texture_UI_Aim

	/* For.Prototype_Component_Texture_Aim */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_QAim"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Cursor/inGameCursor_%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Texture_UI_Aim

#pragma region Texture_UI_Skill

	/* For.Prototype_Component_Texture_UI_Skill*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_UI_Skill"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Skill/Player_Skill_%d.png"), 18))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Skill_Back*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_UI_Skill_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Skill/SkillBack.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_UI_Skill_Icon*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_UI_Skill_Icon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Skill/Player_Skill_%d.png"), 18))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Skill_Gauge*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_UI_Skill_Gauge"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Skill/SkillGauge%d.png"), 3))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Skill_Slot*/

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_UI_Skill_Slot"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Skill/Skill_Slot.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_UI_Skill_Gauge_Sq*/

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_UI_Skill_Gauge_Sq"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Skill/SkillGaugeSq%d.png"), 3))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Skill_CoolTime*/

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_UI_Skill_CoolTime"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Skill/Player_Skill_Cool.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

#pragma endregion Texture_UI_Skill

#pragma region Texture_UI_Weapon
	/* For.Prototype_Component_Texture_UI_Weapon_Back */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_UI_Weapon_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/GunBack%d.png"), 2))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Weapon_Icon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_UI_Weapon_Icon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/Gun%d.png"),12))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_WeaponEx_Icon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_UI_WeaponEx_Icon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/ExGun%d.png"), 12))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Weapon_Main */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_UI_Weapon_Main"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/GunGauge%d.png"),3))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Weapon_Mini */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_UI_Weapon_Mini"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/MiniGunGauge%d.png"),3))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Weapon_Back_Dot */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_UI_Weapon_Back_Dot"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/GunGaugeSq%d.png"),3))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	
		/* For.Prototype_Component_Texture_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_UI_Weapon_Mini_Check"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/GunPreparHalftone.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_UI_Weapon_Mini_Check1"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/SlotCheck%d.png"),2))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
		/* For.Prototype_Component_Texture_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_UI_Weapon_Bulllet_Icon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/UI_Bullet%d.png"),4))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

#pragma endregion Texture_UI_Weapon

#pragma region Texture_Sparrow
	/* For.Prototype_Component_Texture_UI_Sparrow0 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_UI_Sparrow0"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Zoom/zoomUI_0_%d.png"),8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Sparrow1 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_UI_Sparrow1"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Zoom/zoomUI_1_%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Sparrow2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_UI_Sparrow2"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Zoom/zoomUI_2_%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Sparrow3 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_UI_Sparrow3"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Zoom/zoomUI_3_%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Sparrow4 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_UI_Sparrow4"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Zoom/zoomUI_4_%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Sparrow5 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_UI_Sparrow5"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Zoom/zoomUI_5_%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Texture_Sparrow


#pragma endregion Texture_UI

#pragma region Texture_Weapon
	/* For.Prototype_Component_Texture_Weapon_Pupa */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_Weapon_Pupa"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Weapon/Pupa_%d.png"), 19))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Weapon_Rambo */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_Weapon_Rambo"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Weapon/Rambo_%d.png"), 19))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Weapon_Sasin */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_Weapon_Sasin"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Weapon/Sasin_%d.png"), 19))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Weapon_Sparrow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_Weapon_Sparrow"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Weapon/Sparrow_%d.png"), 19))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Weapon_SharkBlood */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_Weapon_SharkBlood"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Weapon/SharkBlood_%d.png"), 19))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Weapon_Atlas */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_Weapon_Atlas"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Weapon/Atlas_%d.png"), 19))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_WeaponEx_Pupa */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_WeaponEx_Pupa"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Weapon/Pupa_Reinforce_12.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_WeaponEx_Rambo */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_WeaponEx_Rambo"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Weapon/Rambo_Reinforce_%d.png"), 19))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_WeaponEx_Sasin */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_WeaponEx_Sasin"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Weapon/Sasin_Reinforce_%d.png"), 19))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_WeaponEx_Sparrow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_WeaponEx_Sparrow"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Weapon/Sparrow_Reinforce_%d.png"), 19))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_WeaponEx_SharkBlood */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_WeaponEx_SharkBlood"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Weapon/SharkBlood_Reinforce_%d.png"), 19))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_WeaponEx_Atlas */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_WeaponEx_Atlas"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Weapon/Atlas_Reinforce_%d.png"), 19))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Bullet */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_Bullet"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Object/Bullet/Bullet/Bullet%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	
#pragma endregion Texture_Weapon


#pragma region Texture_Red_Circle
	/* For.Prototype_Component_Texture_Red_Circle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_Red_Circle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/보스 몬스터/RedCircle/RedCircle.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

#pragma endregion Texture_Red_Circle

#pragma endregion Texture

#pragma region VIBuffer

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("failed to Add_Prototype : CMainApp"));
		return E_FAIL;
	}
	/* For.Prototype_Component_State */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_State"),
		CState::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("failed to Add_Prototype : CMainApp"));
		return E_FAIL;
	}
	/* For.Prototype_Component_VIBuffer_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_VIBuffer_Sphere"),
		CVIBuffer_Sphere::Create(m_pGraphic_Device, 36, 15))))
	{
		MSG_BOX(TEXT("failed to Add_Prototype : CMainApp"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Collider_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Collider_Rect"),
		CCollider_Rect::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("failed to Add_Prototype : CMainApp"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider_Sphere::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("failed to Add_Prototype : CMainApp"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Collider_Cube_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Collider_Cube_AABB"),
		CCollider_Cube_AABB::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("failed to Add_Prototype : CMainApp"));
		return E_FAIL;
	}
	//Load_Terrain();

#pragma endregion VIBuffer

	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("객체의 원형를(을) 로딩 중 입니다.");

#pragma region Prototype
	/* For.Prototype_GameObject_Trigger_Door */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Trigger_Door"), CTrigger_Door::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}


	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"), CTerrain::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"), CPlayer::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Camera_Player_DW */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Player_DW"), CCamera_Player_DW::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}


	/* For.Prototype_GameObject_Red_Circle */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Red_Circle"), CRedCircle::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"), CSky::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Effect_Explosion */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Explosion"), CEffect_Explosion::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Wall */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Wall"), CWall::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_UI_Sparrow */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Sparrow"), CUI_Sparrow::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_GameObject_UI_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Player"), CUI_Player::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_GameObject_UI_Player_Skill */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Player_Skill"), CUI_Player_Skill::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader, CUI_Player_Skill"));
		return E_FAIL;
	}
	/* For.Prototype_GameObject_UI_Player_Weaopon */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Player_Weapon"), CUI_Player_Weapon::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader, CUI_Player_Weapon"));
		return E_FAIL;
	}
	/* For.Prototype_GameObject_MonsterArlertUI */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonsterArlertUI"), CMonsterWarningUI::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_GameObject_QAim */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_QAim"), CQAim::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Barrier */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Barrier"), CBarrier::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Roll */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Roll"), CRoll::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Effect_Splat */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Splat"), CEffect_Splat::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Effect_Splat_White_Circle */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Splat_White_Circle"), CEffect_Splat_White_Circle::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Effect_Splat_Sasin */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Splat_Sasin"), CEffect_Splat_Sasin::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Effect_Splat_Rambo */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Splat_Rambo"), CEffect_Splat_Rambo::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Effect_Splat_Sasin_Spacle */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Splat_Sasin_Spacle"), CEffect_Sasin_Spacle::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Effect_Splat_Orange*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Splat_Orange"), CEffect_Splat_Orange::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Effect_Splat_Orange_Circle*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Splat_Orange_Circle"), CEffect_Orange_Circle::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Effect_Splat_Blue*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Splat_Blue"), CEffect_Splat_Blue::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Effect_Splat_Default*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Splat_Default"), CSplat_Default::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}


	/* For.Prototype_GameObject_Effect_Sparkle */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Sparkle"), CEffect_Sparkle::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Effect_Cloud */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Cloud"), CEffect_Cloud::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Effect_Player_Move */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Player_Move"), CEffect_Player_Move::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Effect_Hit_Bullet */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Hit_Bullet"), CEffect_Hit_Bullet::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Item_Effect */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item_Effect"), CItem_Effect::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Bullet */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bullet"), CBullet::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Weapon_Pupa */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Pupa"), CWeapon_Pupa::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Weapon_Rambo */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Rambo"), CWeapon_Rambo::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Weapon_Sasin */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Sasin"), CWeapon_Sasin::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Weapon_Sparrow */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Sparrow"), CWeapon_Sparrow::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Weapon_SharkBlood */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_SharkBlood"), CWeapon_SharkBlood::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Weapon_Atlas */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Atlas"), CWeapon_Atlas::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Object_Box */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_Box"), CObject_Box::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Item_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item_Weapon"), CItem_Weapon::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Item_Hamburger */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item_Hamburger"), CItem_Hamburger::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	
	/* For.Prototype_GameObject_Dummy */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dummy"), CDummy::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	
	/* For.Prototype_GameObject_ClientCollision */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ClientCollision"), CClientCollision::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_TrailRenderer */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TrailRenderer"), CTrailRenderer::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_UI_Damage */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Damage"), CUI_Damage::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Item_Power */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item_Power"), CItem_Power::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

#pragma endregion Prototype
	m_strLoadingText = TEXT("로딩이 완료되었습니다.");
	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Normal1()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Trigger_Door"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXCUBE, TEXT("../Bin/Resource/Object/Trigger/Trigger_Door%d.dds"),3))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Trigger_Door_In"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Object/Trigger/Trigger_Door1.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma region normal 필수 텍스쳐
#pragma region Texture_Effect
	/* For.Prototype_Component_Texture_Effect_Explosion */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Effect_Explosion"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/Explosion/Explosion_%d.png"), 9))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Effect_Splat */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Effect_Splat"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/Splat/spiky circular.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Effect_Splat_Orange */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Effect_Splat_Orange"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/SplatOrange/Splat%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Effect_Splat_Blue */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Effect_Splat_Blue"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/SplatBlue/Splat%d.png"), 5))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_Effect_Splat_Default */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Effect_Splat_Default"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/SplatDefault/Splat%d.png"), 9))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}


	/* For.Prototype_Component_Texture_Effect_Player_Move */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Effect_Player_Move"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/Cloud/Effect_Player_Move%d.png"), 5))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Effect_Sparkle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Effect_Sparkle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/Sparkle/Sparkle%d.png"), 2))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Effect_Barrier */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Effect_Barrier"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/Barrier/Barrier%d.png"), 2))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	// 준수 추가
	/* For.Prototype_Component_Texture_SpawnEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_SpawnEffect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/Spawn/SpawnEffect_0.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Texture_Effect

#pragma region Texture_Monster 
	/* For.Prototype_Component_Texture_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Monster"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Object/Monster/Awler/Awler_Battle_1_%d.png"), 45))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Monster_Three_Red_Eyes_Idle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Monster_Three_Red_Eyes_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/ThreeEyesMonster/Idle/Awler_Idle_%d.png"), 128))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Monster_Three_Red_Eyes_Move */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Monster_Three_Red_Eyes_Move"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/ThreeEyesMonster/Move/Awler_Move_%d.png"), 64))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Monster_Three_Red_Eyes_Roll */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Monster_Three_Red_Eyes_Roll"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/ThreeEyesMonster/Roll/Awler_Roll_%d.png"), 64))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Monster_Three_Red_Eyes_GunBattle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Monster_Three_Red_Eyes_GunBattle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/ThreeEyesMonster/GunBattle/Awler_Battle_%d.png"), 360))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Monster_Three_Red_Eyes_Death */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Monster_Three_Red_Eyes_Death"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/ThreeEyesMonster/Death/Awler_Death_%d.png"), 5))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Monster_Three_Red_Eyes_Bullet */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Monster_Three_Red_Eyes_Bullet"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/ThreeEyesMonster/Bullet/Bullet0.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}


	/* For.Prototype_Component_Texture_Indicator_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Indicator_Monster"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Indicator/indicator_monster.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma region Texture_White_Fly_Monster
	/* For.Prototype_Component_Texture_White_Fly_Monster_Idle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_White_Fly_Monster_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/WhiteFlyMonster/Idle/Idle%d.png"), 16))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_White_Fly_Monster_Attak */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_White_Fly_Monster_Attak"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/WhiteFlyMonster/Attak/Attak%d.png"), 4))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_White_Fly_Monster_Death */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_White_Fly_Monster_Death"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/WhiteFlyMonster/Death/Death%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_White_Fly_Monster_Bullet_Idle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_White_Fly_Monster_Bullet_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/WhiteFlyMonster/Bullet/Idle/Bullet%d.png"), 6))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_White_Fly_Monster_Bullet_Death */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_White_Fly_Monster_Bullet_Death"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/WhiteFlyMonster/Bullet/Death/Bullet_Death%d.png"), 7))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

#pragma endregion Texture_White_Fly_Monster

#pragma region Texture_Red_Fly_Monster

	/* For.Prototype_Component_Texture_Red_Fly_Monster_Idle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Red_Fly_Monster_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/RedFlyMonster/Idle/Idle%d.png"), 16))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Red_Fly_Monster_Attak */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Red_Fly_Monster_Attak"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/RedFlyMonster/Attak/Attak%d.png"), 4))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Red_Fly_Monster_Death */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Red_Fly_Monster_Death"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/RedFlyMonster/Death/Death%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Red_Fly_Monster_Bullet_Idle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Red_Fly_Monster_Bullet_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/RedFlyMonster/Bullet/Idle/Bullet%d.png"), 6))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Red_Fly_Monster_Bullet_Death */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Red_Fly_Monster_Bullet_Death"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/RedFlyMonster/Bullet/Death/Bullet_Death%d.png"), 20))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}


#pragma endregion Texture_Red_Fly_Monster

#pragma region Texture_Pink_Slime_Monster

	/* For.Prototype_Component_Texture_Pink_Slime_Monster_Idle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Pink_Slime_Monster_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/PinkSlimeMonster/Idle/Idle%d.png"), 32))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Pink_Slime_Monster_Jump_Attak */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Pink_Slime_Monster_Jump_Attak"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/PinkSlimeMonster/JumpAttak/JumpAttak%d.png"), 64))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Pink_Slime_Monster_Death */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Pink_Slime_Monster_Death"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/PinkSlimeMonster/Death/Death%d.png"), 10))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

#pragma endregion Texture_Pink_Slime_Monster


#pragma endregion Texture_Monster 

#pragma region Texture_Sky
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Sky_Normal1"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXCUBE, TEXT("../Bin/Resource/Map/Sky/Sky.dds")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Texture_Sky


#pragma region Texture_UI_Heart

	/* For.Prototype_Component_Texture_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_UI"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/HP/HeartBack.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	//TO DO: 예은 추가
	/* For.Prototype_Component_Texture_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Hp_UI"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/HP/hp_UI_%d.png"), 21))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype - Prototype_Component_Texture_Hp_UI : CLoader"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Hp_Gatto"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/HP/GattoIcon_%d.png"), 9))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype - Prototype_Component_Texture_Hp_Gatto : CLoader"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Hp_Gatto_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/HP/GattoIconBack.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype - Prototype_Component_Texture_Hp_Gatto_Back : CLoader"));
		return E_FAIL;
	}

#pragma endregion Texture_UI_Heart


#pragma region Texture_UI_Skill

	/* For.Prototype_Component_Texture_UI_Skill*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_UI_Skill"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Skill/Player_Skill_%d.png"), 18))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Skill_Back*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_UI_Skill_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Skill/SkillBack.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_UI_Skill_Icon*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_UI_Skill_Icon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Skill/Player_Skill_%d.png"), 18))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Skill_Gauge*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_UI_Skill_Gauge"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Skill/SkillGauge%d.png"), 3))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Skill_Slot*/

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_UI_Skill_Slot"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Skill/Skill_Slot.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_UI_Skill_Gauge_Sq*/

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_UI_Skill_Gauge_Sq"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Skill/SkillGaugeSq%d.png"), 3))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Skill_CoolTime*/

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_UI_Skill_CoolTime"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Skill/Player_Skill_Cool.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

#pragma endregion Texture_UI_Skill
#pragma region Texture_UI_Weapon
	/* For.Prototype_Component_Texture_UI_Weapon_Back */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_UI_Weapon_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/GunBack%d.png"), 2))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Weapon_Icon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_UI_Weapon_Icon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/Gun%d.png"), 12))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_WeaponEx_Icon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_UI_WeaponEx_Icon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/ExGun%d.png"), 12))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Weapon_Main */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_UI_Weapon_Main"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/GunGauge%d.png"), 3))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Weapon_Mini */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_UI_Weapon_Mini"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/MiniGunGauge%d.png"), 3))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Weapon_Back_Dot */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_UI_Weapon_Back_Dot"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/GunGaugeSq%d.png"), 3))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_UI_Weapon_Mini_Check"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/GunPreparHalftone.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_UI_Weapon_Mini_Check1"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/SlotCheck%d.png"), 2))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_UI_Weapon_Bulllet_Icon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/UI_Bullet%d.png"), 4))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

#pragma endregion Texture_UI_Weapon

#pragma region Texture_Red_Circle
	/* For.Prototype_Component_Texture_Red_Circle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_Red_Circle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/보스 몬스터/RedCircle/RedCircle.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

#pragma endregion Texture_Red_Circle

#pragma region Texture_Sparrow
	/* For.Prototype_Component_Texture_UI_Sparrow0 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_UI_Sparrow0"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Zoom/zoomUI_0_%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Sparrow1 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_UI_Sparrow1"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Zoom/zoomUI_1_%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Sparrow2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_UI_Sparrow2"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Zoom/zoomUI_2_%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Sparrow3 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_UI_Sparrow3"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Zoom/zoomUI_3_%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Sparrow4 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_UI_Sparrow4"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Zoom/zoomUI_4_%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Sparrow5 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_Texture_UI_Sparrow5"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Zoom/zoomUI_5_%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Texture_Sparrow

#pragma endregion normal 필수 텍스쳐


#pragma region VIBuffer
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL1, TEXT("Prototype_Component_VIBuffer_Terrain_Normal1"),
	//	CVIBuffer_Terrain::Create(m_pGraphic_Device, 31, 26))))
	//{
	//	MSG_BOX(TEXT("failed to Add_Prototype : CMainApp"));
	//	return E_FAIL;
	//}
#pragma endregion VIBuffer

#pragma region Prototype
	/* For.Prototype_GameObject_Trigger_Summon */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Trigger_Summon"), CTrigger_Summon::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Monster_Three_Red_Eyes */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Three_Red_Eyes"), CMonster_Three_Red_Eyes::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Monster_Bullet */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Three_Red_Eyes_Bullet"), CMonsterBullet::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Monster_White_Fly */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_White_Fly"), CMonster_White_Fly::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_White_Monster_Bullet */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_White_Monster_Bullet"), CWhite_Monster_Bullet::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Monster_Red_Fly */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Red_Fly"), CMonster_Red_Fly::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Red_Monster_Bullet */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Red_Monster_Bullet"), CRed_Monster_Bullet::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Monster_Jump_PinkSlime */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Jump_PinkSlime"), CMonster_Jump_PinkSlime::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	// 준수 추가
	/* For.Prototype_GameObject_SpawnEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpawnEffect"), CSpawnEffect::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Prototype

	m_strLoadingText = TEXT("로딩이 완료되었습니다.");
	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_1945()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_1945, TEXT("Prototype_Component_Texture_Trigger_BlackHole"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Object/Trigger/BlackHole.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma region  Texture

#pragma region Texture_Effect_Destroy

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_1945, TEXT("Prototype_Component_Texture_Effect_Destroy"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/Explosion/Destroy/Explosion%d.png"), 90))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

#pragma endregion Texture_Effect_Destroy

#pragma region Texture_UI_Boss
	/* For.Prototype_Component_Texture_UI_Boss_Hp */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_1945, TEXT("Prototype_Component_Texture_UI_Boss_Hp"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Boss/BossHP%d.png"), 4))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Boss_Hp_Back */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_1945, TEXT("Prototype_Component_Texture_UI_Boss_Groggy"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Boss/BossGroggy%d.png"), 4))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Texture_UI_Boss

	/* For.Prototype_Component_Texture_Emp */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_1945, TEXT("Prototype_Component_Texture_Emp"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Object/AirPlane/Emp.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	// 준수 추가
	/* For.Prototype_Component_Texture_Item_1945 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_1945, TEXT("Prototype_Component_Texture_Item_1945"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Item/Item_1945/Item_1945_%d.png"), 6))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma region Texture_Terrain
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_1945, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Terrain/Tile%d.png"), 19))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_RedRect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_1945, TEXT("Prototype_Component_Texture_RedRect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/Steam/RedRect/RedRect.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

#pragma endregion Texture_Terrain

#pragma region Texture_Sky
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_1945, TEXT("Prototype_Component_Texture_Sky_1945"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXCUBE, TEXT("../Bin/Resource/Map/Sky/Universe_Sky.dds")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Texture_Sky
#pragma region Texture_AirPlane

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_1945, TEXT("Prototype_Component_Texture_AirPlane"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Object/AirPlane/AirPlane.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_1945, TEXT("Prototype_Component_Texture_TurretBody"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXCUBE, TEXT("../Bin/Resource/Object/AirPlane/Turret/TurretBody.dds")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_1945, TEXT("Prototype_Component_Texture_TurretMouse"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXCUBE, TEXT("../Bin/Resource/Object/AirPlane/Turret/TurretMouse.dds")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_1945, TEXT("Prototype_Component_Texture_TurretMouse_Sphere"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Object/AirPlane/Turret/TurretMouseSphere.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

#pragma endregion Texture_AirPlane

#pragma endregion Texture

#pragma region Texture_Monster
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_1945, TEXT("Prototype_Component_Texture_ShootMonster"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/1945ShootMonster/ShootMonster.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_1945, TEXT("Prototype_Component_Texture_ShootMonster_Bullet"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/1945ShootMonster/ShootMonsterBullet/Bullet.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_1945, TEXT("Prototype_Component_Texture_MoonStone"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Object/MoonStone/MoonStone.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_1945, TEXT("Prototype_Component_Texture_BossMonster"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/1945BossMonster/BossMonster.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_1945, TEXT("Prototype_Component_Texture_BossBattleCruiserLayser"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXCUBE, TEXT("../Bin/Resource/Monster/1945BossMonster/Layser/Layser.dds")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

#pragma endregion Texture_Monster
#pragma region Texture_UI_AirPlane

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_1945, TEXT("Prototype_Component_Texture_UI_AirPlane_Gauge"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Ship/Ship_Gauge%d.png"),4))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_1945, TEXT("Prototype_Component_Texture_UI_AirPlane_HP"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Ship/ship_hp.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_1945, TEXT("Prototype_Component_Texture_UI_AirPlane_Monitor"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Ship/Ship_Monitor_%d.png"),2))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_1945, TEXT("Prototype_Component_Texture_UI_AirPlane_Frame"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Ship/Ship_SideBack_BG_Frame%d.png"),2))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_1945, TEXT("Prototype_Component_Texture_UI_AirPlane_Side_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Ship/Ship_SideBack_BG_Line_%d.png"),2))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_1945, TEXT("Prototype_Component_Texture_UI_AirPlane_Side_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Ship/Ship_SideBack_BG%d.png"),2))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_1945, TEXT("Prototype_Component_Texture_UI_AirPlane_SKill"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Ship/Ship_Skill%d.png"),4))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Texture_UI_AirPlane
#pragma endregion Texture

#pragma region VIBuffer


	/* For.Prototype_Component_VIBuffer_AirPlane */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_1945, TEXT("Prototype_Component_VIBuffer_AirPlane"),
		CVIBuffer_AirPlane::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("failed to Add_Prototype : CMainApp"));
		return E_FAIL;
	}

	/* For.Prototype_Component_VIBuffer_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_1945, TEXT("Prototype_Component_VIBuffer_Sphere"),
		CVIBuffer_Sphere::Create(m_pGraphic_Device,5,5))))
	{
		MSG_BOX(TEXT("failed to Add_Prototype : CMainApp"));
		return E_FAIL;
	}

#pragma endregion VIBuffer
	/* For.Prototype_GameObject_Trigger_BlackHole */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Trigger_BlackHole"), CTrigger_BlackHole::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_AirPlane */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AirPlane"), CAirPlane::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_AirPlane_Turret_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AirPlane_Turret_Body"), CAirPlaneTurretBody::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_AirPlane_Turret_Mouse */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AirPlane_Turret_Mouse"), CAirPlaneTurretMouse::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_ShootMonster */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShootMonster"), CShootMonster::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_ShootMonster_Bullet */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShootMonster_Bullet"), CShootMonsterBullet::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_BossBattleCruiser */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossBattleCruiser"), CBossBattleCruiser::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_BossBattleCruiser_Layser */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossBattleCruiser_Layser"), CBossLayser::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_UI_AirPlane */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_AirPlane"), CUI_AirPlane::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_UI_Boss */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Boss_1945"), CUI_Boss::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader, Prototype_GameObject_UI_Boss_1945"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_MoonStone */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MoonStone"), CMoonStone::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Camera_Player_DW2 */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Player_DW2"), CCamera_Player_DW2::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}	

	/* For.Prototype_GameObject_QAim2 */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_QAim2"), CQAim2::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Comet */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Comet"), CComet::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	
	/* For.Prototype_GameObject_EMP */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EMP"), CEmp::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	// 준수 추가
	/* For.Prototype_GameObject_Item_1945 */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item_1945"), CItem_1945::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	
	/* For.Prototype_GameObject_RedRect */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RedRect"), CRedRect::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Destroy_Effect */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Destroy_Effect"), CDestroy_Effect::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Destroy_Effect */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_Destroy_Effect"), CBoss_Destroy_Effect::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	m_strLoadingText = TEXT("로딩이 완료되었습니다.");
	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Normal2()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Trigger_Door"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXCUBE, TEXT("../Bin/Resource/Object/Trigger/Trigger_Door%d.dds"),3))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Trigger_Door_In"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Object/Trigger/Trigger_Door1.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma region normal 필수 텍스쳐
#pragma region Texture_Effect
	/* For.Prototype_Component_Texture_Effect_Explosion */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Effect_Explosion"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/Explosion/Explosion_%d.png"), 9))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Effect_Splat */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Effect_Splat"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/Splat/spiky circular.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Effect_Splat_Orange */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Effect_Splat_Orange"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/SplatOrange/Splat%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Effect_Splat_Blue */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Effect_Splat_Blue"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/SplatBlue/Splat%d.png"), 5))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_Effect_Splat_Default */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Effect_Splat_Default"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/SplatDefault/Splat%d.png"), 9))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}


	/* For.Prototype_Component_Texture_Effect_Player_Move */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Effect_Player_Move"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/Cloud/Effect_Player_Move%d.png"), 5))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Effect_Sparkle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Effect_Sparkle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/Sparkle/Sparkle%d.png"), 2))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Effect_Barrier */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Effect_Barrier"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/Barrier/Barrier%d.png"), 2))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Texture_Effect

#pragma region Texture_Monster 
	/* For.Prototype_Component_Texture_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Monster"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Object/Monster/Awler/Awler_Battle_1_%d.png"), 45))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Monster_Three_Red_Eyes_Idle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Monster_Three_Red_Eyes_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/ThreeEyesMonster/Idle/Awler_Idle_%d.png"), 128))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Monster_Three_Red_Eyes_Move */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Monster_Three_Red_Eyes_Move"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/ThreeEyesMonster/Move/Awler_Move_%d.png"), 64))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Monster_Three_Red_Eyes_Roll */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Monster_Three_Red_Eyes_Roll"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/ThreeEyesMonster/Roll/Awler_Roll_%d.png"), 64))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Monster_Three_Red_Eyes_GunBattle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Monster_Three_Red_Eyes_GunBattle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/ThreeEyesMonster/GunBattle/Awler_Battle_%d.png"), 360))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Monster_Three_Red_Eyes_Death */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Monster_Three_Red_Eyes_Death"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/ThreeEyesMonster/Death/Awler_Death_%d.png"), 5))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Monster_Three_Red_Eyes_Bullet */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Monster_Three_Red_Eyes_Bullet"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/ThreeEyesMonster/Bullet/Bullet0.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}


	/* For.Prototype_Component_Texture_Indicator_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Indicator_Monster"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Indicator/indicator_monster.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma region Texture_White_Fly_Monster
	/* For.Prototype_Component_Texture_White_Fly_Monster_Idle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_White_Fly_Monster_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/WhiteFlyMonster/Idle/Idle%d.png"), 16))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_White_Fly_Monster_Attak */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_White_Fly_Monster_Attak"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/WhiteFlyMonster/Attak/Attak%d.png"), 4))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_White_Fly_Monster_Death */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_White_Fly_Monster_Death"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/WhiteFlyMonster/Death/Death%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_White_Fly_Monster_Bullet_Idle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_White_Fly_Monster_Bullet_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/WhiteFlyMonster/Bullet/Idle/Bullet%d.png"), 6))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_White_Fly_Monster_Bullet_Death */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_White_Fly_Monster_Bullet_Death"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/WhiteFlyMonster/Bullet/Death/Bullet_Death%d.png"), 7))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

#pragma endregion Texture_White_Fly_Monster

#pragma region Texture_Red_Fly_Monster

	/* For.Prototype_Component_Texture_Red_Fly_Monster_Idle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Red_Fly_Monster_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/RedFlyMonster/Idle/Idle%d.png"), 16))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Red_Fly_Monster_Attak */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Red_Fly_Monster_Attak"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/RedFlyMonster/Attak/Attak%d.png"), 4))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Red_Fly_Monster_Death */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Red_Fly_Monster_Death"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/RedFlyMonster/Death/Death%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Red_Fly_Monster_Bullet_Idle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Red_Fly_Monster_Bullet_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/RedFlyMonster/Bullet/Idle/Bullet%d.png"), 6))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Red_Fly_Monster_Bullet_Death */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Red_Fly_Monster_Bullet_Death"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/RedFlyMonster/Bullet/Death/Bullet_Death%d.png"), 20))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}


#pragma endregion Texture_Red_Fly_Monster

#pragma region Texture_Pink_Slime_Monster

	/* For.Prototype_Component_Texture_Pink_Slime_Monster_Idle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Pink_Slime_Monster_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/PinkSlimeMonster/Idle/Idle%d.png"), 32))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Pink_Slime_Monster_Jump_Attak */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Pink_Slime_Monster_Jump_Attak"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/PinkSlimeMonster/JumpAttak/JumpAttak%d.png"), 64))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Pink_Slime_Monster_Death */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Pink_Slime_Monster_Death"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/PinkSlimeMonster/Death/Death%d.png"), 10))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

#pragma endregion Texture_Pink_Slime_Monster

#pragma region Texture_Steam

	/* For.Prototype_Component_Texture_Steam */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Steam"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/Steam/Steam.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_RedRect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_RedRect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/Steam/RedRect/RedRect.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

#pragma region Texture_SteamAttack

	/* For.Prototype_Component_Texture_SteamAttack */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_SteamAttack"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/Steam/Splinter.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

#pragma endregion Texture_SteamAttack

#pragma endregion Texture_Steam 

#pragma endregion Texture_Monster 

#pragma region Texture_Sky
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Sky_Normal2"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXCUBE, TEXT("../Bin/Resource/Map/Sky/Sky.dds")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Texture_Sky


#pragma region Texture_UI_Heart

	/* For.Prototype_Component_Texture_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_UI"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/HP/HeartBack.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	//TO DO: 예은 추가
	/* For.Prototype_Component_Texture_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Hp_UI"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/HP/hp_UI_%d.png"), 21))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype - Prototype_Component_Texture_Hp_UI : CLoader"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Hp_Gatto"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/HP/GattoIcon_%d.png"), 9))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype - Prototype_Component_Texture_Hp_Gatto : CLoader"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Hp_Gatto_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/HP/GattoIconBack.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype - Prototype_Component_Texture_Hp_Gatto_Back : CLoader"));
		return E_FAIL;
	}

#pragma endregion Texture_UI_Heart


#pragma region Texture_UI_Skill

	/* For.Prototype_Component_Texture_UI_Skill*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_UI_Skill"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Skill/Player_Skill_%d.png"), 18))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Skill_Back*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_UI_Skill_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Skill/SkillBack.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_UI_Skill_Icon*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_UI_Skill_Icon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Skill/Player_Skill_%d.png"), 18))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Skill_Gauge*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_UI_Skill_Gauge"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Skill/SkillGauge%d.png"), 3))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Skill_Slot*/

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_UI_Skill_Slot"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Skill/Skill_Slot.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_UI_Skill_Gauge_Sq*/

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_UI_Skill_Gauge_Sq"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Skill/SkillGaugeSq%d.png"), 3))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Skill_CoolTime*/

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_UI_Skill_CoolTime"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Skill/Player_Skill_Cool.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

#pragma endregion Texture_UI_Skill
#pragma region Texture_UI_Weapon
	/* For.Prototype_Component_Texture_UI_Weapon_Back */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_UI_Weapon_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/GunBack%d.png"), 2))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Weapon_Icon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_UI_Weapon_Icon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/Gun%d.png"), 12))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_WeaponEx_Icon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_UI_WeaponEx_Icon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/ExGun%d.png"), 12))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Weapon_Main */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_UI_Weapon_Main"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/GunGauge%d.png"), 3))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Weapon_Mini */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_UI_Weapon_Mini"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/MiniGunGauge%d.png"), 3))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Weapon_Back_Dot */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_UI_Weapon_Back_Dot"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/GunGaugeSq%d.png"), 3))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_UI_Weapon_Mini_Check"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/GunPreparHalftone.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_UI_Weapon_Mini_Check1"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/SlotCheck%d.png"), 2))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_UI_Weapon_Bulllet_Icon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/UI_Bullet%d.png"), 4))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

#pragma endregion Texture_UI_Weapon

#pragma region Texture_UI_Sparrow
	/* For.Prototype_Component_Texture_UI_Sparrow0 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_UI_Sparrow0"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Zoom/zoomUI_0_%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Sparrow1 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_UI_Sparrow1"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Zoom/zoomUI_1_%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Sparrow2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_UI_Sparrow2"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Zoom/zoomUI_2_%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Sparrow3 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_UI_Sparrow3"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Zoom/zoomUI_3_%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Sparrow4 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_UI_Sparrow4"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Zoom/zoomUI_4_%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Sparrow5 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_UI_Sparrow5"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Zoom/zoomUI_5_%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Texture_Sparrow
#pragma region Texture_Red_Circle
	/* For.Prototype_Component_Texture_Red_Circle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_Texture_Red_Circle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/보스 몬스터/RedCircle/RedCircle.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

#pragma endregion Texture_Red_Circle
#pragma endregion normal 필수 텍스쳐


#pragma region VIBuffer
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NORMAL2, TEXT("Prototype_Component_VIBuffer_Terrain_Normal1"),
	//	CVIBuffer_Terrain::Create(m_pGraphic_Device, 31, 26))))
	//{
	//	MSG_BOX(TEXT("failed to Add_Prototype : CMainApp"));
	//	return E_FAIL;
	//}
#pragma endregion VIBuffer

#pragma region Prototype
	/* For.Prototype_GameObject_Steam */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Steam"), CSteam::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Trigger_Camera */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Trigger_Camera"), CTrigger_Camera::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_SteamAttack */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SteamAttack"), CSteamAttack::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Item_Effect */
	/*if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item_Effect"), CItem_Effect::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}*/
#pragma endregion Prototype

	m_strLoadingText = TEXT("로딩이 완료되었습니다.");
	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_SnowBoss()
{
	m_strLoadingText = TEXT("텍스쳐(레벨별 원형)를(을) 로딩 중 입니다.");


	/* For.Prototype_Component_Texture_Ending */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_Ending"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Title/Ending.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_Ending_Logo */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_Ending_Logo"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Title/Logo_%d.png"),30))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}


#pragma region Texture_Effect
	/* For.Prototype_Component_Texture_Effect_Explosion */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_Effect_Explosion"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/Explosion/Explosion_%d.png"), 9))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Effect_Splat */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_Effect_Splat"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/Splat/spiky circular.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Effect_Splat_Orange */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_Effect_Splat_Orange"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/SplatOrange/Splat%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Effect_Splat_Blue */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_Effect_Splat_Blue"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/SplatBlue/Splat%d.png"), 5))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_Effect_Splat_Default */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_Effect_Splat_Default"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/SplatDefault/Splat%d.png"), 9))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}


	/* For.Prototype_Component_Texture_Effect_Player_Move */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_Effect_Player_Move"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/Cloud/Effect_Player_Move%d.png"), 5))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Effect_Sparkle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_Effect_Sparkle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/Sparkle/Sparkle%d.png"), 2))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Effect_Barrier */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_Effect_Barrier"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/Barrier/Barrier%d.png"), 2))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Texture_Effect



#pragma region Texture_Boss_Yeti

	/* For.Prototype_Component_Texture_Boss_Yeti_Idle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_Boss_Yeti_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/보스 몬스터/Idle/Idle%d.png"), 24))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Boss_Yeti_JumpAttak */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_Boss_Yeti_JumpAttak"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/보스 몬스터/JumpAttak/JumpAttak%d.png"), 24))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Boss_Yeti_JumpAttakEnd */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_Boss_Yeti_JumpAttakEnd"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/보스 몬스터/JumpAttakEnd/JumpAttakEnd%d.png"), 16))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Boss_Yeti_GoBottom */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_Boss_Yeti_GoBottom"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/보스 몬스터/GoBottom/GoBottom%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Boss_Yeti_UpBottom */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_Boss_Yeti_UpBottom"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/보스 몬스터/UpBottom/UpBottom%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Boss_Yeti_RollAttak */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_Boss_Yeti_RollAttak"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/보스 몬스터/RollAttak/RollAttak%d.png"), 24))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Boss_Yeti_Death */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_Boss_Yeti_Death"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/보스 몬스터/Death/Death%d.png"), 24))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Boss_Yeti_SkillReady */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_Boss_Yeti_SkillReady"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/보스 몬스터/SkillReady/SkillReady%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/보스 몬스터/Snow/Snow.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_IceSplinter */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_IceSplinter"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/보스 몬스터/IceSplinter/IceSplinter.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Earth_Quake */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_Earth_Quake"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/earthquake_tex.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

#pragma endregion Texture_Boss_Yeti

#pragma region Texture_Sky
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_Sky_Boss"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXCUBE, TEXT("../Bin/Resource/Map/Sky/Sky.dds")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Texture_Sky

#pragma region Texture_UI

#pragma region Texture_Sparrow
	/* For.Prototype_Component_Texture_UI_Sparrow0 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_UI_Sparrow0"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Zoom/zoomUI_0_%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Sparrow1 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_UI_Sparrow1"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Zoom/zoomUI_1_%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Sparrow2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_UI_Sparrow2"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Zoom/zoomUI_2_%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Sparrow3 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_UI_Sparrow3"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Zoom/zoomUI_3_%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Sparrow4 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_UI_Sparrow4"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Zoom/zoomUI_4_%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Sparrow5 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_UI_Sparrow5"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Zoom/zoomUI_5_%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Texture_Sparrow
#pragma region Texture_UI_Heart

	/* For.Prototype_Component_Texture_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_UI"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/HP/HeartBack.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	//TO DO: 예은 추가
	/* For.Prototype_Component_Texture_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_Hp_UI"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/HP/hp_UI_%d.png"), 21))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype - Prototype_Component_Texture_Hp_UI : CLoader"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_Hp_Gatto"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/HP/GattoIcon_%d.png"), 9))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype - Prototype_Component_Texture_Hp_Gatto : CLoader"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_Hp_Gatto_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/HP/GattoIconBack.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype - Prototype_Component_Texture_Hp_Gatto_Back : CLoader"));
		return E_FAIL;
	}

#pragma endregion Texture_UI_Heart

#pragma region Texture_UI_Aim

	/* For.Prototype_Component_Texture_Aim */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_QAim"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Cursor/inGameCursor_%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Texture_UI_Aim

#pragma region Texture_UI_Skill

	/* For.Prototype_Component_Texture_UI_Skill*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_UI_Skill"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Skill/Player_Skill_%d.png"), 18))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Skill_Back*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_UI_Skill_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Skill/SkillBack.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_UI_Skill_Icon*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_UI_Skill_Icon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Skill/Player_Skill_%d.png"), 18))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Skill_Gauge*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_UI_Skill_Gauge"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Skill/SkillGauge%d.png"), 3))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Skill_Slot*/

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_UI_Skill_Slot"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Skill/Skill_Slot.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_UI_Skill_Gauge_Sq*/

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_UI_Skill_Gauge_Sq"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Skill/SkillGaugeSq%d.png"), 3))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Skill_CoolTime*/

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_UI_Skill_CoolTime"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Skill/Player_Skill_Cool.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

#pragma endregion Texture_UI_Skill
#pragma region Texture_UI_Weapon
	/* For.Prototype_Component_Texture_UI_Weapon_Back */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_UI_Weapon_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/GunBack%d.png"), 2))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Weapon_Icon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_UI_Weapon_Icon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/Gun%d.png"), 12))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_WeaponEx_Icon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_UI_WeaponEx_Icon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/ExGun%d.png"), 12))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Weapon_Main */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_UI_Weapon_Main"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/GunGauge%d.png"), 3))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Weapon_Mini */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_UI_Weapon_Mini"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/MiniGunGauge%d.png"), 3))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Weapon_Back_Dot */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_UI_Weapon_Back_Dot"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/GunGaugeSq%d.png"), 3))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_UI_Weapon_Mini_Check"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/GunPreparHalftone.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_UI_Weapon_Mini_Check1"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/SlotCheck%d.png"), 2))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_UI_Weapon_Bulllet_Icon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Gun/UI_Bullet%d.png"), 4))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

#pragma endregion Texture_UI_Weapon

#pragma endregion Texture_UI
#pragma region Texture_Wall

	/* For.Prototype_Component_Texture_Wall */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_Wall_Boss"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Wall/Wall%d.png"), 12))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion
#pragma region Texture_UI_Boss
	/* For.Prototype_Component_Texture_UI_Boss_Hp */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_UI_Boss_Hp"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Boss/BossHP%d.png"), 4))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_UI_Boss_Hp_Back */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_UI_Boss_Groggy"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Boss/BossGroggy%d.png"), 4))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Texture_UI_Boss

#pragma region Texture_Red_Circle
	/* For.Prototype_Component_Texture_Red_Circle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_Texture_Red_Circle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Monster/보스 몬스터/RedCircle/RedCircle.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

#pragma endregion Texture_Red_Circle



#pragma region VIBuffer
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_VIBuffer_Terrain_Boss"),
		CVIBuffer_Terrain::Create(m_pGraphic_Device, 31, 26))))
	{
		MSG_BOX(TEXT("failed to Add_Prototype : CMainApp"));
		return E_FAIL;
	}

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_VIBuffer_Cube_Boss"),
		CVIBuffer_Cube::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("failed to Add_Prototype : CMainApp"));
		return E_FAIL;
	}

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("failed to Add_Prototype : CMainApp"));
		return E_FAIL;
	}

	/* For.Prototype_Component_VIBuffer_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SNOWBOSS, TEXT("Prototype_Component_VIBuffer_Sphere"),
		CVIBuffer_Sphere::Create(m_pGraphic_Device, 36, 15))))
	{
		MSG_BOX(TEXT("failed to Add_Prototype : CMainApp"));
		return E_FAIL;
	}

#pragma endregion VIBuffer

#pragma region Collider


#pragma endregion Collider

	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("객체의 원형를(을) 로딩 중 입니다.");

#pragma region Prototype

	/* For.Prototype_GameObject_Wall */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Wall_Boss"), CWall::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_WallTerrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Wall_Terrain_Boss"), CWall::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky_Boss"), CSky::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}



	/* For.Prototype_GameObject_UI_Boss */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Boss"), CUI_Boss::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader, Prototype_GameObject_UI_Boss"));
		return E_FAIL;
	}
	/* For.Prototype_GameObject_UI_Ending */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Ending"), CUI_Ending::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader, Prototype_GameObject_UI_Ending"));
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Boss_Yeti */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_Yeti"), CBossYeti::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Snow"), CSnow::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_GameObject_IceSplinter */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_IceSplinter"), CIceSplinter::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Earth_Quake */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Earth_Quake"), CEarthQuake::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Prototype_GameObject_Blizzard */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Blizzard"), CBlizzard::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}


#pragma endregion Prototype
	m_strLoadingText = TEXT("로딩이 완료되었습니다.");
	m_isFinished = true;
	return S_OK;
}



CLoader* CLoader::Create(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eNextLevelID)
{
	CLoader* pInstance = new CLoader(pGraphic_Device);
	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Faild to Created : CLoader"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{

	WaitForSingleObject(m_hThread, INFINITE); //?

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_Critical_Section);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pGraphic_Device);
}
