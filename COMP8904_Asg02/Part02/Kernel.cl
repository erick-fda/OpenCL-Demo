__kernel void halveBrightness (	__global cl_float4* startPixels, 
								__global cl_float4* resultPixels)
{
	const int globalId = get_global_id(0);
	
	cl_float4 halfVal = cl_float4();
	halfVal.x = startPixels[globalId].x / 2;
	halfVal.y = startPixels[globalId].y / 2;
	halfVal.z = startPixels[globalId].z / 2;
	halfVal.w = startPixels[globalId].w / 2;

	resultPixels[globalId] = halfVal;
}
