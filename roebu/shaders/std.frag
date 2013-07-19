void main (void) 
{
	vec4 color = calcPerFragmentLighting();
	/*//global fog:
	float distance = distance(uCameraPosition, vFragPosition);
	float fog = calcFogFactor(distance, uFogDensity);
	color = mix(color, uFogColor, fog);//*/
	//color.r = color.g = color.b = .6*color.g+.3*color.r+.1*color.b;
	gl_FragColor = color;
}

/*if (mod(int(vFragPosition.y*.2),2) == 0) {
	tmpCol += vec4(1,0,0,1);
} else
	tmpCol += vec4(0,0,1,1);//RED-BLUE-STRIPES*/

