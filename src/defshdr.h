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
#ifndef _DEF_SHDR_H_
#define _DEF_SHDR_H_

const char* def_vert_sh_src =
"#version 330 core                        \n\
in vec3 position;                         \n\
                                          \n\
void main(void)                           \n\
{                                         \n\
    gl_Position = vec4(position, 1.0f);   \n\
}";

const char* def_frag_sh_src =
"#version 330 core                                                     \n\
out vec4 color;                                                        \n\
                                                                       \n\
uniform float time;                                                    \n\
uniform vec2 resolution;                                               \n\
                                                                       \n\
const float INTERVAL = 2.0;                                            \n\
const float PI = 3.14159265358979323844;                               \n\
                                                                       \n\
float inside_polygon(vec2 pos, vec2 center, float r, float n, float s) \n\
{                                                                      \n\
    float theta = 2.*PI/n;                                             \n\
                                                                       \n\
    vec2 d = pos - center;                                             \n\
                                                                       \n\
    float a = mod(mod(atan(d.y, d.x) + s, 2.0 * PI), theta);           \n\
    float l = length(d);                                               \n\
                                                                       \n\
    float m = r * cos(0.5 * theta) / cos(a - 0.5 * theta);             \n\
    const float border = .001;                                         \n\
                                                                       \n\
    return smoothstep(m + border, m - border, l);                      \n\
}                                                                      \n\
                                                                       \n\
float wobble(vec2 pos)                                                 \n\
{                                                                      \n\
    vec2 d = pos;                                                      \n\
    float a = (atan(d.y, d.x) + PI / 2.0) / (2.0 * PI);                \n\
                                                                       \n\
    float l = .75;                                                     \n\
                                                                       \n\
    float t = mod(time, INTERVAL) / INTERVAL;                          \n\
    float o = t * (1. + l);                                            \n\
                                                                       \n\
    return smoothstep(o, o - l, a);                                    \n\
}                                                                      \n\
                                                                       \n\
float inside_triangle(vec2 pos, vec2 center, float r, float s)         \n\
{                                                                      \n\
    return inside_polygon(pos, center, wobble(center) * r, 3.0, s);    \n\
}                                                                      \n\
                                                                       \n\
float inside_triangles(vec2 pos, float r)                              \n\
{                                                                      \n\
    const float da = 2.0 * PI / 6.0;                                   \n\
    float a = 0.0;                                                     \n\
                                                                       \n\
    float v = 0.0;                                                     \n\
    for (int i = 0; i < 6; i++) {                                      \n\
        float c = cos(a);                                              \n\
        float s = sin(a);                                              \n\
                                                                       \n\
        vec2 d = vec2(c, s);                                           \n\
        vec2 n = vec2(-s, c);                                          \n\
                                                                       \n\
        vec2 o0 = (2./3.) * sqrt(3.) * d * r;                          \n\
        vec2 o1 = (5./6.) * sqrt(3.) * d * r;                          \n\
                                                                       \n\
        float r_triangle = 1.0 * r / sqrt(3.0);                        \n\
                                                                       \n\
        v += inside_triangle(pos, o0, r_triangle, a) +                 \n\
            inside_triangle(pos, o1 - n*.5*r, r_triangle, a + PI) +    \n\
            inside_triangle(pos, o1 + n*.5*r, r_triangle, a + PI);     \n\
                                                                       \n\
        a += da;                                                       \n\
    }                                                                  \n\
    return v;                                                          \n\
}                                                                      \n\
                                                                       \n\
void main()                                                            \n\
{                                                                      \n\
    const float radius = 20.;                                          \n\
                                                                       \n\
    vec2 pos = (gl_FragCoord.xy * 2. - resolution)                     \n\
                / min(resolution.x, resolution.y);                     \n\
                                                                       \n\
    float r0 = 0.25;                                                   \n\
    float r1 = 2.0 * r0;                                               \n\
                                                                       \n\
    float r = mix(r1, r0, mod(time, INTERVAL) / INTERVAL);             \n\
    float v = (inside_triangles(pos, r) +                              \n\
              inside_polygon(pos, vec2(0.0, 0.0), r, 6., PI/6.));      \n\
                                                                       \n\
    color = mix(vec4(.5, 1., 1., 1.), vec4(.0, .25, .25, 1.), v);      \n\
}                                                                      \n\
";

#endif // ! _DEF_SHDR_H_
