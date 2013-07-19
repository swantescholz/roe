//#include "universal_header.vert"
void main(void)
{
	gl_Vertex = (uTransformationMatrix) * gl_Vertex;
	vFragPosition = gl_Vertex.xyz;
	vFragNormal = (uNormalMatrix * vec4(gl_Normal.xyz,1)).xyz;
	//vFragNormal = gl_Normal.xyz;
	//vFragNormal = cvec3y;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = uCameraProjectionMatrix * gl_Vertex;
}
