#version 410


in vec3 v3Pos;


uniform vec3 v3CameraPos;
uniform vec3 v3LightPos;
uniform vec3 v3InvWavelength;
uniform float fCameraHeight;
uniform float fCameraHeight2;
uniform float fInnerRadius;
uniform float fInnerRadius2;
uniform float fOuterRadius;
uniform float fOuterRadius2;
uniform float fKrESun;
uniform float fKmESun;
uniform float fKr4PI;
uniform float fKm4PI;
uniform float fScale;
uniform float fScaleDepth;
uniform float fScaleOverScaleDepth;
uniform float fSamples;
uniform int nSamples;

const float g = -0.99;
const float g2 = g*g;

uniform sampler2D myTextureSampler;
in vec2 TexCoord;

out vec4 fragColor;


float scale(float fCos) {
    float x = 1.0 - fCos;
    return fScaleDepth * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
}

void main() {
    
	vec3 v3Ray = v3Pos - v3CameraPos;
    float fFar = length(v3Ray);
    v3Ray /= fFar;    

	vec3 v3Start = v3CameraPos;
    float fHeight = length(v3Start);
    float fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fCameraHeight));
    float fStartAngle = dot(v3Ray, v3Start) / fHeight;
    float fStartOffset = fDepth*scale(fStartAngle);

	// Initialize the scattering loop variables
    float fSampleLength = fFar / fSamples;
    float fScaledLength = fSampleLength * fScale;
    vec3 v3SampleRay = v3Ray * fSampleLength;
    vec3 v3SamplePoint = v3Start + v3SampleRay * 0.5;

    // Now loop through the sample rays
    vec3 v3FrontColor = vec3(0.0, 0.0, 0.0);
    for(int i=0; i<nSamples; i++) {
        float fHeight = length(v3SamplePoint);
        float fDepth2 = exp(fScaleOverScaleDepth * (fInnerRadius - fHeight));
        float fLightAngle = dot(v3LightPos, v3SamplePoint) / fHeight;
        float fCameraAngle = dot(v3Ray, v3SamplePoint) / fHeight;
        float fScatter = (fStartOffset + fDepth2*(scale(fLightAngle) - scale(fCameraAngle)));
        vec3 v3Attenuate = exp(-fScatter * (v3InvWavelength * fKr4PI + fKm4PI));
        v3FrontColor += v3Attenuate * fScaledLength; //(fDepth2);// * fScaledLength);
        v3SamplePoint += v3SampleRay;
    }

    // Finally, scale the Mie and Rayleigh colors and set up the varying variables for the pixel shader
    vec4 secondaryColor = vec4(v3FrontColor * 0.0015f * 15.0f, 1.0);
    vec4 primaryColor = vec4(v3FrontColor * (v3InvWavelength * fKrESun), 1.0);
    //vec4 primaryColor = vec4(v3FrontColor , 1.0);
    vec3 v3Direction = v3CameraPos - v3Pos;

    float fCos = dot(v3LightPos, v3Direction) / length(v3Direction);
    float fRayleighPhase = 0.75 * (1.0 + fCos*fCos);
    float fMiePhase = 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + fCos*fCos) / pow(1.0 + g2 - 2.0*g*fCos, 1.5);
	
	fragColor = vec4(texture( myTextureSampler, TexCoord ).rgb,1);

	//vec4 atmColor =   fRayleighPhase * primaryColor + fMiePhase * secondaryColor;

	//fragColor =  primaryColor;
	//fragColor = vec4(fRayleighPhase,0,0,1);
    //fragColor = vec4(1.0,0.0,0.0,1.0);
}