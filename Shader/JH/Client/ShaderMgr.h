#pragma once


class CShader;
class CShaderMgr
{
private:
	CShaderMgr();
	virtual ~CShaderMgr();
public:
	DECLARE_SINGLETON(CShaderMgr)
private:
	map<wstring, CShader*>		m_mapShader;
public:
	HRESULT	AddShaderFiles(
		wstring wstrShaderTag,
		wstring wstrFilePath, 
		LPCSTR wstrShaderName, 
		LPCSTR wstrShaderVersion, 
		ShaderType _eType);
	CShader* Clone_Shader(wstring wstrShaderTag);
private:
	CShader* Find_Shader(wstring wstrShaderTag);
private:
	void Release(void);
};

