#ifndef TEST_CUSTOM_PARTICLE
#define TEST_CUSTOM_PARTICLE

#include "ParticleCommon.hlsli" // necessary

float4 PSMain(VSOutput input) : SV_TARGET
{  // 기본 색상
     // 기본 색상
    float4 baseColor = ParticleTexture0.Sample(LinearWrapSampler, input.UV);
    
    baseColor.rgb *= g_startColor.rgb;
    baseColor.rgb = baseColor.rgb / (baseColor.rgb + float3(1, 1, 1)); // 톤 매핑 예시 (Reinhard 톤 매핑)

    baseColor.a *= g_startColor.a;

    // 시간에 따른 애니메이션 조정 (Texture Animate)
    float2 animatedUV = input.UV;
    float2 animatedUVTO;
    Ideal_TilingAndOffset_float(input.UV, float2(5, 2), float2(0, 0), animatedUVTO);
    animatedUVTO += g_Time * g_CustomData1.x; // 애니메이션 속도 조정
    baseColor *= ParticleTexture0.Sample(LinearWrapSampler, animatedUVTO); // 텍스처 애니메이션 적용

    // 노멀 왜곡 (Normal Distortion)
    float2 normalUV;
    Ideal_TilingAndOffset_float(input.UV, float2(2, 2), float2(0, 0), normalUV);
    float3 normal = ParticleTexture1.Sample(LinearWrapSampler, normalUV).rgb; // 노멀 맵 사용
    normal = normalize(normal * 2.0 - 1.0); // [-1, 1] 범위로 변환

    // 마스크 페이드 (Mask Fade)
    float2 maskUV;
    Ideal_TilingAndOffset_float(input.UV, float2(1, -1), float2(0, 0), maskUV);
    float maskAlpha = ParticleTexture2.Sample(LinearWrapSampler, maskUV).a; // 마스크 텍스처 사용

    // Y값을 기준으로 하단 반구만 적용
    float isInLowerHemisphere = step(0.0, -input.Pos.y); // Y가 0보다 작으면 1, 아니면 0
    baseColor.a *= maskAlpha * isInLowerHemisphere; // 알파 값 조정

    // 하단 반구에만 적용되도록 추가 조건
    baseColor.rgb *= isInLowerHemisphere; // 색상도 하단 반구에서만 보이도록 조정
    
    // 최종 색상 반환
    return baseColor;
}


#endif
