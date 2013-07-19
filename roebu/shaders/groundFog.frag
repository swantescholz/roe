uniform float fogMin; //lower y
uniform float fogMax; //higher y

void main (void) 
{
	calcPerFragmentLighting();
	vec4 color = gl_FrontColor;
	
	//ground fog:
	const float distance = calcGroundFogDistance(fogMin, fogMax, uCameraPosition, vFragPosition);
	const float fog = calcFogFactor(distance, uFogDensity);
	color = mix(color, uFogColor, fog);
	gl_FragColor = color;
}
