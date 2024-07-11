#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUIScript : public CGameObject
{
public:
	typedef struct
	{
		LEVELID eLevel;
		_int iTalker;
		const _tchar* Script;	// 이미지 택스쳐 태그

	}SCRIPT_DESC;

public:
	CUIScript(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUIScript(const CUIScript& Prototype);
	virtual ~CUIScript() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	HRESULT Set_Variables(void* pArg);
	void Set_OrthoLH();
	virtual HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

protected:
	CTexture*				m_pTextureCom = { nullptr };
	CTransform*				m_pTransformCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };

protected:
	/* 직교투영 */
	_float4x4				m_ViewMatrix, m_ProjMatrix;
	SCRIPT_DESC				m_tScriptInfo = {};
	_float					m_fSumtime = 0.f;

	wstring					m_strFullText = L"쥬신 145기 김예래 최승준 양새하 화이팅!! \n스크립트 테스트입니다.";
	_int					m_CurrentCharIndex = 4;
	_int					m_EnterShow = 0;
	_bool					m_bSound = false;

	DWORD					m_dwColor = D3DCOLOR_ARGB(255, 255, 255, 255);

private:
	CGameObject*			m_pMyPortrait = nullptr;

private:
	LEVELID					m_eCurrentLevel = LEVEL_GAMEPLAY;
	int						m_iTalker = 1;

public:
	static CUIScript* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END