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

class CGameFG final : public CImage
{
private:
	CGameFG(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameFG(const CGameFG& Prototype);
	virtual ~CGameFG() = default;

public:
	static CGameFG* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END