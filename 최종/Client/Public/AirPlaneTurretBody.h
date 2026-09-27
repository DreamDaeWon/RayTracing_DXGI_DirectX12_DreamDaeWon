#pragma once
#include "LandObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CVIBuffer_Cube;
class CTexture;
END


BEGIN(Client)

class CAirPlaneTurretBody final : public CLandObject
{
public:
	typedef struct tagTurretBodyDesc : public CLandObject::LANDOBJECT_DESC {
		_float3 vPosition = {};
		_uint iFrame = { 0 };
	}TURRET_BODY_DESC;

private:
	CAirPlaneTurretBody(LPDIRECT3DDEVICE9 pGraphic_Device);
	CAirPlaneTurretBody(const CAirPlaneTurretBody& rhs);
	virtual ~CAirPlaneTurretBody() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Set_RenderState();
	HRESULT Reset_RenderState();


private:

	_float m_fScale = { 0.3f }; // 크기값을 정해 줌


	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Cube* m_pVIBuffer_Com = { nullptr };
	
	_uint m_iFrame = { 0 };

public:
	static CAirPlaneTurretBody* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
