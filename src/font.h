#pragma once
#include "texture.h"
#include "buffer.h"
#include "vmath.h"
#include "descriptor.h"

typedef struct letter
{
    vec8 v[3];
} letter;

typedef struct font 
{
    desc_set set;
    mapped_buffer quad;
    texture atlas;
    float lineHeight;
	float width;
	float height;
	int nChars;

    letter      charmap[128];
    float       strides[128];

    uint nletter_frame;
} font;

 font* create_font(char* file, desc_set set);

void draw_text(font* font, const char* text, vec2 pos);

 void dispatch_font(font* font);