#include "font.h"

char* dump_file(const char* path, uint* size)
{
    FILE* code = fopen(path, "r");
    fseek(code, 0, SEEK_END);
    *size = ftell(code);
    rewind(code);
    char* file = calloc(*size, 1);
    fread(file, 1, *size, code);
    fclose(code);
    return file;
}

letter process_char(float x, float y, float w, float h, float ox, float oy, float tx, float ty)
{
    float x0 = x;
    float y0 = y;
    float x1 = x + w;
    float y1 = y + h;
    float cx0 = ox;
    float cy0 = oy;
    float cx1 = ox + w;
    float cy1 = oy + h;
    x0 /= tx;
    y0 /= ty;
    x1 /= tx;
    y1 /= ty;

    vec8 v0 = { cx0, cy0, x0, y0, cx1, cy0, x1, y0 };
    vec8 v1 = { cx0, cy1, x0, y1, cx1, cy0, x1, y0 };
    vec8 v2 = { cx0, cy1, x0, y1, cx1, cy1, x1, y1 };
    letter re = { v0, v1, v2};

    return re;
}

void read_char(font* font)
{
    uint id;
    char* token;
    float x, y, w, h, ox, oy, stride;
    token = strtok(0, " \t\r\n\v\f");   //char
    token = strtok(0, " \t\r\n\v\f");   //id=32
    id = atoi(token + strlen("id="));
    token = strtok(0, " \t\r\n\v\f");   //x=0
    x = atof(token + strlen("x="));
    token = strtok(0, " \t\r\n\v\f");   //y=0
    y = atof(token + strlen("y="));
    token = strtok(0, " \t\r\n\v\f");   //width=0
    w = atof(token + strlen("width="));
    token = strtok(0, " \t\r\n\v\f");   //height=0
    h = atof(token + strlen("height="));
    token = strtok(0, " \t\r\n\v\f");   //xoffset=0
    ox = atof(token + strlen("xoffset="));
    token = strtok(0, " \t\r\n\v\f");   //yoffset=46
    oy = atof(token + strlen("yoffset="));
    token = strtok(0, " \t\r\n\v\f");   //xadvance=50
    font->strides[id] = atof(token + strlen("xadvance="));
    token = strtok(0, " \t\r\n\v\f");   //page=0
    token = strtok(0, " \t\r\n\v\f");   //chnl=0

    font->charmap[id] = process_char(x, y, w, h, ox, oy, font->width, font->height);
}

void parse_file(char* path, font* info)
{
    uint size;
    char* file = dump_file(path, &size);
    char* token = strtok(file, " \t\r\n\v\f");
    uint plen = strlen(path);
    char cwd[256] = {};
    strcpy(cwd, path);
    for(int i = plen - 1; i >= 0 ; --i)
    {
        if(cwd[i] == '/' || cwd[i] == '\\') 
        {
            cwd[i + 1] = 0;
            break;
        }
        if(i == 0) cwd[0] = 0;
    }

    while(strcmp(token, "chars"))
    {
        if(strncmp(token, "lineHeight", strlen("lineHeight")) == 0)
            info->lineHeight = atof(token + strlen("lineHeight="));
        if(strncmp(token, "scaleW", strlen("scaleW")) == 0)
            info->width = atof(token + strlen("scaleW="));
        if(strncmp(token, "scaleH", strlen("scaleH")) == 0)
            info->height = atof(token + strlen("scaleH="));
        if(strncmp(token, "file", strlen("file")) == 0)
        {
            strcat(cwd, token + strlen("file= "));
            cwd[strlen(cwd) - 1] = 0;
        }
        token = strtok(0, " \t\r\n\v\f");
    }
    token = strtok(0, " \t\r\n\v\f");
    info->nChars = atoi(token + strlen("count="));

    info->atlas = create_texture(cwd);
    for(int i = 0; i < info->nChars; ++i)
    {
        read_char(info);
    }

    free(file);
}

font* create_font(char* path, desc_set set)
{ 
    font* info = calloc(sizeof(font), 1);
    parse_file(path, info);
    info->quad = create_mapped_buffer(usage_vertex, sizeof(letter) * 1024);
    set_bind_texture(set, &info->atlas, 0);
    info->set = set;
    return info;
}
