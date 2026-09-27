#pragma once
#include "BlendObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)
//모든 이펙트의 부모가 되는 클래스.
//자식 클래스는 이니셜라이즈에서 디스크립션으로 fFrame과 fMaxFrame을 채워서 던져야함.
//애드 컴포넌트로 멤버 채워주기

class CEffect_Base abstract : public CBlendObject
{
public:
	typedef struct tagEffectBaseDesc : public CGameObject::GAMEOBJECT_DESC {
		_float			fFrame = { 0.f };
		_float			fMaxFrame = { 0.f };
	}EFFECT_BASE_DESC;

protected:
	CEffect_Base(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect_Base(const CEffect_Base& rhs);
	virtual ~CEffect_Base() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


protected:

	CTexture*		m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer_Com = { nullptr };
	_float			m_fFrame = { 0.f };
	_float			m_fMaxFrame = { 0.f };

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END