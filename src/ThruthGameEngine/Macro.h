#pragma once
/// 매크로 함수를 모아둔 헤더

// #define COMPONENT_HEADER \
// public: \
// 	inline static size_t m_typeID = HashString(__FILE__); \
// private:
// 
// template <typename Str>
// constexpr size_t HashString(const Str& toHash)
// {
// 	// For this example, I'm requiring size_t to be 64-bit, but you could
// 	// easily change the offset and prime used to the appropriate ones
// 	// based on sizeof(size_t).
// 	static_assert(sizeof(size_t) == 8);
// 	// FNV-1a 64 bit algorithm
// 	size_t result = 0xcbf29ce484222325; // FNV offset basis
// 
// 	for (char c : toHash) {
// 		result ^= c;
// 		result *= 1099511628211; // FNV prime
// 	}
// 
// 	return result;
// }

// #pragma region OverLoading
// 
// #define WARP(expression) expression
// #define PREFIX(...) 0, ##__VA_ARGS__
// #define LASTOF10(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, ...) _10
// 
// #define GET_ARGS_COUNT_IMPLE(...) WARP(LASTOF10(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1, 0))
// #define GET_ARGS_COUNT(...) GET_ARGS_COUNT_IMPLE(PREFIX(__VA_ARGS__))
// 
// #define _ARGS_COUNT_MACRO(__FUNC__, __NAME__) __FUNC__##__NAME__
// #define ARGS_COUNT_MACRO(__FUNC__, __NAME__) _ARGS_COUNT_MACRO(__FUNC__, __NAME__)
// 
// #pragma endregion OverLoading
// 
// #pragma region EVENT_BIND
// 
// #define EVENT_BIND(__NAME__, __FUNTION__) \
// Managers::Event()->Subscribe(__NAME__, MakeListenerInfo(__FUNTION__))
// 
// #define EVENT_UNBIND(__NAME__, __LISTNER__) \
// Managers::Event()->Unsubscribe(__NAME__, __LISTNER__)
// 
// #define EVENT_UNBIND_ALL(__LISTNER__) \
// Managers::Event()->RemoveListener(__LISTNER__)
// 
// #pragma endregion EVENT_BIND
// 
// #pragma region EVENT_PUBLISH
// 
// #define EVENT(...) ARGS_COUNT_MACRO(EVENT, GET_ARGS_COUNT(__VA_ARGS__))(__VA_ARGS__)
// 
// #define EVENT1(__NAME__) \
// Managers::Event()->PublishEvent(__NAME__)
// 
// #define EVENT2(__NAME__, __PARAM__) \
// Managers::Event()->PublishEvent(__NAME__, __PARAM__)
// 
// #define EVENT3(__NAME__, __PARAM__, __DELAY__) \
// Managers::Event()->PublishEvent(__NAME__, __PARAM__, __DELAY__)
// 
// #pragma endregion EVENT_PUBLISH
