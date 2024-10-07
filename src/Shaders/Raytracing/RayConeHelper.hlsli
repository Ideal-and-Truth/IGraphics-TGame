#ifndef RAYCONEHELPER_H
#define RAYCONEHELPER_H

// RayCone 구조체
struct RayCone
{
    float width;           // 레이의 확산 범위
    float spreadAngle;     // 레이의 확산 각도
};

// propagate 함수 - RayCone 확장
RayCone propagate(RayCone cone, float surfaceSpreadAngle, float hitT)
{
    RayCone newCone;
    newCone.width = cone.spreadAngle * hitT + cone.width;  // wi = wi-1 + γi * ti
    newCone.spreadAngle = cone.spreadAngle + surfaceSpreadAngle;  // γi = γi-1 + βi-1
    return newCone;
}

// 표면 스프레드 각도(β) 계산 (식 32 기반)
float calculateSurfaceSpreadAngle(float3 normal, float3 position, float3x3 ObjectToWorldMatrix)
{
    // ddx, ddy를 사용하여 법선의 변화량 계산 (HLSL에서 제공하는 미분 함수)
    float3 dn_dx = ddx(normal);
    float3 dn_dy = ddy(normal);

    float phi = sqrt(dot(dn_dx, dn_dx) + dot(dn_dy, dn_dy));

    float k1 = 1.0;  // 사용자가 정의하는 상수
    float k2 = 0.0;  // 사용자가 정의하는 상수

    // s값 계산 (표면이 볼록한지 오목한지 결정)
    float3 dp_dx = ddx(position);
    float3 dp_dy = ddy(position);
    float s = sign(dot(dp_dx, dn_dx) + dot(dp_dy, dn_dy));

    // 최종 β 값 계산 (식 32)
    float beta = 2 * k1 * s * phi + k2;

    return beta;
}

// 픽셀 스프레드 각도 계산 (식 30 기반)
float pixelSpreadAngle(float verticalFOV, float imageHeight)
{
    float tanHalfFOV = tan(verticalFOV * 0.5);
    return atan(2 * tanHalfFOV / imageHeight);
}

// 텍스처 좌표의 넓이 계산 (식 4 기반)
float computeTexelArea(float2 t0, float2 t1, float2 t2, float textureWidth, float textureHeight)
{
    // ta = wh * |(t1x - t0x)(t2y - t0y) - (t2x - t0x)(t1y - t0y)|
    return textureWidth * textureHeight * abs((t1.x - t0.x) * (t2.y - t0.y) - (t2.x - t0.x) * (t1.y - t0.y));
}

// 화면 공간의 삼각형 넓이 계산 (식 5 기반)
float computeScreenSpaceArea(float2 p0, float2 p1, float2 p2)
{
    // pa = |(p1x - p0x)(p2y - p0y) - (p2x - p0x)(p1y - p0y)|
    return abs((p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y));
}


// 삼각형 LOD 상수 계산 (식 3 기반)
float getTriangleLODConstant(float texelArea, float triangleArea)
{
    // Δ = 0.5 * log2(ta / pa)
    return 0.5 * log2(texelArea / triangleArea);
}


// 텍스처 LOD 계산
float computeTextureLOD(float3 direction, float3 normal, RayCone cone, float textureWidth, float textureHeight)
{
    // 삼각형 LOD 상수 계산
    float triangleLODConstant = getTriangleLODConstant(
        computeTexelArea(normal.xy, normal.xy, normal.xy, textureWidth, textureHeight),
        computeScreenSpaceArea(normal.xy, normal.xy, normal.xy)
    );

    float lambda = triangleLODConstant;
    lambda += log2(abs(cone.width));  // log2(|wi|)
    lambda += 0.5 * log2(textureWidth * textureHeight);
    lambda -= log2(abs(dot(direction, normal))); 
    return lambda;
}
float3x3 ExtractRotationMatrix(float3x4 objectToWorld)
{
    // 상위 3x3 부분을 추출하여 회전 행렬로 반환
    return float3x3(objectToWorld[0].xyz, objectToWorld[1].xyz, objectToWorld[2].xyz);
}

#endif