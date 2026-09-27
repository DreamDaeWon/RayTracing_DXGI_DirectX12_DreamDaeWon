#include "Camera.h"

CCamera::CCamera(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CCamera::CCamera(const CCamera& rhs)
	: CGameObject(rhs),
	m_fAspect(rhs.m_fAspect),
	m_fFar(rhs.m_fFar),
	m_fNear(rhs.m_fNear),
	m_fFovy(rhs.m_fFovy)
{
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Initialize : CCamera::Initialize"));
		return E_FAIL;
	}

	if (nullptr != pArg)
	{
		CAMERA_DESC* pCameraDesc = (CAMERA_DESC*)pArg;

		m_fFovy = pCameraDesc->fFovy;
		m_fAspect = pCameraDesc->fAspect;
		m_fNear = pCameraDesc->fNear;
		m_fFar = pCameraDesc->fFar;

		
		m_pTransform->Set_State(CTransform::STATE_POSITION, pCameraDesc->vEye);
		m_pTransform->LookAt(pCameraDesc->vAt);
	}

	return S_OK;
}

_uint CCamera::Tick(_float fTimedelta)
{
	return 0;
}

void CCamera::Late_Tick(_float fTimedelta)
{
}

HRESULT CCamera::Bind_PipeLines()
{
	/* SetTransform함수는 한번 바인딩 되면 재 바인딩될때 까지 계속 값을 유지한다. */

	_float4x4 ViewMatrix, ProjMatrix;
	
	ViewMatrix = *(m_pTransform->Get_WorldMatrix_Inverse());
	ProjMatrix = *D3DXMatrixPerspectiveFovLH(&ProjMatrix, m_fFovy, m_fAspect, m_fNear, m_fFar);

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &ProjMatrix);
		
	return S_OK;
}

void CCamera::Free()
{
	__super::Free();
}
