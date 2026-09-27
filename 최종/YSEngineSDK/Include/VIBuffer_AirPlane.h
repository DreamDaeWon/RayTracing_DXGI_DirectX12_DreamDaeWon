#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_AirPlane final : public CVIBuffer
{
private:
	CVIBuffer_AirPlane(LPDIRECT3DDEVICE9 pGraphic_Device);
	CVIBuffer_AirPlane(const CVIBuffer_AirPlane& rhs);
	virtual ~CVIBuffer_AirPlane() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;


public:
	static CVIBuffer_AirPlane* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END