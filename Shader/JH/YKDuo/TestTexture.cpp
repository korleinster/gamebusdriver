#include "stdafx.h"
//
//HRESULT CTexture::Create_Texture(TEXTURETYPE eTextureType, const TCHAR* pFilePath, const WORD& wCnt)
//{
//	ID3D11ShaderResourceView* pShaderResourceView = NULL;
//	TCHAR szFullPath[MAX_PATH] = L"";
//	HRESULT hr = E_FAIL;
//
//	ScratchImage image;
//	TexMetadata info;
//
//	size_t iTextureCnt = 1;
//	if (wCnt > 0) iTextureCnt = wCnt;
//
//	m_vecTexture.reserve(iTextureCnt);
//
//	for (size_t i = 0; i < iTextureCnt; ++i)
//	{
//		if (wCnt == 0)   wsprintf(szFullPath, pFilePath);
//		else         wsprintf(szFullPath, pFilePath, i);
//
//		switch (eTextureType)
//		{
//		case TYPE_NORMAL:
//			hr = D3DX11CreateShaderResourceViewFromFile(m_pGraphicDev, szFullPath
//				, NULL, NULL, &pShaderResourceView, NULL);
//			break;
//
//		case TYPE_TGA:
//			hr = LoadFromTGAFile(szFullPath, &info, image);
//			if (FAILED(hr))   break;
//
//			hr = CreateShaderResourceView(m_pGraphicDev, image.GetImages(), image.GetImageCount(), info, &pShaderResourceView);
//			break;
//
//		case TYPE_DDSCUBE:
//			hr = LoadFromDDSFile(szFullPath, DDS_FLAGS_NONE, &info, image);
//			if (FAILED(hr))   break;
//
//			info.miscFlags &= ~TEX_MISC_TEXTURECUBE;
//
//			hr = CreateShaderResourceView(m_pGraphicDev, image.GetImages(), image.GetImageCount(), info, &pShaderResourceView);
//			break;
//		}
//
//		if (FAILED(hr) == TRUE)
//			return E_FAIL;
//
//		m_vecTexture.push_back(pShaderResourceView);
//	}
//
//	m_sizetContainerSize = m_vecTexture.size();
//
//	return S_OK;
//}"