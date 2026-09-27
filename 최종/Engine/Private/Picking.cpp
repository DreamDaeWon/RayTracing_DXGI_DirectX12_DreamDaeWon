#include "Picking.h"
#include "Transform.h"
CPicking::CPicking(LPDIRECT3DDEVICE9 pGraphic_Device) :
	m_pGraphic_Device(pGraphic_Device)
{
	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CPicking::Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
	m_hWnd = hWnd;

	m_iWinSizeX = iWinSizeX;
	m_iWinSizeY = iWinSizeY;

	return S_OK;
}

void CPicking::Update() //매틱 마우스의 월드 좌표를 구함.
{
	//마우스의 뷰포트위치를 가져옴.
	POINT ptMouse = {};
	GetCursorPos(&ptMouse);
	ScreenToClient(m_hWnd, &ptMouse);

	//마우스를 투영스페이스로 변환
	_float3 vMousePos = {};

	vMousePos.x = ptMouse.x / (m_iWinSizeX * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / -(m_iWinSizeY * 0.5f) + 1.f;
	vMousePos.z = 0.f;

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
	m_vRayPos = { 0.f,0.f,0.f };
	m_vRayDir = vMousePos;

	//레이포즈와 레이디렉션을 월드까지 변환한다

	D3DXVec3TransformCoord(&m_vRayPos, &m_vRayPos, &ViewMatrixInv);
	D3DXVec3TransformNormal(&m_vRayDir, &m_vRayDir, &ViewMatrixInv);
}

void CPicking::Transform_PickingToLocalSpace(class CTransform* pTransform, _Out_ _float3* pRayDir, _Out_ _float3* pRayPos)
{
	//레이포즈와 레이디렉션을 매개변수로 들어온 객체의 트랜스폼을 이용해서 객체의 로컬로 변환해서 아웃에 넣어줌.
	D3DXVec3TransformNormal(pRayDir, &m_vRayDir, pTransform->Get_WorldMatrix_Inverse());
	D3DXVec3TransformCoord(pRayPos, &m_vRayPos, pTransform->Get_WorldMatrix_Inverse());
}

void CPicking::Transform_PickingToLocalSpace(_float4x4 WorldMaxrixInv, _float3* pRayDir, _float3* pRayPos)
{
	_float4x4 ObjWorldMaxrixInv = WorldMaxrixInv;
	//레이포즈와 레이디렉션을 매개변수로 들어온 객체의 트랜스폼을 이용해서 객체의 로컬로 변환해서 아웃에 넣어줌.
	D3DXVec3TransformNormal(pRayDir, &m_vRayDir, &ObjWorldMaxrixInv);
	D3DXVec3TransformCoord(pRayPos, &m_vRayPos, &ObjWorldMaxrixInv);
}

CPicking* CPicking::Create(LPDIRECT3DDEVICE9 pGraphic_Device, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
	CPicking* pInstance = new CPicking(pGraphic_Device);
	if (FAILED(pInstance->Initialize(hWnd, iWinSizeX, iWinSizeY)))
	{
		MSG_BOX(TEXT("Failed to Created : CPicking"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPicking::Free()
{
	Safe_Release(m_pGraphic_Device);

	__super::Free();
}
