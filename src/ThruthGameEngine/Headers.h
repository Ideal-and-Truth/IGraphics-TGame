#pragma once

// extern library
#include "SimpleMath.h"
#include "Types.h"
#include "Logger.h"
#include "Reflection.h"
using namespace DirectX::SimpleMath;

// window
#include <windows.h>

// memory
#include <memory>

// data type
#include <string>

// data structer
#include <map>
#include <unordered_map>
#include <vector>
#include <list>
#include <queue>
#include <set>

// type trait
#include <type_traits>
#include <typeinfo>
#include <any>

// assert
#include <assert.h>

// 함수 관련
#include <functional>

// 매크로 관련
#include "Macro.h"

// 그래픽 관련
#include "ICamera.h"
#include "IdealRendererFactory.h"
#include "IdealRenderer.h"
#include "IMeshObject.h"