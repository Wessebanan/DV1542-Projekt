cbuffer TRANSFORM_BUFFER : register(b0)
{
	matrix world;
	matrix view;
	matrix proj;
	matrix lightView;
	matrix lightProj;
}

struct GS_IN
{
	float4 Pos : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
};

struct GS_OUT
{
	float4 Pos : SV_POSITION;
	float3 Nor : NORMAL;
	float3 WPos : POSITION0;
	float2 TexCoord : TEXCOORD;
	float3 Tangent : TANGENT;
	float3 Bitangent : BINORMAL;
	float4 lightPos : POSITION1;
};

[maxvertexcount(3)]
void main(triangle GS_IN input[3], inout TriangleStream< GS_OUT > output)
{
	//The distance between the positions of the vertices.
	float3 dPos1 = input[1].Pos.xyz - input[0].Pos.xyz;
	float3 dPos2 = input[2].Pos.xyz - input[0].Pos.xyz;

	//The distance between the texcoords of the vertices
	float2 dUV1 = input[1].TexCoord - input[0].TexCoord;
	float2 dUV2 = input[2].TexCoord - input[0].TexCoord;

	//Calculating the tangent and bitangent.
	float r = 1.0f / (dUV1.x * dUV2.y - dUV1.y * dUV2.x);
	float3 tangent = (dPos1 * dUV2.y - dPos2 * dUV1.y) * r;
	float3 bitangent = (dPos2 * dUV1.y - dPos1 * dUV2.y) * r;
	//--------------------------------------

	matrix wvp = mul(proj, mul(view, world));
	matrix lightWvp = mul(lightProj, mul(lightView, world));

	//The face normal is shared between the three veritces of the triangle.
	float3 normal = normalize(mul(world, float4(input[0].Normal, 0.0f)).xyz);

	for (uint i = 0; i < 3; i++)
	{
		GS_OUT element;
		element.Pos = mul(wvp, input[i].Pos);
		element.Nor = normal;
		element.WPos = mul(world, input[i].Pos).xyz;
		element.TexCoord = input[i].TexCoord;
		element.Tangent = normalize(mul(world, float4(tangent, 0.0f)).xyz);
		element.Bitangent = normalize(mul(world, float4(bitangent, 0.0f)).xyz);
		//The position from the light's point of view.
		element.lightPos = mul(lightWvp, input[i].Pos);
		output.Append(element);
	}
};