#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_BattleCruiser final : public CVIBuffer
{
private:
	CVIBuffer_BattleCruiser(LPDIRECT3DDEVICE9 pGraphic_Device);
	CVIBuffer_BattleCruiser(const CVIBuffer_BattleCruiser& rhs);
	virtual ~CVIBuffer_BattleCruiser() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;


public:
	static CVIBuffer_BattleCruiser* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END