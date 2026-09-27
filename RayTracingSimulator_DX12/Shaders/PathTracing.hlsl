static const float PI = 3.14159265359;
static const float EPS = 0.001;
struct Object { float4 centerRadius; float4 extentsType; float4 rotation; uint4 material; };
struct Material { float4 color; float4 emission; float4 parameters; };
Texture2D<float4> Previous : register(t0);
StructuredBuffer<Object> Objects : register(t1);
StructuredBuffer<Material> Materials : register(t2);
RWTexture2D<float4> Output : register(u0);
RWTexture2D<float4> NoisyColor : register(u1);
RWTexture2D<float4> DiffuseAlbedo : register(u2);
RWTexture2D<float4> SpecularAlbedo : register(u3);
RWTexture2D<float4> NormalRoughness : register(u4);
RWTexture2D<float> ViewDepth : register(u5);
RWTexture2D<float2> MotionVectors : register(u6);
RWTexture2D<float> SpecularHitDistance : register(u7);
cbuffer Constants : register(b0) {
    float4 CameraPosition, CameraForward, CameraRight, CameraUp;
    uint4 Dimensions; // width, height, history frame index, independent RNG sequence
    uint4 Options; // object count, max bounces, spp, mode: GI / direct / split
    float4 Settings; // split position, sky intensity, aspect, tan half FOV
    float4 Jitter;
    row_major float4x4 PreviousViewProjection, ViewProjection;
};
uint PCG(inout uint state) {
    state = state * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}
float Random(inout uint state) { return float(PCG(state) >> 8) * (1.0 / 16777216.0); }
float3 Rotate(float3 v, float4 q) { return v + 2 * cross(q.xyz, cross(q.xyz, v) + q.w * v); }
float3 LocalToWorld(float3 v, float3 n) {
    float3 tangent = normalize(cross(abs(n.y) < 0.99 ? float3(0, 1, 0) : float3(1, 0, 0), n));
    return tangent * v.x + cross(n, tangent) * v.y + n * v.z;
}
struct Hit { float distance; float3 position; float3 normal; uint objectIndex; bool front; };
bool IntersectObject(uint i, float3 origin, float3 direction, float maxDistance, out Hit hit) {
    hit = (Hit)0; Object object = Objects[i];
    float3 offset = origin - object.centerRadius.xyz;
    float t = maxDistance; float3 outward = float3(0, 1, 0); bool valid = false;
    if (object.extentsType.w < 0.5) {
        float b = dot(offset, direction), c = dot(offset, offset) - object.centerRadius.w * object.centerRadius.w;
        float discriminant = b * b - c;
        float root = sqrt(max(0, discriminant)); t = -b - root; if (t <= EPS) t = -b + root;
        valid = discriminant >= 0 && t > EPS && t < maxDistance;
        outward = normalize(offset + direction * t);
    } else {
        float4 inverseRotation = float4(-object.rotation.xyz, object.rotation.w);
        float3 localOrigin = Rotate(offset, inverseRotation), localDirection = Rotate(direction, inverseRotation);
        float3 safeDirection = float3(localDirection.x >= 0 ? 1 : -1, localDirection.y >= 0 ? 1 : -1, localDirection.z >= 0 ? 1 : -1) * max(abs(localDirection), 1e-8);
        float3 a = (-object.extentsType.xyz - localOrigin) / safeDirection;
        float3 b = (object.extentsType.xyz - localOrigin) / safeDirection;
        float3 lo = min(a, b), hi = max(a, b);
        float nearT = max(lo.x, max(lo.y, lo.z)), farT = min(hi.x, min(hi.y, hi.z));
        t = nearT > EPS ? nearT : farT;
        valid = nearT <= farT && farT > EPS && t < maxDistance;
        float3 p = localOrigin + localDirection * t;
        float3 edge = abs(p) / object.extentsType.xyz;
        float3 normal = edge.x > edge.y && edge.x > edge.z ? float3(sign(p.x), 0, 0) : (edge.y > edge.z ? float3(0, sign(p.y), 0) : float3(0, 0, sign(p.z)));
        outward = normalize(Rotate(normal, object.rotation));
    }
    hit.distance = t; hit.position = origin + direction * t; hit.front = dot(direction, outward) < 0;
    hit.normal = hit.front ? outward : -outward; hit.objectIndex = i; return valid;
}
bool Trace(float3 origin, float3 direction, float maxDistance, out Hit closest) {
    closest = (Hit)0; closest.distance = maxDistance; bool found = false;
    [loop] for (uint i = 0; i < Options.x; ++i) {
        Hit candidate;
        if (IntersectObject(i, origin, direction, closest.distance, candidate)) { closest = candidate; found = true; }
    }
    return found;
}
float3 Environment(float3 direction) { return lerp(float3(0.38, 0.43, 0.54), float3(0.72, 0.83, 1.0), saturate(direction.y * 0.5 + 0.5)) * Settings.y; }
float3 Fresnel(float cosine, float3 f0) { return f0 + (1 - f0) * pow(1 - saturate(cosine), 5); }
float GGX(float noH, float alpha) {
    float a2 = alpha * alpha, d = noH * noH * (a2 - 1) + 1;
    return a2 / max(PI * d * d, 1e-10);
}
float G1(float noV, float alpha) { return 2 * noV / max(noV + sqrt(alpha * alpha + (1 - alpha * alpha) * noV * noV), 1e-6); }
float SpecularProbability(Material m) { return lerp(0.35, 1.0, m.parameters.y); }
float3 BRDF(Material m, float3 n, float3 v, float3 l, out float pdf) {
    pdf = 0; float3 result = 0; float noV = saturate(dot(n, v)), noL = saturate(dot(n, l));
    if (noL > 0 && noV > 0 && dot(v + l, v + l) >= 1e-10) {
    float3 h = normalize(v + l); float noH = saturate(dot(n, h)), voH = saturate(dot(v, h));
    float alpha = max(m.parameters.x * m.parameters.x, 0.0025);
    float d = GGX(noH, alpha); float3 f = Fresnel(voH, lerp(0.04.xxx, m.color.rgb, m.parameters.y));
    float probability = SpecularProbability(m);
    pdf = lerp(noL / PI, d * noH / max(4 * voH, 1e-6), probability);
    float3 diffuse = (1 - f) * (1 - m.parameters.y) * m.color.rgb / PI;
    result = diffuse + f * d * G1(noV, alpha) * G1(noL, alpha) / max(4 * noV * noL, 1e-6);
    }
    return result;
}
float3 SampleBRDF(Material m, float3 n, float3 view, inout uint rng) {
    float choice = Random(rng), u = Random(rng), phi = 2 * PI * Random(rng);
    float3 result = 0;
    if (choice < SpecularProbability(m)) {
        float alpha = max(m.parameters.x * m.parameters.x, 0.0025);
        float cosine = sqrt((1 - u) / (1 + (alpha * alpha - 1) * u));
        float sine = sqrt(max(0, 1 - cosine * cosine));
        float3 h = LocalToWorld(float3(sine * cos(phi), sine * sin(phi), cosine), n);
        result = reflect(-view, h);
    } else {
        float radius = sqrt(u);
        result = LocalToWorld(float3(radius * cos(phi), radius * sin(phi), sqrt(1 - u)), n);
    }
    return result;
}
float3 DirectLighting(Hit surface, float3 view, Material material, inout uint rng) {
    float3 result = 0;
    // One sample per emissive object. Sphere lights use solid-angle sampling.
    // Box lights sample a face proportionally to its area (two-sided emission).
    [loop] for (uint i = 0; i < Options.x; ++i) {
        Object light = Objects[i]; Material lightMaterial = Materials[light.material.x];
        if (lightMaterial.emission.w <= 0 || i == surface.objectIndex) continue;
        float3 origin = surface.position + surface.normal * EPS * 2;
        float3 l; float pdf; float distance;
        if (light.extentsType.w < 0.5) {
            float3 delta = light.centerRadius.xyz - origin; float d2 = dot(delta, delta);
            if (d2 <= light.centerRadius.w * light.centerRadius.w) continue;
            float cosMax = sqrt(max(0, 1 - light.centerRadius.w * light.centerRadius.w / d2));
            float cosTheta = lerp(1, cosMax, Random(rng)), phi = 2 * PI * Random(rng);
            float sinTheta = sqrt(max(0, 1 - cosTheta * cosTheta));
            l = LocalToWorld(float3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta), normalize(delta));
            pdf = 1 / max(2 * PI * (1 - cosMax), 1e-7); distance = 1e20;
        } else {
            float3 e = light.extentsType.xyz; float3 areas = float3(e.y * e.z, e.x * e.z, e.x * e.y);
            float sumArea = areas.x + areas.y + areas.z; float face = Random(rng) * sumArea;
            float side = Random(rng) < 0.5 ? -1 : 1;
            float2 uv = float2(Random(rng), Random(rng)) * 2 - 1; float3 p, n;
            if (face < areas.x) { p = float3(side * e.x, uv.x * e.y, uv.y * e.z); n = float3(side, 0, 0); }
            else if (face < areas.x + areas.y) { p = float3(uv.x * e.x, side * e.y, uv.y * e.z); n = float3(0, side, 0); }
            else { p = float3(uv.x * e.x, uv.y * e.y, side * e.z); n = float3(0, 0, side); }
            float3 delta = light.centerRadius.xyz + Rotate(p, light.rotation) - origin;
            distance = length(delta); l = delta / max(distance, EPS);
            pdf = distance * distance / max(abs(dot(Rotate(n, light.rotation), -l)) * 8 * sumArea, 1e-7);
        }
        float noL = saturate(dot(surface.normal, l)); if (noL <= 0) continue;
        Hit blocker; if (!Trace(origin, l, distance + EPS * 4, blocker) || blocker.objectIndex != i) continue;
        // Reject back faces sampled through the same box: they have a different area PDF.
        if (light.extentsType.w > 0.5 && abs(blocker.distance - distance) > EPS * 8) continue;
        float brdfPdf; result += BRDF(material, surface.normal, view, l, brdfPdf) * lightMaterial.emission.rgb * lightMaterial.emission.w * noL / pdf;
    }
    return result;
}
float3 Integrate(float3 origin, float3 direction, bool directOnly, inout uint rng) {
    float3 radiance = 0, throughput = 1; bool previousDelta = true;
    uint bounces = directOnly ? 1 : Options.y;
    [loop] for (uint bounce = 0; bounce < bounces; ++bounce) {
        Hit hit;
        if (!Trace(origin, direction, 1e20, hit)) { radiance += throughput * Environment(direction); break; }
        Material m = Materials[Objects[hit.objectIndex].material.x];
        if (m.emission.w > 0) {
            // NEE accounts for emitter hits from non-delta opaque bounces; do not count twice.
            if (bounce == 0 || previousDelta) radiance += throughput * m.emission.rgb * m.emission.w;
            break;
        }
        if (directOnly) {
            radiance += throughput * DirectLighting(hit, -direction, m, rng) * (1 - m.parameters.w);
            // Deliberately simple single-surface baseline, including ambient and environment specular.
            radiance += throughput * m.color.rgb * Settings.y * 0.12 * (1 - m.parameters.y) * (1 - m.parameters.w);
            radiance += throughput * Environment(reflect(direction, hit.normal)) * lerp(0.04.xxx, m.color.rgb, m.parameters.y) * (1 - m.parameters.x) * (1 - m.parameters.w);
            if (m.parameters.w > 0) radiance += throughput * Environment(direction) * m.color.rgb * m.parameters.w;
            break;
        }
        bool dielectric = Random(rng) < m.parameters.w;
        float3 next;
        if (dielectric) {
            float ior = max(1.001, m.parameters.z); float eta = hit.front ? 1 / ior : ior;
            float cosine = saturate(dot(-direction, hit.normal));
            float r0 = (1 - ior) / (1 + ior); r0 *= r0;
            float reflection = r0 + (1 - r0) * pow(1 - cosine, 5);
            float sin2T = eta * eta * (1 - cosine * cosine);
            if (sin2T >= 1 || Random(rng) < reflection) next = reflect(direction, hit.normal);
            else { next = refract(direction, hit.normal, eta); throughput *= m.color.rgb * eta * eta; }
            previousDelta = true;
        } else {
            radiance += throughput * DirectLighting(hit, -direction, m, rng);
            next = SampleBRDF(m, hit.normal, -direction, rng);
            float pdf; float3 value = BRDF(m, hit.normal, -direction, next, pdf);
            float cosine = saturate(dot(hit.normal, next)); if (pdf <= 1e-8 || cosine <= 0) break;
            throughput *= value * cosine / pdf; previousDelta = false;
        }
        direction = normalize(next);
        origin = hit.position + hit.normal * (dot(direction, hit.normal) > 0 ? EPS * 2 : -EPS * 2);
        if (bounce >= 3) {
            float survival = clamp(max(throughput.r, max(throughput.g, throughput.b)), 0.05, 0.95);
            if (Random(rng) > survival) break; throughput /= survival;
        }
    }
    return radiance;
}
[numthreads(8, 8, 1)]
void CSMain(uint3 dispatchID : SV_DispatchThreadID) {
    uint2 pixel = dispatchID.xy; if (any(pixel >= Dimensions.xy)) return;
    uint rng = (pixel.x + pixel.y * Dimensions.x) ^ (Dimensions.w * 2891336453u + 12345u); PCG(rng);
    bool directOnly = Options.w == 1 || (Options.w == 2 && (pixel.x + 0.5) / Dimensions.x < Settings.x);
    float3 current = 0;
    [loop] for (uint sampleIndex = 0; sampleIndex < Options.z; ++sampleIndex) {
        // RR requires coherent primary jitter and uncorrelated path-space noise.
        float2 offset = Jitter.z > 0 ? 0.5 + Jitter.xy : float2(Random(rng), Random(rng));
        float2 uv = (float2(pixel) + offset) / float2(Dimensions.xy);
        float2 screen = (uv * 2 - 1) * float2(Settings.z, -1) * Settings.w;
        float3 direction = normalize(CameraForward.xyz + screen.x * CameraRight.xyz + screen.y * CameraUp.xyz);
        current += Integrate(CameraPosition.xyz, direction, directOnly, rng);
    }
    current /= Options.z;
    NoisyColor[pixel] = float4(current, 1);
    float2 guideUV = (float2(pixel) + 0.5 + Jitter.xy) / float2(Dimensions.xy);
    float2 guideScreen = (guideUV * 2 - 1) * float2(Settings.z, -1) * Settings.w;
    float3 guideDirection = normalize(CameraForward.xyz + guideScreen.x * CameraRight.xyz + guideScreen.y * CameraUp.xyz);
    Hit primary; float3 worldPosition = CameraPosition.xyz + guideDirection * 10000;
    DiffuseAlbedo[pixel] = 0; SpecularAlbedo[pixel] = 0;
    NormalRoughness[pixel] = float4(0, 0, 0, 1); ViewDepth[pixel] = 65504;
    SpecularHitDistance[pixel] = 65504;
    if (Trace(CameraPosition.xyz, guideDirection, 1e20, primary)) {
        worldPosition = primary.position;
        Material m = Materials[Objects[primary.objectIndex].material.x];
        float3 f0 = lerp(0.04.xxx, m.color.rgb, m.parameters.y);
        float nv = saturate(dot(primary.normal, -guideDirection));
        // Integrated environment BRDF approximation for view-dependent specular reflectance.
        float4 r = m.parameters.x * float4(-1, -0.0275, -0.572, 0.022) + float4(1, 0.0425, 1.04, -0.04);
        float a004 = min(r.x * r.x, exp2(-9.28 * nv)) * r.x + r.y;
        float2 ab = float2(-1.04, 1.04) * a004 + r.zw;
        float3 specular = saturate(f0 * ab.x + ab.y);
        DiffuseAlbedo[pixel] = float4(m.color.rgb * (1 - m.parameters.y) * (1 - m.parameters.w), 1);
        SpecularAlbedo[pixel] = float4(specular, 1);
        NormalRoughness[pixel] = float4(primary.normal, m.parameters.w > 0.5 ? 0 : m.parameters.x);
        ViewDepth[pixel] = dot(worldPosition - CameraPosition.xyz, CameraForward.xyz);
        Hit reflected;
        if (Trace(primary.position + primary.normal * EPS * 2, reflect(guideDirection, primary.normal), 1e20, reflected)) SpecularHitDistance[pixel] = reflected.distance;
    }
    float4 previousClip = mul(float4(worldPosition, 1), PreviousViewProjection);
    float4 currentClip = mul(float4(worldPosition, 1), ViewProjection);
    float2 motion = 0;
    if (previousClip.w > 0.001 && currentClip.w > 0.001)
        motion = (previousClip.xy / previousClip.w - currentClip.xy / currentClip.w) * float2(0.5, -0.5) * Dimensions.xy;
    MotionVectors[pixel] = motion;
    // Do not read uninitialized/stale history at frame zero; multiplying NaN by zero is insufficient.
    float3 accumulated = current;
    if (Dimensions.z > 0) accumulated = lerp(current, Previous.Load(int3(pixel, 0)).rgb, Dimensions.z / (Dimensions.z + 1.0f));
    Output[pixel] = float4(accumulated, 1);
}

