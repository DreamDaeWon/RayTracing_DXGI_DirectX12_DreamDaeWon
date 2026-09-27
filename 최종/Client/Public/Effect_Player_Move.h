#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
/* 총 이미지 분석
0-6 등에 매달고 있을 때 사용하는 이미지
12-17?  조준 했을 때 반동표현하는 이미지
*/
class CEffect_Player_Move final : public CGameObject
{
	//public:
	//	typedef struct tagEffectSplatDesc : public CEffect_Base::EFFECT_BASE_DESC {
	//
	//	}EFFECT_SPLAT_DESC;


private:
	CEffect_Player_Move(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect_Player_Move(const CEffect_Player_Move& rhs);
	virtual ~CEffect_Player_Move() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	HRESULT Clone_Explosion();


public:
	void Show_Explosion(_float3 vPos, _float fTimeDelta);

private:
	_uint m_iVectorCursor = { 0 };
	_uint m_iMaxEffectNum = { 0 };
	_float m_fShowTerm = { 0.f };
	vector<class CEffect_Cloud*> m_vecEffectCloud = {};

public:
	static CEffect_Player_Move* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END