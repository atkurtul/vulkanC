#include "device.h"
#include "window.h"
#include "swapchain.h"
#include "command_pool.h"
#include "renderer.h"
#include "descriptor.h"
#include "buffer.h"
#include "math.h"
#include "pipeline.h"
#include "font.h"

int main()
{
    create_device();
    create_window(1920, 1080, 0);
    create_swapchain(8, 8);
    init_cmd_pools();
    create_renderer();

    desc_pool_size size[2] = { { desc_uniform, 1 }, {desc_combined_image_sampler, 16 } };
    desc_pool dpool = create_descriptor_pool(2, &size);
    desc_set_layout layouts[2];
    layouts[0] = create_desc_set_layout(desc_uniform, stage_vertex);
    layouts[1] = create_desc_set_layout(desc_combined_image_sampler, stage_frag);

    pipeline* pipe = create_pipeline(2, layouts, 
        "shader.vert", "shader.frag", "2f3f", 
        triangle_list, 
        polygon_fill, 0, 1, 0);
    pipeline* uipipe = create_pipeline(2, layouts, 
        "text.vert", "text.frag", "2f2f", 
        triangle_list, 
        polygon_fill, 0, 0, 1);


    desc_set set = create_desc_set(dpool, layouts[0]);
    mapped_buffer cbuffer = create_mapped_buffer(usage_uniform, 256);
    set_bind_buffer(set, cbuffer.buff, desc_uniform, 0);


    desc_set tset = create_desc_set(dpool, layouts[1]);
    texture tex = create_texture("assets/nightsky.jpg");
    set_bind_texture(tset, &tex, 0);


    float vert[] =  {  
        0, 0,   1, 0, 0,
        1, 0,   0, 1, 0,
        0, 1,   0, 0, 1
    };

    buffer vertex_buffer = create_buffer(usage_vertex, vert, sizeof(vert));
    
    vec4* cam = cbuffer.ptr;
    cam[0] = _mm_setr_ps(1, 0, 0, 0);
    cam[1] = _mm_setr_ps(0, 1, 0, 0);
    cam[2] = _mm_setr_ps(0, 0, 1, 0);
    cam[3] = _mm_setr_ps(0, 0, 0, 1);
    
    font* font = create_font("assets\\consolas.fnt",  create_desc_set(dpool, layouts[1]));

    char wsizestr[256] = {};
    char invwsizestr[256] = {};
    while(poll_window())
    {
        r_begin_frame();
        r_bind_pipeline(pipe);
        r_bind_desc_sets(0, 1, &set);
        r_bind_desc_sets(1, 1, &tset);
        r_bind_vertex_buffers(1, &vertex_buffer.buff);
        //r_draw(1, 3, 0, 0);
        r_bind_pipeline(uipipe);
        vec2 pos = {0, 0};
        sprintf(wsizestr, "%d %d", get_window()->x, get_window()->y);
        sprintf(invwsizestr, "%f %f", get_window()->inv_size[0], get_window()->inv_size[1]);
        draw_text(font, wsizestr, pos);
        pos[1] += 0.2;
        draw_text(font, invwsizestr, pos);
        dispatch_font(font);
        r_end_frame();
    }

    destroy_swapchain();
    destroy_renderer();
    destroy_pipeline(pipe);
    destroy_pipeline(uipipe);
}