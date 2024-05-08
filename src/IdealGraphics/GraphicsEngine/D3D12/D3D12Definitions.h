#pragma once
#include "Core/Core.h"

#define STATIC_MESH_ROOT_DESCRIPTOR_TABLE_PIXEL_SRV_INDEX_DIFFUSE 0
#define STATIC_MESH_ROOT_DESCRIPTOR_TABLE_PIXEL_SRV_INDEX_SPECULAR 1
#define STATIC_MESH_ROOT_DESCRIPTOR_TABLE_PIXEL_SRV_INDEX_NORMAL 2
#define STATIC_MESH_ROOT_CONSTANT_INDEX_MATERIAL 3
#define STATIC_MESH_ROOT_CONSTANT_INDEX_TRANSFORM 4

#define DYNAMIC_MESH_ROOT_DESCRIPTOR_TABLE_PIXEL_DIFFUSE_INDEX 0
#define DYNAMIC_MESH_ROOT_DESCRIPTOR_TABLE_PIXEL_SPECULAR_INDEX 1
#define DYNAMIC_MESH_ROOT_DESCRIPTOR_TABLE_PIXEL_NORMAL_INDEX 2
#define DYNAMIC_MESH_ROOT_CONSTANT_INDEX_MATERIAL 3
#define DYNAMIC_MESH_ROOT_CONSTANT_INDEX_TRANSFORM 4
#define DYNAMIC_MESH_ROOT_CONSTANT_INDEX_BONE 5

#define STATIC_MESH_ROOT_DESCRIPTOR_TABLE_CBV_INDEX 0


/// Static Mesh Macro
// 2024.05.08
// Root Table Index Macro
#define STATIC_MESH_DESCRIPTOR_TABLE_INDEX_OBJ 0		// 공용 root parameter
#define	STATIC_MESH_DESCRIPTOR_INDEX_CBV_TRANSFORM 0	// b0 : transform

#define STATIC_MESH_DESCRIPTOR_TABLE_INDEX_MESH 1		// 개별 매쉬 root parameter
#define	STATIC_MESH_DESCRIPTOR_INDEX_CBV_MATERIAL 0		// b1 : material
#define	STATIC_MESH_DESCRIPTOR_INDEX_SRV_DIFFUSE 1		// t0 : diffuse
#define	STATIC_MESH_DESCRIPTOR_INDEX_SRV_SPECULAR 2		// t1 : specular
#define	STATIC_MESH_DESCRIPTOR_INDEX_SRV_NORMAL 3		// t2 : normal

/// Skinned Mesh Macro
// 2024.05.08
// Root Table Index Macro
#define SKINNED_MESH_DESCRIPTOR_TABLE_INDEX_OBJ 0
#define SKINNED_MESH_DESCRIPTOR_INDEX_CBV_TRANSFORM 0	// b0 : transform
#define SKINNED_MESH_DESCRIPTOR_INDEX_CBV_BONE 1		// b1 : bone

#define SKINNED_MESH_DESCRIPTOR_TABLE_INDEX_MESH 1
#define SKINNED_MESH_DESCRIPTOR_INDEX_CBV_MATERIAL 0	// b2 : material
#define SKINNED_MESH_DESCRIPTOR_INDEX_SRV_DIFFUSE 1		// t0 : diffuse
#define SKINNED_MESH_DESCRIPTOR_INDEX_SRV_SPECULAR 2	// t1 : specular
#define SKINNED_MESH_DESCRIPTOR_INDEX_SRV_NORMAL 3		// t2 : normal
