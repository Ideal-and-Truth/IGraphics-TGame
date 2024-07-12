#ifndef RAYTRACINGSHADERHELPER_H
#define RAYTRACINGSHADERHELPER_H

namespace BxDF
{
    bool IsBlack(float3 color)
    {
        return !any(color);
    }
    namespace Diffuse
    {
        float3 F(in float3 Albedo, in float Roughness, in float3 N, in float3 V, in float3 L, in float3 Fo)
        {
            float3 diffuse = 0;

            float3 H = normalize(V + L);
            float NoH = dot(N, H);
            if (NoH > 0)
            {
                float a = Roughness * Roughness;

                float NoV = saturate(dot(N, V));
                float NoL = saturate(dot(N, L));
                float LoV = saturate(dot(L, V));

                float facing = 0.5 + 0.5 * LoV;
                float rough = facing * (0.9 - 0.4 * facing) * ((0.5 + NoH) / NoH);
                float3 smooth = 1.05 * (1 - pow(1 - NoL, 5)) * (1 - pow(1 - NoV, 5));

                    // Extract 1 / PI from the single equation since it's ommited in the reflectance function.
                float3 single = lerp(smooth, rough, a);
                float multi = 0.3641 * a; // 0.3641 = PI * 0.1159

                diffuse = Albedo * (single + Albedo * multi);
                

            }
            return diffuse;
        }
    }
    
    // Fresnel reflectance - schlick approximation.
    float3 Fresnel(in float3 F0, in float cos_thetai)
    {
        return F0 + (1 - F0) * pow(1 - cos_thetai, 5);
    }
    
    namespace Specular
    {
        namespace Reflection
        {
            // Calculates L and returns BRDF value for that direction.
            // Assumptions: V and N are in the same hemisphere.
            // Note: to avoid unnecessary precision issues and for the sake of performance the function doesn't divide by the cos term
            // so as to nullify the cos term in the rendering equation. Therefore the caller should skip the cos term in the rendering equation.
            float3 Sample_Fr(in float3 V, out float3 L, in float3 N, in float3 Fo)
            {
                L = reflect(-V, N);
                float cos_thetai = dot(N, L);
                return Fresnel(Fo, cos_thetai);
            }
            
            // Calculate whether a total reflection occurs at a given V and a normal
            // Ref: eq 27.5, Ray Tracing from the Ground Up
            bool IsTotalInternalReflection(
                in float3 V,
                in float3 normal)
            {
                float ior = 1;
                float eta = ior;
                float cos_thetai = dot(normal, V); // Incident angle

                return 1 - 1 * (1 - cos_thetai * cos_thetai) / (eta * eta) < 0;
            }
        }
        
        namespace GGX
        {
             // Compute the value of BRDF
            float3 F(in float Roughness, in float3 N, in float3 V, in float3 L, in float3 Fo)
            {
                float3 H = V + L;
                float NoL = dot(N, L);
                float NoV = dot(N, V);
                float3 specular = 0;

                if (NoL > 0 && NoV > 0 && all(H))
                {
                    H = normalize(H);
                    float a = Roughness; // The roughness has already been remapped to alpha.
                    float3 M = H; // microfacet normal, equals h, since BRDF is 0 for all m =/= h. Ref: 9.34, RTR
                    float NoM = saturate(dot(N, M));
                    float HoL = saturate(dot(H, L));

                    // D
                    // Ref: eq 9.41, RTR
                    float denom = 1 + NoM * NoM * (a * a - 1);
                    float D = a * a / (denom * denom); // Karis

                    // F
                    // Fresnel reflectance - Schlick approximation for F(h,l)
                    // Ref: 9.16, RTR
                    float3 F = Fresnel(Fo, HoL);

                    // G
                    // Visibility due to shadowing/masking of a microfacet.
                    // G coupled with BRDF's {1 / 4 DotNL * DotNV} factor.
                    // Ref: eq 9.45, RTR
                    float G = 0.5 / lerp(2 * NoL * NoV, NoL + NoV, a);

                    // Specular BRDF term
                    // Ref: eq 9.34, RTR
                    specular = F * G * D;
                }

                return specular;
            }
        }
    }
    
    namespace DirectLighting
    {
        float3 Shade(
        in float3 Albedo,
        in float3 Fo,
        in float3 Radiance,
        in bool isInShadow,
        in float Roughness,
        in float3 N,
        in float3 V,
        in float3 L)
        {
            float3 directLighting = 0;
            
            float NoL = dot(N, L);
            if (!isInShadow && NoL > 0)
            {
                float3 directDiffuse = 0;
                if(!IsBlack(Albedo))
                {
                    directDiffuse = BxDF::Diffuse::F(Albedo, Roughness, N, V, L, Fo);
                }
                
                float3 directSpecular = 0;
                directSpecular = BxDF::Specular::GGX::F(Roughness, N, V, L, Fo);
                directLighting = NoL * Radiance * (directDiffuse + directSpecular);
            }
            return directLighting;
        }
    }
}

// Sample normal map, convert to signed, apply tangent-to-world space transform.
float3 BumpMapNormalToWorldSpaceNormal(float3 bumpNormal, float3 surfaceNormal, float3 tangent)
{
    // Compute tangent frame.
    surfaceNormal = normalize(surfaceNormal);
    tangent = normalize(tangent);

    float3 bitangent = normalize(cross(tangent, surfaceNormal));
    float3x3 tangentSpaceToWorldSpace = float3x3(tangent, bitangent, surfaceNormal);

    //return mul(bumpNormal, tangentSpaceToWorldSpace);
    return normalize(mul(bumpNormal, tangentSpaceToWorldSpace));
}


namespace Ideal
{
    float Attenuate(float distance, float range)
    {
        float att = saturate(1.f - (distance * distance / (range * range)));
        return att * att;
        
        //float numer = distance / range;
        //numer = numer * numer;
        //numer = numer * numer;
        //numer = saturate(1 - numer);
        //numer = numer * numer;
        //float denom = dist * dist + 1;
        //return (numer / denom);
    }
    
    namespace Light
    {
        float3 ComputeDirectionalLight(
        in float3 Albedo,
        in float3 Fo,
        in float3 Radiance,
        in bool isInShadow,
        in float Roughness,
        in float3 N,
        in float3 V,
        in float3 DirectionalLightVector
        )
        {
            float3 directLighting = 0;
            float3 L = -DirectionalLightVector;
            
            float NoL = dot(N, L);
            if (!isInShadow && NoL > 0)
            {
                float3 directDiffuse = 0;
                if (!BxDF::IsBlack(Albedo))
                {
                    directDiffuse = BxDF::Diffuse::F(Albedo, Roughness, N, V, L, Fo);
                }
                
                float3 directSpecular = 0;
                directSpecular = BxDF::Specular::GGX::F(Roughness, N, V, L, Fo);
                directLighting = NoL * Radiance * (directDiffuse + directSpecular);
            }
            return directLighting;
        }
        
        float3 ComputePointLight(
        in float3 Albedo,
        in float3 Fo,
        in float3 Radiance,
        in bool isInShadow,
        in float Roughness,
        in float3 N,
        in float3 V,
        in float3 L,
        in float LightDistance,
        in float LightRange,
        in float LightIntensity
        )
        {
            float3 directLighting = 0;
            
            if (LightDistance > LightRange)
            {
                return directLighting;
            }
            
            float NoL = dot(N, L);
            if (!isInShadow && NoL > 0)
            {
                 // 임의 추가
                float distance = LightDistance;
                float attenuation = Attenuate(distance, LightRange);
                float intensity = LightIntensity * attenuation;
                    
                float3 directDiffuse = 0;
                float3 directSpecular = 0;
                
                if (!BxDF::IsBlack(Albedo))
                {
                    directDiffuse = BxDF::Diffuse::F(Albedo, Roughness, N, V, L, Fo);
                }
                
                directSpecular = BxDF::Specular::GGX::F(Roughness, N, V, L, Fo);
                
                directDiffuse *= LightIntensity;
                directSpecular *= LightIntensity;
                
                directLighting = NoL * Radiance * (directDiffuse + directSpecular);
            }
            return directLighting;
        }
    }
}

#endif