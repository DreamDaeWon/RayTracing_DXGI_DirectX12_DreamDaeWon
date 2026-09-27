#pragma once
#include "Base.h"
#include "Collider_Rect.h"
#include "Collider_Cube_AABB.h"
#include "Collider_Sphere.h"

//콜라이더끼리의 충돌을 계산해서 _bool값을 리턴해주는 클래스
/* 콜리전 컴포넌트 사용법
* 
*/
BEGIN(Engine)

class CCollision_Manager final : public CBase
{
private:
	CCollision_Manager(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CCollision_Manager() = default;


public:
	HRESULT Initialize();

public:
	// 콜라이더와 객체의 트랜스폼, 객체의 로컬로 다운한 레이를 입력해주면 충돌여부를 판단하는 함수.
	_bool Collision_Rect_Ray(CCollider_Rect* pColliderRect, //월드 콜라이더 컴포넌트
		class CTransform* pTransform, //콜라이더 객체의 트랜스폼
		_float3 vRayDir, //로컬상의 레이디랙션과 레이포즈
		_float3 vRayPos); //	조준했을 때 초록 or 빨간 커서 할때 사용하면 좋을듯?

	//콜라이더와 객체의 트랜스폼, 객체의 로컬로 다운한 레이, 레이의 길이를 입력해주면 충돌여부를 판단하는 함수.총알 충돌처리시 사용.
	_bool Collision_Rect_Ray(CCollider_Rect* pColliderRect, class CTransform* pTransform, _float3 vRayDir, _float3 vRayPos, _float fLength); 
	_bool Collision_Rect_Ray_Same_Space(CCollider_Rect* pColliderRect, _float3 vRayDir, _float3 vRayPos); 
	_bool Collision_Rect_Ray_Same_Space(CCollider_Rect* pColliderRect, _float3 vRayDir, _float3 vRayPos, _float fLength); 
	_bool Collision_Rect_Ray_Same_Space(CCollider_Rect* pColliderRect, _float3 vRayDir, _float3 vRayPos, _float fLength, _float* pDistance);

	_bool Collision_AABB(CCollider_Cube_AABB* pSrcColliderCubeAABB, CCollider_Cube_AABB* pDstColliderCubeAABB);
	void Collision_AABB_List(list<class CGameObject*>* pSrcObjectList, list<class CGameObject*>* pDstObjectList);
	void Collision_AABB_List(list<class CGameObject*>* pSrcObjectList, list<class CGameObject*>* pDstObjectList, _float fSrcPower, _float fDstPower);
	void Collision_AABB_List_Fixed_Dst(list<class CGameObject*>* pSrcObjectList, list<class CGameObject*>* pDstObjectList, _float fTimeDelta);

	_bool Collision_AABB_Dot(CCollider_Cube_AABB* pColliderCubeAABB, _float3 vPosition); //큐브와 점 충돌
	void Collision_AABB_Dst_Dot_List(list<class CGameObject*>* pSrcObjectList, list<class CGameObject*>* pDstObjectList);

	//구충돌은 뭘가 필요할까? 구 콜라이더와 점충돌, 구콜라이더와 구콜라이더 충돌.
	_bool Collision_Sphere(CCollider_Sphere* pSrcColliderSphere, CCollider_Sphere* pDstColliderSphere);

public:
	static CCollision_Manager* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual	void Free() override;
};

END