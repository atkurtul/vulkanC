#include "swapchain.h"

static swapchain chain;

swapchain* get_swapchain()
{
    return &chain;
}

image_buffer create_image_buffer(VkSampleCountFlagBits MSAA, int x, int y, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspect, uint mip)
{
    device* dev = get_device();

    VkImageCreateInfo info = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
    info.imageType         = VK_IMAGE_TYPE_2D;
    info.extent.width      = x;
    info.extent.height     = y;
    info.extent.depth      = 1;
    info.mipLevels         = mip;
    info.arrayLayers       = 1;
    info.tiling            = VK_IMAGE_TILING_OPTIMAL;
    info.sharingMode       = VK_SHARING_MODE_EXCLUSIVE;
    info.samples           = MSAA;
    info.format            = format;
    info.usage             = usage;

    image_buffer buffer = { };
    vkCreateImage(dev->device, &info, 0, &buffer.image);

    VkMemoryAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    allocInfo.allocationSize = image_memory_req(buffer.image).size;
    allocInfo.memoryTypeIndex = LOCAL_MEMORY;

    vkAllocateMemory(dev->device, &allocInfo, 0, &buffer.mem);
    vkBindImageMemory(dev->device, buffer.image, buffer.mem, 0);

    VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
    viewInfo.image                          = buffer.image;
    viewInfo.viewType                       = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format                         = format;
    viewInfo.subresourceRange.aspectMask    = aspect;
    viewInfo.subresourceRange.levelCount    = 1;
    viewInfo.subresourceRange.layerCount    = 1;

    vkCreateImageView(dev->device, &viewInfo, 0, &buffer.view);

    return buffer;
}

void create_frame_buffers(swapchain* chain, int x, int y)
{
    device* dev = get_device();

    uint count;
    vkGetSwapchainImagesKHR(dev->device, chain->chain, &count, 0);
    chain->images = malloc(sizeof(VkImage) * count);
    chain->views = malloc(sizeof(VkImageView) * count);
    chain->frame_buffer = malloc(sizeof(VkFramebuffer) * count);

    vkGetSwapchainImagesKHR(dev->device, chain->chain, &count, chain->images);

    for (size_t i = 0; i < count; i++) {
        VkImageViewCreateInfo createInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        createInfo.image = chain->images[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.layerCount = 1;

        vkCreateImageView(dev->device, &createInfo, 0, &chain->views[i]);
        
        VkImageView attachments[3] = { chain->color_buffer.view, chain->depth_buffer.view, chain->views[i] };
        VkFramebufferCreateInfo info = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
        info.renderPass = chain->pass;
        info.attachmentCount = 3;
        info.pAttachments = attachments;
        info.width = x;
        info.height = y;
        info.layers = 1;
        vkCreateFramebuffer(dev->device, &info, 0, &chain->frame_buffer[i]);
    }
}

VkRenderPass create_renderpass(VkSampleCountFlagBits MSAA)
{
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = VK_FORMAT_B8G8R8A8_UNORM;
    colorAttachment.samples = MSAA;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format = VK_FORMAT_D32_SFLOAT;
    depthAttachment.samples = MSAA;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription colorAttachmentResolve = {};
    colorAttachmentResolve.format = VK_FORMAT_B8G8R8A8_UNORM;
    colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorRef = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
    VkAttachmentReference depthRef = { 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
    VkAttachmentReference resolveRef = { 2, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

    VkSubpassDescription subpass = { 0 };
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorRef;
    subpass.pDepthStencilAttachment = &depthRef;
    subpass.pResolveAttachments = &resolveRef;

    VkSubpassDependency dependency = { };
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkAttachmentDescription attachments[3] = { colorAttachment, depthAttachment, colorAttachmentResolve };
    VkRenderPassCreateInfo passInfo = { };
    passInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    passInfo.attachmentCount = 3;
    passInfo.pAttachments = attachments;
    passInfo.subpassCount = 1;
    passInfo.pSubpasses = &subpass;
    passInfo.dependencyCount = 1;
    passInfo.pDependencies = &dependency;

    VkRenderPass pass;
    vkCreateRenderPass(get_device()->device, &passInfo, 0, &pass);
    return pass;
}


void init_swapchain(uint nframes, uint MSAA)
{
    device* dev             = get_device();
    VkSurfaceKHR surface    = get_window()->surface;

    VkSurfaceCapabilitiesKHR cap;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(dev->p_device, surface, &cap);
    int x = cap.currentExtent.width;
    int y = cap.currentExtent.height;
    chain.msaa      = MSAA;
    chain.width     = x;
    chain.height    = y;
    chain.nframes   = nframes;
    chain.swapidx   = 0;
    chain.depth_buffer = create_image_buffer(MSAA, x, y, 
        VK_FORMAT_D32_SFLOAT, 
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
        VK_IMAGE_ASPECT_DEPTH_BIT, 1);

    chain.color_buffer = create_image_buffer(MSAA, x, y, 
        VK_FORMAT_B8G8R8A8_UNORM, 
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | 
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 
        VK_IMAGE_ASPECT_COLOR_BIT, 1);

    chain.pass = create_renderpass(MSAA);

    VkSwapchainCreateInfoKHR createInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
    createInfo.surface = surface;
    createInfo.minImageCount = nframes;
    createInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
    createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    createInfo.imageExtent.width = x;
    createInfo.imageExtent.height = y;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT ;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;
    vkCreateSwapchainKHR(dev->device, &createInfo, 0, &chain.chain);

    create_frame_buffers(&chain, x, y); 
}

void create_swapchain(uint nframes, uint MSAA)
{
    chain.semaphore = malloc(sizeof(VkSemaphore) * nframes);
    chain.callbacks = 0;
    chain.ncallback = 0;
    VkSemaphoreCreateInfo semaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
    for(uint i = 0; i < nframes; ++i)
        vkCreateSemaphore(get_device()->device, &semaphoreInfo, 0, &chain.semaphore[i]);

    init_swapchain(nframes, MSAA);
}

uint get_next_image()
{
    uint imageidx;
    
    while(vkAcquireNextImageKHR(get_device()->device, chain.chain, UINT64_MAX, chain.semaphore[chain.swapidx], VK_NULL_HANDLE, &imageidx)
        == VK_ERROR_OUT_OF_DATE_KHR)
    {
        printf("Failed to acquire image...Reacreating swapchain\n");
        recreate_swapchain();
    }
    ++chain.swapidx;
    chain.swapidx %= chain.nframes;
    return imageidx;
}


void free_image_buffer(image_buffer buff)
{
    VkDevice dev = get_device()->device;
    vkDestroyImageView(dev, buff.view, 0);
    vkFreeMemory(dev, buff.mem, 0);
    vkDestroyImage(dev, buff.image, 0);
}

void free_swapchain()
{
    VkDevice dev = get_device()->device;
    vkDeviceWaitIdle(dev);
    for(int i = 0; i < chain.nframes; ++i)
    {
        vkDestroyFramebuffer(dev, chain.frame_buffer[i], 0);
        vkDestroyImageView(dev, chain.views[i], 0);
    }
    free_image_buffer(chain.color_buffer);
    free_image_buffer(chain.depth_buffer);
    vkDestroyRenderPass(dev, chain.pass, 0);
    vkDestroySwapchainKHR(dev, chain.chain, 0);
}   

void recreate_swapchain()
{
    free_swapchain();

    init_swapchain(chain.nframes, chain.msaa);

    for(int i = 0; i < chain.ncallback; ++i)
    {
        chain.callbacks[i].fn(chain.callbacks[i].ptr);
    }
}

void push_swapchain_recreation_callback(fn_callback fn, void* ptr)
{
    chain.callbacks = realloc(chain.callbacks, (chain.ncallback  + 1) * sizeof(callback));
    chain.callbacks[chain.ncallback].fn     = fn;
    chain.callbacks[chain.ncallback].ptr    = ptr;
    chain.ncallback++;
}

void destroy_swapchain()
{
    free_swapchain();
    for(int i = 0; i < chain.nframes; ++i)
    {
        vkDestroySemaphore(get_device()->device, chain.semaphore[i], 0);
    }
    free(chain.images);
    free(chain.views);
    free(chain.frame_buffer);
    free(chain.semaphore);
    free(chain.callbacks);
}