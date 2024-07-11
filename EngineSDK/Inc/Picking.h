#pragma once

#include "Base.h"

BEGIN(Engine)

class CPicking final : public CBase
{
private:
	CPicking(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CPicking() = default;

public:
	HRESULT Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	void Update(); /* 윈도우좌표상(뷰포트상)의 마우스 좌표를 구하고 이 마우스를 좌표를 공용 스페이스(월드 스페이스)까지 내릴 수 있을 만큼 내린다. */
	void Transform_ToLocalSpace(const _float4x4& WorldMatrix);
	_bool isPicked_InWorldSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut);
	_bool isPicked_InLocalSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut, _float* pDist );
	_bool isPicked_InLocalSpaceForBlending(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut );

private:
	LPDIRECT3DDEVICE9			m_pGraphic_Device = { nullptr };
	HWND						m_hWnd = {};
	_uint						m_iWinSizeX = {};
	_uint						m_iWinSizeY = {};

private:
	_float3						m_vRayPos = {};
	_float3						m_vRayDir = {};

private:
	_float3						m_vRayPos_InLocalSpace = {};
	_float3						m_vRayDir_InLocalSpace = {};

public:
	static CPicking* Create(LPDIRECT3DDEVICE9 pGraphic_Device, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	virtual void Free();
};

END