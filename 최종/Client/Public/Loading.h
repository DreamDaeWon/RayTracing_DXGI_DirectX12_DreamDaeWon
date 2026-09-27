#pragma once
#include "GameObject.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)

class CLoading final: public CGameObject
{
public:
	typedef struct tagLoading_Desc : public CGameObject::GAMEOBJECT_DESC {
		class CLoader* pLoader = { nullptr };
	}LOADING_DESC;

private:
	CLoading(LPDIRECT3DDEVICE9 pGraphic_Device);
	CLoading(const CLoading& rhs);
	virtual ~CLoading() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	HRESULT							Render_Again(_float fSizeX, _float fSizeY, _float fX, _float fY, _uint iFrame);
	HRESULT Add_Components();
	HRESULT Set_RenderState(_ulong lAphaRef = 0);
	HRESULT Reset_RenderState();
	void							Reset_First_State();

private:
	_float m_fTime = { 0.f };
	_float4x4						m_vFirstWorldMatrix = {};
	_float			m_fTexture = { 16.f };
	CTexture*		m_pTextureCom = { nullptr };
	CTexture*		m_pTextureBackCom = { nullptr };
	CTexture*		m_pTextureGattoCom = { nullptr };
	CTexture*		m_pTextureBarCom = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer_Com = { nullptr };
	_float						m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4					m_ViewMatrix, m_ProjMatrix;
	class CLoader* m_pLoader = { nullptr };
public:
	static CGameObject* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END