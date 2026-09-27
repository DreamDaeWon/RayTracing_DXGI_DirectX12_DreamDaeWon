#pragma once
#include "Renderer.h"
#include "Sound_Manager.h"
#include "Component_Manager.h"

BEGIN(Engine)

class CGraphic_Device;
class CLevel;
class CLevel_Manager;
class CObjectManager;
class CTimer_Manager;
class CPicking;
class CFont_Manager;
class CKey_Manager;
class CCollision_Manager;
class CSound_Manager;



/* 클라이언트개발자가 엔진의 기능을 사용하기위해서 항상 접근해야하는 클래스. */
class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)

private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT		Initialize_Engine(_uint iNumLevels, const ENGINE_DESC& EngineDesc, _Out_ LPDIRECT3DDEVICE9* ppGraphic_Device );
	HRESULT		Draw();
	void		Tick_Engine(_float fTimeDelta);
	HRESULT		Clear(_uint iClearLevelIndex);

public: //For. CLevel_Manager
	HRESULT		Open_Level(_uint eNextLevelID, CLevel* pLevel);
	_uint		Get_Level();

public: //For. CObject_Manager
	HRESULT Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_Clone(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);
	HRESULT Add_Object(class CGameObject* pGameObjcet, _uint iLevelIndex, const wstring& strLayerTag);//오브젝트를 원하는 레벨에 원하는 태그로 넣어주는 함수.
	CComponent* Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComTag, _uint iIndex = 0);
	//오브젝트의 주소를 반환하는 함수?
	HRESULT Get_Object(_uint iLevelIndex, const wstring& strLayerTag, _Out_ CGameObject** ppObject, _uint iIndex = 0);
	CGameObject* Get_Object(_uint iLevelIndex, const wstring& strLayerTag, _uint iIndex = 0);
	list<class CGameObject*>* Get_List(_uint iLevelIndex, const wstring& strLayerTag);

public: //For. CComponent_Manager
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, CComponent* pPrototype);
	CComponent* Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg = nullptr);

public: //For. CRenderer
	HRESULT Add_RenderObject(CRenderer::RENDER_GROUP eRenderGroup, class CGameObject* pRenderObject);
	void Render_Object_List_Clear();

public: //For. CTimer_Manager
	HRESULT Add_Timer(const wstring& strTimerTag);
	_float Compute_TimeDelta(const wstring& strTimerTag);

public: //For. CPicking
	void Transform_PickingToLocalSpace(class CTransform* pTransform, _Out_ _float3* pRayDir, _Out_ _float3* pRayPos);
	void Transform_PickingToLocalSpace(_float4x4 WorldMaxrixInv, _Out_ _float3* pRayDir, _Out_ _float3* pRayPos);
	void Get_World_Mouse_Ray(_Out_ _float3* pRayDir, _Out_ _float3* pRayPos); //TODO: 용수 콜라이더 충돌을 위해 추가

public: //For. CFont_Manager
	HRESULT		Add_Font(const wstring& strFontTag, const wstring& strFontType, const _uint& iWidth, const _uint& iHeight, const _uint& iWeight);
	void		Render_Font(const wstring& strFontTag, const wstring& strText, const _float2* pPos, D3DXCOLOR Color);

public: //For. CKey_Manager
	_bool		Key_Pressing(_uint _iKey);
	_bool		Key_Down(_uint _iKey);
	_bool		Key_Up(_uint _iKey);
	
public: //For. CCollision_Manager
	_bool Collision_Rect_Ray(CCollider_Rect* ColliderRect, class CTransform* pTransform, _float3 vRayDir, _float3 vRayPos); //조준했을 때 초록 or 빨간 커서 할때 사용하면 좋을듯?
	_bool Collision_Rect_Ray(CCollider_Rect* ColliderRect, class CTransform* pTransform, _float3 vRayDir, _float3 vRayPos, _float fLength); //총알 충돌처리시 사용. fLength는 레이의 길이
	_bool Collision_Rect_Ray_Same_Space(CCollider_Rect* pColliderRect, _float3 vRayDir, _float3 vRayPos);
	_bool Collision_Rect_Ray_Same_Space(CCollider_Rect* pColliderRect, _float3 vRayDir, _float3 vRayPos, _float fLength);
	_bool Collision_Rect_Ray_Same_Space(CCollider_Rect* pColliderRect, _float3 vRayDir, _float3 vRayPos, _float fLength, _float* pDistance);
	_bool Collision_AABB(CCollider_Cube_AABB* pSrcColliderCubeAABB, CCollider_Cube_AABB* pDstColliderCubeAABB);
	void Collision_AABB_List(list<class CGameObject*>* pSrcObjectList, list<class CGameObject*>* pDstObjectList);
	void Collision_AABB_List(list<class CGameObject*>* pSrcObjectList, list<class CGameObject*>* pDstObjectList, _float fSrcPower, _float fDstPower);
	void Collision_AABB_List_Fixed_Dst(list<class CGameObject*>* pSrcObjectList, list<class CGameObject*>* pDstObjectList,_float fTimeDelta);
	_bool Collision_Sphere(CCollider_Sphere* pSrcColliderSphere, CCollider_Sphere* pDstColliderSphere);
	_bool Collision_AABB_Dot(CCollider_Cube_AABB* pColliderCubeAABB, _float3 vPosition);
	void Collision_AABB_Dst_Dot_List(list<class CGameObject*>* pSrcObjectList, list<class CGameObject*>* pDstObjectList);
public: //For. CSound_Manager
	int  VolumeUp(CSound_Manager::CHANNELID eID, _float _vol);
	int  VolumeDown(CSound_Manager::CHANNELID eID, _float _vol);
	int  BGMVolumeUp(_float _vol);
	int  BGMVolumeDown(_float _vol);
	int  Pause(CSound_Manager::CHANNELID eID);
	void PlaySoundW(TCHAR* pSoundKey, CSound_Manager::CHANNELID eID, _float _vol);
	void PlayBGM(TCHAR* pSoundKey);
	void StopSound(CSound_Manager::CHANNELID eID);
	void StopAll();

private:
	CGraphic_Device* m_pGraphic_Device = { nullptr };
	CLevel_Manager* m_pLevel_Manager = { nullptr };
	CObjectManager* m_pObject_Manager = { nullptr };
	CComponent_Manager* m_pComponent_Manager = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CTimer_Manager* m_pTimer_Manager = { nullptr };
	CPicking* m_pPicking = { nullptr };
	CFont_Manager* m_pFont_Manager = { nullptr };
	CKey_Manager* m_pKey_Manager = { nullptr };
	CCollision_Manager* m_pCollision_Manager = { nullptr };
	CSound_Manager* m_pSound_Manager = { nullptr };

public:
	static void Release_Engine();
	virtual void Free() override;
};

END