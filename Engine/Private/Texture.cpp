#include "..\Public\Texture.h"

#include "GameInstance.h"

CTexture::CTexture(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent { pGraphic_Device }
{

}

CTexture::CTexture(const CTexture & Prototype)
	: CComponent{ Prototype }
	, m_iNumTextures{ Prototype.m_iNumTextures }
	, m_Textures{ Prototype.m_Textures }
{
	for (auto& pTexture : m_Textures)
		Safe_AddRef(pTexture);


}

HRESULT CTexture::Initialize_Prototype(TYPE eTextureType, const _wstring & strTextureFilePath, _uint iNumTextures)
{	
	m_iNumTextures = iNumTextures;

	m_Textures.reserve(iNumTextures);

	for (size_t i = 0; i < iNumTextures; i++)
	{
		IDirect3DBaseTexture9*		pTexture = { nullptr };

		_tchar		szTextureFilePath[MAX_PATH] = TEXT("");

		wsprintf(szTextureFilePath, strTextureFilePath.c_str(), i);

		HRESULT hr = eTextureType == TYPE_TEXTURE2D
			? D3DXCreateTextureFromFile(m_pGraphic_Device, szTextureFilePath, reinterpret_cast<LPDIRECT3DTEXTURE9*>(&pTexture))
			: D3DXCreateCubeTextureFromFile(m_pGraphic_Device, szTextureFilePath, reinterpret_cast<LPDIRECT3DCUBETEXTURE9*>(&pTexture));

		if (FAILED(hr))
			return E_FAIL;

		m_Textures.emplace_back(pTexture);
	}

	return S_OK;
}

HRESULT CTexture::Initialize(void * pArg)
{
	if (pArg != nullptr)
	{
		memcpy(&m_TextureDesc, pArg, sizeof(TEXTUREDESC));
		m_TextureDesc.iCurrentTex = m_TextureDesc.iStartTex;
	}

	return S_OK;
}

HRESULT CTexture::Bind_Texture(_uint iIndex)
{
	if (iIndex >= m_iNumTextures)
		return E_FAIL;

	return m_pGraphic_Device->SetTexture(0, m_Textures[iIndex]);
}

bool CTexture::Move_Frame(const _float fTimer, _bool bLoop) // stop이면 true 반환
{
	//루핑
	m_fSumTime += fTimer;
	if (bLoop)
	{
		if (m_fSumTime >= m_TextureDesc.fSpeed)
		{
			m_TextureDesc.iCurrentTex++;

			if (m_TextureDesc.iCurrentTex >= m_TextureDesc.iEndTex)
				m_TextureDesc.iCurrentTex = m_TextureDesc.iStartTex;

			m_fSumTime = 0.f;

			return false;
		}
	}

	// 한번만 돌리기
	else
	{
		if (m_fSumTime >= m_TextureDesc.fSpeed)
		{
			if (m_TextureDesc.iCurrentTex < m_TextureDesc.iEndTex)
				m_TextureDesc.iCurrentTex++;

			if (m_TextureDesc.iCurrentTex >= m_TextureDesc.iEndTex)
			{
				m_TextureDesc.iCurrentTex = m_TextureDesc.iEndTex - 1;
				return true;
			}

			m_fSumTime = 0.f;

			return false;
		}
	}

	return false;
}

void CTexture::Set_Frame(int iStartTex, int iEndTex, _float fSpeed)
{
	m_TextureDesc.iStartTex = iStartTex;
	m_TextureDesc.iEndTex = iEndTex;
	m_TextureDesc.fSpeed = fSpeed;
	m_TextureDesc.iCurrentTex = m_TextureDesc.iStartTex;
}

void CTexture::Turn_Frame(int iStartTex, int _iEndTex)
{ //%프레임 끝을 빼놔서 eof 걸릴수있다 이거 그러면 매개변수에 finalTex 받아서 나머지연산 해주긴 해야될듯
	if (0 == _iEndTex)
		return;
	m_TextureDesc.iCurrentTex = iStartTex + m_TextureDesc.iCurrentTex - m_TextureDesc.iStartTex;
	m_TextureDesc.iEndTex = m_TextureDesc.iEndTex - m_TextureDesc.iStartTex + iStartTex;
	m_TextureDesc.iStartTex = iStartTex;
}

void CTexture::Reset_Frame()
{
	m_TextureDesc.iCurrentTex = m_TextureDesc.iStartTex;
}

void CTexture::Set_FrameSpeed(_float _fSpeed)
{
	m_TextureDesc.fSpeed = _fSpeed;
}

CTexture * CTexture::Create(LPDIRECT3DDEVICE9 pGraphic_Device, TYPE eTextureType, const _wstring & strTextureFilePath, _uint iNumTextures)
{
	CTexture*		pInstance = new CTexture(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype(eTextureType, strTextureFilePath, iNumTextures)))
	{
		MSG_BOX(TEXT("Failed to Created : CTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CComponent* CTexture::Clone(void* pArg)
{
	CTexture*		pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for (auto& pTexture : m_Textures)
		Safe_Release(pTexture);

	m_Textures.clear();
}
