#ifdef NDEBUG
ReadOnly global_variable b32 enable_validation_layers = false;
#else
ReadOnly global_variable b32 enable_validation_layers = true;
#endif

typedef struct VKState VKState;
struct VKState
{
	VkApplicationInfo           app_info;
	VkInstanceCreateInfo        create_info;
	VkDeviceQueueCreateInfo     queue_create_info;
	VkPhysicalDevice            physical_device;
	u32                         device_count;
	VkInstance                  instance;
	VkPhysicalDevice            *devices;
	VkPhysicalDevice            device;
	f32                         queue_priority;
	QueueFamiliyIndices         indices;
	VkPhysicalDeviceFeatures    device_features;
	VkPhysicalDeviceProperties  device_properties;
	u32                         queue_families_count;
};

internal void
vk_render_entrypoint(Arena *arena, const char *name)
{
	VKState vk_state = {};
	VKState *state = &vk_state;
	
	//- create instance
	{
		const char *layers[] =
		{
			"VK_LAYER_KHRONOS_validation"
		};
		
		const char *instance_extensions[] =
		{
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
#if DEBUG
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
		};
		
		state->app_info.sType                      = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		state->app_info.pApplicationName           = "Hello Triangle";
		state->app_info.applicationVersion         = VK_MAKE_VERSION(1, 0, 0);
		state->app_info.pEngineName                = "No Engine";
		state->app_info.engineVersion              = VK_MAKE_VERSION(1, 0, 0);
		state->app_info.apiVersion                 = VK_API_VERSION_1_0;
		
		state->create_info.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		state->create_info.pApplicationInfo        = &state->app_info;
		state->create_info.enabledExtensionCount   = ArrayCount(instance_extensions);
		state->create_info.ppEnabledExtensionNames = instance_extensions;
		state->create_info.enabledLayerCount       = ArrayCount(layers);
		state->create_info.ppEnabledLayerNames     = layers;
		
		state->instance                            = VK_NULL_HANDLE;
		
		if(vkCreateInstance(&state->create_info, NULL, &state->instance) != VK_SUCCESS)
		{
			Assert(0);
		}
	}
	
	//- pick physical device
	{
		state->physical_device = VK_NULL_HANDLE;
		state->device_count = 0;
		
		state->devices = PushArrayZero(arena, VkPhysicalDevice, state->device_count);
		vkEnumeratePhysicalDevices(state->instance, &state->device_count, NULL);
		
		Assert(state->device_count);
		
		vkEnumeratePhysicalDevices(state->instance, &state->device_count, state->devices);
		VkDevice device = NULL;
		
		// TODO: with each feature found score the device and the highest device is  the deticated gpu  the one below that probably the integrated gpu. that way we always have a GPU to use
		
		for(u32 index = 0; index < state->device_count; ++index)
		{
			b32 is_suitable = 0;
			
			//- is device suitable family
			{
				vkGetPhysicalDeviceProperties(state->device, &state->device_properties);
				
				//- find queue families
				{
					QueueFamiliyIndices indices =  {0};
					indices.graphics_family = EAV_QUEUE_FAMILY_NONE;
					
					state->queue_families_count = 0;
					vkGetPhysicalDeviceQueueFamilyProperties(state->device, &state->queue_families_count, NULL);
					
					VkQueueFamilyProperties *queue_families = PushArrayZero(arena,  VkQueueFamilyProperties, state->queue_families_count);
					vkGetPhysicalDeviceQueueFamilyProperties(state->device, &state->queue_families_count, queue_families);
					
					// we need to find atleast one queue familiy that supportsd VK_QUEU_GRAPHICS_BIT
					for(u32 index = 0; index < state->queue_families_count; ++index)
					{
						if(queue_families[index].queueFlags & VK_QUEUE_GRAPHICS_BIT)
						{
							indices.graphics_family =  index;
						}
					}
					
					state->indices = indices;
				}
				
				vkGetPhysicalDeviceFeatures(state->device, &state->device_features);
				
				is_suitable = (state->device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
					&& state->device_features.geometryShader
					&& (state->indices.graphics_family != EAV_QUEUE_FAMILY_NONE);
			}
			
			if(is_suitable)
			{
				state->device = state->devices[index];
				break;
			}
		}
		Assert(state->physical_device  != VK_NULL_HANDLE);
		
		//- create device
		// TODO: this is a physical device
		if (vkCreateDevice(state->physical_device, (const VkDeviceCreateInfo *)&state->create_info, NULL, (VkDevice *)state->device) != VK_SUCCESS)
		{
			Log("failed to create device");
		}
	}
	
	//- create logical device
	{
		state->queue_priority                     = 1.0f;
		state->queue_create_info.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		state->queue_create_info.queueFamilyIndex = state->indices.graphics_family;
		state->queue_create_info.queueCount       = 1;
		state->queue_create_info.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		state->queue_create_info.queueFamilyIndex = state->indices.graphics_family;
		state->queue_create_info.queueCount       = 1;
		state->queue_create_info.pQueuePriorities = &state->queue_priority;
		
		vkGetPhysicalDeviceFeatures(state->device, &state->device_features);
	}
	
	//- cleanup
	{
		vkDestroyInstance(state->instance, NULL);
	}
}
