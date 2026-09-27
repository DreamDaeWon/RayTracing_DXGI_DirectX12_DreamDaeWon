#pragma once
#include "BlendObject.h"
#include "Client_Defines.h"

#define TRAILRENDERER_LIFE_TIME 0.3f;

BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CTrailRenderer final : public CBlendObject
{
private:
	CTrailRenderer(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTrailRenderer(const CTrailRenderer& rhs);
	virtual ~CTrailRenderer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_Pos_XScale_Rotation(_float3 vPos, _float fXScale, _float3 vDir);

private:
	HRESULT Add_Components();
	HRESULT Set_RenderState();
	HRESULT Reset_RenderState();

	void Set_LifeTime() { m_fLifeTime = TRAILRENDERER_LIFE_TIME; }
private:
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer_Com = { nullptr };

	_float m_fLifeTime = { 0.f };

public:
	static CTrailRenderer* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END