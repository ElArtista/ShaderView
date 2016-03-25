/*********************************************************************************************************************/
/*                                                  /===-_---~~~~~~~~~------____                                     */
/*                                                 |===-~___                _,-'                                     */
/*                  -==\\                         `//~\\   ~~~~`---.___.-~~                                          */
/*              ______-==|                         | |  \\           _-~`                                            */
/*        __--~~~  ,-/-==\\                        | |   `\        ,'                                                */
/*     _-~       /'    |  \\                      / /      \      /                                                  */
/*   .'        /       |   \\                   /' /        \   /'                                                   */
/*  /  ____  /         |    \`\.__/-~~ ~ \ _ _/'  /          \/'                                                     */
/* /-'~    ~~~~~---__  |     ~-/~         ( )   /'        _--~`                                                      */
/*                   \_|      /        _)   ;  ),   __--~~                                                           */
/*                     '~~--_/      _-~/-  / \   '-~ \                                                               */
/*                    {\__--_/}    / \\_>- )<__\      \                                                              */
/*                    /'   (_/  _-~  | |__>--<__|      |                                                             */
/*                   |0  0 _/) )-~     | |__>--<__|     |                                                            */
/*                   / /~ ,_/       / /__>---<__/      |                                                             */
/*                  o o _//        /-~_>---<__-~      /                                                              */
/*                  (^(~          /~_>---<__-      _-~                                                               */
/*                 ,/|           /__>--<__/     _-~                                                                  */
/*              ,//('(          |__>--<__|     /                  .----_                                             */
/*             ( ( '))          |__>--<__|    |                 /' _---_~\                                           */
/*          `-)) )) (           |__>--<__|    |               /'  /     ~\`\                                         */
/*         ,/,'//( (             \__>--<__\    \            /'  //        ||                                         */
/*       ,( ( ((, ))              ~-__>--<_~-_  ~--____---~' _/'/        /'                                          */
/*     `~/  )` ) ,/|                 ~-_~>--<_/-__       __-~ _/                                                     */
/*   ._-~//( )/ )) `                    ~~-'_/_/ /~~~~~~~__--~                                                       */
/*    ;'( ')/ ,)(                              ~~~~~~~~~~                                                            */
/*   ' ') '( (/                                                                                                      */
/*     '   '  `                                                                                                      */
/*********************************************************************************************************************/
#ifndef _FONT_H_
#define _FONT_H_

#include "vecmath.h"
#include "glad/glad.h"

/* Opaque datatype that holds loaded fonts */
typedef struct fontstash* fontstash_t;
/* Opaque datatype that represents a loaded font */
typedef struct font* font_t;

/* Constructs a fontstash instance */
fontstash_t init_fontstash();
/* Deallocates a fontstash instance resources */
void free_fontstash(fontstash_t);
/* Loads a font from given memory buffer into the fontstash */
font_t load_font(fontstash_t fs, const float pixel_height, const unsigned char* buf, size_t buf_sz);
/* Unloads the given font from the fontstash */
void unload_font(fontstash_t fs, font_t font);

/* Returns the minimum size of memory buffer needed to render the given sentence with the given font */
size_t get_min_buf_size(font_t font, const char* text);

/*
  Renders text with the given font to a preallocated memory bitmap.
  Minimum size can be retrieved with get_bitmap_size function.
  Final bitmap width and height is filled in by the function

Sample:
    // Load font data
    const char* fontfile = "ext/visitor.ttf";
    long int fontfile_sz = get_filesize(fontfile);
    unsigned char* font_data_buf = malloc(fontfile_sz);
    read_file_to_mem(fontfile, font_data_buf, fontfile_sz);

    // Parse font data
    fontstash_t font_stash = init_fontstash();
    font_t font = load_font(font_stash, font_data_buf, fontfile_sz);

    // Render sample memory bitmap
    const char* text = "Crazy cows all over the place";
    unsigned char* tex_data;
    int width, height;
    tex_data = malloc(get_min_buf_size(font, text));
    draw_text_mem_buf(font, text, tex_data, &width, &height);

    // Write output
    stbi_write_png("out.png", width, height, 1, tex_data, width);

    // Release font resources
    free(tex_data);
    free(font_data_buf);
    free_fontstash(font_stash);
 */
void draw_text_mem_buf(font_t font, const char* text, unsigned char* buf, int* width, int* height);

/* Renders text to the given coordinates */
void draw_text(fontstash_t fs, font_t font, const char* string, float x, float y, float r, float g, float b);

#endif // ! _FONT_H_
