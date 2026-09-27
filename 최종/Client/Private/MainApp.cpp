#include "pch.h"
#include "../Public/MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "State.h"
#include "Loading.h"

CMainApp::CMainApp() : m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);

	/* 알파블렌드 : 알파값을 기준으로 픽셀을 섞는다. */
	/*m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);*/

	/* vSourColor : 내가 그릴려고하는 색. */
	/* vDestColor : 이미 그려져있던 색.  */
	//_float4		vSourColor = (1.f, 0.f, 0.f, 0.0f), vDestColor = (0.f, 1.f, 0.f, 1.f);
	//_float4		vResult;

	//vResult = vSourColor.rgb * vSourColor.a + vDestColor.rgb * (1.f - vSourColor.a);

	//(0.5f, 0.5f, 0.f, 1.f)





	///* */
	//m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, FALSE);
	//m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, false);

	///* 하늘을 그린다. */

	//m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

HRESULT CMainApp::Initialize()
{
	ENGINE_DESC EngineDesc = {};
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;


	/* 내 게임의 기초 초기화 과정을 거치자. */
	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, EngineDesc, &m_pGraphic_Device)))
	{
		MSG_BOX(TEXT("Failed to Initialize_Engine : CMainApp"));
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Default"), TEXT("함초롬바탕"), 30, 50, FW_HEAVY)))
	{
		MSG_BOX(TEXT("Failed to Add_Font : CMainApp"));
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Comic_Shark"), TEXT("Comic Shark"), 10, 10, FW_THIN)))
	{
		MSG_BOX(TEXT("Failed to Add_Font : CMainApp"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Comic_Shark1"), TEXT("Comic Shark"), 10, 15, FW_THIN)))
	{
		MSG_BOX(TEXT("Failed to Add_Font : CMainApp"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_BB"), TEXT("BadaBoom BB"), 20, 30, FW_NORMAL)))
	{
		MSG_BOX(TEXT("Failed to Add_Font : CMainApp"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_BB1"), TEXT("BadaBoom BB"), 20, 30, FW_NORMAL)))
	{
		MSG_BOX(TEXT("Failed to Add_Font : CMainApp"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_gool"), TEXT("굴림체"), 10, 20, FW_HEAVY)))
	{
		MSG_BOX(TEXT("Failed to Add_Font : CMainApp"));
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_gool1"), TEXT("굴림체"), 10, 20, FW_BOLD)))
	{
		MSG_BOX(TEXT("Failed to Add_Font : CMainApp"));
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_gool2"), TEXT("굴림체"), 15, 25, FW_HEAVY)))
	{
		MSG_BOX(TEXT("Failed to Add_Font : CMainApp"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_gool3"), TEXT("굴림체"), 15, 15, FW_HEAVY)))
	{
		MSG_BOX(TEXT("Failed to Add_Font : CMainApp"));
		return E_FAIL;
	}

	if (FAILED(Ready_Default_Setting()))
	{
		MSG_BOX(TEXT("Failed to Ready_Default_Setting : CMainApp"));
		return E_FAIL;
	}

	if (FAILED(Ready_Prototype_Component_For_Static()))
	{
		MSG_BOX(TEXT("Failed to Ready_Prototype_Component_For_Static : CMainApp"));
		return E_FAIL;
	}
		
	if (FAILED(Open_Level(LEVEL_LOGO)))
	{
		MSG_BOX(TEXT("Failed to Open_Level : CMainApp::Initialize()"));
		return E_FAIL;
	}

	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	if(nullptr == m_pGameInstance)
	{
		MSG_BOX(TEXT("nullptr == m_pGameInstance : CMainApp"));
		return;
	}
	m_pGameInstance->Tick_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance)
	{
		MSG_BOX(TEXT("nullptr == m_pGameInstance : CMainApp"));
		return E_FAIL;
	}
	m_pGameInstance->Draw();

	return S_OK;
}

HRESULT CMainApp::Ready_Default_Setting()
{
	if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_LIGHTING"));
		return E_FAIL;
	}
	/*if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_CULLMODE"));
		return;
	}*/

	if (FAILED(m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DSAMP_MINFILTER"));
		return E_FAIL;
	}
	if (FAILED(m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DSAMP_MAGFILTER"));
		return E_FAIL;
	}
	if (FAILED(m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DSAMP_MIPFILTER"));
		return E_FAIL;
	}
	if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE,TRUE)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DSAMP_MIPFILTER"));
		return E_FAIL;
	}
	ShowCursor(false);
	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL eLevelID)
{
	if (LEVEL_LOADING == eLevelID)
	{
		MSG_BOX(TEXT("NextLevelID Error : CMainApp"));
		return E_FAIL;
	}

	return m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pGraphic_Device, eLevelID));
}

HRESULT CMainApp::Ready_Prototype_Component_For_Static()
{

#pragma region VIBuffer
	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("failed to Add_Prototype : CMainApp"));
		return E_FAIL;
	}

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("failed to Add_Prototype : CMainApp"));
		return E_FAIL;
	}

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube_Boss"),
		CVIBuffer_Cube::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("failed to Add_Prototype : CMainApp"));
		return E_FAIL;
	}

	/* For.Prototype_Component_VIBuffer_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Sphere"),
		CVIBuffer_Sphere::Create(m_pGraphic_Device, 36, 15))))
	{
		MSG_BOX(TEXT("failed to Add_Prototype : CMainApp"));
		return E_FAIL;
	}
#pragma endregion VIBuffer

#pragma region Texture_Terrain

	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Terrain/Tile%d.png"), 19))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Texture_Terrain

#pragma region Texture_Wall

	/* For.Prototype_Component_Texture_Wall */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Wall"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Wall/Wall%d.png"), 37))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Texture_Wall

#pragma region Texture_UI_Aim

	/* For.Prototype_Component_Texture_Aim */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_QAim"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Cursor/inGameCursor_%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Texture_UI_Aim

#pragma region Texture_UI_Damage

	/* For.Prototype_Component_Texture_UI_Damage */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Damage"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Number/Number/%d.png"), 10))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Texture_UI_Damage

#pragma region Texture_Effect
	/* For.Prototype_Component_Texture_Effect_Explosion */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Explosion"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/Explosion/Explosion_%d.png"), 9))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Effect_Splat */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Splat"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/Splat/spiky circular.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Effect_Splat_Orange */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Splat_Orange"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/SplatOrange/Splat%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Effect_Splat_Blue */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Splat_Blue"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/SplatBlue/Splat%d.png"), 5))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_Effect_Splat_Default */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Splat_Default"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/SplatDefault/Splat%d.png"), 9))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Effect_Splat_Rambo */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Splat_Rambo"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/PupleCircle/PupleCircle.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Effect_Splat_Orange_Circle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Splat_Orange_Circle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/SplatOrange/Circle/Splat.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Effect_Splat_White_Circle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Splat_White_Circle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/SplatWhiteCircle/Splat%d.png"), 9))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Effect_Splat_Sasin */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Splat_Sasin"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/Splat_Sasin/Splat%d.png"), 9))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Effect_Splat_Sasin_Spacle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Splat_Sasin_Spacle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/Splat_Sasin/Spacle/Splat%d.png"), 9))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}


	/* For.Prototype_Component_Texture_Effect_Player_Move */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Player_Move"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/Cloud/Effect_Player_Move%d.png"), 5))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Effect_Sparkle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Sparkle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/Sparkle/Sparkle%d.png"), 2))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Effect_Barrier */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Barrier"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/Barrier/Barrier%d.png"), 2))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	// 준수 추가
	/* For.Prototype_Component_Texture_Item_Effect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WeaponItemEffect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/WeaponEffect/Rambo_Effect/WeaponEffect_%d.png"), 100))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_SpawnEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SpawnEffect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/Spawn/SpawnEffect_0.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Texture_Effect

#pragma region Texture_Sky
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sky_Boss"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXCUBE, TEXT("../Bin/Resource/Map/Sky/Sky.dds")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Texture_Sky

#pragma region Texture_Object

	/* For.Prototype_Component_Texture_Object_Box */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Object_Box"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXCUBE, TEXT("../Bin/Resource/Object/Box/LibraryWooden%d.dds"), 4))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Texture_Object
#pragma region Texture_Item_1945
	// 준수 추가
/* For.Prototype_Component_Texture_Item_1945 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_1945"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Item/Item_1945/Item_1945_%d.png"), 6))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Texture_Item_1945
#pragma region Texture_Player
	/* For.Prototype_Component_Texture_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Object/Player/Gatto_Idle_%d.png"), 128))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Move"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Object/Player/Gatto_Move_%d.png"), 64))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Roll"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Object/Player/Gatto_Roll_%d.png"), 64))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_TPS"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Object/Player/Gatto_TPS_%d.png"), 16))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Death"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Object/Player/Gatto_Die_%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_TPS_Body"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Object/Player/Gatto_TPS_Body_%d.png"), 36))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_TPS_Head"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Object/Player/Gatto_TPS_Head_%d.png"), 4))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_TPS_Hand"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Object/Player/Gatto_TPS_Hand_%d.png"), 13))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

#pragma endregion Texture_Player

#pragma region Texture_Weapon
	/* For.Prototype_Component_Texture_Weapon_Pupa */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Weapon_Pupa"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Weapon/Pupa_%d.png"), 19))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Weapon_Rambo */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Weapon_Rambo"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Weapon/Rambo_%d.png"), 19))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Weapon_Sasin */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Weapon_Sasin"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Weapon/Sasin_%d.png"), 19))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Weapon_Sparrow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Weapon_Sparrow"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Weapon/Sparrow_%d.png"), 19))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Weapon_SharkBlood */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Weapon_SharkBlood"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Weapon/SharkBlood_%d.png"), 19))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Weapon_Atlas */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Weapon_Atlas"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Weapon/Atlas_%d.png"), 19))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_WeaponEx_Pupa */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WeaponEx_Pupa"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Weapon/Pupa_Reinforce_12.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_WeaponEx_Rambo */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WeaponEx_Rambo"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Weapon/Rambo_Reinforce_%d.png"), 19))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_WeaponEx_Sasin */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WeaponEx_Sasin"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Weapon/Sasin_Reinforce_%d.png"), 19))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_WeaponEx_Sparrow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WeaponEx_Sparrow"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Weapon/Sparrow_Reinforce_%d.png"), 19))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_WeaponEx_SharkBlood */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WeaponEx_SharkBlood"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Weapon/SharkBlood_Reinforce_%d.png"), 19))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_WeaponEx_Atlas */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WeaponEx_Atlas"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Weapon/Atlas_Reinforce_%d.png"), 19))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Bullet */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Bullet"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Object/Bullet/Bullet/Bullet%d.png"), 8))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}

#pragma endregion Texture_Weapon
	
#pragma region Texture_Item
	/* For.Prototype_Component_Texture_Item_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_Weapon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Item/Item_Weapon/Item_Weapon_%d.png"), 6))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
	/* For.Prototype_Component_Texture_Item_Hamburger */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_Hamburger"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Item/Item_Hamburger/hamburger.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Texture_Item

#pragma region Texture_TrailRenderer
	/* For.Prototype_Component_Texture_TrailRenderer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TrailRenderer"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Effect/glow.png")))))
	{
		MSG_BOX(TEXT("failed to Add_Prototype : CMainApp"));
		return E_FAIL;
	}
#pragma endregion Texture_TrailRenderer


#pragma region Texture_Loading
	/* For.Prototype_Component_Texture_Item_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Title/Loading_Back.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Bar"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/Title/LoadingBar.png")))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Texture_Loading



#pragma region Collider
	/* For.Prototype_Component_Collider_Rect */  
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Rect"),
		CCollider_Rect::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("failed to Add_Prototype : CMainApp"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider_Sphere::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("failed to Add_Prototype : CMainApp"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Collider_Cube_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube_AABB"),
		CCollider_Cube_AABB::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("failed to Add_Prototype : CMainApp"));
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Collider */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Collider"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resource/UI/Collider/ColliderTexture%d.png"), 3))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}
#pragma endregion Collider

/* For.Prototype_Component_State */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_State"),
		CState::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("failed to Add_Prototype : CMainApp"));
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Loading"), CLoading::Create(m_pGraphic_Device))))
	{
		MSG_BOX(TEXT("Faild to Add_Prototype : CLoader"));
		return E_FAIL;
	}


	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Loading"))))
	//{
	//	MSG_BOX(TEXT("Failed to Ready_Layer_BackGround : CLevel_Logo"));
	//	return E_FAIL;
	//}

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pMainApp = new CMainApp;
	if (FAILED(pMainApp->Initialize()))
	{
		MSG_BOX(TEXT("Faild to Created : CMainApp"));

		Safe_Release(pMainApp);
	}

	return pMainApp;
}

void CMainApp::Free()
{

	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();

	__super::Free();
}
