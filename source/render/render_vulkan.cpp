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
		vkEnumerateInstanceLayerProperties(&layer_count, NULL);
		VkLayerProperties *available_layers = PushArrayZero(scratch.arena, VkLayerProperties, layer_count);
		vkEnumerateInstanceLayerProperties(&layer_count, available_layers);
		
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
	
	VkInstance instance = VK_NULL_HANDLE;
	VkResult result     = vkCreateInstance(&create_info, NULL, &instance);
	if (result != VK_SUCCESS)
	{
		Log("failed to create a VkInstance: %d\n", result);
		return;
	}
	
	
	// checking for extensions
	{
		
		ScratchArena scratch              = scratch_start(arena);
		u32 extension_count               = 0;
		vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL);
		VkExtensionProperties  *extensions = PushStructZero(scratch.arena, VkExtensionProperties);
		vkEnumerateInstanceExtensionProperties(NULL, &extension_count, extensions);
		for (u32 index                    = 0; index < extension_count; ++index)
		{
			Log("%s\n", extensions[index].extensionName);
		}
	}
	
	
	vkDestroyInstance(instance, NULL);
}
