#include "..\Public\VIBuffer.h"

CVIBuffer::CVIBuffer(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent { pGraphic_Device } 
{
}

CVIBuffer::CVIBuffer(const CVIBuffer & Prototype)
	: CComponent{ Prototype }
	, m_pVB { Prototype.m_pVB }
	, m_pIB { Prototype.m_pIB }
	, m_iVertexStride{ Prototype.m_iVertexStride }
	, m_iNumVertices{ Prototype.m_iNumVertices }
	, m_dwFVF{ Prototype.m_dwFVF }
	, m_iNumFaces{ Prototype.m_iNumFaces }
	, m_iIndexStride { Prototype.m_iIndexStride }
	, m_iNumIndices { Prototype.m_iNumIndices }
	, m_eIndexFormat { Prototype.m_eIndexFormat }
	, m_pVerticesPos { Prototype.m_pVerticesPos }
	
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

HRESULT CVIBuffer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	// m_pGraphic_Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_iNumFaces);
	m_pGraphic_Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iNumVertices, 0, m_iNumFaces);

	return S_OK;
}

HRESULT CVIBuffer::Bind_Buffers()
{
	m_pGraphic_Device->SetStreamSource(0, m_pVB, 0, m_iVertexStride);
	m_pGraphic_Device->SetFVF(m_dwFVF);
	m_pGraphic_Device->SetIndices(m_pIB);

	return S_OK;
}

HRESULT CVIBuffer::Ready_VertexBuffer()
{
	/* 정점배열의 공간을 생성할 수 있도록 함수를 호출한다. */
	/* 1. Length : 정점 배열공간을 몇 바이트나 할당할까요? (정점하나의 바이트 크기 * 정점의 갯수) */
	/* 2. Usage : 이 버퍼의 속성(STATIC : 0, DYNAMIC: D3DUSAGE_DYNAMIC)을 설정해달라. */
	/* 3. fvf : D3DFVF 내 정점이 이러이러한 구성으로 되어있어.구성만 알려주느 ㄴ것이기 때문에 변수 선언의 순서를 유의해한다. */
	/* 4. MemoryPool : D3DPOOL_DEFAULT(VRam), D3DPOOL_MANAGED(VRam or Systemmem), D3DPOOL_SYSTEMMEM(Ram) */
	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(m_iVertexStride * m_iNumVertices,
		0, m_dwFVF, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return E_FAIL;


	return S_OK;
}

HRESULT CVIBuffer::Ready_IndexBuffer()
{
	if (FAILED(m_pGraphic_Device->CreateIndexBuffer(m_iIndexStride * m_iNumIndices, 0, m_eIndexFormat, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return E_FAIL;

	return S_OK;
}



void CVIBuffer::Free()
{
	__super::Free();

	if(false == m_isCloned)
		Safe_Delete_Array(m_pVerticesPos);

	Safe_Release(m_pIB);
	Safe_Release(m_pVB);

}
