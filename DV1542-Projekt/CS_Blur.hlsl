Texture2D<float> origSM : register (t0);
RWTexture2D<float> blurSM : register (u0);

cbuffer dimensions : register(b0)
{
	int height;
	int width;
};

//TODO: n�got klyftigt med dimensions f�r numthreads
[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{

}