

void main(void)
{
	gl_Vertex = (uTransformationMatrix) * gl_Vertex;
	vFragPosition = gl_Vertex.xyz;
	vFragNormal = uNormalMatrix * gl_Normal;
	gl_Position = uCameraProjectionMatrix * gl_Vertex;
}