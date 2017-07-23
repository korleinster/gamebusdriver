#pragma once
#include "Include.h"
#include "CascadedMatrixSet.h"

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
	void DoLighting(ID3D11DeviceContext* pd3dImmediateContext, CMultiRenderTarget* pGBuffer, CMultiRenderTarget* pLineMRT);
	void DoDebugLightVolume(ID3D11DeviceContext* pd3dImmediateContext);

	// Set the ambient values
	void SetAmbient(const D3DXVECTOR3& vAmbientLowerColor, const D3DXVECTOR3& vAmbientUpperColor);

	// Set the directional light values
	void SetDirectional(const D3DXVECTOR3& vDirectionalDir, const D3DXVECTOR3& vDirectionalColor);

	// Clear the lights from the previous frame
	void ClearLights() { m_arrLights.clear(); }

	// Add a single point light
	void AddPointLight(const D3DXVECTOR3& vPointPosition, float fPointRange, const D3DXVECTOR3& vPointColor);
	void AddSpotLight(const D3DXVECTOR3& vSpotPosition, const D3DXVECTOR3& vSpotDirection, float fSpotRange, float fSpotOuterAngle, float fSpotInnerAngle, const D3DXVECTOR3& vSpotColor);
	void AddCapsuleLight(const D3DXVECTOR3& vCapsulePosition, const D3DXVECTOR3& vCapsuleDirection, float fCapsuleRange, float fCapsuleLength, const D3DXVECTOR3& vCapsuleColor);

	void PrepareNextShadowLight(ID3D11DeviceContext* pd3dImmediateContext);

private:
	// Do the directional light calculation
	void DirectionalLight(ID3D11DeviceContext* pd3dImmediateContext);

	// Based on the value of bWireframe, either do the lighting or render the volume
	void PointLight(ID3D11DeviceContext* pd3dImmediateContext, const D3DXVECTOR3& vPos, float fRange, const D3DXVECTOR3& vColor);

	// Based on the value of bWireframe, either do the lighting or render the volume
	void SpotLight(ID3D11DeviceContext* pd3dImmediateContext, const D3DXVECTOR3& vPos, const D3DXVECTOR3& vDir, float fRange, float fInnerAngle, float fOuterAngle, const D3DXVECTOR3& vColor);

	// Based on the value of bWireframe, either do the lighting or render the volume
	void CapsuleLight(ID3D11DeviceContext* pd3dImmediateContext, const D3DXVECTOR3& vPos, const D3DXVECTOR3& vDir, float fRange, float fLen, const D3DXVECTOR3& vColor);


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

	// Spot light
	CShader*			m_pSpotLightVertexShader;
	CShader*			m_pSpotLightHullShader;
	CShader*			m_pSpotLightDomainShader;
	CShader*			m_pSpotLightPixelShader;
	ID3D11Buffer*		m_pSpotLightDomainCB;
	ID3D11Buffer*		m_pSpotLightPixelCB;

	// Capsule light
	CShader*			m_pCapsuleLightVertexShader;
	CShader*			m_pCapsuleLightHullShader;
	CShader*			m_pCapsuleLightDomainShader;
	CShader*			m_pCapsuleLightPixelShader;
	ID3D11Buffer*		m_pCapsuleLightDomainCB;
	ID3D11Buffer*		m_pCapsuleLightPixelCB;

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

	int	m_iShadowMapSize = 1024;

	// Cascaded shadow maps
	ID3D11Texture2D* m_pCascadedDepthStencilRT;
	ID3D11DepthStencilView* m_pCascadedDepthStencilDSV;
	ID3D11ShaderResourceView* m_pCascadedDepthStencilSRV;

	// Cascaded shadow maps matrices
	CCascadedMatrixSet m_CascadedMatrixSet;

	//ForShadowmap
	ID3D11RasterizerState* m_pCascadedShadowGenRS;

	// Near plane distance for shadow map generation
	const float m_fShadowNear;

	// Cascaded shadow maps generation assets
	CShader* m_pCascadedShadowGenVertexShader;
	CShader* m_pCascadedShadowGenGeometryShader;
	ID3D11Buffer* m_pCascadedShadowGenGeometryCB;

	ID3D11SamplerState* m_pPCFSamplerState;
	ID3D11DepthStencilState* m_pShadowGenDepthState;
};

