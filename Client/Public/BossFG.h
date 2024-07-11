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

class CBossFG final : public CImage
{
private:
	CBossFG(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBossFG(const CBossFG& Prototype);
	virtual ~CBossFG() = default;

public:
	static CBossFG* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END