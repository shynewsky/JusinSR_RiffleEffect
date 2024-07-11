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

class CBossFace final : public CImage
{
private:
	CBossFace(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBossFace(const CBossFace& Prototype);
	virtual ~CBossFace() = default;

public:
	static CBossFace* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END