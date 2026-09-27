#include "pch.h"
#include "ShootMonster.h"
#include "GameInstance.h"
#include "Bullet.h"
#include "State.h"
#include "Camera_Player_DW.h"
#include "ShootMonsterBullet.h"
#include "Weapon.h"
#include "AirPlane.h"
#include "Boss_Destroy_Effect.h"
#include "Item_1945.h"

CShootMonster::CShootMonster(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CLandObject(pGraphic_Device)
{
}

CShootMonster::CShootMonster(const CShootMonster& rhs) :
	CLandObject(rhs)
{
}

HRESULT CShootMonster::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CShootMonster"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CShootMonster::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		LANDOBJECT_DESC* pLandObjectDesc = (LANDOBJECT_DESC*)pArg;
		pLandObjectDesc->fSpeedPerSec = 1.f;
		pLandObjectDesc->fRotationPerSec = 1.f;

		MONSTER_SHOOT_MONSTER_DESC* pShootMonsterDesc = (MONSTER_SHOOT_MONSTER_DESC*)pArg;
		m_eDirState = pShootMonsterDesc->eMoveDir;
		m_vStartPos = pShootMonsterDesc->vPos;
		m_fScale = pShootMonsterDesc->fScale;
	}

	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Initialize : __super,CShootMonster"));
		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX(TEXT("Failed to Add_Components : __super,CShootMonster"));
		return E_FAIL;
	}

	//m_pTransform->Set_State(CTransform::STATE_RIGHT, m_pTransform->Get_State(CTransform::STATE_RIGHT) * 5);
	//m_pTransform->Set_State(CTransform::STATE_UP, m_pTransform->Get_State(CTransform::STATE_UP) * 5);
	//m_pTransform->Set_State(CTransform::STATE_LOOK, m_pTransform->Get_State(CTransform::STATE_LOOK) * 5);

	
	m_pTransform->Set_State(CTransform::STATE_LOOK, _float3(0.f, 0.f, -1.f));
	m_pTransform->Set_State(CTransform::STATE_RIGHT, _float3(-1.f, 0.f, 0.f));
	m_pTransform->Set_State(CTransform::STATE_UP, _float3(0.f, 1.f, 0.f));

	m_pTransform->Set_State(CTransform::STATE_POSITION, m_vStartPos);

	switch (m_eDirState)
	{
	case DIR_DOWN:

		//m_pTransform->Set_State(CTransform::STATE_POSITION, _float3((_float)(rand() % (_int)m_vStartPos.x) + 8.f, 0.f, 20.f));

		break;

	case DIR_RIGHT:
		// z값 랜덤
		//m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(0.f, 0.f, (_float)(rand() % (_int)m_vStartPos.z)));
		m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_UP), D3DXToRadian(-90.f));
		break;

	case DIR_LEFT:
		// z값 랜덤
		//m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(50.f, 0.f, (_float)(rand() % (_int)m_vStartPos.z)));
		m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_UP), D3DXToRadian(90.f));
		break;

	case DIR_RIGHT_DOWN:
		// z값 랜덤
		//m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(0.f, 0.f, (_float)(rand() % (_int)m_vStartPos.z) + 25.f));
		m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_UP), D3DXToRadian(-45.f));
		break;

	case DIR_LEFT_DOWN:
		// z값 랜덤
		//m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(50.f, 0.f, (_float)(rand() % (_int)m_vStartPos.z) + 25.f));
		m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_UP), D3DXToRadian(45.f));
		break;

	default:
		break;
	}

	m_pTransform->Set_Scale(_float3(m_fScale, m_fScale, m_fScale));

	

	//m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(10.f,0.5f,10.f)); //_float3((_float)(rand() % 20), 0.5f, (_float)(rand() % 20)));
	//TODO: 용수 콜라이더가 너무 커서 줄임
	m_pCollider_Com[COLLIDER_RECT]->Set_Scale(_float3(m_fScale * 2.f, m_fScale * 2.f, 1.f));
	m_pCollider_Com[COLLIDER_SPHERE]->Set_Scale(_float3(1.f, 1.f, 1.f));
	m_pCollider_Com[COLLIDER_CUBE_AABB]->Set_Scale(_float3(1.f, 1.f, 1.f));
	m_pCollider_Com[COLLIDER_CUBE_AABB]->Update_Collider_Info(*m_pTransform->Get_WorldMatrix());

	return S_OK;
}

_uint CShootMonster::Tick(_float fTimeDelta)
{
	if (LEVEL_LOADING == m_pGameInstance->Get_Level())
		return 0;

	m_fIdleAngle += 1.f;
	m_fTime += fTimeDelta;

	if (m_fTime > m_fSootBulletTime && !m_bShootBullet)
	{
		Shot_Bullet();
		m_bShootBullet = !m_bShootBullet;
	}

	if (m_bDead)
	{
		m_pUI_Damage->Set_RealDead();
		return 1;
	}
	if (m_eNowState != STATE_DEATH)
	{
		//__super::SetUp_OnTerrain(0.f);
		__super::Tick(fTimeDelta);
		CheckPlayer();
	}

	Do_State(fTimeDelta);


	if (m_pGameInstance->Key_Down('K')) //타노스 버튼 // 죽을 때 설정해 주어야 하는 것
	{
		//Set_Dead();
		//m_fTime = 0.f;
		m_eNowState = STATE_DEATH;
		m_bDead = true;
	}

	if (m_fTime > m_fLifeTime)
	{
		Set_Dead();
	}

	m_pUI_Damage->Update_Position(Return_ViewPort_Pos(), Get_ViewZ());

	//TODO: 용수 충돌처리 테스트
	_float4x4 WorldMatrix = *m_pTransform->Get_WorldMatrix();
	m_pCollider_Com[COLLIDER_RECT]->Update_Collider_Info(WorldMatrix);
	m_pCollider_Com[COLLIDER_SPHERE]->Update_Collider_Info(WorldMatrix);
	m_pCollider_Com[COLLIDER_CUBE_AABB]->Update_Collider_Info(WorldMatrix);
	Collider_Billboarding();
	return OBJECT_NOTHING;
}

void CShootMonster::Late_Tick(_float fTimeDelta)
{
	if (LEVEL_LOADING == m_pGameInstance->Get_Level())
		return;

	if (m_bDead && m_bDeadShow)
	{
		return;
	}
	__super::Late_Tick(fTimeDelta);

	//충돌 처리
	//기능 나를 제외한 오브젝트들을 순회하면서 충돌을 판단한다.
	//몬스터끼리의 밀리는 건 스피어로 처리하자. 그리고 어디서 그걸 해줄지는 생각해봐야할듯.
	if (STATE_DEATH != m_eNowState)
	{
		Collision_Bullet(fTimeDelta);
		//Collision_Wall();
	}



	//if (m_eNowState == STATE_DEATH)
	//{
	//	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
	//	return;
	//}

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CShootMonster::Render()
{
	if (LEVEL_LOADING == m_pGameInstance->Get_Level())
		return S_OK;
	//if (FAILED(__super::Render()))
	//{
	//	MSG_BOX(TEXT("Failed to Render : __super,CPlayer"));
	//	return E_FAIL;
	//}
	if (m_bDead && m_bDeadShow)
	{
		return S_OK;
	}
	if (FAILED(m_pTransform->Bind_WorldMatrix()))
	{
		MSG_BOX(TEXT("Failed to Bind_WorldMatrix : Render"));
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->Bind_Texture(0, 0))) // 첫번째 인자는 0 몇 번째 텍스쿠드좌표를 사용할 지?
	{
		MSG_BOX(TEXT("Failed to Bind_Texture : Render"));
		return E_FAIL;
	}
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	if (FAILED(Set_RenderState()))
	{
		MSG_BOX(TEXT("Failed to Set_RenderState : Render"));
		return E_FAIL;
	}
	if (FAILED(m_pVIBuffer_Com->Render()))
	{
		MSG_BOX(TEXT("Failed to Render : Render"));
		return E_FAIL;
	}
	
	if (FAILED(Reset_RenderState()))
	{
		MSG_BOX(TEXT("Failed to Reset_RenderState : Render"));
		return E_FAIL;
	}


	return S_OK;
}

void CShootMonster::Return_Look_Position(_float3* pvLook, _float3* pvPosition)
{
	*pvLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
	*pvPosition = m_pTransform->Get_State(CTransform::STATE_POSITION);
}


void CShootMonster::Do_State(_float fTimeDelta)
{
	m_fTime += 1.f * fTimeDelta;
	
	m_pTransform->Go_Straight(5.f * fTimeDelta);

}


void CShootMonster::CheckPlayer()
{
	const CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));

	_float3 fCheck = { pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_pTransform->Get_State(CTransform::STATE_POSITION) };

	if (D3DXVec3Length(&fCheck) <= m_fCheckDestence)
	{
		m_vMonsterLook = fCheck;
		m_bCheckPlayer = true;
	}
	else
	{
		m_bCheckPlayer = false;
	}
}

HRESULT CShootMonster::Add_Components()
{	
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_VIBuffer_AirPlane"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_AirPlane : Add_Components"));
		return E_FAIL;
	}


	//State추가
	CState::STATE_DESC StateDest = {};
	StateDest.fCP = 1.f;
	StateDest.fMaxHp = 15.f;

	/* For. Com_State*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_State"), TEXT("Com_State"), reinterpret_cast<CComponent**>(&m_pState_Com), &StateDest)))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_State : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Texture */
	//if (FAILED(__super::Add_Component(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_PlayerBack"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	//{
	//	MSG_BOX(TEXT("Failed to Prototype_Component_Texture_PlayerBack : Add_Components"));
	//	return E_FAIL;
	//}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_ShootMonster"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_ShootMonster : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Rect"), TEXT("Com_Collider_Rect"), reinterpret_cast<CComponent**>(&m_pCollider_Com[COLLIDER_RECT]))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Collider_Rect : Add_Components"));
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pCollider_Com[COLLIDER_SPHERE]))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Collider_Rect : Add_Components"));
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube_AABB"), TEXT("Com_Collider_Cube_AABB"), reinterpret_cast<CComponent**>(&m_pCollider_Com[COLLIDER_CUBE_AABB]))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Collider_Cube_AABB : Add_Components"));
		return E_FAIL;
	}

	return S_OK;
}


HRESULT CShootMonster::Set_RenderState()
{
	if (m_eNowState == STATE_DEATH)
	{

		if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE)))
		{
			MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHABLENDENABLE"));
			return E_FAIL;
		}
		if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA)))
		{
			MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_SRCBLEND"));
			return E_FAIL;
		}
		if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA)))
		{
			MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_DESTBLEND"));
			return E_FAIL;
		}
		if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD)))
		{
			MSG_BOX(TEXT("Failed to SetRenderState : D3DBLENDOP_ADD"));
			return E_FAIL;
		}

		m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		return S_OK;
	}

	if (FAILED(m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR)))
	{
		MSG_BOX(TEXT("Failed to SetSamplerState : D3DSAMP_MINFILTER"));
		return E_FAIL;
	}
	if (FAILED(m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR)))
	{
		MSG_BOX(TEXT("Failed to SetSamplerState : D3DSAMP_MAGFILTER"));
		return E_FAIL;
	}
	if (FAILED(m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR)))
	{
		MSG_BOX(TEXT("Failed to SetSamplerState : D3DSAMP_MIPFILTER"));
		return E_FAIL;
	}

	if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHATESTENABLE"));
		return E_FAIL;
	}
	if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200))) // 테스트 할 알파값
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHAREF"));
		return E_FAIL;
	}
	if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHAFUNC"));
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CShootMonster::Reset_RenderState()
{
	if (m_eNowState == STATE_DEATH)
	{
		m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE)))
		{
			MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHATESTENABLE"));
			return E_FAIL;
		}

		if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE)))
		{
			MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHABLENDENABLE"));
			return E_FAIL;
		}
		return S_OK;
	}


	if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHATESTENABLE"));
		return E_FAIL;
	}

	if (FAILED(m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR)))
	{
		MSG_BOX(TEXT("Failed to SetSamplerState : D3DSAMP_MINFILTER"));
		return E_FAIL;
	}
	if (FAILED(m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR)))
	{
		MSG_BOX(TEXT("Failed to SetSamplerState : D3DSAMP_MAGFILTER"));
		return E_FAIL;
	}
	if (FAILED(m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR)))
	{
		MSG_BOX(TEXT("Failed to SetSamplerState : D3DSAMP_MIPFILTER"));
		return E_FAIL;
	}
	return S_OK;
}

void CShootMonster::Chase_Player(_float fTimeDelta, _float Min_Distance)
{
	const CTransform* pTransform = dynamic_cast<const CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), g_strTransformTag));

	_float3 vPosition = pTransform->Get_State(CTransform::STATE_POSITION);

	m_pTransform->LookAt_LandObject(vPosition);

	m_pTransform->Move_To_Target(vPosition, fTimeDelta, Min_Distance);
}

_bool CShootMonster::MousePicking()
{
	_float3 vRayDir = {};
	_float3 vRayPos = {};

	// 플레이어를 인식하는 범위에 들어온다면
	if (!m_bCheckPlayer)
	{
		return false;
	}

	// 카메라가 숄더뷰라면?
	if (dynamic_cast<CCamera_Player_DW*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Camera_Player")))->Get_Camera_Mode() != CCamera_Player_DW::CAMERA_PLAYER_RIGHT_VIEW)
		return false;

	// 만약 다 성립하고 마우스가 있다면?
	m_pGameInstance->Get_World_Mouse_Ray(&vRayDir, &vRayPos);
	if (m_pGameInstance->Collision_Rect_Ray_Same_Space(dynamic_cast<CCollider_Rect*>(m_pCollider_Com[COLLIDER_RECT]), vRayDir, vRayPos))
	{
		return true;
	}

	//if ((GetAsyncKeyState('F') & 0x8000))
	//{
	//	return true;
	//}
	return false;
}

void CShootMonster::Shot_Bullet()
{
	CMonster_Bullet_Base::MONSTER_BULLET_BASE_DESC MonsterBulletDesc = {};

	MonsterBulletDesc.pTerrainTranformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(g_eLevel, TEXT("Layer_Terrain"), g_strTransformTag.c_str()));
	MonsterBulletDesc.pTerrainVIBufferCom = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(g_eLevel, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));

	MonsterBulletDesc.m_fSpeed = 8.0f;
	MonsterBulletDesc.m_fLifeTime = 15.f;
	// 플레이어의 위치좌표 - 내 위치좌표 (방향벡터 구하기) 그 후 정규화
	MonsterBulletDesc.m_vLook = *D3DXVec3Normalize(&MonsterBulletDesc.m_vLook, &_float3(
		dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_1945, TEXT("Layer_AirPlane"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION)
	 - m_pTransform->Get_State(CTransform::STATE_POSITION)));

	MonsterBulletDesc.m_fScale = 0.5f;
	MonsterBulletDesc.m_vPos = m_pTransform->Get_State(CTransform::STATE_POSITION);

	m_pGameInstance->Add_Clone(g_eLevel, TEXT("Layer_Monster_Bullet"), TEXT("Prototype_GameObject_ShootMonster_Bullet"), &MonsterBulletDesc);

	++m_iAttakNum;
}

void CShootMonster::Collision_Bullet(_float fTimeDelta)
{
	CAirPlane* pPlayer = dynamic_cast<CAirPlane*>(m_pGameInstance->Get_Object(LEVEL_1945, TEXT("Layer_AirPlane")));
	//CWeapon* pWeapons[3] = { pPlayer->Get_Weapon(0),pPlayer->Get_Weapon(1) ,pPlayer->Get_Weapon(2) };


	if (nullptr == pPlayer)
		return;
	_uint iRand = rand() % 2+2;

	_float fTotalDamage = pPlayer->Collision_Bullet_Rect(dynamic_cast<CCollider_Rect*>(m_pCollider_Com[COLLIDER_RECT]), fTimeDelta);
	if (0.f < fTotalDamage)
		m_pUI_Damage->Hit_Damage(fTotalDamage);
	if (OBJECT_DEAD == m_pState_Com->Set_Hp(-fTotalDamage))
	{
		Set_Dead();
		Drop_Item();
		switch (iRand)
		{
		case 2:
			m_pGameInstance->StopSound(CSound_Manager::MONSTER3);
			m_pGameInstance->PlaySoundW(TEXT("Explosion.wav"), CSound_Manager::MONSTER3, 0.8f);
			break;
		case 3:
			m_pGameInstance->StopSound(CSound_Manager::MONSTER4);
			m_pGameInstance->PlaySoundW(TEXT("Explosion.wav"), CSound_Manager::MONSTER4, 0.8f);
			break;
		default:
			break;
		}

		// 준수 추가 2/5
		CBoss_Destroy_Effect::BOSS_DESTROY_EFFECT_DESC BossDestroyEffectDesc = {};
		BossDestroyEffectDesc.vPosition = m_pTransform->Get_State(CTransform::STATE_POSITION);
		BossDestroyEffectDesc.fMaxFrame = 89.f;
		BossDestroyEffectDesc.fScale = 3.f;
		if (FAILED(m_pGameInstance->Add_Clone(g_eLevel, TEXT("Layer_DestroyEffect"), TEXT("Prototype_GameObject_Boss_Destroy_Effect"), &BossDestroyEffectDesc)))
		{
			MSG_BOX(TEXT("Failed to Ready_Layer_Destroy_Effect : CLevel_GamePlay"));
			return;
		}

	}
}

void CShootMonster::Collision_Wall()
{
	_float3 vRayPos = m_pTransform->Get_State(CTransform::STATE_POSITION);
	_float3 vRayDir[4] = {
		 m_pTransform->Get_State(CTransform::STATE_LOOK),
		 m_pTransform->Get_State(CTransform::STATE_RIGHT),
		 -m_pTransform->Get_State(CTransform::STATE_LOOK),
		 -m_pTransform->Get_State(CTransform::STATE_RIGHT)
	};
	_float fLength[4] = {
		1.f,
		1.f,
		1.f,
		1.f
		/*D3DXVec3Length(&vRayDir[0]),
		D3DXVec3Length(&vRayDir[1]),
		D3DXVec3Length(&vRayDir[2]),
		D3DXVec3Length(&vRayDir[3])*/
	};
	for (size_t i = 0; i < 4; i++)
	{
		D3DXVec3Normalize(&vRayDir[i], &vRayDir[i]);
	}

	list<CGameObject*>* pWallList = m_pGameInstance->Get_List(g_eLevel, TEXT("Layer_Wall"));
	if (nullptr == pWallList)
		return;

	_float fMinDist(1000.f); //큰 값으로 초기화
	for (auto& iter : *pWallList)//Wall List 전부를 순회하면서 충돌을 판단한다.
	{
		CCollider_Rect* pCollider_Rect = dynamic_cast<CCollider_Rect*>(iter->Get_Component(TEXT("Com_Collider_Rect")));
		if (nullptr == pCollider_Rect)
			continue;
		_float fDist(1000.f);
		for (size_t i = 0; 4 > i; ++i)
		{
			if (m_pGameInstance->Collision_Rect_Ray_Same_Space(pCollider_Rect, vRayDir[i], vRayPos, fLength[i], &fDist)) //충돌이 일어났을 때
			{
				if (fMinDist > fDist) //
				{
					fMinDist = fDist;
					_float3 vPos = vRayPos + vRayDir[i] * (fMinDist - 1.f);
					m_pTransform->Set_State(CTransform::STATE_POSITION, vPos);
					_float4x4 WorldMatrix = *m_pTransform->Get_WorldMatrix();
					m_pCollider_Com[COLLIDER_RECT]->Update_Collider_Info(WorldMatrix);
					m_pCollider_Com[COLLIDER_SPHERE]->Update_Collider_Info(WorldMatrix);
					m_pCollider_Com[COLLIDER_CUBE_AABB]->Update_Collider_Info(WorldMatrix);
				}
			}
		}
	}
}

void CShootMonster::Collider_Billboarding()
{
	CTransform* pCameraTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_1945, TEXT("Layer_Camera"), g_strTransformTag));

	if (nullptr == pCameraTransformCom)
	{
		MSG_BOX(TEXT("nullptr == pCameraTransformCom : Collider_Billboarding()"));
		return;
	}

	const _float4x4 pCameraWorldMatrix = *pCameraTransformCom->Get_WorldMatrix();


	_float3 vScale = m_pCollider_Com[COLLIDER_RECT]->Get_Scale();

	m_pCollider_Com[COLLIDER_RECT]->Set_State(CCollider::STATE_RIGHT, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0]) * vScale.x);
	//m_pTransform->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_UP][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_UP][0]) * vScale.y);
	m_pCollider_Com[COLLIDER_RECT]->Set_State(CCollider::STATE_LOOK, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0]) * vScale.z);

	/*m_pTransform->Set_State(CTransform::STATE_RIGHT, *(_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0]);
	m_pTransform->Set_State(CTransform::STATE_LOOK, *(_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0]);*/
}

HRESULT CShootMonster::Drop_Item()
{
	_uint iRand = rand() % 5;
	if (0 != iRand)
		return S_OK;

	CItem::ITEM_DESC ItemDesc = {};
	ItemDesc.vPos = m_pTransform->Get_State(CTransform::STATE_POSITION);
	ItemDesc.fRotationPerSec = D3DXToRadian(90.f);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_1945, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item_1945"), &ItemDesc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_Item_1945 : CLevel_GamePlay"));
		return E_FAIL;
	}

	return S_OK;
}


CShootMonster* CShootMonster::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CShootMonster* pInstance = new CShootMonster(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Faild to Created : CShootMonster"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShootMonster::Clone(void* pArg)
{
	CShootMonster* pInstance = new CShootMonster(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Faild to Cloned : CShootMonster"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShootMonster::Free()
{
	Safe_Release(m_pVIBuffer_Com);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pCollider_Com[COLLIDER_RECT]);
	Safe_Release(m_pCollider_Com[COLLIDER_SPHERE]);
	Safe_Release(m_pCollider_Com[COLLIDER_CUBE_AABB]);
	Safe_Release(m_pState_Com);
	Safe_Release(m_pCameraTransform);
	__super::Free();
}
