#pragma once

enum ANGLE {ANGLE_X, ANGLE_Y, ANGLE_Z, ANGLE_END};

enum RENDERGROUP { TYPE_PRIORITY, TYPE_NONEALPHA, TYPE_ALPHA, TYPE_UI, TYPE_END };

enum RESOURCETYPE { RESOURCE_LOGO, RESOURCE_STAGE, RESOURCE_STATIC, RESOURCE_END };

enum BUFFERTYPE { BUFFER_RCCOL,BUFFER_RCTEX, BUFFER_TERRAIN, BUFFER_CUBETEX, BUFFER_CUBECOL };

enum TEXTURETYPE { TEXTURE_NORMAL, TEXTURE_CUBE };

enum ShaderType {SHADER_VS, SHADER_PS, SHADER_ANI, SHADER_END};

enum ReleaseType {Release_Static, Release_Dynamic, Release_End};

enum Scene_Tyep {SCENE_LOGO, SCENE_STAGE, SCENE_END};

enum DrawType { DRAW_INDEX, DRAW_VERTEX };

enum MESHTYPE{ MESH_STATIC, MESH_DYNAMIC };