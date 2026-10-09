#if 0
#define VK_NO_PROTOTYPES

internal void
vk_render_entrypoint(MemArena *arena, WinParams *window, Params *params)
{

    // NOTE: requires #define VK_NO_PROTOTYPES
    // NOTE: dlopen searches for a path starting with a '/' afterwards it starts searching other directories
    // thats why the param is called path here. but we pass the name in stead
    // https://tldp.org/HOWTO/Program-Library-HOWTO/dl-libraries.html



    VkApplicationInfo app_info{}; // used for creating a vkInstance
    VkInstanceCreateInfo create_info{};

    //  fill in app info
    {
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName = params->name;
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName = window->name;
        app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.apiVersion = VK_API_VERSION_1_0;
    }

    // fill in create info
    {
        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pApplicationInfo = &app_info;

#if 0
        // TODO: fill in the extenions for windows.
        // where do we find these?
        create_info.enabledExtensionCount   = ...;
        create_info.ppEnabledExtensionNames = ...;
#endif
    }

#if DEBUG
    bool enable_validation_layer = true;
#else
    bool enable_validation_layer = false;
#endif


    bool layer_found  = false;
    // check validation support
    ScratchArena scratch = scratch_start(arena);


    // gather the validation layers and available layers
    u32 layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);
    VkLayerProperties *available_layers;
    String8List       *validation_layers = PushStructZero(arena, String8List);
    String8Node vk_layer_khronos_validation_node = ToString8Node(str8("VK_LAYER_KHRONOS_validation"));
    str8_list_push_node(validation_layers, &vk_layer_khronos_validation_node);
    available_layers = PushArrayZero(arena, VkLayerProperties, layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers);

    for(String8Node *current_node = validation_layers->first; !is_nil_str8_node(current_node); current_node = current_node->next) {
        for(u32 index = 0; index < layer_count; ++index) {
            VkLayerProperties *current_layer = &available_layers[index];
            String8 current_layer_name = str8(current_layer->layerName);
            if(str8_is_match(current_node->value, current_layer_name)) {
                layer_found = true;
                break;
            }
        }
    }

    // check if we found that specific layer and we can continue

    if(enable_validation_layer) { // check the avalaiobe layers after creating the instance
        if(!layer_found) {
            Log("requested validation layer not found!");
        }


        // extract names, null terminated
        String8 *names_in_one_str8_lit = PushStructZero(arena, String8);
        for(u32 index = 0; index < layer_count; ++index) {
            VkLayerProperties *current_layer = &available_layers[index];
            String8 current_layer_name;
            if(current_layer != NULL) {
                current_layer_name       = str8(current_layer->layerName);
            } else {
                breakpoint();
            }
            str8_append(names_in_one_str8_lit, &current_layer_name);
        }

        create_info.enabledLayerCount   = layer_count;
        create_info.ppEnabledLayerNames = (const char *)names_in_one_str8_lit->data;  // TODO: is this fixed???
    }

    VkInstance instance = VK_NULL_HANDLE;
    VkResult result = vkCreateInstance(&create_info, NULL, &instance);

    if(result != VK_SUCCESS) { // check the return code of the instance creation
        Log("Failed to create a VkInstance");
    } else {
        scratch_end(&scratch);

        {
            // TODO: window polling untill it closes
        }
    }

    if(instance != VK_NULL_HANDLE) {
        vkDestroyInstance(instance, nullptr);
    }
}


internal void
render_loop()
{




}



#endif