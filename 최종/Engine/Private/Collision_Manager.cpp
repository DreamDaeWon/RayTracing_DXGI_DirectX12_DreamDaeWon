#include "Collision_Manager.h"
#include "Transform.h"
#include "GameObject.h"
#include "GameInstance.h"

CCollision_Manager::CCollision_Manager(LPDIRECT3DDEVICE9 pGraphic_Device)
{
}

HRESULT CCollision_Manager::Initialize()
{
	return S_OK;
}

_bool CCollision_Manager::Collision_Rect_Ray(CCollider_Rect* pColliderRect, CTransform* pTransform, _float3 vRayDir, _float3 vRayPos)
{
	CCollider_Rect::COL_RECT tRect = *pColliderRect->Get_Points();


	//정점을 들어온 트랜스폼의 로컬로 내린다.
	_float4x4 WorldMatrix = *pTransform->Get_WorldMatrix_Inverse();
	D3DXVec3TransformCoord(&tRect.LeftBottom, &tRect.LeftBottom, &WorldMatrix);
	D3DXVec3TransformCoord(&tRect.LeftTop, &tRect.LeftTop, &WorldMatrix);
	D3DXVec3TransformCoord(&tRect.RightBottom, &tRect.RightBottom, &WorldMatrix);
	D3DXVec3TransformCoord(&tRect.RightTop, &tRect.RightTop, &WorldMatrix);

	_float fU(0.f), fV(0.f), fDistance(0.f);
	//레이가 오른쪽 위 삼각형과 충돌 또는 왼쪽 아래 삼각형과 충돌 이면 트루리턴
	_bool Result = D3DXIntersectTri(&tRect.LeftTop, &tRect.RightTop, &tRect.RightBottom, &vRayPos, &vRayDir, &fU, &fV, &fDistance);
	_bool Result2 = D3DXIntersectTri(&tRect.LeftTop, &tRect.RightBottom, &tRect.LeftBottom, &vRayPos, &vRayDir, &fU, &fV, &fDistance);

	return Result || Result2;
}

_bool CCollision_Manager::Collision_Rect_Ray(CCollider_Rect* pColliderRect, class CTransform* pTransform, _float3 vRayDir, _float3 vRayPos, _float fLength)
{
	CCollider_Rect::COL_RECT tRect = *pColliderRect->Get_Points();

	_float4x4 WorldMatrix = *pTransform->Get_WorldMatrix_Inverse();
	D3DXVec3TransformCoord(&tRect.LeftBottom, &tRect.LeftBottom, &WorldMatrix);
	D3DXVec3TransformCoord(&tRect.LeftTop, &tRect.LeftTop, &WorldMatrix);
	D3DXVec3TransformCoord(&tRect.RightBottom, &tRect.RightBottom, &WorldMatrix);
	D3DXVec3TransformCoord(&tRect.RightTop, &tRect.RightTop, &WorldMatrix);

	_float fU(0.f), fV(0.f), fDistance(0.f);
	//레이가 오른쪽 위 삼각형과 충돌 또는 왼쪽 아래 삼각형과 충돌 이면
	if (TRUE == D3DXIntersectTri(&tRect.LeftTop, &tRect.RightTop, &tRect.RightBottom, &vRayPos, &vRayDir, &fU, &fV, &fDistance) ||
		TRUE == D3DXIntersectTri(&tRect.LeftTop, &tRect.RightBottom, &tRect.LeftBottom, &vRayPos, &vRayDir, &fU, &fV, &fDistance))
	{
		//레이의 길이와 렉트의 중점에서 레이 포즈까지의 거리를 비교
		if (fDistance <= fLength) //디스턴스가 길이보다 짧다? 충돌
			return TRUE;
	}

	return FALSE;
}

_bool CCollision_Manager::Collision_Rect_Ray_Same_Space(CCollider_Rect* pColliderRect, _float3 vRayDir, _float3 vRayPos)
{
	CCollider_Rect::COL_RECT tRect = *pColliderRect->Get_Points();

	_float fU(0.f), fV(0.f), fDistance(0.f);
	//레이가 오른쪽 위 삼각형과 충돌 또는 왼쪽 아래 삼각형과 충돌 이면 트루리턴
	_bool Result = D3DXIntersectTri(&tRect.LeftTop, &tRect.RightTop, &tRect.RightBottom, &vRayPos, &vRayDir, &fU, &fV, &fDistance);
	_bool Result2 = D3DXIntersectTri(&tRect.LeftTop, &tRect.RightBottom, &tRect.LeftBottom, &vRayPos, &vRayDir, &fU, &fV, &fDistance);

	return Result || Result2;
}

_bool CCollision_Manager::Collision_Rect_Ray_Same_Space(CCollider_Rect* pColliderRect, _float3 vRayDir, _float3 vRayPos, _float fLength)
{
	if (nullptr == pColliderRect)
		return false;

	CCollider_Rect::COL_RECT tRect = *pColliderRect->Get_Points();

	_float fU(0.f), fV(0.f), fDistance(0.f);
	//레이가 오른쪽 위 삼각형과 충돌 또는 왼쪽 아래 삼각형과 충돌 이면
	if (TRUE == D3DXIntersectTri(&tRect.LeftTop, &tRect.RightTop, &tRect.RightBottom, &vRayPos, &vRayDir, &fU, &fV, &fDistance) ||
		TRUE == D3DXIntersectTri(&tRect.LeftTop, &tRect.RightBottom, &tRect.LeftBottom, &vRayPos, &vRayDir, &fU, &fV, &fDistance))
	{
		//레이의 길이와 렉트의 중점에서 레이 포즈까지의 거리를 비교
		if (fDistance <= fLength) //디스턴스가 길이보다 짧다? 충돌
			return TRUE;
	}

	return FALSE;
}

_bool CCollision_Manager::Collision_Rect_Ray_Same_Space(CCollider_Rect* pColliderRect, _float3 vRayDir, _float3 vRayPos, _float fLength, _float* pDistance)
{
	CCollider_Rect::COL_RECT tRect = *pColliderRect->Get_Points();

	_float fU(0.f), fV(0.f);
	//레이가 오른쪽 위 삼각형과 충돌 또는 왼쪽 아래 삼각형과 충돌 이면
	if (TRUE == D3DXIntersectTri(&tRect.LeftTop, &tRect.RightTop, &tRect.RightBottom, &vRayPos, &vRayDir, &fU, &fV, pDistance) ||
		TRUE == D3DXIntersectTri(&tRect.LeftTop, &tRect.RightBottom, &tRect.LeftBottom, &vRayPos, &vRayDir, &fU, &fV, pDistance))
	{
		//레이의 길이와 렉트의 중점에서 레이 포즈까지의 거리를 비교
		if (*pDistance <= fLength) //디스턴스가 길이보다 짧다? 충돌
			return TRUE;
	}
	return FALSE;
}

_bool CCollision_Manager::Collision_AABB(CCollider_Cube_AABB* pSrcColliderCubeAABB, CCollider_Cube_AABB* pDstColliderCubeAABB)
{
	if (nullptr == pSrcColliderCubeAABB || nullptr == pDstColliderCubeAABB)
		return false;


	_float3 vSrcMin = pSrcColliderCubeAABB->Get_Min_Point();
	_float3 vSrcMax = pSrcColliderCubeAABB->Get_Max_Point();
	_float3 vDstMin = pDstColliderCubeAABB->Get_Min_Point();
	_float3 vDstMax = pDstColliderCubeAABB->Get_Max_Point();

	if (min(vSrcMax.x, vDstMax.x) < max(vSrcMin.x, vDstMin.x))
		return false;
	if (min(vSrcMax.y, vDstMax.y) < max(vSrcMin.y, vDstMin.y))
		return false;
	if (min(vSrcMax.z, vDstMax.z) < max(vSrcMin.z, vDstMin.z))
		return false;

	return true;
}

void CCollision_Manager::Collision_AABB_List(list<class CGameObject*>* pSrcObjectList, list<class CGameObject*>* pDstObjectList)
{
	for (auto Dst : *pDstObjectList)
	{
		if (true == Dst->Get_Dead())
			continue;
		CCollider_Cube_AABB* pDstCollider = dynamic_cast<CCollider_Cube_AABB*>(Dst->Get_Component(TEXT("Com_Collider_Cube_AABB")));
		for (auto Src : *pSrcObjectList)
		{
			if (true == Src->Get_Dead())
				continue;
			CCollider_Cube_AABB* pSrcCollider = dynamic_cast<CCollider_Cube_AABB*>(Src->Get_Component(TEXT("Com_Collider_Cube_AABB")));
			if (Collision_AABB(pDstCollider, pSrcCollider))
			{
				//이거 콜리전의 xyz델타 받아서 해보자.
				CTransform* pDstTranformCom = dynamic_cast<CTransform*>(Dst->Get_Component(g_strTransformTag));
				CTransform* pSrcTranformCom = dynamic_cast<CTransform*>(Src->Get_Component(g_strTransformTag));
				_float3 vDstColliderPos = pDstCollider->Get_State(CCollider_Cube_AABB::STATE_POSITION);
				_float3 vSrcColliderPos = pSrcCollider->Get_State(CCollider_Cube_AABB::STATE_POSITION);
				_float fXDelta = abs(vDstColliderPos.x - vSrcColliderPos.x);
				_float fYDelta = abs(vDstColliderPos.y - vSrcColliderPos.y);
				_float fZDelta = abs(vDstColliderPos.z - vSrcColliderPos.z);
				if (fYDelta < fXDelta && fZDelta < fXDelta)
				{
					if (vDstColliderPos.x > vSrcColliderPos.x)
					{
						_float fXCenter = (vDstColliderPos.x + vSrcColliderPos.x) * 0.5f;
						vDstColliderPos.x = fXCenter + (1.f - fXDelta * 0.5f);
						vSrcColliderPos.x = fXCenter - (1.f - fXDelta * 0.5f);
					}
					else
					{
						_float fXCenter = (vDstColliderPos.x + vSrcColliderPos.x) * 0.5f;
						vDstColliderPos.x = fXCenter - (1.f - fXDelta * 0.5f);
						vSrcColliderPos.x = fXCenter + (1.f - fXDelta * 0.5f);
					}
				}
				else if (fXDelta < fYDelta && fZDelta < fYDelta)
				{
					if (vDstColliderPos.y > vSrcColliderPos.y)
					{
						_float fYCenter = (vDstColliderPos.y + vSrcColliderPos.y) * 0.5f;
						vDstColliderPos.y = fYCenter + (1.f - fYDelta * 0.5f);
						vSrcColliderPos.y = fYCenter - (1.f - fYDelta * 0.5f);
					}
					else
					{
						_float fYCenter = (vDstColliderPos.y + vSrcColliderPos.y) * 0.5f;
						vDstColliderPos.y = fYCenter - (1.f - fYDelta * 0.5f);
						vSrcColliderPos.y = fYCenter + (1.f - fYDelta * 0.5f);
					}
				}
				else if (fYDelta < fZDelta && fXDelta < fZDelta)
				{
					if (vDstColliderPos.z > vSrcColliderPos.z)
					{
						_float fZCenter = (vDstColliderPos.z + vSrcColliderPos.z) * 0.5f;
						vDstColliderPos.z = fZCenter + (1.f - fZDelta * 0.5f);
						vSrcColliderPos.z = fZCenter - (1.f - fZDelta * 0.5f);
					}
					else
					{
						_float fZCenter = (vDstColliderPos.z + vSrcColliderPos.z) * 0.5f;
						vDstColliderPos.z = fZCenter - (1.f - fZDelta * 0.5f);
						vSrcColliderPos.z = fZCenter + (1.f - fZDelta * 0.5f);
					}
				}
				pDstTranformCom->Set_State(CTransform::STATE_POSITION, vDstColliderPos);
				pSrcTranformCom->Set_State(CTransform::STATE_POSITION, vSrcColliderPos);
			}
		}
	}
	return;
}

void CCollision_Manager::Collision_AABB_List(list<class CGameObject*>* pSrcObjectList, list<class CGameObject*>* pDstObjectList, _float fSrcPower, _float fDstPower)
{
	_float fSrcPowerRatio = fSrcPower / (fSrcPower + fDstPower);
	_float fDstPowerRatio = 1.f - fSrcPowerRatio;

	for (auto Dst : *pDstObjectList)
	{
		if (true == Dst->Get_Dead())
			continue;
		CCollider_Cube_AABB* pDstCollider = dynamic_cast<CCollider_Cube_AABB*>(Dst->Get_Component(TEXT("Com_Collider_Cube_AABB")));
		for (auto Src : *pSrcObjectList)
		{
			if (true == Src->Get_Dead())
				continue;
			CCollider_Cube_AABB* pSrcCollider = dynamic_cast<CCollider_Cube_AABB*>(Src->Get_Component(TEXT("Com_Collider_Cube_AABB")));
			if (Collision_AABB(pDstCollider, pSrcCollider))
			{
				//이거 콜리전의 xyz델타 받아서 해보자.
				CTransform* pDstTranformCom = dynamic_cast<CTransform*>(Dst->Get_Component(g_strTransformTag));
				CTransform* pSrcTranformCom = dynamic_cast<CTransform*>(Src->Get_Component(g_strTransformTag));
				_float3 vDstColliderPos = pDstCollider->Get_State(CCollider_Cube_AABB::STATE_POSITION);
				_float3 vSrcColliderPos = pSrcCollider->Get_State(CCollider_Cube_AABB::STATE_POSITION);
				_float fXDelta = abs(vDstColliderPos.x - vSrcColliderPos.x);
				_float fYDelta = abs(vDstColliderPos.y - vSrcColliderPos.y);
				_float fZDelta = abs(vDstColliderPos.z - vSrcColliderPos.z);
				if (fYDelta < fXDelta && fZDelta < fXDelta)
				{
					if (vDstColliderPos.x > vSrcColliderPos.x)
					{
						_float fXCenter = (vDstColliderPos.x + vSrcColliderPos.x) * 0.5f;
						vDstColliderPos.x = fXCenter + (1.f - fXDelta * fDstPowerRatio);
						vSrcColliderPos.x = fXCenter - (1.f - fXDelta * fSrcPowerRatio);
					}
					else
					{
						_float fXCenter = (vDstColliderPos.x + vSrcColliderPos.x) * 0.5f;
						vDstColliderPos.x = fXCenter - (1.f - fXDelta * fDstPowerRatio);
						vSrcColliderPos.x = fXCenter + (1.f - fXDelta * fSrcPowerRatio);
					}
				}
				else if (fXDelta < fYDelta && fZDelta < fYDelta)
				{
					if (vDstColliderPos.y > vSrcColliderPos.y)
					{
						_float fYCenter = (vDstColliderPos.y + vSrcColliderPos.y) * 0.5f;
						vDstColliderPos.y = fYCenter + (1.f - fYDelta * fDstPowerRatio);
						vSrcColliderPos.y = fYCenter - (1.f - fYDelta * fSrcPowerRatio);
					}
					else
					{
						_float fYCenter = (vDstColliderPos.y + vSrcColliderPos.y) * 0.5f;
						vDstColliderPos.y = fYCenter - (1.f - fYDelta * fDstPowerRatio);
						vSrcColliderPos.y = fYCenter + (1.f - fYDelta * fSrcPowerRatio);
					}
				}
				else if (fYDelta < fZDelta && fXDelta < fZDelta)
				{
					if (vDstColliderPos.z > vSrcColliderPos.z)
					{
						_float fZCenter = (vDstColliderPos.z + vSrcColliderPos.z) * 0.5f;
						vDstColliderPos.z = fZCenter + (1.f - fZDelta * fDstPowerRatio);
						vSrcColliderPos.z = fZCenter - (1.f - fZDelta * fSrcPowerRatio);
					}
					else
					{
						_float fZCenter = (vDstColliderPos.z + vSrcColliderPos.z) * 0.5f;
						vDstColliderPos.z = fZCenter - (1.f - fZDelta * fDstPowerRatio);
						vSrcColliderPos.z = fZCenter + (1.f - fZDelta * fSrcPowerRatio);
					}
				}
				pDstTranformCom->Set_State(CTransform::STATE_POSITION, vDstColliderPos);
				pSrcTranformCom->Set_State(CTransform::STATE_POSITION, vSrcColliderPos);
			}
		}
	}
	return;
}

void CCollision_Manager::Collision_AABB_List_Fixed_Dst(list<class CGameObject*>* pSrcObjectList, list<class CGameObject*>* pDstObjectList, _float fTimeDelta)
{
	for (auto Src : *pSrcObjectList)
	{
		if (true == Src->Get_Dead())
			continue;
		CCollider_Cube_AABB* pSrcCollider = dynamic_cast<CCollider_Cube_AABB*>(Src->Get_Component(TEXT("Com_Collider_Cube_AABB")));
		if (nullptr == pSrcCollider)
			continue;
		CCollider_Cube_AABB* pClosestCollider = { nullptr };
		_float fMinDistance = 1000.f;
		for (auto Dst : *pDstObjectList)
		{
			if (true == Dst->Get_Dead())
				continue;
			CCollider_Cube_AABB* pDstCollider = dynamic_cast<CCollider_Cube_AABB*>(Dst->Get_Component(TEXT("Com_Collider_Cube_AABB")));
			if (nullptr == pDstCollider)
				continue;
			_float3 vSrcPos = pSrcCollider->Get_State(CCollider::STATE_POSITION);
			_float3 vDstPos = pDstCollider->Get_State(CCollider::STATE_POSITION);
			_float3 vDir = vSrcPos - vDstPos;
			_float fDistance = D3DXVec3Length(&vDir);
			if (fMinDistance > fDistance)
			{
				pClosestCollider = pDstCollider;
				fMinDistance = fDistance;
			}
			
		}
		if (nullptr != pClosestCollider && Collision_AABB(pClosestCollider, pSrcCollider))
		{
			//가장 가까운녀석과 충돌하게하자.,
			//CTransform* pDstTranformCom = dynamic_cast<CTransform*>(Dst->Get_Component(g_strTransformTag));
			CTransform* pSrcTranformCom = dynamic_cast<CTransform*>(Src->Get_Component(g_strTransformTag));
			_float3 vDistance = (pClosestCollider->Get_Scale() + pSrcCollider->Get_Scale()) * 0.5f;
			_float3 vDstColliderPos = pClosestCollider->Get_State(CCollider_Cube_AABB::STATE_POSITION);
			_float3 vSrcColliderPos = pSrcCollider->Get_State(CCollider_Cube_AABB::STATE_POSITION);
			_float fXDelta = abs(vDstColliderPos.x - vSrcColliderPos.x);
			_float fYDelta = abs(vDstColliderPos.y - vSrcColliderPos.y);
			_float fZDelta = abs(vDstColliderPos.z - vSrcColliderPos.z);
			if (fYDelta < fXDelta && fZDelta < fXDelta)
			{
				if (vDstColliderPos.x > vSrcColliderPos.x)
					vSrcColliderPos.x = vDstColliderPos.x - vDistance.x;
				else
					vSrcColliderPos.x = vDstColliderPos.x + vDistance.x;
			}
			else if (fXDelta < fYDelta && fZDelta < fYDelta)
			{
				if (vDstColliderPos.y > vSrcColliderPos.y)
					vSrcColliderPos.y = vDstColliderPos.y - vDistance.y;
				else
					vSrcColliderPos.y = vDstColliderPos.y + vDistance.y;
			}
			else if (fYDelta < fZDelta && fXDelta < fZDelta)
			{
				if (vDstColliderPos.z > vSrcColliderPos.z)
					vSrcColliderPos.z = vDstColliderPos.z - vDistance.z;
				else
					vSrcColliderPos.z = vDstColliderPos.z + vDistance.z;
			}
			//pDstTranformCom->Set_State(CTransform::STATE_POSITION, vDstColliderPos);
			pSrcTranformCom->Set_State(CTransform::STATE_POSITION, vSrcColliderPos);
			return;
		}
	}
	return;
}

_bool CCollision_Manager::Collision_AABB_Dot(CCollider_Cube_AABB* pColliderCubeAABB, _float3 vPosition)
{
	_float3 vMinPoint = pColliderCubeAABB->Get_Min_Point();
	_float3 vMaxPoint = pColliderCubeAABB->Get_Max_Point();

	if (vMinPoint.x > vPosition.x || vMaxPoint.x < vPosition.x)
		return false;
	if (vMinPoint.y > vPosition.y || vMaxPoint.y < vPosition.y)
		return false;
	if (vMinPoint.z > vPosition.z || vMaxPoint.z < vPosition.z)
		return false;
	return true;
}

void CCollision_Manager::Collision_AABB_Dst_Dot_List(list<class CGameObject*>* pSrcObjectList, list<class CGameObject*>* pDstObjectList)
{
	for (auto Dst : *pDstObjectList)
	{
		if (true == Dst->Get_Dead())
			continue;
		CTransform* pDstTranformCom = dynamic_cast<CTransform*>(Dst->Get_Component(g_strTransformTag));
		_float3 vDstPosition = pDstTranformCom->Get_State(CTransform::STATE_POSITION);
		for (auto Src : *pSrcObjectList)
		{
			if (true == Src->Get_Dead())
				continue;
			CCollider_Cube_AABB* pSrcCollider = dynamic_cast<CCollider_Cube_AABB*>(Src->Get_Component(TEXT("Com_Collider_Cube_AABB")));
			if (Collision_AABB_Dot(pSrcCollider, vDstPosition))
			{
				//이거 콜리전의 xyz델타 받아서 해보자.
				Dst->Set_Dead();
				break;
			}
		}
	}
	return;
}

_bool CCollision_Manager::Collision_Sphere(CCollider_Sphere* pSrcColliderSphere, CCollider_Sphere* pDstColliderSphere)
{
	if (nullptr == pSrcColliderSphere || nullptr == pDstColliderSphere)
		return false;

	_float3 vSrcPos = pSrcColliderSphere->Get_State(CCollider::STATE_POSITION);
	_float3 vDstPos = pDstColliderSphere->Get_State(CCollider::STATE_POSITION);

	_float3 vDir = vSrcPos - vDstPos;
	_float fDistance = D3DXVec3Length(&vDir);

	_float fRange = pSrcColliderSphere->Get_Range() + pDstColliderSphere->Get_Range();

	if (fRange <= fDistance) //거리가 범위의 합보다 큼. 충돌 안함.
		return false;
	else
		return true;
}

CCollision_Manager* CCollision_Manager::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCollision_Manager* pInstance = new CCollision_Manager(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CCollision_Manager"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollision_Manager::Free()
{
	__super::Free();
}
