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

class CLogoBT_BossRush final : public CButton
{
private:
	CLogoBT_BossRush(LPDIRECT3DDEVICE9 pGraphic_Device);
	CLogoBT_BossRush(const CLogoBT_BossRush& Prototype);
	virtual ~CLogoBT_BossRush() = default;

public:
	static CLogoBT_BossRush* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END