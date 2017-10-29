/*===================================================================================*//**
	Part 02
	
	Example of calculating average colour for a collection of pixels using OpenCL to 
	run on the CPU only.

    @author Erick Fernandez de Arteaga, John Janzen
	@version 0.0.0
	@file
	
*//*====================================================================================*/

/*========================================================================================
	Dependencies
========================================================================================*/
#include <iostream>
#include <map>
#include <vector>

#include <CL/cl.h>

/*========================================================================================
	Fields
========================================================================================*/
cl_uint numPlatforms;
std::vector<cl_platform_id> platformIds;
std::map<cl_platform_id, std::vector<cl_device_id>> platforms;
cl_platform_id cpuToUse;

/*========================================================================================
	Main Function
========================================================================================*/
/**
	Creates a collection of "pixels" (represented by float4s indicating RGBA values) and 
	calculates the average color for the pixels.
*/
int main()
{
	int result = 0;

	/* Get the number of available platforms. */
	numPlatforms = 0;
	clGetPlatformIDs(0, nullptr, &numPlatforms);

	/* Get the IDs of available platforms. */
	platformIds = std::vector<cl_platform_id>(numPlatforms);
	result = clGetPlatformIDs(numPlatforms, platformIds.data(), nullptr);

	/* Get the devices for each platform. */
	platforms = std::map<cl_platform_id, std::vector<cl_device_id>>();
	for each (cl_platform_id eachPlatform in platformIds)
	{
		cl_uint numDevices = 0;
		clGetDeviceIDs(eachPlatform, CL_DEVICE_TYPE_ALL, 0, nullptr, &numDevices);

		platforms[eachPlatform] = std::vector<cl_device_id>(numDevices);
		clGetDeviceIDs(eachPlatform, CL_DEVICE_TYPE_ALL, numDevices, platforms[eachPlatform].data(), nullptr);
	}

	/* Print out platforms and devices. */
	std::cout << "Detected the following platforms and devices:\n";
	for each (std::pair<cl_platform_id, std::vector<cl_device_id>> eachPlatform in platforms)
	{
		std::cout << "\t" << eachPlatform.first << "\n";

		for each (cl_device_id eachDevice in eachPlatform.second)
		{
			cl_device_type deviceType;
			clGetDeviceInfo(eachDevice, CL_DEVICE_TYPE, sizeof(cl_device_id), &deviceType, nullptr);

			std::string deviceTypeString;
			switch (deviceType)
			{
			case CL_DEVICE_TYPE_CPU:
				deviceTypeString = "CPU";
				break;
			case CL_DEVICE_TYPE_GPU:
				deviceTypeString = "GPU";
				break;
			default:
				deviceTypeString = "OTHER";
				break;
			}

			std::cout << "\t\t" << eachDevice << " (" << deviceTypeString.data() << ")\n";
		}
	}
	std::cout << "\n";

	/* Identify which CPU to use. */
	for each (std::pair<cl_platform_id, std::vector<cl_device_id>> eachPlatform in platforms)
	{
		bool isCpu = true;

		for each (cl_device_id eachDevice in eachPlatform.second)
		{
			cl_device_type deviceType;
			clGetDeviceInfo(eachDevice, CL_DEVICE_TYPE, sizeof(cl_device_id), &deviceType, nullptr);

			if (deviceType == CL_DEVICE_TYPE_GPU)
			{
				isCpu = false;
				break;
			}
		}

		if (isCpu)
		{
			cpuToUse = eachPlatform.first;
			break;
		}
	}

	/* Create open  */

	std::cin.ignore();
	return 0;
}
