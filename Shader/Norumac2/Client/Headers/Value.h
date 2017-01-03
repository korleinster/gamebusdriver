#pragma once
#include "stdafx.h"

const UINT WINCX = 800;
const UINT WINCY = 600;

const UINT VERTEXCOUNTX = 129;
const UINT VERTEXCOUNTZ = 129;
const UINT VERTEXINTERVAL = 1;

const D3DXVECTOR3 g_vLook(0.f, 0.f, 1.f);

#define VS_SLOT_VIEW_MATRIX			0x00
#define VS_SLOT_PROJECTION_MATRIX	0x01
#define VS_SLOT_WORLD_MATRIX		0x02

#define VS_SLOT_BONE_MATRIX			0x03

#define PS_SLOT_TEXTURE				0x00
#define PS_SLOT_SAMPLER_STATE		0x00