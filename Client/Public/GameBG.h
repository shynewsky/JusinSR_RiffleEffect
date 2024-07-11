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

class CGameBG final : public CImage
{
private:
	CGameBG(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameBG(const CGameBG& Prototype);
	virtual ~CGameBG() = default;

public:
	static CGameBG* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END