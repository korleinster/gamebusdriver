#pragma once
#include "Include.h"

class CShader;
class CMultiRenderTarget;
class CLightMgr
{
public:
	DECLARE_SINGLETON(CLightMgr)

public:
	CLightMgr();
	virtual ~CLightMgr();

public:
	HRESULT Initialize();
	void Release();
	void DoLighting(ID3D11DeviceContext* pd3dImmediateContext, CMultiRenderTarget* pGBuffer);
	void DoDebugLightVolume(ID3D11DeviceContext* pd3dImmediateContext);

	// Set the ambient values
	void SetAmbient(const D3DXVECTOR3& vAmbientLowerColor, const D3DXVECTOR3& vAmbientUpperColor);

	// Set the directional light values
	void SetDirectional(const D3DXVECTOR3& vDirectionalDir, const D3DXVECTOR3& vDirectionalColor);

	// Clear the lights from the previous frame
	void ClearLights() { m_arrLights.clear(); }

	// Add a single point light
	void AddPointLight(const D3DXVECTOR3& vPointPosition, float fPointRange, const D3DXVECTOR3& vPointColor);

private:
	// Do the directional light calculation
	void DirectionalLight(ID3D11DeviceContext* pd3dImmediateContext);

	// Based on the value of bWireframe, either do the lighting or render the volume
	void PointLight(ID3D11DeviceContext* pd3dImmediateContext, const D3DXVECTOR3& vPos, float fRange, const D3DXVECTOR3& vColor);


private:

	typedef enum
	{
		TYPE_POINT = 0,
		TYPE_SPOT,
		TYPE_CAPSULE
	} LIGHT_TYPE;

	// Light storage
	typedef struct
	{
		LIGHT_TYPE eLightType;
		D3DXVECTOR3 vPosition;
		D3DXVECTOR3 vDirection;
		float fRange;
		float fLength;
		float fOuterAngle;
		float fInnerAngle;
		D3DXVECTOR3 vColor;
	} LIGHT;

	// Directional light
	CShader*			m_pDirLightVertexShader;
	CShader*			m_pDirLightPixelShader;
	ID3D11Buffer*		m_pDirLightCB;

	// Point light
	CShader*			m_pPointLightVertexShader;
	CShader*			m_pPointLightHullShader;
	CShader*			m_pPointLightDomainShader;
	CShader*			m_pPointLightPixelShader;
	ID3D11Buffer*		m_pPointLightDomainCB;
	ID3D11Buffer*		m_pPointLightPixelCB;

	// Light volume debug shader
	//ID3D11PixelShader* m_pDebugLightPixelShader;

	// Depth state with no writes and stencil test on
	ID3D11DepthStencilState* m_pNoDepthWriteLessStencilMaskState;
	ID3D11DepthStencilState* m_pNoDepthWriteGreatherStencilMaskState;

	// Additive blend state to accumulate light influence
	ID3D11BlendState* m_pAdditiveBlendState;

	// Front face culling for lights volume
	ID3D11RasterizerState* m_pNoDepthClipFrontRS;

	// Wireframe render state for light volume debugging
	ID3D11RasterizerState* m_pWireframeRS;

	// Visualize the lights volume
	bool		m_bShowLightVolume;

	// Ambient light information
	D3DXVECTOR3 m_vAmbientLowerColor;
	D3DXVECTOR3 m_vAmbientUpperColor;

	// Directional light information
	D3DXVECTOR3 m_vDirectionalDir;
	D3DXVECTOR3 m_vDirectionalColor;

	// Linked list with the active lights
	vector<LIGHT> m_arrLights;
};

