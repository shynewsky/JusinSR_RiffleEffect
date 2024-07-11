#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cone final : public CVIBuffer
{
private:
	CVIBuffer_Cone(LPDIRECT3DDEVICE9 pGraphic_Device);
	CVIBuffer_Cone(const CVIBuffer_Cone& Prototype);
	virtual ~CVIBuffer_Cone() = default;

public:
	virtual HRESULT Initialize_Prototype(const float bottomRadius, const float topRadius, float height, int sliceCount);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override;

public:
	static CVIBuffer_Cone* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const float bottomRadius, const float topRadius, float height, int sliceCount);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END