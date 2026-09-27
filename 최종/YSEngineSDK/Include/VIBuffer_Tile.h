#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Tile final : public CVIBuffer
{
private:
	CVIBuffer_Tile(LPDIRECT3DDEVICE9 pGraphic_Device);
	CVIBuffer_Tile(const CVIBuffer_Tile& rhs);
	virtual ~CVIBuffer_Tile() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CVIBuffer_Tile* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END