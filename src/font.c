#include "font.h"
#include "renderer.h"

void offset(const letter* ref, vec2 p, letter* dst)
{
    vec2 s = get_window()->inv_size;
    vec8 sca   = { s[0], s[1], 1, 1, s[0], s[1], 1, 1 };
    vec8 xoff  = { p[0], p[1], 0, 0, p[0], p[1], 0, 0 };
    dst->v[0] = ref->v[0] * sca + xoff;
    dst->v[1] = ref->v[1] * sca + xoff;
    dst->v[2] = ref->v[2] * sca + xoff;
}

void draw_text(font* font, const char* text, vec2 pos)
{
    uint len = strlen(text);
    vec2 xoff = pos;

    letter* quad = font->quad.ptr;
    quad += font->nletter_frame;
    for(uint i = 0; i < len; ++i)
    {
        char c = text[i];
        offset(&font->charmap[c], xoff, quad + i);
        xoff[0] += font->strides[c] * get_window()->inv_size[0];
    }

    font->nletter_frame += len;
}

 void dispatch_font(font* font)
 {
     if(!font->nletter_frame) return;
     r_bind_desc_sets(1, 1, &font->set);
     r_bind_vertex_buffers(1, &font->quad);
     r_draw(font->nletter_frame, 6 * font->nletter_frame, 0, 0);
     font->nletter_frame = 0;
 }