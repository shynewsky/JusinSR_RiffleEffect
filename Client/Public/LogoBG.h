#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Image.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CLogoBG final : public CImage
{
private:
	CLogoBG(LPDIRECT3DDEVICE9 pGraphic_Device);
	CLogoBG(const CLogoBG& Prototype);
	virtual ~CLogoBG() = default;

public:
	static CLogoBG* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END