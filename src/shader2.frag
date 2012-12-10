#version 120
// #version 330
/*
*/

const int MAX_POINT_LIGHTS = 2;
const int MAX_SPOT_LIGHTS = 2;

// Input from the vertex shader
varying vec2 TexCoord0;
varying vec3 Normal0;
varying vec3 WorldPos0;
varying vec3 fN;
varying vec3 fL;
varying vec3 fE;

uniform sampler2D gSampler;
// uniform mat4 gWorld;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct; // m
uniform vec4 LightPosition; // m 
uniform float Shineness; // m

void main()
{
/*
*/
  // Normalize the input lighting vectors
  vec3 N = normalize(fN);
  vec3 E = normalize(fE);
  vec3 L = normalize(fL);
  vec3 H = normalize( L + E );
  vec4 ambient = AmbientProduct;
  float Kd = max(dot(L, N), 0.0);
  vec4 diffuse = Kd*DiffuseProduct;
  float Ks = pow(max(dot(N, H), 0.0), Shineness);
  vec4 specular = Ks*SpecularProduct;

  // discard the specular highlight if the light's behind the vertex
  if( dot(L, N) < 0.0 )
  specular = vec4(0.0, 0.0, 0.0, 1.0);
  gl_FragColor = ambient + diffuse + specular;
  gl_FragColor = gl_FragColor * texture2D(gSampler, TexCoord0.st); // fix
  gl_FragColor.a = 1.0;


//  gl_FragColor = vec4(Normal0, 1);
}

/*

out vec4 FragColor;

struct BaseLight
{
    vec3 Color;
    float AmbientIntensity;
    float DiffuseIntensity;
};

struct DirectionalLight
{
    BaseLight Base;
    vec3 Direction;
};

struct Attenuation
{
    float Constant;
    float Linear;
    float Exp;
};

struct PointLight
{
    BaseLight Base;
    vec3 Position;
    Attenuation Atten;
};

struct SpotLight
{
    PointLight Base;
    vec3 Direction;
    float Cutoff;
};

uniform int gNumPointLights;
uniform int gNumSpotLights;
uniform DirectionalLight gDirectionalLight;
uniform PointLight gPointLights[MAX_POINT_LIGHTS];
uniform SpotLight gSpotLights[MAX_SPOT_LIGHTS];
uniform sampler2D gSampler;
uniform vec3 gEyeWorldPos;
uniform float gMatSpecularIntensity;
uniform float gSpecularPower;

vec4 CalcLightInternal(BaseLight Light, vec3 LightDirection, vec3 Normal)
{
    vec4 AmbientColor = vec4(Light.Color, 1.0f) * Light.AmbientIntensity;
    float DiffuseFactor = dot(Normal, -LightDirection);

    vec4 DiffuseColor  = vec4(0, 0, 0, 0);
    vec4 SpecularColor = vec4(0, 0, 0, 0);

    if (DiffuseFactor > 0) {
        DiffuseColor = vec4(Light.Color, 1.0f) * Light.DiffuseIntensity * DiffuseFactor;

        vec3 VertexToEye = normalize(gEyeWorldPos - WorldPos0);
        vec3 LightReflect = normalize(reflect(LightDirection, Normal));
        float SpecularFactor = dot(VertexToEye, LightReflect);
        SpecularFactor = pow(SpecularFactor, gSpecularPower);
        if (SpecularFactor > 0) {
            SpecularColor = vec4(Light.Color, 1.0f) *
                            gMatSpecularIntensity * SpecularFactor;
        }
    }

    return (AmbientColor + DiffuseColor + SpecularColor);
}

vec4 CalcDirectionalLight(vec3 Normal)
{
    return CalcLightInternal(gDirectionalLight.Base, gDirectionalLight.Direction, Normal);
}

vec4 CalcPointLight(PointLight l, vec3 Normal)
{
    vec3 LightDirection = WorldPos0 - l.Position;
    float Distance = length(LightDirection);
    LightDirection = normalize(LightDirection);

    vec4 Color = CalcLightInternal(l.Base, LightDirection, Normal);
    float Attenuation =  l.Atten.Constant +
                         l.Atten.Linear * Distance +
                         l.Atten.Exp * Distance * Distance;

    return Color / Attenuation;
}

vec4 CalcSpotLight(SpotLight l, vec3 Normal)
{
    vec3 LightToPixel = normalize(WorldPos0 - l.Base.Position);
    float SpotFactor = dot(LightToPixel, l.Direction);

    if (SpotFactor > l.Cutoff) {
        vec4 Color = CalcPointLight(l.Base, Normal);
        return Color * (1.0 - (1.0 - SpotFactor) * 1.0/(1.0 - l.Cutoff));
    }
    else {
        return vec4(0,0,0,0);
    }
}
/*


/* void main()
{

    vec3 Normal = normalize(Normal0);
    vec4 TotalLight = CalcDirectionalLight(Normal);

    for (int i = 0 ; i < gNumPointLights ; i++) {
        TotalLight += CalcPointLight(gPointLights[i], Normal);
    }

    for (int i = 0 ; i < gNumSpotLights ; i++) {
        TotalLight += CalcSpotLight(gSpotLights[i], Normal);
    }

    // FragColor = vec4(1,1,0, 1.0f); // hack

    // gl_FragColor = texture2D(gSampler, TexCoord0.xy) * TotalLight + vec4(1,1,1,1);
    FragColor = texture2D(gSampler, TexCoord0.xy) * TotalLight; 
    // gl_FragColor = vec4(0,0,1, 1.0f); // hack
    // gl_FragColor = vec4(Normal, 1.0f); // hack
    // gl_FragColor = vec4(0,0,0,0);

    // gl_FragColor = vec4(1,1,0,1);
    

}
*/
