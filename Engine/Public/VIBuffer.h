#pragma once

#include "Component.h"

/* VIBuffer = Vertex + Index */
/* Vertex : 점 */
/* Line : 점두개를 이어서 직선을 그린다. */
/* Triangle : 점세개를 이어서 면을 그린다. 폴리곤 */
/* Vertex -> Polygon -> Mesh(Model) */

/* Index : 정점을 그리는 순서를 저장한 버퍼다. */

/* VIBuffer : 모델을 구성하는 각각의 자식클래스의 부모가 되는 클래스다. */
/* 자식클래스 -> VIBuffer_Rect, VIBuffer_Cube, VIBuffer_Terrain */


BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(LPDIRECT3DDEVICE9 pGraphic_Device);
	CVIBuffer(const CVIBuffer& Prototype);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render();

public:
	HRESULT Bind_Buffers();

public:
	virtual _bool isPicking(const _float4x4& WorldMatrix, _float3* pOut, _float* pDist) { return false; }

protected:
	/* dx가 알아서 할당해낸 정점 배열의 공간을 대표하는 컴객체.  */
	LPDIRECT3DVERTEXBUFFER9			m_pVB = { nullptr };
	LPDIRECT3DINDEXBUFFER9			m_pIB = { nullptr };
	_uint							m_iVertexStride = { 0 };
	_uint							m_iNumVertices = { 0 };
	_ulong							m_dwFVF = {};
	_uint							m_iNumFaces = {};

	_uint							m_iIndexStride = { 0 };
	_uint							m_iNumIndices = { 0 };
	D3DFORMAT						m_eIndexFormat = {  };

	_float3*						m_pVerticesPos = { nullptr };

protected:
	HRESULT Ready_VertexBuffer();
	HRESULT Ready_IndexBuffer();

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END