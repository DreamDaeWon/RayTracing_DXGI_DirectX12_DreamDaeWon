#pragma once
#include "LandObject.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CVIBuffer_Cube;
class CVIBuffer_Rect;
class CTexture;
//콜라이더 실험 BEGIN.
class CCollider_Sphere;
//콜라이더 실험 END.
END

BEGIN(Client)

class CTrigger_Door final : public CLandObject
{
public:
	typedef struct tagTrigger_Desc : public CLandObject::LANDOBJECT_DESC {

		_float3 vLook;
		_float3 vPos; 
		_float fScale;
		_uint iTexNum;
		_float fSpeed;

	}TRIGGER_DESC;

private:
	CTrigger_Door(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTrigger_Door(const CTrigger_Door& rhs);
	virtual ~CTrigger_Door() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Return_Look_Position(_float3* pvLook, _float3* pvPosition);
	_float3 Return_ViewPort_Pos(); 


private:
	HRESULT Add_Components();
	HRESULT Set_RenderState();
	HRESULT Reset_RenderState();

	void Collision_Player();

private:
	_bool	 m_bDoor = { false };
	_bool	 m_bOnce = { true };

	CTexture* m_pTextureCom = { nullptr };
	CTexture* m_pTextureCom1 = { nullptr };
	CVIBuffer_Cube* m_pVIBuffer_Com_RightDoor = { nullptr }; //왼짝문~
	CVIBuffer_Cube* m_pVIBuffer_Com_LeftDoor = { nullptr }; //왼짝문~
	CVIBuffer_Rect* m_pVIBuffer_Com_Rect = { nullptr }; //왼짝문~
	class CCamera_Player_DW* m_pCamera = { nullptr };

	CCollider_Sphere* m_pCollider_Com = { nullptr };
	CCollider_Sphere* m_pCollider_Com1 = { nullptr };
	CTransform* m_pTransformLeftDoor = { nullptr };
	//콜라이더 실험 END.

	// 속도
	_float m_fGoSpeed = { 10.f }; 

	// 받아오는 문의 정보
	_float3 m_vDoorLook = {};
	_float3 m_vDoorPos = {};
	_float3 m_vDoorPos_Right = {};
	_float m_fScale = {};
	_float m_fTime = {0.f};

	
	_uint m_iTexNum = { 0 };


public:
	static CTrigger_Door* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override; // 이거 보스가 실행
	virtual void Free() override;
};

END