#pragma once
#include "LandObject.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)

class CSpawnEffect final : public CLandObject
{
public:
	typedef struct tagSteam_Desc : public CLandObject::LANDOBJECT_DESC {
		_float3 vPosition = { 0.f,0.f,0.f };
		_uint iFrame = { 0 };
	}STEAM_DESC;

private:
	CSpawnEffect(LPDIRECT3DDEVICE9 pGraphic_Device);
	CSpawnEffect(const CSpawnEffect& rhs);
	virtual ~CSpawnEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_Pos(_float3 vPosition) {
		m_pTransform->Set_State(CTransform::STATE_POSITION, vPosition);
	}

	HRESULT Add_Components();
	HRESULT Set_RenderState();
	HRESULT Reset_RenderState();

public:
	void SetUp_BillBoard();

private:
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer_Com = { nullptr };
	CTransform* m_pCameraTransform = { nullptr };

	_float m_fScale = { 0.f };
	_float m_fScaleBig = { 0.f };
	_float m_fScaleSmall = { 0.f };

public:
	static CSpawnEffect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END