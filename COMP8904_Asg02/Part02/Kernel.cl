__kernel void halveBrightness (	__global float4* startPixels, 
								__global float4* resultPixels)
{
	const int globalId = get_global_id(0);
	
	float x = startPixels[globalId].x / 2;
	float y = startPixels[globalId].y / 2;
	float z = startPixels[globalId].z / 2;
	float w = startPixels[globalId].w / 2;
	float4 halfVal = {x, y, z, w};

	resultPixels[globalId] = halfVal;
}
