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
	float3 Normal : NORMAL;
};

struct GS_OUT
{
	float4 Pos : SV_POSITION;
	float3 Nor : NORMAL;
	float3 WPos : POSITION;
	float3 Color : COLOR;
	float2 TexCoord : TEXCOORD;
};

[maxvertexcount(3)]
void main( triangle GS_IN input[3],  inout TriangleStream< GS_OUT > output)
{
	float4 vec0 = input[1].Pos - input[0].Pos;
	float4 vec1 = input[2].Pos - input[0].Pos;
	float3 nor = normalize(cross(vec0, vec1).xyz);

	matrix wvp = mul(proj, mul(view, world));
	for (uint i = 0; i < 3; i++)
	{
		GS_OUT element;
		element.Pos = mul(wvp, input[i].Pos);
		element.Color = input[i].Color;
		//element.Nor = input[i].Normal;
		element.Nor = mul(world,nor);
		
		element.WPos = mul(world, input[i].Pos);
		element.TexCoord = input[i].TexCoord;
		output.Append(element);
	}
};