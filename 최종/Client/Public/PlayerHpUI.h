#pragma once
#include "UI_Base.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)

class CPlayerHpUI final: public CUI_Base
{
public:
	typedef struct tagPlayerHpUI_Desc : public CUI_Base::UI_BASE_DESC {

	}PLAYER_HP_UI_DESC;

private:
	CPlayerHpUI(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPlayerHpUI(const CPlayerHpUI& rhs);
	virtual ~CPlayerHpUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Set_RenderState();
	// CUI_Base을(를) 통해 상속됨
	virtual HRESULT Set_RenderState(_ulong lAphaRef) override;
	HRESULT Reset_RenderState();

private:
	void Chase_Player();

private:
	CTexture*		m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer_Com = { nullptr };

public:
	static CPlayerHpUI* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;


};

END