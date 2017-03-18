cbuffer TRANSFORM_BUFFER : register(b0)
{
	matrix world;
	matrix view;
	matrix proj;
	matrix lightView;
	matrix lightProj;
	float4 camPos;
}

struct GS_IN
{
	float4 Pos		: POSITION;
	float3 Normal	: NORMAL;
	float2 TexCoord : TEXCOORD;
};

struct GS_OUT
{
	float4 Pos		 : SV_POSITION;
	float3 Nor		 : NORMAL;
	float3 WPos		 : POSITION0;
	float2 TexCoord  : TEXCOORD;
	float3 Tangent	 : TANGENT;
	float3 Bitangent : BINORMAL;
	float4 lightPos  : POSITION1;
	float testFloat  : FLOAT;
};

[maxvertexcount(3)]
void main(triangle GS_IN input[3], inout TriangleStream< GS_OUT > output)
{
	//The face normal is shared between the three vertices of the triangle.
	float3 normal = normalize(mul(world, float4(input[0].Normal, 0.0f)).xyz);

	float3 worldPositions[3];
	for (int i = 0; i < 3; i++)
	{
		worldPositions[i] = mul(world, input[i].Pos).xyz;
	}

	float3 edge0 = worldPositions[1] - worldPositions[0];
	float3 edge1 = worldPositions[2] - worldPositions[0];
	float3 nor2 = normalize(cross(edge0, edge1));

	//Creating a vector from the camera to the first vertex of the
	//triangle and calculating the cosine of the angle to determine
	//if the triangle is backfacing.
	float3 camToPoint = normalize(worldPositions[0] - camPos.xyz);
	float3 pointToCam = normalize(camPos.xyz - worldPositions[0]);
	float cosAngle = dot(pointToCam, nor2);
	
	if (cosAngle /*- 0.3f */ >= 0.0f) 
	//if(true)
	{
		//The distance between the positions of the vertices.
		float3 dPos1 = input[1].Pos.xyz - input[0].Pos.xyz;
		float3 dPos2 = input[2].Pos.xyz - input[0].Pos.xyz;

		//The distance between the texcoords of the vertices
		float2 dUV1 = input[1].TexCoord - input[0].TexCoord;
		float2 dUV2 = input[2].TexCoord - input[0].TexCoord;

		//Calculating the tangent and bitangent.
		float r = 1.0f / (dUV1.x * dUV2.y - dUV1.y * dUV2.x);
		float3 tangent	 = (dPos1 * dUV2.y - dPos2 * dUV1.y) * r;
		float3 bitangent = (dPos2 * dUV1.y - dPos1 * dUV2.y) * r;
		
		//Constructing the transformation matrices outside of
		//the coming loop to cut down on the total number of
		//matrix multiplications performed.
		matrix wvp = mul(proj, mul(view, world));
		matrix lightWvp = mul(lightProj, mul(lightView, world));

		for (uint i = 0; i < 3; i++)
		{
			GS_OUT element;
			element.Pos		  = mul(wvp, input[i].Pos);
			element.Nor		  = nor2;
			element.WPos	  = worldPositions[i];
			element.TexCoord  = input[i].TexCoord;
			element.Tangent	  = normalize(mul(world, float4(tangent, 0.0f)).xyz);
			element.Bitangent = normalize(mul(world, float4(bitangent, 0.0f)).xyz);
			element.lightPos  = mul(lightWvp, input[i].Pos);
			element.testFloat = -cosAngle;
			output.Append(element);
		}
	}
};