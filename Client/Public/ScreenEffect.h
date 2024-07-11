#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTransform;
class CVIBuffer_Rect;
class CShader;
END


BEGIN(Client)

class CScreenEffect final : public CGameObject
{
public:
	enum EFFECT_TYPE { ET_HORIZONTAL, ET_HIT, ET_WAVE, ET_TORNADO, ET_GAUSSIAN, ET_CHROMATIC, ET_NOISE, ET_CENEMA, ET_HEAL, ET_BUFF, ET_TIME, ET_END};
	typedef struct tagEffectInfo
	{
		EFFECT_TYPE eType;
		float		fDuration;
	}EFFECT_DESC;

private:
	CScreenEffect(LPDIRECT3DDEVICE9 pGraphic_Device);
	CScreenEffect(const CScreenEffect& Prototype);
	virtual ~CScreenEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTransform*				m_pTransformCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };

private:
	/* 어떤 위치(fX, fY)에 어떤 사이즈(fSizeX, fSizeY)로 그릴건지를 설정한 데이터다.  */
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4				m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

	_float					m_fTime = 0.f;
	EFFECT_DESC				m_EffectDesc;

private:
	HRESULT Ready_Components();

public:
	static CScreenEffect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END