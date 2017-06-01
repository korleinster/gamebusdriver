#include "stdafx.h"
#include "UI.h"

#include "Include.h"
#include "Texture.h"
#include "VIBuffer.h"
#include "Shader.h"


CUI::CUI()
{
	m_pBuffer = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pTexture = NULL;
	m_fX = 0.f;
	m_fY = 0.f;
	m_fOriginX = 0.f;
	m_fOriginY = 0.f;
	m_fSizeX = 0.f;
	m_fSizeY=0.f;
}

CUI :: ~CUI()
{

}


