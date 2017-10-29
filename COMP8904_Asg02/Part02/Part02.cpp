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
cl_uint _numPlatforms;
std::vector<cl_platform_id> _platformIds;
std::map<cl_platform_id, std::vector<cl_device_id>> _platforms;

cl_platform_id _cpuPlatform;
cl_platform_id _gpuPlatform;
cl_device_id _cpuDevice;
cl_device_id _gpuDevice;

cl_context _context;

const cl_uint NUM_PIXELS = 10;
std::vector<cl_float4> _pixels;

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

	if (!CreateContextForPlatform(_cpuPlatform))
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
	_context = clCreateContext(
		contextProperties, _platforms[platform].size(),
		_platforms[platform].data(), nullptr,
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
	_gpuPlatform = GetFirstPlatformWithDeviceOfType(CL_DEVICE_TYPE_GPU);
	if (_gpuPlatform)
	{
		_gpuDevice = GetFirstDeviceOfTypeFromPlatform(_gpuPlatform, CL_DEVICE_TYPE_GPU);
	}
	else
	{
		std::cout << "ERROR: No GPU could be detected on any available platform.\n\n";
		return false;
	}

	/* Check the GPU platform for a CPU device. */
	_cpuDevice = GetFirstDeviceOfTypeFromPlatform(_gpuPlatform, CL_DEVICE_TYPE_CPU);

	if (_cpuDevice)
	{
		_cpuPlatform = _gpuPlatform;
	}
	else
	{
		/* If the CPU is not on the same platform as the GPU, check the other platforms. */
		_cpuPlatform = GetFirstPlatformWithDeviceOfType(CL_DEVICE_TYPE_CPU);

		/* Get the CPU device if we found a platform for it and error out if we didn't. */
		if (_cpuPlatform)
		{
			_cpuDevice = GetFirstDeviceOfTypeFromPlatform(_cpuPlatform, CL_DEVICE_TYPE_CPU);
		}
		else
		{
			std::cout << "ERROR: No CPU could be detected on any available platform.\n\n";
			return false;
		}
	}

	/* Print out info on CPU and GPU. */
	std::cout << "Platform containing CPU is " << _cpuPlatform << "\n";
	std::cout << "CPU is device " << _cpuDevice << "\n\n";
	std::cout << "Platform containing GPU is " << _gpuPlatform << "\n";
	std::cout << "GPU is device " << _gpuDevice << "\n\n";

	return true;
}

/**
	Returns the first available platform that contains a device of the given type.
	Returns 0 if no platform has a device of the given type.
*/
cl_platform_id GetFirstPlatformWithDeviceOfType(cl_device_type typeToFind)
{
	for each (std::pair<cl_platform_id, std::vector<cl_device_id>> eachPlatform in _platforms)
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
	for each (cl_device_id eachDevice in _platforms[platform])
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
	_numPlatforms = 0;
	clGetPlatformIDs(0, nullptr, &_numPlatforms);

	/* Get the IDs of available platforms. */
	_platformIds = std::vector<cl_platform_id>(_numPlatforms);
	clGetPlatformIDs(_numPlatforms, _platformIds.data(), nullptr);

	/* Get the devices for each platform. */
	_platforms = std::map<cl_platform_id, std::vector<cl_device_id>>();
	for each (cl_platform_id eachPlatform in _platformIds)
	{
		cl_uint numDevices = 0;
		clGetDeviceIDs(eachPlatform, CL_DEVICE_TYPE_ALL, 0, nullptr, &numDevices);

		_platforms[eachPlatform] = std::vector<cl_device_id>(numDevices);
		clGetDeviceIDs(eachPlatform, CL_DEVICE_TYPE_ALL, numDevices, _platforms[eachPlatform].data(), nullptr);
	}

	/* Print out platforms and devices. */
	std::cout << "Detected the following platforms and devices:\n";
	for each (std::pair<cl_platform_id, std::vector<cl_device_id>> eachPlatform in _platforms)
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
