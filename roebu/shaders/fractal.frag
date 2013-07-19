uniform int maxIterations;
uniform vec2 center;
uniform float zoom;

vec4 calcMandelbrotFractal();

void main()
{
	vec4 color = calcPerFragmentLighting();
	color += calcMandelbrotFractal();
	color *= 0.5;
	/*//global fog:
	float distance = distance(uCameraPosition, vFragPosition);
	float fog = calcFogFactor(distance, uFogDensity);
	color = mix(color, uFogColor, fog);//*/
	gl_FragColor = color;
}

vec4 calcMandelbrotFractal() {
	float real = vFragPosition.x * (1.0/zoom) + center.x;
	float imag = vFragPosition.z * (1.0/zoom) + center.y;
	float cReal   = real;
	float cImag = imag;
	float r2 = 0.0;
	int iter;
	for (iter = 0; iter < maxIterations && r2 < 4.0; ++iter)
	{
		float tempreal = real;
		real = (tempreal * tempreal) - (imag * imag) + cReal;
		imag = 2.0 * tempreal * imag + cImag;
		r2 = real*real*imag*imag;
	}
	// Base the color on the number of iterations.
	vec4 color;
	if (r2 < 4.0) {
		color = vec4(0,0,0,1);
	}
	else {
		color = texture1D(uGradient, fract(float(iter)*0.05));
	}
	//return vec4 (clamp(color, 0.0, 1.0), 1.0);
	return color;
}
