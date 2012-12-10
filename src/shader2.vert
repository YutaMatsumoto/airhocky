#version 120

// Does not work in ecc opengl
// layout (location = 0) in vec3 Position; // syntax error, unexpected IDENTIFIER
// layout (location = 1) in vec2 TexCoord;
// layout (location = 2) in vec3 Normal;

// > Input from the application to vertex shader
attribute vec3 Position;
attribute vec2 TexCoord;
attribute vec3 Normal;
// uniform mat4 gWorld;

// > Uniform variables
uniform mat4 gWVP;
uniform vec4 LightPosition;
uniform mat4 Projection;

// > output variables from vertex shader to fragment shader
varying vec2 TexCoord0;
varying vec3 Normal0;
	// varying vec3 WorldPos0;
varying vec3 fN;
varying vec3 fE;
varying vec3 fL;

void main()
{

  TexCoord0   = TexCoord;
  fN = Normal;
  fE = Position;
  fL = LightPosition.xyz; // here

  if( LightPosition.w != 0.0 ) {
    fL = LightPosition.xyz - Position;
  }
  gl_Position = gWVP*vec4(Position, 1.0);

  /* from slides
    gl_Position = gWVP * vec4(Position, 1.0);
    TexCoord0   = TexCoord;
    Normal0     = (gWorld * vec4(Normal, 0.0)).xyz;
    WorldPos0   = (gWorld * vec4(Position, 1.0)).xyz;

    // Transform vertex position into eye coordinates
    vec3 pos = (gWorld * vPosition).xyz;
    vec3 L = normalize( LightPosition.xyz - Position );
    vec3 E = normalize( -Position );
    vec3 H = normalize( L + E );

    // Transform vertex normal into eye coordinates
    vec3 N = normalize( ModelView*vec4(vNormal, 0.0) ).xyz;

    // Compute terms in the illumination equation
    vec4 ambient = AmbientProduct;
    float Kd = max( dot(L, N), 0.0 );
    vec4 diffuse = Kd*DiffuseProduct;
    float Ks = pow( max(dot(N, H), 0.0), Shininess );
    vec4 specular = Ks * SpecularProduct;
    if( dot(L, N) < 0.0 ) specular = vec4(0.0, 0.0, 0.0, 1.0);
    gl_Position = Projection * ModelView * vPosition;
    color = ambient + diffuse + specular;
    color.a = 1.0;
  */

}
