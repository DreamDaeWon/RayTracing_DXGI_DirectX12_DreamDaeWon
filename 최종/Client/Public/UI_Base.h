#pragma once

#include "GameObject.h"
#include"Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_Base abstract : public CGameObject
{

public:
	typedef struct tagUI_Base_Desc : public CGameObject::GAMEOBJECT_DESC {

	}UI_BASE_DESC;
protected:
	CUI_Base(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI_Base(const CUI_Base& rhs);
	virtual ~CUI_Base() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

protected:
	_float						m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4					m_ViewMatrix, m_ProjMatrix;
	_float						m_fFrame = { 0.f };


protected:
	virtual HRESULT Add_Components() =0;
	virtual HRESULT Set_RenderState(_ulong lAphaRef = 0)=0; //블랜드를 하기위해 _ulong lAphaRef 받아옴.
	virtual HRESULT Reset_RenderState()=0;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END