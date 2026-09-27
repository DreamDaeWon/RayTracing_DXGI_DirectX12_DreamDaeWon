#pragma once
#include "LandObject.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
//콜라이더 실험 BEGIN.
class CCollider_Sphere;
//콜라이더 실험 END.
END

BEGIN(Client)

class CTrigger_Camera final : public CLandObject
{
public:
	enum CAMERAMODE { CAMERAMODE1, CAMERAMODE2, CAMERAMODE_END};
public:
	typedef struct tagTrigger_Desc : public CLandObject::LANDOBJECT_DESC {
		CAMERAMODE eCameraMode;
		_float3 vPos; 
		_float fScale;
		_uint iTexNum;
		_float fSpeed;

	}TRIGGER_DESC;

private:
	CTrigger_Camera(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTrigger_Camera(const CTrigger_Camera& rhs);
	virtual ~CTrigger_Camera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	HRESULT Add_Components();


public:
	void Set_Active() { m_bActive =  false; }
	_bool Get_Active() { return m_bActive; }
	CAMERAMODE Get_CameraMode() { return m_eCurCameraMode; }

private:

	_bool	 m_bMove = { false };
	_bool	m_bActive = { true };
	CAMERAMODE m_eCurCameraMode = CAMERAMODE1;
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer_Com = { nullptr }; 

	CCollider_Sphere* m_pCollider_Com = { nullptr };
	//콜라이더 실험 END.

	// 속도
	_float m_fGoSpeed = { 10.f }; 


	_float3 m_vPos = {};
	_float m_fScale = {};
	_float m_fTime = {0.f};

	
	_uint m_iTexNum = { 0 };


public:
	static CTrigger_Camera* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override; // 이거 보스가 실행
	virtual void Free() override;
};

END