
void main (void) 
{
	vec4 color = calcPerFragmentLighting();
	
	/*//global fog:
	float distance = distance(uCameraPosition, vFragPosition);
	float fog = calcFogFactor(distance, uFogDensity);
	color = mix(color, uFogColor, fog);//*/
	gl_FragColor = color;
}