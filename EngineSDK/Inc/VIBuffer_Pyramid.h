#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Pyramid final : public CVIBuffer
{
private:
	CVIBuffer_Pyramid(LPDIRECT3DDEVICE9 pGraphic_Device);
	CVIBuffer_Pyramid(const CVIBuffer_Pyramid& Prototype);
	virtual ~CVIBuffer_Pyramid() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CVIBuffer_Pyramid* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END