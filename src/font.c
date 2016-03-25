#include "font.h"
#include <stdlib.h>
#include <stdio.h>
#include <stb_truetype.h>
#include <glad/glad.h>
#include "vecmath.h"

/*
  Stores data about a certain glyph's codepoint,
  about it position and size in the glyph atlas
  and properties about its alignment when rendering
 */
struct font_glyph_info
{
    int codepoint;        /* Codepoint the glyph corresponds to */
    struct ivec2 offset;  /* Offset in atlas bitmap */
    struct ivec2 size;    /* Bounding box size */
    struct ivec2 bearing; /* Bearing */
    long advance;         /* Distance to next font */
};

/*
  Contains a texture atlas generated for a specific
  font and size, with its accompanied glyph info
 */
struct font
{
    /* The parsed font data as a bitmap */
    unsigned char* tex_data;
    int tex_width;
    int tex_height;

    /* Info for each glyph in bitmap */
    struct font_glyph_info* glyphs;
    size_t num_glyphs;
};

/*
  Holds various loaded fontsets
 */
struct fontstash
{
    font_t* fonts;
    size_t num_fonts;
};

/* =------------------------------------------------------------------------= */
/* Frees resources for a given font instance */
static void free_font(font_t font)
{
    free(font->tex_data);
    free(font->glyphs);
    free(font);
}

/*
  Parses given font file data to a corresponding font instance.
  Return value must be freed by the free_font function.
  This is usually done by the fontstore functions
 */
static font_t parse_font_to_mem_buf(const float pixel_height, const unsigned char* buf, size_t buf_sz)
{
    (void) buf_sz;
    /* Default values */
    const char min_glyph_cp = 32;
    const char max_glyph_cp = 127;
    const int num_glyphs = max_glyph_cp - min_glyph_cp;

    /*
     Each .ttf/.ttc file may have more than one font.
     Here we retrieve the first font offset.
     */
    int font_off = stbtt_GetFontOffsetForIndex(buf, 0);

    /* Prepare fontinfo struct */
    stbtt_fontinfo fi;
    if(!stbtt_InitFont(&fi, buf, font_off))
    {
        /* TODO: Error handling */
    }

    /* Calculate font scaling */
    float scale_factor = stbtt_ScaleForPixelHeight(&fi, pixel_height);

    /* The font stucture to be filled and returned */
    struct font* font = malloc(sizeof(struct font));

    /* Calculate the texture dimensions that will hold the rasterized data */
    int x0, y0, x1, y1;
    stbtt_GetFontBoundingBox(&fi, &x0, &y0, &x1, &y1);
    font->tex_width = (int)((x1 - x0) * num_glyphs * scale_factor);
    font->tex_height = (int)((y1 - y0) * scale_factor);

    /* Create the texture data buffer that will hold all the loaded glyphs */
    font->tex_data = malloc((font->tex_width * font->tex_height) * num_glyphs);
    memset(font->tex_data, 0, (font->tex_width * font->tex_height) * num_glyphs);

    /* Create the glyph info array */
    font->glyphs = malloc(num_glyphs * sizeof(struct font_glyph_info));
    font->num_glyphs = num_glyphs;
    memset(font->glyphs, 0, num_glyphs * sizeof(struct font_glyph_info));

    /* Get font ascent and descent */
    int ascent, descent;
    stbtt_GetFontVMetrics(&fi, &ascent, &descent, 0);

    /* Render fonts to single bitmap */
    int x = 0;
    for (char c = min_glyph_cp; c < max_glyph_cp; ++c)
    {
        /* The glyph info data structure for the current codepoint */
        struct font_glyph_info* glyph_info = font->glyphs + c - min_glyph_cp;

        /* Get bounding box for character */
        int ix0, iy0, ix1, iy1;
        stbtt_GetCodepointBitmapBox(&fi, c, scale_factor, scale_factor, &ix0, &iy0, &ix1, &iy1);

        /* Render character */
        size_t offset = ((int)(ascent * scale_factor) + iy0) * font->tex_width + (x + ix0);
        stbtt_MakeCodepointBitmap(&fi, font->tex_data + offset, ix1 - ix0, iy1 - iy0, font->tex_width, scale_factor, scale_factor, c);

        /* Fetch various metrics */
        int advance, left_side_bearing;
        stbtt_GetCodepointHMetrics(&fi, c, &advance, &left_side_bearing);
        int kern = stbtt_GetCodepointKernAdvance(&fi, c, c + 1);

        /* Store glyph info for later rendering */
        glyph_info->codepoint = c;
        glyph_info->advance = (int)(advance * scale_factor) + (int)(kern * scale_factor);
        glyph_info->size.x = ix1 - ix0;
        glyph_info->size.y = iy1 - iy0;
        glyph_info->bearing.x = (int)(left_side_bearing * scale_factor);
        glyph_info->bearing.y = iy1;

        /* Glyph position in font atlas */
        glyph_info->offset.x = x + ix0;
        glyph_info->offset.y = (int)(ascent * scale_factor) + iy0;

        /* Increase x position of the cursor */
        x += (int)((x1 - x0) * scale_factor);
    }
    return font;
}

/* =------------------------------------------------------------------------= */
/* Retrieves glyph info for the given codepoint in the given font */
static struct font_glyph_info* get_glyph(font_t font, int codepoint)
{
    for (size_t i = 0; i < font->num_glyphs; ++i)
        if (font->glyphs[i].codepoint == codepoint)
            return font->glyphs + i;
    return 0;
}

/* Helper function to assist copying a chunk of a bitmap to another */
static void copy_subbitmap(unsigned char* dest, unsigned char* src, int dest_width, int dest_height, int dest_stride, int src_stride)
{
    int i = 0, j = 0;
    for (i = 0; i < dest_height; ++i)
        for (j = 0; j < dest_width; ++j)
            dest[i * dest_stride + j] = *(src + i * src_stride + j);
}
/* =------------------------------------------------------------------------= */
/* Number of chars in text * width * height of common glyph bounding box in atlas */
size_t get_min_buf_size(font_t font, const char* text)
{
    size_t char_cnt = strlen(text);
    int stride = (int)(font->tex_width / font->num_glyphs);
    return stride * font->tex_height * char_cnt;
}

void draw_text_mem_buf(font_t font, const char* text, unsigned char* buf, int* width, int* height)
{
    /* Character count in given text */
    size_t char_cnt = strlen(text);
    /* X distance between two baked fonts in source bitmap */
    int stride = (int)(font->tex_width / font->num_glyphs);

    /* Destination width */
    *width = (int)(stride * char_cnt);
    /* Destination height */
    *height = font->tex_height;
    /* Clear destination buffer */
    memset(buf, 0, stride * font->tex_height * char_cnt);

    /* Render letter by letter */
    for (size_t i = 0; i < char_cnt; ++i)
    {
        /* Glyph info for current char */
        struct font_glyph_info* gi = get_glyph(font, text[i]);

        /* Perform copy operation from source to destination bitmap */
        copy_subbitmap(
            buf + i * stride,              /* Destination buffer */
            font->tex_data + gi->offset.x, /* Start of the current source location */
            stride,                        /* Destination width */
            font->tex_height,              /* Destination height */
            (int)(stride * char_cnt),      /* Destination stride */
            font->tex_width                /* Source stride */
        );
    }
}

/* =------------------------------------------------------------------------= */
/* Simple alloc function */
fontstash_t init_fontstash()
{
    fontstash_t fs = (fontstash_t) malloc(sizeof(struct fontstash));
    memset(fs, 0, sizeof(struct fontstash));
    return fs;
}

/* Releases also every font stored in the fontstore before killing it */
void free_fontstash(fontstash_t fs)
{
    for (size_t i = 0; i < fs->num_fonts; ++i)
        free_font(fs->fonts[i]);
    free(fs);
}

/* Simple vector resize and append */
font_t load_font(fontstash_t fs, const float pixel_height, const unsigned char* buf, size_t buf_sz)
{
    (void) buf_sz;

    /* Allocate space for new font */
    fs->num_fonts++;
    font_t* new_fonts = malloc(fs->num_fonts * sizeof(font_t));
    if (fs->fonts)
    {
        memcpy(new_fonts, fs->fonts, fs->num_fonts * sizeof(font_t));
        free(fs->fonts);
    }
    fs->fonts = new_fonts;

    /* Append new font */
    font_t font = parse_font_to_mem_buf(pixel_height, buf, buf_sz);
    fs->fonts[fs->num_fonts - 1] = font;

    return font;
}

/* Simple vector remove */
void unload_font(fontstash_t fs, font_t font)
{
    for (size_t i = 0; i < fs->num_fonts; ++i)
    {
        if (fs->fonts[i] == font)
        {
            /* Free removed font */
            free_font(font);

            /* Alloc new fonts array */
            fs->num_fonts--;
            font_t* new_fonts = malloc(fs->num_fonts * sizeof(font_t));

            /* Copy up to the found element from previous to new array */
            memcpy(new_fonts, fs->fonts, i * sizeof(font_t));

            /* Copy the rest elements from the previous to new array */
            memcpy(new_fonts + i, fs->fonts + i + 1, i * sizeof(font_t));

            /* Replace previous font array with new one */
            free(fs->fonts);
            fs->fonts = new_fonts;

            break;
        }
    }
}
/* =------------------------------------------------------------------------= */
/* Vertex shader for text rendering */
static const char* text_vertex_src = "                        \n\
#version 330 core                                             \n\
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex> \n\
out vec2 TexCoords;                                           \n\
                                                              \n\
uniform mat4 projection;                                      \n\
                                                              \n\
void main()                                                   \n\
{                                                             \n\
    TexCoords = vertex.zw;                                    \n\
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);     \n\
}                                                             \n\
";

/* Fragment shader for text rendering */
static const char* text_frag_src = "                                 \n\
#version 330 core                                                    \n\
in vec2 TexCoords;                                                   \n\
out vec4 color;                                                      \n\
                                                                     \n\
uniform sampler2D text;                                              \n\
uniform vec3 textColor;                                              \n\
                                                                     \n\
void main()                                                          \n\
{                                                                    \n\
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);  \n\
    color = vec4(textColor, 1.0) * sampled;                          \n\
}                                                                    \n\
";

struct text_color
{
    float r, g, b;
};

struct render_text_context
{
    GLuint shader_program;
    GLuint vao;
    GLuint vbo;
    GLuint glyph_texture_atlas;
};

static void render_text(struct render_text_context* rtc, font_t font, const char* string, float x, float y, struct text_color col)
{
    /* Store previous blending state */
    GLboolean blend = glIsEnabled(GL_BLEND);

    /* Store previous src alpha blend function */
    GLint srcAlphaBlendFunc;
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &srcAlphaBlendFunc);

    /* Enable blending */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* Setup program params */
    mat4x4 projection;
    mat4_ortho(0.0f, 800, 0.0f, 600, projection);
    glUseProgram(rtc->shader_program);
    glUniformMatrix4fv(glGetUniformLocation(rtc->shader_program, "projection"), 1, GL_FALSE, (float*)projection);
    glUniform3f(glGetUniformLocation(rtc->shader_program, "textColor"), col.r, col.g, col.b);
    glUniform1i(glGetUniformLocation(rtc->shader_program, "text"), 0);

    /* Bind texture atlas */
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rtc->glyph_texture_atlas);

    /* Iterate through all input characters */
    float cur_x = x;
    size_t text_len = strlen(string);
    for (size_t i = 0; i < text_len; ++i)
    {
        const char c = string[i];
        struct font_glyph_info* glyph = get_glyph(font, c);

        /* Baseline position */
        GLfloat x_pos = cur_x;
        GLfloat y_pos = y - glyph->bearing.y;

        /* Glyph size */
        GLfloat width = (GLfloat)glyph->size.x;
        GLfloat height = (GLfloat)glyph->size.y;

        /* Upper left point of glyph region in atlas texture */
        GLfloat atlas_xoffset = (GLfloat)glyph->offset.x;
        GLfloat atlas_yoffset = (GLfloat)glyph->offset.y;

        GLfloat vertices[4][4];
        /* Upper left */
        vertices[0][0] = x_pos;
        vertices[0][1] = y_pos + height;
        vertices[0][2] = atlas_xoffset / font->tex_width;
        vertices[0][3] = atlas_yoffset / font->tex_height;
        /* Lower left */
        vertices[1][0] = x_pos;
        vertices[1][1] = y_pos;
        vertices[1][2] = atlas_xoffset / font->tex_width;
        vertices[1][3] = (atlas_yoffset + height) / font->tex_height;
        /* Upper right */
        vertices[2][0] = x_pos + width;
        vertices[2][1] = y_pos + height;
        vertices[2][2] = (atlas_xoffset + width) / font->tex_width;
        vertices[2][3] = atlas_yoffset / font->tex_height;
        /* Lower right */
        vertices[3][0] = x_pos + width;
        vertices[3][1] = y_pos;
        vertices[3][2] = (atlas_xoffset + width) / font->tex_width;
        vertices[3][3] = (atlas_yoffset + height) / font->tex_height;

        /* Render glyph texture over quad */
        glBindBuffer(GL_ARRAY_BUFFER, rtc->vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * 4 * sizeof(GLfloat), vertices);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        /* Advance cursor for next glyph */
        cur_x += glyph->advance;
    }

    glBindVertexArray(rtc->vao);

    glBindVertexArray(0);
    glUseProgram(0);

    /* Restore blending options */
    glBlendFunc(GL_SRC_ALPHA, srcAlphaBlendFunc);
    if(blend)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
}

void draw_text(fontstash_t fs, font_t font, const char* string, float x, float y, float r, float g, float b)
{
    (void) fs;
    struct render_text_context rtc;

    /* Compile vertex shader */
    GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_shader, 1, &text_vertex_src, 0);
    glCompileShader(vert_shader);

    /* Compile fragment shader */
    GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, &text_frag_src, 0);
    glCompileShader(frag_shader);

    /* Link shader program */
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vert_shader);
    glAttachShader(shader_program, frag_shader);
    glLinkProgram(shader_program);
    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
    rtc.shader_program = shader_program;

    /* Setup vbo */
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    /* 2D quad (line strip) needs 4 vertices * 4 floats each (2 for pos + 2 for texCoords) */
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 4, 0, GL_DYNAMIC_DRAW);
    rtc.vbo = vbo;

    /* Setup vao */
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);
    rtc.vao = vao;

    /* Store current unpack alignment */
    GLint pixelStoreAlignment;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &pixelStoreAlignment);
    /* Disable 4 byte alignment because we use one byte per pixel */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    /* Setup texture atlas */
    GLuint glyph_atlas;
    glGenTextures(1, &glyph_atlas);
    glBindTexture(GL_TEXTURE_2D, glyph_atlas);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, font->tex_width, font->tex_height, 0, GL_RED, GL_UNSIGNED_BYTE, font->tex_data);
    rtc.glyph_texture_atlas = glyph_atlas;

    /* Re-enable byte alignment */
    glPixelStorei(GL_UNPACK_ALIGNMENT, pixelStoreAlignment);

    /* Render */
    struct text_color col;
    col.r = r;
    col.g = g;
    col.b = b;
    render_text(&rtc, font, string, x, y, col);

    /* Release resources */
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &glyph_atlas);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &vbo);
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao);
    glUseProgram(0);
    glDeleteProgram(shader_program);
}
