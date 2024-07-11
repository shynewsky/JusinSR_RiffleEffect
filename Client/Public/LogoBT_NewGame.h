#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Button.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CLogoBT_NewGame final : public CButton
{
private:
	CLogoBT_NewGame(LPDIRECT3DDEVICE9 pGraphic_Device);
	CLogoBT_NewGame(const CLogoBT_NewGame& Prototype);
	virtual ~CLogoBT_NewGame() = default;

public:
	static CLogoBT_NewGame* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END


