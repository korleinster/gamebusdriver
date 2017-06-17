#pragma once

#ifndef Font_h__
#define Font_h__

#include "Resources.h"
#include "FW1FontWrapper.h"

class CFont
{

private:
	IFW1FontWrapper*		m_pFW1FontWarpper;
	IFW1Factory*			m_pFW1FontFactory;
	ID3D11DeviceContext*	m_pDeivceContext;

public:
	FontType		m_eType;//외곽선 여부
	wstring			m_wstrText;//텍스트 내용
	float			m_fSize;//폰트 사이즈
	UINT32			m_nColor;//폰트 색상 (0xAABBGGRR)
	UINT			m_nFlag;/*
	폰트의 플레그 설정.
	FW1_LEFT - 왼쪽정렬(디폴트)
	FW1_CENTER - 가운데정렬
	FW1_RIGHT - 오른쪽정렬

	FW1_TOP - 텍스트가 레이아웃박스 위에 있음(디폴트)
	FW1_VCENTER - 텍스트가 중앙에 수직으로 있음
	FW1_BOTTOM - 텍스트가 레이아웃 박스 아래에 있음.

	FW1_NOWORDWRAP - 자동으로 레이아웃 박스가 겹치는걸 막지 않음.
	FW1_ALIASED - 안티에어라이징을 무시하고 출력.
	FW1_CLIPRECT - clip-rect가 이 플래그와 명시되었을시, 모든 텍스트들은 다각형에 클립됨.
	FW1_NOGEOMETRYSHADER - 기하셰이더 없이 그려질때, 인덱싱된 쿼드들은 기하셰이더 대신 cpu를 사용
	FW1_CONSTANTSPREPARED - 트렌스폼 행렬과 clpi-rect가 컨스턴트 버퍼에 업데이트 되지 않음. 이전에 설정한 값을 통해 최적화함.
	FW1_BUFFERSPREPARED - 내부정점과 인덱스 버퍼가 이미 바인딩되었을때, 이전에 설정한 버퍼를 호출하여 최적화함.
	FW1_STATEPREPARED - 올바른 쉐이더/컨스턴트 버퍼 등이 이미 바인딩되었을때, 이전에 설정한 상태나 오버라이딩한 기본 상태를 호출하여 최적화함.
	FW1_IMMEDIATECALL = FW1_CONSTANTSPREPARED | FW1_BUFFERSPREPARED | FW1_STATEPREPARED - 최적화를 이렇게 할수 있음. 여러 텍스트들을 같은 세팅을 했을시에 추천.
	FW1_RESTORESTATE - 출력 방식이 리턴되면, 디바이스 컨텍스트가 이전의 호출 상태를 복원함(디폴트).
	FW1_NOFLUSH - 호출하는동안 추가된 새 문자들이 디바이스-리소스에서 지워지지 않음. 적절한 작업 순서를 보장하기 위해 여러 스레드에 대한 텍스트를 동시에 그릴 때 이 플래그를 사용하여 디퍼드 컨텍스트에 대해 이 플래그를 사용하는 것이 좋음.
	FW1_CACHEONLY - Any new glyphs will be cached in the atlas and glyph-maps, but no geometry is drawn.
	FW1_NONEWGLYPHS - No new glyphs will be added to the atlas or glyph-maps. Any glyphs not already present in the atlas will be replaced with a default fall-back glyph (empty box).
	FW1_ANALYZEONLY - A text-layout will be run through DirectWrite and new fonts will be prepared, but no actual drawing will take place, and no additional glyphs will be cached.
	FW1_UNUSED - 쓰지마.
	*/
	D3DXVECTOR2		m_vPos;//폰트의 위치(2D)
	float			m_fOutlineSize;//외곽선 사이즈
	UINT32			m_nOutlineColor;//외곽선 색상 (0xAARRGGBB)

	DWORD			m_dwRefCount;

public:
	static CFont*		Create(const wstring& _wstPath);
	virtual CFont*		CloneFont(void);

private:
	HRESULT			Load_Font(const wstring& _wstPath);

public:
	HRESULT			Initialize(const wstring& _wstPath);
	void			Render();
	DWORD			Release();


protected:
	 CFont();
public:
	virtual ~CFont();
};

#endif // Font_h__
