void applyWave(inout vec3 vertex, vec3 direction, float fac, float time, float T);
void applyFunc(inout vec3 v);

uniform vec2 uvMin, uvMax;

void main(void)
{
	gl_Vertex = (uTransformationMatrix) * gl_Vertex;
	applyFunc(gl_Vertex.xyz);
	vFragPosition = gl_Vertex.xyz;
	vec3 tmp1 = vec3(gl_Vertex.x+0.001, gl_Vertex.y, gl_Vertex.z);
	vec3 tmp2 = vec3(gl_Vertex.x, gl_Vertex.y, gl_Vertex.z+0.001);
	applyFunc(tmp1); applyFunc(tmp2);
	vFragNormal = -normalize(cross(tmp1, tmp2));
	
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = uCameraProjectionMatrix * gl_Vertex;
}

void applyWave(inout vec3 vertex, vec3 direction, float fac, float time, float T) {
	const float factor = 2*M_PI/T;
	const vec3 v = fac*vertex;
	vertex += direction*
		((sin(v.x*0.2+time*3.0)*cos(v.z*0.2+time*3.0))*
		sin(time+v.x*0.1+v.z*0.1)*2.0+sin(time)*0.5);
}

float yFunc(float x, float z) {
	//return .1*(.1*x*x-10*cos(2*M_PI*z));
	//return .3*(-x*sin(sqrt(abs(z*6))));
	//return .03*x*x-.5*z-10/x+30/z/z + 5*sin(x*z);
	return .02*(z*z-x*x);
}
vec3 uvFunc(float u, float v) {
	/*return vec3(
	u*v*sin(15*v),
	v,
	u*v*cos(15*v))
	*vec3(1,30,1);*/
	/*return vec3(
	cos(u)*sin(v),
	sin(u)*sin(v)*2,
	cos(v)+log(tan(v/2))+u
	)*vec3(1,1,1)*40;*/
	/*return vec3(
	sin(u)/(sqrt(2)+cos(v)),
	sin(u+2*M_PI/3)/(sqrt(2)+cos(v+2*M_PI/3)),
	cos(u-2*M_PI/3)/(sqrt(2)+cos(v-2*M_PI/3))
	)*vec3(1,1,1)*40;*/
	/*return vec3(
	(2+cos(u))*(v/3-sin(v)),
	(2+cos(u - 2*M_PI/3))*(cos(v)-1),
	(2+cos(u + 2*M_PI/3))*(cos(v)-1)
	)*vec3(1,1,1)*40;*/
	/*return vec3(
	u*cos(v),
	v*cos(u),
	u*sin(v)
	)*vec3(1,1,1)*40;*/
	u *= 0.102; v *= 0.102;
	const float f = u/(2*M_PI)+pow(u/(2*M_PI),20)*2;
	return vec3(
	(2 + u/(2*M_PI)*cos(v))*sin(u) - 0.1*u,
	(2 + f*cos(v))*cos(u) + 0.5*u,
	f*sin(v)
	)*vec3(1,1,1)*40;
}
void applyFunc(inout vec3 v) {
	//v.y = yFunc(v.x, v.z);
	v = uvFunc(mix(uvMin.x,uvMax.x,v.x), mix(uvMin.y,uvMax.y,v.z));
}