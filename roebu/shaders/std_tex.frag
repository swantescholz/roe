const float ymax = 29.0;
const float ymin = -29.0;
const float inv = 1.0 / (ymax-ymin);

void main (void) 
{
	vec4 color = calcPerFragmentLighting();
	//color += texture2D(uTexture, (uTextureMatrix[0]*gl_TexCoord[0]).st);
	color += texture2D(uTexture, gl_TexCoord[0].st);
	//color += texture1D(uGradient, clamp((vFragPosition.y-ymin)*inv, 0.01, 0.99));
	color *= 0.5;
	/*//global fog:
	float distance = distance(uCameraPosition, vFragPosition);
	float fog = calcFogFactor(distance, uFogDensity);
	color = mix(color, uFogColor, fog);//*/
	gl_FragColor = color;
}

