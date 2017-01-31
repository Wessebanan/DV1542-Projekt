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
};

struct GS_OUT
{
	float4 Pos : SV_POSITION;
	float3 Color : COLOR;
};

[maxvertexcount(3)]
void main( triangle GS_IN input[3],  inout TriangleStream< GS_OUT > output)
{
	matrix wvp = mul(proj, mul(view, world));
	for (uint i = 0; i < 3; i++)
	{
		GS_OUT element;
		element.Pos = mul(wvp, input[i].Pos);
		element.Color = input[i].Color;
		output.Append(element);
	}
};