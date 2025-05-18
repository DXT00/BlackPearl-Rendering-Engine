
// Constants
#define PI 3.141592653589793
#define INV_PI 0.3183098861837907
#define EPSILON 1e-6

// Material properties structure
struct DisneyMaterial {
    vec3 baseColor;
    float subsurface;
    vec3 subsurfaceRadius;
    vec3 subsurfaceColor;
    float metallic;
    float specular;
    float specularTint;
    float roughness;
    float anisotropic;
    float anisotropicRotation;
    float sheen;
    float sheenTint;
    float clearcoat;
    float clearcoatRoughness;
    float ior;
    float transmission;
    float transmissionRoughness;
    vec3 emission;
    float alpha;
};

// Geometry structure
struct SurfaceGeometry {
    vec3 position;
    vec3 normal;
    vec3 viewDir;
    vec3 tangent;
    vec3 bitangent;
};

// Helper functions
float sqr(float x) { return x * x; }
float SchlickFresnel(float u) { return pow(clamp(1.0 - u, 0.0, 1.0), 5.0); }

float GTR1(float NdotH, float a) {
    if (a >= 1.0) return INV_PI;
    float a2 = a * a;
    float t = 1.0 + (a2 - 1.0) * NdotH * NdotH;
    return (a2 - 1.0) / (PI * log(a2) * t);
}

float GTR2(float NdotH, float a) {
    float a2 = a * a;
    float t = 1.0 + (a2 - 1.0) * NdotH * NdotH;
    return a2 / (PI * t * t);
}

float GTR2_aniso(float NdotH, float HdotX, float HdotY, float ax, float ay) {
    return 1.0 / (PI * ax * ay * sqr(sqr(HdotX / ax) + sqr(HdotY / ay) + NdotH * NdotH));
}

float smithG_GGX(float NdotV, float alphaG) {
    float a = alphaG * alphaG;
    float b = NdotV * NdotV;
    return 1.0 / (NdotV + sqrt(a + b - a * b));
}

float smithG_GGX_aniso(float NdotV, float VdotX, float VdotY, float ax, float ay) {
    return 1.0 / (NdotV + sqrt(sqr(VdotX * ax) + sqr(VdotY * ay) + sqr(NdotV)));
}

// Disney Diffuse BRDF
vec3 DisneyDiffuse(DisneyMaterial mat, float NdotV, float NdotL, float LdotH) {
    float FL = SchlickFresnel(NdotL);
    float FV = SchlickFresnel(NdotV);
    
    float Fd90 = 0.5 + 2.0 * LdotH * LdotH * mat.roughness;
    float Fd = mix(1.0, Fd90, FL) * mix(1.0, Fd90, FV);
    
    return mat.baseColor * INV_PI * Fd;
}

// Disney Subsurface BRDF (approximation)
vec3 DisneySubsurface(DisneyMaterial mat, float NdotV, float NdotL, float LdotH) {
    float FL = SchlickFresnel(NdotL);
    float FV = SchlickFresnel(NdotV);
    
    float Fss90 = LdotH * LdotH * mat.roughness;
    float Fss = mix(1.0, Fss90, FL) * mix(1.0, Fss90, FV);
    float ss  = 1.25 * (Fss * (1.0 / (NdotL + NdotV) - 0.5) + 0.5);

    
    return mat.subsurfaceColor * INV_PI * ss;
}

// Disney Sheen BRDF
vec3 DisneySheen(DisneyMaterial mat, float NdotV, float NdotL, float LdotH) {
    float FH = SchlickFresnel(LdotH);
    vec3 sheenColor = mix(vec3(1.0), mat.baseColor, mat.sheenTint);
    return sheenColor * FH * pow(1.0 - LdotH, 2.0) * 0.1;
}

// Disney Specular BRDF
vec3 DisneySpecular(DisneyMaterial mat, SurfaceGeometry geom,
                    float NdotV, float NdotL, float NdotH, float LdotH, 
                    vec3 H) {
    vec3 Cspec0 = mix(mat.specular * 0.08 * mix(vec3(1.0), mat.baseColor, mat.specularTint), 
                     mat.baseColor, mat.metallic);
    
    float aspect = sqrt(1.0 - mat.anisotropic * 0.9);
    float ax = max(0.001, sqr(mat.roughness) / aspect);
    float ay = max(0.001, sqr(mat.roughness) * aspect);
    
    // Rotate the anisotropy direction
    float cosPhi = cos(mat.anisotropicRotation * 2.0 * PI);
    float sinPhi = sin(mat.anisotropicRotation * 2.0 * PI);
    vec3 Hrot = vec3(H.x * cosPhi - H.y * sinPhi, H.x * sinPhi + H.y * cosPhi, H.z);
    float HdotX = dot(Hrot, geom.tangent);
    float HdotY = dot(Hrot, geom.bitangent);
    
    float D = GTR2_aniso(NdotH, HdotX, HdotY, ax, ay);
    float G = smithG_GGX_aniso(NdotV, dot(geom.tangent, geom.viewDir), 
                              dot(geom.bitangent, geom.viewDir), ax, ay) * 
              smithG_GGX_aniso(NdotL, dot(geom.tangent, -geom.viewDir), 
                              dot(geom.bitangent, -geom.viewDir), ax, ay);
    
    vec3 F = Cspec0 + (1.0 - Cspec0) * SchlickFresnel(LdotH);
    
    return F * D * G;
}

// Disney Clearcoat BRDF
float DisneyClearcoat(DisneyMaterial mat, float NdotH, float NdotV, float NdotL, float LdotH) {
    float alpha = mix(0.1, 0.001, mat.clearcoatRoughness);
    float D = GTR1(NdotH, alpha);
    
    // Clearcoat is assumed to have IOR of 1.5
    float F0 = 0.04; // (1.5-1)/(1.5+1)^2
    float F = F0 + (1.0 - F0) * SchlickFresnel(LdotH);
    
    float G = smithG_GGX(NdotV, 0.25) * smithG_GGX(NdotL, 0.25);
    
    return mat.clearcoat * D * F * G;
}

// Disney BSDF Main Function
vec3 evaluateDisneyBRDF(DisneyMaterial mat, SurfaceGeometry geom, vec3 lightDir) {
    vec3 L = normalize(lightDir);
    vec3 V = geom.viewDir;
    vec3 N = geom.normal;
    vec3 H = normalize(L + V);
    
    float NdotL = dot(N, L);
    float NdotV = dot(N, V);
    float NdotH = dot(N, H);
    float LdotH = dot(L, H);
    
    // Early out if light is behind surface and no transmission
    if (NdotL <= 0.0 && mat.transmission <= 0.0) return vec3(0.0);
    
    vec3 diffuse = vec3(0.0);
    vec3 subsurfaceScatter = vec3(0.0);
    vec3 spec = vec3(0.0);
    float coat = 0.0;
    vec3 sheenColor = vec3(0.0);
    
    // Diffuse and subsurface
    if (mat.metallic < 1.0) {
        diffuse = DisneyDiffuse(mat, NdotV, abs(NdotL), LdotH);
        subsurfaceScatter = DisneySubsurface(mat, NdotV, abs(NdotL), LdotH);
    }
    
    // Sheen (only for non-metallic)
    if (mat.metallic < 1.0 && mat.sheen > 0.0) {
        sheenColor = DisneySheen(mat, NdotV, abs(NdotL), LdotH);
    }
    
    // Specular
    spec = DisneySpecular(mat, geom, NdotV, abs(NdotL), NdotH, LdotH, H);
    
    // Clearcoat
    if (mat.clearcoat > 0.0) {
        coat = DisneyClearcoat(mat, NdotH, NdotV, abs(NdotL), LdotH);
    }
    
    // Transmission (simplified)
    vec3 trans = vec3(0.0);
    if (mat.transmission > 0.0 && NdotL < 0.0) {
        // Simplified transmission - for full implementation need to handle refraction properly
        float F = SchlickFresnel(abs(NdotL));
        trans = mat.baseColor * (1.0 - F) * mat.transmission;
    }
    
    // Blend between diffuse and subsurface
    vec3 diffSub = mix(diffuse, subsurfaceScatter, mat.subsurface);
    
    // Combine all terms
    vec3 result = diffSub * (1.0 - mat.metallic) + spec + 
                 sheenColor * mat.sheen + coat * vec3(0.25) + trans;
    
    return result * abs(NdotL);
}


//// Example usage in a fragment shader
//uniform DisneyMaterial uMaterial;
//uniform vec3 uLightDirection;
//uniform vec3 uLightColor;
//
//in vec3 vPosition;
//in vec3 vNormal;
//in vec3 vTangent;
//in vec2 vTexCoord;
//
//out vec4 fragColor;
//
//void main() {
//    // Setup geometry
//    SurfaceGeometry geom;
//    geom.position = vPosition;
//    geom.normal = normalize(vNormal);
//    geom.viewDir = normalize(-vPosition); // Assuming eye is at (0,0,0)
//    geom.tangent = normalize(vTangent);
//    geom.bitangent = normalize(cross(geom.normal, geom.tangent));
//    
//    // Evaluate BRDF
//    vec3 brdf = evaluateDisneyBRDF(uMaterial, geom, uLightDirection);
//    
//    // Combine with light color
//    vec3 color = brdf * uLightColor;
//    
//    // Add emission
//    color += uMaterial.emission;
//    
//    // Output final color with alpha
//    fragColor = vec4(color, uMaterial.alpha);
//}