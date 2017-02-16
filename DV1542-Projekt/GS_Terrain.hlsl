cbuffer TRANSFORM_BUFFER : register(b0)
{
	matrix world;
	matrix view;
	matrix proj;
}


struct GS_IN
{
	float4 Pos : POSITION;
	float3 Color : COLOR;
	float2 TexCoord : TEXCOORD;
	//float3 Normal : NORMAL;
};

struct GS_OUT
{
	float4 Pos : SV_POSITION;
	float3 Nor : NORMAL;
	float3 WPos : POSITION;
	float3 Color : COLOR;
	float2 TexCoord : TEXCOORD;
	float3 Tangent : TANGENT;
	float3 Bitangent : BINORMAL;
};

[maxvertexcount(3)]
void main(triangle GS_IN input[3], inout TriangleStream< GS_OUT > output)
{

	float4 vec0 = input[1].Pos - input[0].Pos;
	float4 vec1 = input[2].Pos - input[0].Pos;
	float3 nor = normalize(cross(vec0, vec1).xyz);
	
	float3 tangents[3];		
	float3 bitangents[3];
	int counter = 0;

	for (int i = 0; i < 3; i++)
	{
		float3 v0 = input[counter % 3].Pos.xyz;
		float3 v1 = input[(counter + 1) % 3].Pos.xyz;
		float3 v2 = input[(counter + 2) % 3].Pos.xyz;

		float2 uv0 = input[counter % 3].TexCoord;
		float2 uv1 = input[(counter + 1) % 3].TexCoord;
		float2 uv2 = input[(counter + 2) % 3].TexCoord;

		counter++;

		float3 deltaPos1 = v1 - v0;
		float3 deltaPos2 = v2 - v0;

		float2 deltaUV1 = uv1 - uv0;
		float2 deltaUV2 = uv2 - uv0;

	float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

		tangents[i] = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
		bitangents[i] = (deltaPos2 * deltaUV1.y - deltaPos1 * deltaUV2.y) * r;
	}

	matrix wvp = mul(proj, mul(view, world));
	for (uint i = 0; i < 3; i++)
	{
		GS_OUT element;
		element.Pos = mul(wvp, input[i].Pos);
		element.Color = input[i].Color;
		element.Nor = mul(world, nor);		
		element.WPos = mul(world, input[i].Pos);
		element.TexCoord = input[i].TexCoord;
		element.Tangent = normalize(mul(world, tangents[i]));
		element.Bitangent = normalize(mul(world, bitangents[i]));
		output.Append(element);
	}
};