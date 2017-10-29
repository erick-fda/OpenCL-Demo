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
	Forward Declarations
========================================================================================*/
void GetAvailablePlatforms();
cl_platform_id GetFirstPlatformWithDeviceOfType(cl_device_type typeToFind);
cl_device_id GetFirstDeviceOfTypeFromPlatform(cl_platform_id platform, cl_device_type typeToCheck);
bool GetCpuAndGpu();
bool CreateContextForPlatform(cl_platform_id platform);

/*========================================================================================
	Fields
========================================================================================*/
cl_uint numPlatforms;
std::vector<cl_platform_id> platformIds;
std::map<cl_platform_id, std::vector<cl_device_id>> platforms;

cl_platform_id cpuPlatform;
cl_platform_id gpuPlatform;
cl_device_id cpuDevice;
cl_device_id gpuDevice;

cl_context context;

/*========================================================================================
	Main Function
========================================================================================*/
/**
	Example of calculating average colour for a collection of pixels using OpenCL to 
	run on the CPU only.
*/
int main()
{
	GetAvailablePlatforms();

	if (!GetCpuAndGpu())
	{
		std::cin.ignore();
		return 1;
	}

	if (!CreateContextForPlatform(cpuPlatform))
	{
		std::cin.ignore();
		return 1;
	}

	std::cin.ignore();
	return 0;
}

/**
	Create the OpenCL context.
*/
bool CreateContextForPlatform(cl_platform_id platform)
{
	/* Initialize properties for the context. */
	const cl_context_properties contextProperties[] =
	{
		CL_CONTEXT_PLATFORM,
		reinterpret_cast<cl_context_properties>(platform),
		0,
		0
	};

	/* Create the context. */
	cl_int result = 0;
	context = clCreateContext(
		contextProperties, platforms[platform].size(),
		platforms[platform].data(), nullptr,
		nullptr, &result
	);

	return (result == CL_SUCCESS);
}

/**
	Get the CPU and GPU.
*/
bool GetCpuAndGpu()
{
	/* Find the platform with the GPU. */
	gpuPlatform = GetFirstPlatformWithDeviceOfType(CL_DEVICE_TYPE_GPU);
	if (gpuPlatform)
	{
		gpuDevice = GetFirstDeviceOfTypeFromPlatform(gpuPlatform, CL_DEVICE_TYPE_GPU);
	}
	else
	{
		std::cout << "ERROR: No GPU could be detected on any available platform.\n\n";
		return false;
	}

	/* Check the GPU platform for a CPU device. */
	cpuDevice = GetFirstDeviceOfTypeFromPlatform(gpuPlatform, CL_DEVICE_TYPE_CPU);

	if (cpuDevice)
	{
		cpuPlatform = gpuPlatform;
	}
	else
	{
		/* If the CPU is not on the same platform as the GPU, check the other platforms. */
		cpuPlatform = GetFirstPlatformWithDeviceOfType(CL_DEVICE_TYPE_CPU);

		/* Get the CPU device if we found a platform for it and error out if we didn't. */
		if (cpuPlatform)
		{
			cpuDevice = GetFirstDeviceOfTypeFromPlatform(cpuPlatform, CL_DEVICE_TYPE_CPU);
		}
		else
		{
			std::cout << "ERROR: No CPU could be detected on any available platform.\n\n";
			return false;
		}
	}

	/* Print out info on CPU and GPU. */
	std::cout << "Platform containing CPU is " << cpuPlatform << "\n";
	std::cout << "CPU is device " << cpuDevice << "\n\n";
	std::cout << "Platform containing GPU is " << gpuPlatform << "\n";
	std::cout << "GPU is device " << gpuDevice << "\n\n";

	return true;
}

/**
	Returns the first available platform that contains a device of the given type.
	Returns 0 if no platform has a device of the given type.
*/
cl_platform_id GetFirstPlatformWithDeviceOfType(cl_device_type typeToFind)
{
	for each (std::pair<cl_platform_id, std::vector<cl_device_id>> eachPlatform in platforms)
	{
		if (GetFirstDeviceOfTypeFromPlatform(eachPlatform.first, typeToFind))
		{
			return eachPlatform.first;
		}
	}

	return 0;
}

/**
	Returns the ID of the first device of the given type on the given platform.
	Returns 0 if there is no device of the given type on the given platform.
*/
cl_device_id GetFirstDeviceOfTypeFromPlatform(cl_platform_id platform, cl_device_type typeToFind)
{
	for each (cl_device_id eachDevice in platforms[platform])
	{
		cl_device_type deviceType;
		clGetDeviceInfo(eachDevice, CL_DEVICE_TYPE, sizeof(cl_device_id), &deviceType, nullptr);

		if (deviceType == typeToFind)
		{
			return eachDevice;
		}
	}

	return 0;
}

/**
	Get the OpenCL platforms and devices available.
*/
void GetAvailablePlatforms()
{
	/* Get the number of available platforms. */
	numPlatforms = 0;
	clGetPlatformIDs(0, nullptr, &numPlatforms);

	/* Get the IDs of available platforms. */
	platformIds = std::vector<cl_platform_id>(numPlatforms);
	clGetPlatformIDs(numPlatforms, platformIds.data(), nullptr);

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
}
