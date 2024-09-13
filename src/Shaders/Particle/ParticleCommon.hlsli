    #ifndef PARTICLE_COMMON_HLSLI
    #define PARTICLE_COMMON_HLSLI

    cbuffer Global : register(b0)
    {
        float4x4 View;
        float4x4 Proj;
        float4x4 ViewProj;
    };

    cbuffer Transform : register(b1)
    {
        float4x4 World;
        float4x4 WorldInvTranspose;
    }
    cbuffer ParticleSystemData : register(b2)
    {
        // Time
        float4 g_CustomData1;
        float4 g_CustomData2;
        float g_Time;
        float3 pad;
    };

    Texture2D ParticleTexture : register(t0);

    SamplerState LinearWrapSampler : register(s0);

    // Noise Texture?

    struct VSInput
    {
        float3 Pos : POSITION;
        float3 Normal : NORMAL;
        float2 UV : TEXCOORD;
    };

    struct VSOutput
    {
        float4 PosH : SV_POSITION;
        float3 Pos : POSITION;
        float3 Normal : NORMAL;
        float2 UV : TEXCOORD;
    };
    //https://docs.unity3d.com/Packages/com.unity.shadergraph@6.9/manual/Simple-Noise-Node.html
    void TilingAndOffset_float(float2 UV, float2 Tiling, float2 Offset, out float2 Out)
    {
        Out = UV * Tiling + Offset;
    }

    //--------------------MATH-------------------//
    // SimpleNoise
    inline float Noise_RandomValue(float2 UV)
    {
        return frac(sin(dot(UV, float2(12.9898, 78.233))) * 43758.5453);
    }
    inline float Noise_Interpolate(float a, float b, float t)
    {
        return (1.0 - t) * a + (t * b);
    }
    inline float ValueNoise(float2 UV)
    {
        float2 i = floor(UV);
        float2 f = frac(UV);
        f = f * f * (3.0 - 2.0 * f);
    
        UV = abs(frac(UV) - 0.5);
        float2 c0 = i + float2(0.0, 0.0);
        float2 c1 = i + float2(1.0, 0.0);
        float2 c2 = i + float2(0.0, 1.0);
        float2 c3 = i + float2(1.0, 1.0);
        float r0 = Noise_RandomValue(c0);
        float r1 = Noise_RandomValue(c1);
        float r2 = Noise_RandomValue(c2);
        float r3 = Noise_RandomValue(c3);
    
        float bottomOfGrid = Noise_Interpolate(r0, r1, f.x);
        float topOfGrid = Noise_Interpolate(r2, r3, f.x);
        float t = Noise_Interpolate(bottomOfGrid, topOfGrid, f.y);
        return t;
    }

    void SimpleNoise_float(float2 UV, float Scale, out float Out)
    {
        float t = 0.0;
        float freq = pow(2.0, float(0));
        float amp = pow(0.5, float(3 - 0));
        t += ValueNoise(float2(UV.x * Scale / freq, UV.y * Scale / freq)) * amp;
    
        freq = pow(2.0, float(1));
        amp = pow(0.5, float(3 - 1));
        t += ValueNoise(float2(UV.x * Scale / freq, UV.y * Scale / freq)) * amp;
    
        freq = pow(2.0, float(2));
        amp = pow(0.5, float(3 - 2));
        t += ValueNoise(float2(UV.x * Scale / freq, UV.y * Scale / freq)) * amp;
    
        Out = t;
    }

    // Round // Step

    void Step_float4(float4 Edge, float4 In, out float4 Out)
    {
        Out = step(Edge, In);
    }

    #endif
