#include "pch.h"
#include "LandObject.h"
#include "GameInstance.h"


CLandObject::CLandObject(LPDIRECT3DDEVICE9 pGraphic_Device) :
    CBlendObject(pGraphic_Device)
{
}

CLandObject::CLandObject(const CLandObject& rhs) :
	CBlendObject(rhs)
{
}

HRESULT CLandObject::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CLandObject"));
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CLandObject::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CLandObject"));
        return E_FAIL;
    }

    if (nullptr != pArg)
    {
        LANDOBJECT_DESC* pLandObjectDesc = (LANDOBJECT_DESC*)pArg;
        m_pTerrainTranformCom = pLandObjectDesc->pTerrainTranformCom;
        m_pTerrainVIBufferCom =  pLandObjectDesc->pTerrainVIBufferCom;

		// 이렇게 넣으면 나중에 몬스터를 툴로 배치하거나 할 때 유용하다.
		//m_pTransform->Set_State(CTransform::STATE_POSITION, pLandObjectDesc->vPos);

        Safe_AddRef(m_pTerrainTranformCom);
        Safe_AddRef(m_pTerrainVIBufferCom);
    }

	//데미지 유아이 추가하는 곳.
	if (FAILED(m_pGameInstance->Add_Clone(g_eLevel, TEXT("Layer_UI_Damage"), TEXT("Prototype_GameObject_UI_Damage"))))
	{
		MSG_BOX(TEXT("Failed to Add_Components ,Prototype_GameObject_UI_Damage"));
		return E_FAIL;
	}
	list<CGameObject*>* pList = m_pGameInstance->Get_List(g_eLevel, TEXT("Layer_UI_Damage"));
	if (nullptr != pList)
	{
		auto iter = pList->end();
		--iter;

		m_pUI_Damage = dynamic_cast<CUI_Damage*>((*iter));
		Safe_AddRef(m_pUI_Damage);
	}

    return S_OK;
}

_uint CLandObject::Tick(_float fTimeDelta)
{
    return 0;
}

void CLandObject::Late_Tick(_float fTimeDelta)
{
}

HRESULT CLandObject::Render()
{
    return S_OK;
}

HRESULT CLandObject::SetUp_OnTerrain(_float fRevision)
{
    _float3 vPosition = m_pTransform->Get_State(CTransform::STATE_POSITION);
    //터레인의 월드 행렬의 역행렬 가져오기. 가져와서 위치에 적용.
    vPosition = *D3DXVec3TransformCoord(&vPosition, &vPosition, m_pTerrainTranformCom->Get_WorldMatrix_Inverse());
    //변환한 위치로 높이 계산
    vPosition.y = m_pTerrainVIBufferCom->Compute_Height(vPosition) + fRevision;
    //다시 위치를 원래 월드위치로 변환해주기.
    vPosition = *D3DXVec3TransformCoord(&vPosition, &vPosition, m_pTerrainTranformCom->Get_WorldMatrix());

    m_pTransform->Set_State(CTransform::STATE_POSITION, vPosition);

    return S_OK;
}

_float3 CLandObject::Return_ViewPort_Pos()
{
    _float3 vPosition = m_pTransform->Get_State(CTransform::STATE_POSITION);
    _float4x4 ViewMatrix, ProjectionMatrix;

    m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
    m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjectionMatrix);

    vPosition = *D3DXVec3TransformCoord(&vPosition, &vPosition, &ViewMatrix);
    vPosition = *D3DXVec3TransformCoord(&vPosition, &vPosition, &ProjectionMatrix);

    vPosition.x = vPosition.x * g_iWinSizeX * 0.5f + g_iWinSizeX * 0.5f;
    vPosition.y = vPosition.y * g_iWinSizeY * 0.5f + g_iWinSizeY * 0.5f;

    return vPosition;
}

void CLandObject::Chase_Player(_float fTimeDelta, _float Min_Distance)
{
    const CTransform* pTransform = dynamic_cast<const CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), g_strTransformTag));

    _float3 vPosition = pTransform->Get_State(CTransform::STATE_POSITION);

    m_pTransform->LookAt_LandObject(vPosition);

    m_pTransform->Move_To_Target(vPosition, fTimeDelta, Min_Distance);
}

_float  CLandObject::Set_Texture_Monster_Dir(_float3 vGameLook)
{
	const CTransform* pCameraTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera_Player"), TEXT("Com_Transform")));


	_float3 vNowLook = { vGameLook }; // 게임상에서 현재 객체가 바로보고 있는 룩벡터를 가져옴
	_float3 vLook = { pCameraTransform->Get_State(CTransform::STATE_LOOK) }; // 카메라의 룩벡터
	_float3 vRight = { pCameraTransform->Get_State(CTransform::STATE_RIGHT) }; // 카메라의 Right 벡터

	_float3 vCameraPos{ pCameraTransform->Get_State(CTransform::STATE_POSITION) };
	_float3 vNowPos{ m_pTransform->Get_State(CTransform::STATE_POSITION) }; // 현재 객체의 포지션값을 구함

	D3DXVec3Normalize(&vLook, &vLook); // 카메라의 Look 벡터 정규화
	D3DXVec3Normalize(&vRight, &vRight); // 카메라의 Right 벡터 정규화
	D3DXVec3Normalize(&vNowLook, &vNowLook); // 게임상에서 현재 객체가 바로보고 있는 룩벡터의 룩벡터 정규화

	_float4x4 ViewMatrix = { *pCameraTransform->Get_WorldMatrix_Inverse() }; // 뷰행렬을 가져옴

	//m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	vNowPos = *D3DXVec3TransformCoord(&vNowPos, &vNowPos, &ViewMatrix); // 현재객체의 뷰스페이스 상의 위치를 구함

	_float fAngle = { D3DXVec3Dot(&vRight, &vNowLook) };
	if (fAngle >= 1.f)
		fAngle = 1.f;
	else if (fAngle <= -1.f)
		fAngle = -1.f;
	fAngle = acosf(fAngle);
	
	fAngle = D3DXToDegree(fAngle); // 내적해서 각도 계산

	// 카메라의 룩벡터랑 몬스터의 게임에서의 룩벡터 비교
	if (0.f <= D3DXVec3Dot(&vLook, &vNowLook)) // 양수면 앞쪽(카메라 반대편)을 보고 있는거
	{
		if (90.f >= fAngle) // 90도 이하이면 카메라 기준 오른쪽 보고있는 것
		{
			if (fAngle <= 20.f)
			{
				return 2.f;
			}
			else if (fAngle >= 20.f && fAngle <= 80.f)
			{
				return 3.f;
			}
			else
			{
				return 4.f;
			}
		}
		else if (90.f < fAngle) // 왼쪽
		{
			if ((fAngle - 90.f) <= 20.f)
			{
				return 4.f;
			}
			else if ((fAngle - 90.f) >= 20.f && (fAngle - 90.f) <= 80.f)
			{
				return 5.f;
			}
			else
			{
				return 6.f;
			}
		}
	}
	else if (0.f > D3DXVec3Dot(&vLook, &vNowLook)) // 음수면 뒤쪽(카메라 쪽)을 보고 있는거
	{
		if ((90.f >= fAngle)) // 양수면 카메라 기준 오른쪽 보고있는 것
		{
			if (fAngle <= 20.f)
			{
				return 2.f;
			}
			else if (fAngle >= 20.f && fAngle <= 80.f)
			{
				return 1.f;
			}
			else
			{
				return 0.f;
			}
		}
		else if (90.f < fAngle)
		{
			if ((fAngle - 90.f) <= 20.f)
			{
				return 0.f;
			}
			else if ((fAngle - 90.f) >= 20.f && (fAngle - 90.f) <= 80.f)
			{
				return 7.f;
			}
			else
			{
				return 6.f;
			}
		}
	}
	return 0.f;
}

_float CLandObject::Set_Texture_Player_Dir(_float3 vGameLook) // 객체가 실제 게임에서 보고있는 룩 벡터를 가지고 와야 함
{
	const CTransform* pCameraTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera_Player"), TEXT("Com_Transform")));
	if (nullptr == pCameraTransform)
		return 0.f;

	_float3 vNowLook = { vGameLook }; // 게임상에서 현재 객체가 바로보고 있는 룩벡터를 가져옴
	_float3 vLook = { pCameraTransform->Get_State(CTransform::STATE_LOOK) }; // 카메라의 룩벡터
	_float3 vRight = { pCameraTransform->Get_State(CTransform::STATE_RIGHT) }; // 카메라의 Right 벡터

	_float3 vCameraPos{ pCameraTransform->Get_State(CTransform::STATE_POSITION) };
	_float3 vNowPos{ m_pTransform->Get_State(CTransform::STATE_POSITION) }; // 현재 객체의 포지션값을 구함

	D3DXVec3Normalize(&vLook, &vLook); // 카메라의 Look 벡터 정규화
	D3DXVec3Normalize(&vRight, &vRight); // 카메라의 Right 벡터 정규화
	D3DXVec3Normalize(&vNowLook, &vNowLook); // 게임상에서 현재 객체가 바로보고 있는 룩벡터의 룩벡터 정규화

	_float4x4 ViewMatrix = { *pCameraTransform->Get_WorldMatrix_Inverse() }; // 뷰행렬을 가져옴

	//m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	vNowPos = *D3DXVec3TransformCoord(&vNowPos, &vNowPos, &ViewMatrix); // 현재객체의 뷰스페이스 상의 위치를 구함
	_float fAngle = { D3DXVec3Dot(&vRight, &vNowLook) }; //right와 nowLook이 완전히 일치할때에 대한 예외처리 필요
	if (fAngle >= 1.f)
		fAngle = 1.f;
	else if (fAngle <= -1.f)
		fAngle = -1.f;
	fAngle = acosf(fAngle);
	fAngle = D3DXToDegree(fAngle); // 내적해서 각도 계산

	// 카메라의 룩벡터랑 몬스터의 게임에서의 룩벡터 비교
	if (0.f <= D3DXVec3Dot(&vLook, &vNowLook)) // 양수면 앞쪽(카메라 반대편)을 보고 있는거
	{
		if (90.f >= fAngle) // 90도 이하이면 카메라 기준 오른쪽 보고있는 것
		{
			if (fAngle <= 20.f)
			{
				return 2.f;
			}
			else if (fAngle >= 20.f && fAngle <= 80.f)
			{
				return 1.f;
			}
			else
			{
				return 0.f;
			}
		}
		else if (90.f < fAngle) // 왼쪽
		{
			if ((fAngle - 90.f) <= 20.f)
			{
				return 0.f;
			}
			else if ((fAngle - 90.f) >= 20.f && (fAngle - 90.f) <= 80.f)
			{
				return 7.f;
			}
			else
			{
				return 6.f;
			}
		}
	}
	else if (0.f > D3DXVec3Dot(&vLook, &vNowLook)) // 음수면 뒤쪽(카메라 쪽)을 보고 있는거
	{
		if ((90.f >= fAngle)) // 양수면 카메라 기준 오른쪽 보고있는 것
		{
			if (fAngle <= 20.f)
			{
				return 2.f;
			}
			else if (fAngle >= 20.f && fAngle <= 80.f)
			{
				return 3.f;
			}
			else
			{
				return 4.f;
			}
		}
		else if (90.f < fAngle)
		{
			if ((fAngle - 90.f) <= 20.f)
			{
				return 4.f;
			}
			else if ((fAngle - 90.f) >= 20.f && (fAngle - 90.f) <= 80.f)
			{
				return 5.f;
			}
			else
			{
				return 6.f;
			}
		}
	}
	return 0.f;//TODO: 용수, 필요하다는데?
}

void CLandObject::Set_Terrain(LANDOBJECT_DESC* pLandDesc)
{
	Safe_Release(m_pTerrainTranformCom);
	m_pTerrainTranformCom = pLandDesc->pTerrainTranformCom;
	Safe_AddRef(m_pTerrainTranformCom);
	Safe_Release(m_pTerrainVIBufferCom);
	m_pTerrainVIBufferCom = pLandDesc->pTerrainVIBufferCom;
	Safe_AddRef(m_pTerrainVIBufferCom);

}


void CLandObject::Free()
{
    Safe_Release(m_pTerrainTranformCom);
    Safe_Release(m_pTerrainVIBufferCom);
    Safe_Release(m_pUI_Damage);

    __super::Free();
}
