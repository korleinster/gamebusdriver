#include "stdafx.h"
#include "ShaderMgr.h"
#include "Shader.h"

IMPLEMENT_SINGLETON(CShaderMgr)

CShaderMgr::CShaderMgr()
{
}


CShaderMgr::~CShaderMgr()
{
	Release();
}

HRESULT CShaderMgr::AddShaderFiles(wstring wstrShaderTag, wstring wstrFilePath, LPCSTR wstrShaderName, LPCSTR wstrShaderVersion, ShaderType _eType)
{
	CShader*		pShader = Find_Shader(wstrShaderTag);
	NULL_CHECK_RETURN(!pShader, E_FAIL);

	pShader = CShader::Create(wstrFilePath, wstrShaderName, wstrShaderVersion, _eType);

	NULL_CHECK_RETURN(pShader, E_FAIL);

	m_mapShader.insert(map<wstring, CShader*>::value_type(wstrShaderTag, pShader));
	return S_OK;
}

CShader * CShaderMgr::Clone_Shader(wstring wstrShaderTag)
{
	CShader*		pShader = Find_Shader(wstrShaderTag);
	if (pShader == NULL)
		return NULL;

	return pShader->CloneShader();
}

CShader * CShaderMgr::Find_Shader(wstring wstrShaderTag)
{
	map<wstring, CShader*>::iterator iter = m_mapShader.find(wstrShaderTag);

	if (iter == m_mapShader.end())
		return NULL;

	return iter->second;
}

void CShaderMgr::Release(void)
{
	map<wstring, CShader*>::iterator iter = m_mapShader.begin();
	map<wstring, CShader*>::iterator iter_end = m_mapShader.end();

	for (iter; iter != iter_end; ++iter)
	{
		Safe_Delete(iter->second);
	}

}
