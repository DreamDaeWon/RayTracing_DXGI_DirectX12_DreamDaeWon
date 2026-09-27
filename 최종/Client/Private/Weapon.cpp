#include "pch.h"
#include "Weapon.h"
#include "GameInstance.h"
#include "Player.h"
#include "Camera_Player_DW.h"
#include "Bullet.h"
#include "Effect_Hit_Bullet.h"
#include "Collider_Rect.h"
#include "Splat_Default.h"

CWeapon::CWeapon(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CGameObject(pGraphic_Device)
{
}

CWeapon::CWeapon(const CWeapon& rhs) :
	CGameObject(rhs),
	m_eID(rhs.m_eID),
	m_bActivate(rhs.m_bActivate),
	m_fFrame(rhs.m_fFrame),
	m_fShootingTerm(rhs.m_fShootingTerm),
	m_fGauge(0.0f),
	m_fMaxGauge(rhs.m_fMaxGauge),
	m_eBulletID(rhs.m_eBulletID),
	m_iExtraBulletNum(rhs.m_iExtraBulletNum),
	m_iMaxBulletNum(rhs.m_iMaxBulletNum),
	m_iNowBulletNum(rhs.m_iNowBulletNum),
	m_fCP(rhs.m_fCP),
	m_fTerm(rhs.m_fTerm),
	m_eShooting_Way(rhs.m_eShooting_Way),
	m_iQAimSize(rhs.m_iQAimSize),
	m_fReboundSize(rhs.m_fReboundSize)
{
}


HRESULT CWeapon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CWeapon_Pupa"));
		return E_FAIL;
		m_eID = WEAPON_PUPA;
	}
	return S_OK;
}

HRESULT CWeapon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Initialize : CWeapon"));
		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX(TEXT("Failed to Add_Components ,CWeapon"));
		return E_FAIL;
	}
	m_vScale = m_pTransform->Get_Scale();
	m_fFrame = 0.f;

	return S_OK;
}

_uint CWeapon::Tick(_float fTimeDelta)
{
	if (LEVEL_LOADING == m_pGameInstance->Get_Level() || LEVEL_1945 == m_pGameInstance->Get_Level())
		return 0;

	if (m_bActivate == true)
	{
		if (m_bEx)
		{
			m_fGauge -= fTimeDelta*40.f;
			if (m_fGauge < 0.f)
			{
				m_fGauge = 0.f;
				m_bEx = false;
			}

		}


		if (0.f >= m_fShootingTerm && (STATE_SHOOTING == m_eCurState))
			m_eCurState = STATE_AIMING;

		if (0.f < m_fShootingTerm)
			m_fShootingTerm -= fTimeDelta;

		//if (m_pGameInstance->Key_Pressing(VK_RBUTTON) && m_pGameInstance->Key_Down(VK_LBUTTON))
		//	m_fLifeTime = EFFECT_SPLAT_LIFE_TIME;

		__super::Tick(fTimeDelta);
		Motion_Change();

	}
	return OBJECT_NOTHING;
}

void CWeapon::Late_Tick(_float fTimeDelta)
{
	_uint iLevelIndex = m_pGameInstance->Get_Level();
	if (LEVEL_LOADING == m_pGameInstance->Get_Level() || LEVEL_1945 == m_pGameInstance->Get_Level())
		return;
	//======임시 플레이어 바인딩
	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), g_strTransformTag));
	_float3 vPlayerRight = pPlayerTransform->Get_State(CTransform::STATE_RIGHT);
	_float3 vPlayerUP = pPlayerTransform->Get_State(CTransform::STATE_UP);
	_float3 vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);
	_float3 vPlayerPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	switch (m_eCurState)
	{
	case Client::CWeapon::STATE_IDLE:
		m_pTransform->Set_State(CTransform::STATE_POSITION, vPlayerPosition - vPlayerLook * 0.012f + (-vPlayerUP) * 0.08f+vPlayerRight * 0.03f);
		break;
	case Client::CWeapon::STATE_AIMING:
	case Client::CWeapon::STATE_SHOOTING:
		m_pTransform->Set_State(CTransform::STATE_POSITION, vPlayerPosition + vPlayerRight * 0.3f + vPlayerLook * 0.01f + ( - vPlayerUP)*0.07f);

		break;
	case Client::CWeapon::STATE_END:
		break;
	default:
		break;
	}

	//빌보딩
	//CCamera_Player_DW* pCamera = dynamic_cast<CCamera_Player_DW*>(m_pGameInstance->Get_Object(g_eLevel, TEXT("Layer_Camera_Player")));
	
	if (nullptr == m_pCamera)
	{
		m_pCamera = dynamic_cast<CCamera_Player_DW*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Camera_Player")));
		if (nullptr != m_pCamera)
			Safe_AddRef(m_pCamera);
	}
	m_iCameraMode = m_pCamera->Get_Camera_Mode();
	if (4 == m_iCameraMode)
		SetUp_BillBoard_Camera();
	else
		SetUp_BillBoard();

	//m_pTransform->Set_State(CTransform::STATE_RIGHT, -m_pTransform->Get_State(CTransform::STATE_RIGHT));

	//m_fFrame = 12.f;
	//======================
	__super::Late_Tick(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CWeapon::Render()
{
	if (m_bActivate == false||(m_eID == WEAPON_PUPA&&m_eCurState==STATE_IDLE))
		return S_OK;

	if (FAILED(m_pTransform->Bind_WorldMatrix()))
	{
		MSG_BOX(TEXT("Failed to Bind_WorldMatrix : Render"));
		return E_FAIL;
	}
	if (m_bEx)
	{
		if (m_eID == WEAPON_PUPA)
		{
			if (FAILED(m_pTextureCom1->Bind_Texture(0, 0)))
			{
				MSG_BOX(TEXT("Failed to Bind_Texture : Render"));
				return E_FAIL;
			}
		}
		else
		{
			if (FAILED(m_pTextureCom1->Bind_Texture(0, (_uint)m_fFrame)))
			{
				MSG_BOX(TEXT("Failed to Bind_Texture : Render"));
				return E_FAIL;
			}
		}
	
	}
	else
	{
		if (FAILED(m_pTextureCom->Bind_Texture(0, (_uint)m_fFrame)))
		{
			MSG_BOX(TEXT("Failed to Bind_Texture : Render"));
			return E_FAIL;
		}
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

HRESULT CWeapon::Set_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 32);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	return S_OK;
}

HRESULT CWeapon::Reset_RenderState()
{
	if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHATESTENABLE"));
		return E_FAIL;
	}
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	return S_OK;
}

void CWeapon::SetUp_BillBoard()
{
	if (m_eID == WEAPON_PUPA)
		return;

	_uint iLevelIndex = m_pGameInstance->Get_Level();

	_float3 vScale = m_pTransform->Get_Scale();
	m_pTransform->Set_State(CTransform::STATE_RIGHT, _float3(1.f, 0.f, 0.f) * vScale.x);
	m_pTransform->Set_State(CTransform::STATE_UP, _float3(0.f, 1.f, 0.f) * vScale.y);
	m_pTransform->Set_State(CTransform::STATE_LOOK, _float3(0.f, 0.f, 1.f) * vScale.z);



	//CTransform* pCameraTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_TUTORIAL, TEXT("Layer_Camera_Free"), g_strTransformTag));
	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), g_strTransformTag));
	_float3 vPlayerPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	_float3 vPlayerLook =* dynamic_cast<CPlayer*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Get_LookPos();
	if (m_eCurState != STATE_AIMING)
	{
		if ((_uint)m_fTextureDir == 0)
		{
			if (m_iSlotIndex)
				m_fFrame = 0.f;
			else
				m_fFrame = 4.f;
		}
		else if ((_uint)m_fTextureDir == 1)
		{
			if (m_iSlotIndex)
				m_fFrame = 1.f;
			else
				m_fFrame = 3.f;
		}
		else if ((_uint)m_fTextureDir == 2)
		{
			if (m_iSlotIndex)
				m_fFrame = 18.f;
			else
				m_fFrame = 2.f;
		}
		else if ((_uint)m_fTextureDir == 3)
		{
			if (m_iSlotIndex)
				m_fFrame = 3.f;
			else
				m_fFrame = 1.f;
		}
		else if ((_uint)m_fTextureDir == 4)
		{
			if (m_iSlotIndex)
				m_fFrame = 4.f;
			else
				m_fFrame = 0.f;
		}
		else if ((_uint)m_fTextureDir == 5)
		{
			if (m_iSlotIndex)
				m_fFrame = 3.f;
			else
				m_fFrame = 1.f;
		}
		else if ((_uint)m_fTextureDir == 6)
		{
			if (m_iSlotIndex)
				m_fFrame = 2.f;
			else
				m_fFrame = 18.f;
		}
		else if ((_uint)m_fTextureDir == 7)
		{
			if (m_iSlotIndex)
				m_fFrame = 1.f;
			else
				m_fFrame = 3.f;
		}

	}
	
	m_pTransform->Set_State(CTransform::STATE_POSITION, vPlayerPosition - vPlayerLook * 0.1f);

	//CTransform* pCameraTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(iLevelIndex, TEXT("Layer_Camera_Player"), g_strTransformTag));
	if (nullptr == m_pCameraTransform)
	{
		m_pCameraTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera_Player"), g_strTransformTag));
		if (nullptr != m_pCameraTransform)
			Safe_AddRef(m_pCameraTransform);
	}

	if (nullptr == m_pCameraTransform)
	{
		MSG_BOX(TEXT("nullptr == m_pCameraTransformCom : CMonster::SetUp_BillBoard()"));
		return;
	}

	const _float4x4 pCameraWorldMatrix = *m_pCameraTransform->Get_WorldMatrix();

	m_pTransform->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0]) * vScale.x);
	//m_pTransform->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_UP][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_UP][0]) * vScale.y);
	m_pTransform->Set_State(CTransform::STATE_LOOK, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0]) * vScale.z);
	
	//m_pTransform->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_UP][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_UP][0]) * vScale.y);

}

void CWeapon::SetUp_BillBoard_Camera()
{
	
	//CTransform* pCameraTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_TUTORIAL, TEXT("Layer_Camera_Free"), g_strTransformTag));
	//CTransform* pCameraTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(g_eLevel, TEXT("Layer_Camera_Player"), g_strTransformTag));
	if (nullptr == m_pCameraTransform)
	{
		m_pCameraTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera_Player"), g_strTransformTag));
		if (nullptr != m_pCameraTransform)
			Safe_AddRef(m_pCameraTransform);
	}

	if (nullptr == m_pCameraTransform)
	{
		MSG_BOX(TEXT("nullptr == m_pCameraTransformCom : CMonster::SetUp_BillBoard()"));
		return;
	}

	const _float4x4 pCameraWorldMatrix = *m_pCameraTransform->Get_WorldMatrix();

	if (m_iSlotIndex && m_eCurState == STATE_IDLE)
		m_fFrame = 0.f;
	else if(m_eCurState == STATE_IDLE)
		m_fFrame = 4.f;
	_float3 vScale = m_pTransform->Get_Scale();

	m_pTransform->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0]) * vScale.x);
	m_pTransform->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_UP][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_UP][0]) * vScale.y);
	m_pTransform->Set_State(CTransform::STATE_LOOK, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0]) * vScale.z);
}

void CWeapon::Motion_Change()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case Client::CWeapon::STATE_IDLE:
			m_fFrame = 0.f;
			break;
		case Client::CWeapon::STATE_AIMING:
			m_fFrame = 12.f;
			break;
		case Client::CWeapon::STATE_SHOOTING:
			m_fFrame = 17.f;
			break;
		case Client::CWeapon::STATE_END:
			break;
		default:
			break;
		}
		m_ePreState = m_eCurState;
	}
}

void CWeapon::Set_Idle()
{
	m_eCurState = STATE_IDLE;
}

void CWeapon::Plus_GunGauge(_float fPlusGauge)
{
	if (!m_bEx)
	{
		m_fGauge += fPlusGauge;
		if (m_fGauge >= m_fMaxGauge)
		{
			m_fGauge = m_fMaxGauge;
			m_bEx = true;
			m_iNowBulletNum = m_iMaxBulletNum;
			m_pGameInstance->StopSound(CSound_Manager::EFFECT);
			m_pGameInstance->PlaySoundW(TEXT("GetReinforceItem.wav"), CSound_Manager::CHANNEL_WEAPON, 0.2f);
		}
	}
}

void CWeapon::Reload()
{
	//엇 보니까 없어서 구현함_김천 갔다오고나서임
	_uint iSum = m_iNowBulletNum + m_iExtraBulletNum; //현재+남아있는거
	m_iNowBulletNum = min(iSum, m_iMaxBulletNum); // 더한거, 탄창하나
	m_iExtraBulletNum = iSum - m_iNowBulletNum;

}

_float CWeapon::Collision_Bullet_Rect(CCollider_Rect* pCollider_Rect, _float fTimeDelta)
{
	
	_float fResult(0.f);
	CEffect_Hit_Bullet* pEffectHitBullet = dynamic_cast<CEffect_Hit_Bullet*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Effect_Hit_Bullet")));

	for (size_t i = 0; i < m_iMaxBulletNum; i++)
	{
		if (m_vecBullet[i]->Get_Dead())
			continue;

		_float fLength(0.f), fDistance(0.f);
		_float3 vRayDir = { 0.f,0.f,0.f };
		_float3 vRayPos = { 0.f,0.f,0.f };

		m_vecBullet[i]->Get_RayPos_RayDir_Length(fTimeDelta, &vRayPos, &vRayDir, &fLength);
		if (m_pGameInstance->Collision_Rect_Ray_Same_Space(pCollider_Rect, vRayDir, vRayPos, fLength, &fDistance))
		{
			m_vecBullet[i]->Set_Dead();
			pEffectHitBullet->Show_Explosion(vRayPos + vRayDir * (fDistance - 0.01f), fTimeDelta);
			fResult += m_fCP;
		}
	}
	return fResult;
}

void CWeapon::Collision_Bullet_Rect_For_Box(CCollider_Rect* pCollider_Rect, _float fTimeDelta)
{
	CEffect_Hit_Bullet* pEffectHitBullet = dynamic_cast<CEffect_Hit_Bullet*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Effect_Hit_Bullet")));

	for (size_t i = 0; i < m_iMaxBulletNum; i++)
	{
		if (m_vecBullet[i]->Get_Dead())
			continue;

		_float fLength(0.f), fDistance(0.f);
		_float3 vRayDir = { 0.f,0.f,0.f };
		_float3 vRayPos = { 0.f,0.f,0.f };

		m_vecBullet[i]->Get_RayPos_RayDir_Length(fTimeDelta, &vRayPos, &vRayDir, &fLength);
		if (m_pGameInstance->Collision_Rect_Ray_Same_Space(pCollider_Rect, vRayDir, vRayPos, fLength, &fDistance))
		{
			m_vecBullet[i]->Set_Dead();
			pEffectHitBullet->Show_Explosion(vRayPos + vRayDir * (fDistance - 0.5f), fTimeDelta);
		}
	}
	return;
}

void CWeapon::Get_World_Mouse_Ray_Shot_Grouping(_float3* vRayDir, _float3* vRayPos)
{
	POINT ptMouse = {};
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	//마우스를 투영스페이스로 변환
	_float3 vMousePos = {};
	_float3 vWeaponPos = { (_float)g_iWinSizeX * 0.5f - 50.f,(_float)g_iWinSizeY * 0.5f + 50.f, 0.f };

	ptMouse.x += rand() % m_iQAimSize - m_iQAimSize * 0.5f;
	ptMouse.y += rand() % m_iQAimSize - m_iQAimSize * 0.5f;
	vMousePos.x = ptMouse.x / (g_iWinSizeX * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / -(g_iWinSizeY * 0.5f) + 1.f;
	vMousePos.z = 1.f;

	vWeaponPos.x = vWeaponPos.x / (g_iWinSizeX * 0.5f) - 1.f;
	vWeaponPos.y = vWeaponPos.y / -(g_iWinSizeY * 0.5f) + 1.f;
	vWeaponPos.z = 0.f;

	//뷰행렬과 투영행렬의 역행렬을 구함.
	_float4x4 ViewMatrixInv, ProjectionMatrixInv;

	if (FAILED(m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrixInv)))
	{
		MSG_BOX(TEXT("Failed to GetTransform : CPicking"));
		return;
	}
	ViewMatrixInv = *D3DXMatrixInverse(&ViewMatrixInv, nullptr, &ViewMatrixInv);
	if (FAILED(m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjectionMatrixInv)))
	{
		MSG_BOX(TEXT("Failed to GetTransform : CPicking"));
		return;
	}
	ProjectionMatrixInv = *D3DXMatrixInverse(&ProjectionMatrixInv, nullptr, &ProjectionMatrixInv);

	//마우스를 뷰 스페이스로 변환, 레이포즈와 레이디렉션을 구함.

	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &ProjectionMatrixInv);
	D3DXVec3TransformCoord(&vWeaponPos, &vWeaponPos, &ProjectionMatrixInv);
	*vRayPos = vWeaponPos;
	*vRayDir = vMousePos - *vRayPos;

	
	//레이포즈와 레이디렉션을 월드까지 변환한다
	//*vRayPos += *vRayDir * 0.3f;
	D3DXVec3TransformCoord(vRayPos, vRayPos, &ViewMatrixInv);
	D3DXVec3TransformNormal(vRayDir, vRayDir, &ViewMatrixInv);

	*vRayPos += *vRayDir * 0.0005f;
}

void CWeapon::Free()
{
	for (auto iter = m_vecBullet.begin(); iter != m_vecBullet.end(); ++iter)
	{
		Safe_Release(*iter);
	}
	m_vecBullet.clear();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTextureCom1);
	Safe_Release(m_pVIBuffer_Com);
	Safe_Release(m_pCamera);
	Safe_Release(m_pCameraTransform);
	__super::Free();
}
