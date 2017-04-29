#pragma once
#include "stdafx.h"

const UINT WINCX = 1024;
const UINT WINCY = 768;

const UINT	BONE_NUM = 8;
const UINT	BONE_MATRIX_NUM = 128;

const UINT VERTEXCOUNTX = 513;
const UINT VERTEXCOUNTZ = 513;
const UINT VERTEXINTERVAL = 1;

const D3DXVECTOR3 g_vLook(0.f, 0.f, 1.f);

// Texture formats
static const DXGI_FORMAT depthStencilTextureFormat = DXGI_FORMAT_R24G8_TYPELESS;
static const DXGI_FORMAT basicColorTextureFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
static const DXGI_FORMAT normalTextureFormat = DXGI_FORMAT_R11G11B10_FLOAT;
static const DXGI_FORMAT specPowTextureFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

// Render view formats
static const DXGI_FORMAT depthStencilRenderViewFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
static const DXGI_FORMAT basicColorRenderViewFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
static const DXGI_FORMAT normalRenderViewFormat = DXGI_FORMAT_R11G11B10_FLOAT;
static const DXGI_FORMAT specPowRenderViewFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

// Resource view formats
static const DXGI_FORMAT depthStencilResourceViewFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
static const DXGI_FORMAT basicColorResourceViewFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
static const DXGI_FORMAT normalResourceViewFormat = DXGI_FORMAT_R11G11B10_FLOAT;
static const DXGI_FORMAT specPowResourceViewFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

#define VS_SLOT_VIEW_MATRIX			0x00
#define VS_SLOT_PROJECTION_MATRIX	0x01
#define VS_SLOT_WORLD_MATRIX		0x02

#define VS_SLOT_BONE_MATRIX			0x03

#define PS_SLOT_TEXTURE				0x00
#define PS_SLOT_SAMPLER_STATE		0x00

