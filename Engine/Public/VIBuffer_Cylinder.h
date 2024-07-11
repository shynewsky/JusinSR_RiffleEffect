#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cylinder final : public CVIBuffer
{
private:
	CVIBuffer_Cylinder(LPDIRECT3DDEVICE9 pGraphic_Device);
	CVIBuffer_Cylinder(const CVIBuffer_Cylinder& Prototype);
	virtual ~CVIBuffer_Cylinder() = default;

public:
	virtual HRESULT Initialize_Prototype(const float bottomRadius, const float topRadius, float height, int sliceCount);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override;

public:
	static CVIBuffer_Cylinder* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const float bottomRadius, const float topRadius, float height, int sliceCount);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END