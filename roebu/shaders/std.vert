//#include "universal_header.vert"
void main(void)
{
	gl_Vertex = (uTransformationMatrix) * gl_Vertex;
	vFragPosition = gl_Vertex.xyz;
	vFragNormal = (uNormalMatrix * vec4(gl_Normal.xyz,1)).xyz;
	gl_Position = uCameraProjectionMatrix * gl_Vertex;
}
