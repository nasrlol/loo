struct QueueFamiliyIndices
{
	u32 graphics_family;
};

#define EA_QUEUE_FAMILY_NONE 0xFFFFFFFF

internal QueueFamiliyIndices find_queue_famillites(Arena *arena, VkPhysicalDevice device)
{
	QueueFamiliyIndices indices =  {0};
	indices.graphics_family = EA_QUEUE_FAMILY_NONE;
	
	u32 queue_families_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_families_count, NULL);
	
	VkQueueFamilyProperties *queue_families = PushArrayZero(arena,  VkQueueFamilyProperties, queue_families_count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_families_count, queue_families);
	
	// we need to find atleast one queue familiy that supportsd VK_QUEU_GRAPHICS_BIT
	for(u32 index = 0; index < queue_families_count; ++index)
	{
		
		if(queue_families[index].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphics_family =  index;
		}
	}
	
	return indices;
}

internal void create_logical_device()
{
	
	// TODO:
	
	
}

// TODO: with each feature found score the device and the highest device is  the deticated gpu  the one below that probably the integrated gpu.
// that way we always have a GPU to use
internal b32 is_device_suitable(Arena *arena, VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties device_properties;
	VkPhysicalDeviceFeatures   device_features;
	
	vkGetPhysicalDeviceProperties(device, &device_properties);
	vkGetPhysicalDeviceFeatures(device, &device_features);
	
	QueueFamiliyIndices indices = find_queue_famillites(arena, device);
	
	
	VkDeviceQueueCreateInfo queue_create_info = {};
	
	queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_create_info.queueFamilyIndex = indices.graphics_family;
	queue_create_info.queueCount = 1;
	f32 queue_priority = 1.0f;
	queue_create_info.pQueuePriorities = &queue_priority;
	
	VkPhysicalDeviceFeatures device_features = {};
	
	VkDeviceCreateInfo create_info           = {};
	
	create_info.sType                        = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos             = &queue_create_info;
	createInfo.queueCreateInfoCount          = 1;
	createInfo.pEnabledFeatures              = &device_features;
	
	
	// TODO: validation layer
	b32 enabled_validation_layers false;
	u32 validation_layers_count = 0;
	
	create_info.enabledExtensionCount = 0;
	if(enable_validation_layers) // TODO: we don't handle validation layers yet
	{
		create_info.enabledLayerCount = validation_layers_count;
	}
	else
	{
		create_info.enabledLayerCount = 0;
	}
	
	
	
	return device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
		&& device_features.geometryShader
		&& indices.graphics_family != EA_QUEUE_FAMILY_NONE;
}

internal void
vk_render_entrypoint(Arena *arena, const char *name)
{
	VkApplicationInfo app_info{};
	app_info.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName   = name;
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.pEngineName        = "loo";
	app_info.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
	app_info.apiVersion         = VK_API_VERSION_1_3;
	
	VkInstanceCreateInfo create_info{};
	create_info.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;
	
	b32 enable_validation_layer  = true;
	const char *validation_layer = "VK_LAYER_KHRONOS_validation";
	
	if (enable_validation_layer)
	{
		ScratchArena scratch = scratch_start(arena);
		
		u32 layer_count = 0;
		vkEnumerateInstanceLayerProperties(&layer_count, NULL); VkLayerProperties *available_layers = PushArrayZero(scratch.arena, VkLayerProperties, layer_count); vkEnumerateInstanceLayerProperties(&layer_count, available_layers);
		
		b32 layer_found = false;
		for (u32 index = 0; index < layer_count; ++index)
		{
			if (CstringIsMatch(available_layers[index].layerName, validation_layer))
			{
				layer_found = true;
				break;
			}
		}
		scratch_end(&scratch);
		
		if (layer_found)
		{
			create_info.enabledLayerCount   = 1;
			create_info.ppEnabledLayerNames = &validation_layer;
		}
		else
		{
			Log("requested validation layer not found\n");
		}
		
	}
	
	//- checking for extensions
	{
		
		ScratchArena scratch              = scratch_start(arena); // TODO: can this even go into a scratch?
		u32 extension_count               = 0;
		vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL);
		VkExtensionProperties  *extensions = PushArrayZero(scratch.arena, VkExtensionProperties, extension_count);
		vkEnumerateInstanceExtensionProperties(NULL, &extension_count, extensions);
		for (u32 index                    = 0; index < extension_count; ++index)
		{
			Log("%s\n", extensions[index].extensionName);
		}
		scratch_end(&scratch);
	}
	
	
	VkInstance instance = VK_NULL_HANDLE;
	VkResult result     = vkCreateInstance(&create_info, NULL, &instance);
	if (result != VK_SUCCESS)
	{
		Log("failed to create a VkInstance: %d\n", result);
		return;
	}
	
	// TODO: validation layers for debugging if we want them in the future
	{
		
	}
	
	
	
	{
		VkPhysicalDevice physical_device =  VK_NULL_HANDLE;
		u32 device_count                 =  0;
		vkEnumeratePhysicalDevices(instance, &device_count, NULL);
		
		Assert(device_count);
		
		VkPhysicalDevice *devices = PushArrayZero(arena, VkPhysicalDevice, device_count);
		vkEnumeratePhysicalDevices(instance, &device_count, devices);
		VkPhysicalDevice device = NULL;
		
		for(u32 index = 0; index < device_count; ++index)
		{
			if(is_device_suitable(arena, devices[index]))
			{
				device = devices[index];
				break;
			}
		}
		Assert(physical_device  != VK_NULL_HANDLE);
		
		if (vkCreateDevice(physical_device, &create_info, NULL, &device) != VK_SUCCESS) {
			Log("failed to create device");
		}
		
		
		
	}
	
	
	
	
	vkDestroyInstance(instance, NULL);
}
