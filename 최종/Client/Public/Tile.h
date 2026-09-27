#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CVIBuffer_Tile;
class CTexture;
END

BEGIN(Client)

class CTile final : public CGameObject
{
private:
	CTile(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTile(const CTile& rhs);
	virtual ~CTile() = default;

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
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Tile* m_pVIBuffer_Com = { nullptr };
	//CVIBuffer_Rect* m_pVIBuffer_Com = { nullptr };

	_float m_fFrame = { 0.f };


public:
	static CTile* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END