#include "pch.h"
#include "Camera_Player.h"

#include "Player.h"

CCamera_Player::CCamera_Player(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CCamera(pGraphic_Device)
{
}

CCamera_Player::CCamera_Player(const CCamera_Player& rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Player::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) 
	{
		MSG_BOX(TEXT("Failed to Initialize : CCamera_Player"));
		return E_FAIL;
	}

	if (nullptr != pArg)
	{
		CAMERA_PLYAER_DESC* pCameraPlayerDesc = (CAMERA_PLYAER_DESC*)pArg;
		m_pPlayer = pCameraPlayerDesc->pPlayer;
		m_fQuarterViewDistance = pCameraPlayerDesc->fQuarterViewDistance;
		m_fTPSViewDistance = pCameraPlayerDesc->fTPSViewDistance;
		Safe_AddRef(m_pPlayer);
	}
	m_eState = STATE_TPS;
	m_fQuarterViewDistance = 10.f;

	return S_OK;
}

_uint CCamera_Player::Tick(_float fTimeDelta)
{
	//플레이어 룩벡터 가져와서 위치보정?
	Key_Input();
	
	if (FAILED(__super::Bind_PipeLines()))
	{
		MSG_BOX(TEXT("Failed to Bind_PipeLines : CCamera_Player"));
		return 1;
	}
	return 0;
}

void CCamera_Player::Late_Tick(_float fTimeDelta)
{
	if (STATE_QUARTER != m_eState)
		Mouse_Axis_Turn(fTimeDelta);
	Chase_Player();
}

void CCamera_Player::Key_Input()
{

	if (GetAsyncKeyState('Y') & 0x8000)
	{
		switch (m_eState)
		{
		case Client::CCamera_Player::STATE_QUARTER:
			m_eState = STATE_TPS;
			break;
		case Client::CCamera_Player::STATE_TPS:
			m_eState = STATE_QUARTER;
			break;
		case Client::CCamera_Player::STATE_ZOOM:
			break;
		case Client::CCamera_Player::STATE_END:
			break;
		default:
			break;
		}
	}
	if (GetAsyncKeyState('C') & 0x8000)
	{
		if (m_eState == STATE_TPS)
		{
			m_eState = STATE_ZOOM;
		}
		else if (m_eState == STATE_ZOOM)
		{
			m_eState = STATE_TPS;
			m_fFovy = D3DXToRadian(90.f);
		}
	}
}

void CCamera_Player::Chase_Player()
{
	if (nullptr == m_pPlayer)
		return;

	switch (m_eState)
	{
	case Client::CCamera_Player::STATE_QUARTER:
	{	
		_float3 vScale = m_pTransform->Get_Scale();

		_float3 vRight = { 1.f,0.f,0.f };
		_float3 vUp = { 0.f,1.f,1.f };
		vUp = *D3DXVec3Normalize(&vUp, &vUp) * vScale.y;
		_float3 vLook = { 0.f,-1.f,1.f };
		vLook = *D3DXVec3Normalize(&vLook, &vLook) * vScale.z;
		_float3 vPosition = {};

		_float3 vPlayerLook = {};
		_float3 vPlayerPosition = {};

		m_pPlayer->Return_Look_Position(&vPlayerLook, &vPlayerPosition);

		vPosition = vPlayerPosition + _float3(0.f, 1.f, -1.f) * m_fQuarterViewDistance;

		m_pTransform->Set_State(CTransform::STATE_RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE_UP, vUp);
		m_pTransform->Set_State(CTransform::STATE_LOOK, vLook);
		m_pTransform->Set_State(CTransform::STATE_POSITION, vPosition);
		break;
	}
	case Client::CCamera_Player::STATE_TPS:
	{
		_float3 vScale = m_pTransform->Get_Scale();

		_float3 vPlayerLook = {};
		_float3 vPlayerPosition = {};

		_float3 vRight = {};
		_float3 vUp = {};
		_float3 vLook = {};
		_float3 vPosition = {};

		_float3 v010 = { 0.f,1.f,0.f };

		m_pPlayer->Return_Look_Position(&vPlayerLook, &vPlayerPosition);


		vLook = *D3DXVec3Normalize(&vPlayerLook, &vPlayerLook) * vScale.z;
		vRight = *D3DXVec3Cross(&vRight, &v010, &vPlayerLook);
		vRight = *D3DXVec3Normalize(&vRight, &vRight) * vScale.x;
		vUp = *D3DXVec3Cross(&vUp, &vLook, &vRight);
		vUp = *D3DXVec3Normalize(&vUp, &vUp) * vScale.y;

		vPosition = vPlayerPosition - *D3DXVec3Normalize(&vPlayerLook, &vPlayerLook) * m_fTPSViewDistance;

		m_pTransform->Set_State(CTransform::STATE_RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE_UP, vUp);
		m_pTransform->Set_State(CTransform::STATE_LOOK, vLook);
		m_pTransform->Set_State(CTransform::STATE_POSITION, vPosition);

		break;
	}
	case Client::CCamera_Player::STATE_ZOOM:
	{
		_float3 vScale = m_pTransform->Get_Scale();

		_float3 vPlayerLook = {};
		_float3 vPlayerPosition = {};

		m_pPlayer->Return_Look_Position(&vPlayerLook, &vPlayerPosition);

		_float3 vPosition = vPlayerPosition + vPlayerLook * 1.5f;

		m_pTransform->Set_State(CTransform::STATE_POSITION, vPosition);

		m_fFovy = D3DXToRadian(15.f);

		break;
	}
	case Client::CCamera_Player::STATE_END:
		break;
	default:
		break;
	}
	

}

void CCamera_Player::Mouse_Axis_Turn(_float fTimeDelta)
{


	POINT ptMouse = {};
	GetCursorPos(&ptMouse);

	_long lDeltaX = ptMouse.x - m_ptMouse.x;
	_long lDeltaY = ptMouse.y - m_ptMouse.y;

	if (0.f != lDeltaX)
	{
		m_pTransform->Turn(_float3(0.f, 1.f, 0.f), fTimeDelta * lDeltaX);
	}

	if (0.f != lDeltaY)
	{
		m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_RIGHT), fTimeDelta * lDeltaY);
	}
	_float3 vPlayerPos = {};
	_float3 vPlayerLook = {};
	m_pPlayer->Return_Look_Position(&vPlayerLook, &vPlayerPos);
	m_pPlayer->Look_At(vPlayerPos + m_pTransform->Get_State(CTransform::STATE_LOOK));

	m_ptMouse = ptMouse;
}


CCamera_Player* CCamera_Player::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCamera_Player* pInstance = new CCamera_Player(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Faild to Created : CCamera_Player"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Player::Clone(void* pArg)
{
	CCamera_Player* pInstance = new CCamera_Player(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Faild to Cloned : CCamera_Player"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Player::Free()
{
	Safe_Release(m_pPlayer);

	__super::Free();
}
