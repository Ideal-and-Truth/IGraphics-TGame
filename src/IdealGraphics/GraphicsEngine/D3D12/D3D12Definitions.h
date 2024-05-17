#pragma once

#define D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN   ((D3D12_GPU_VIRTUAL_ADDRESS)-1)

/// Static Mesh Macro
// 2024.05.08
// Root Table Index Macro
#define STATIC_MESH_DESCRIPTOR_TABLE_INDEX_OBJ 1		// 공용 root parameter
#define	STATIC_MESH_DESCRIPTOR_INDEX_CBV_TRANSFORM 0	// b1 : transform

#define STATIC_MESH_DESCRIPTOR_TABLE_INDEX_MESH 2		// 개별 매쉬 root parameter
#define	STATIC_MESH_DESCRIPTOR_INDEX_CBV_MATERIAL 0		// b2 : material
#define	STATIC_MESH_DESCRIPTOR_INDEX_SRV_DIFFUSE 1		// t0 : diffuse
#define	STATIC_MESH_DESCRIPTOR_INDEX_SRV_SPECULAR 2		// t1 : specular
#define	STATIC_MESH_DESCRIPTOR_INDEX_SRV_NORMAL 3		// t2 : normal

/// Skinned Mesh Macro
// 2024.05.08
// Root Table Index Macro
#define SKINNED_MESH_DESCRIPTOR_TABLE_INDEX_OBJ 1
#define SKINNED_MESH_DESCRIPTOR_INDEX_CBV_TRANSFORM 0	// b1 : transform
#define SKINNED_MESH_DESCRIPTOR_INDEX_CBV_BONE 1		// b2 : bone

#define SKINNED_MESH_DESCRIPTOR_TABLE_INDEX_MESH 2
#define SKINNED_MESH_DESCRIPTOR_INDEX_CBV_MATERIAL 0	// b3 : material
#define SKINNED_MESH_DESCRIPTOR_INDEX_SRV_DIFFUSE 1		// t0 : diffuse
#define SKINNED_MESH_DESCRIPTOR_INDEX_SRV_SPECULAR 2	// t1 : specular
#define SKINNED_MESH_DESCRIPTOR_INDEX_SRV_NORMAL 3		// t2 : normal

/// Global
// 2024.05.13
#define GLOBAL_DESCRIPTOR_TABLE_INDEX 0
#define GLOBAL_DESCRIPTOR_INDEX_CBV_GLOBAL 0 // b0 : global

/// Screen Quad
// 2024.05.15 Temp
#define SCREEN_DESCRIPTOR_TABLE_INDEX 1
#define SCREEN_DESCRIPTOR_INDEX_SRV_NORMAL 0
#define SCREEN_DESCRIPTOR_INDEX_SRV_POSITION 1
#define SCREEN_DESCRIPTOR_INDEX_SRV_DIFFUSE 2