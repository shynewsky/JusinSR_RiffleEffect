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

class CBossBG final : public CImage
{
private:
	CBossBG(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBossBG(const CBossBG& Prototype);
	virtual ~CBossBG() = default;

public:
	static CBossBG* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END