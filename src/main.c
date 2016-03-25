#include "window.h"
#include "renderer.h"
#include "timer.h"
#include "font.h"
#include "assetload.h"

#define FPS 25

int main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    struct window window;
    struct render_context rctx;

    /* Init */
    open_window(&window);
    init_renderer(&rctx);

    /* Load font data */
    const char* fontfile = "ext/Beeb.ttf";
    long int fontfile_sz = get_filesize(fontfile);
    unsigned char* font_data_buf = malloc(fontfile_sz);
    read_file_to_mem(fontfile, font_data_buf, fontfile_sz);

    /* Parse font data */
    fontstash_t font_stash = init_fontstash();
    font_t font = load_font(font_stash, 18, font_data_buf, fontfile_sz);

    /* Main loop */
    time_val_t t1 = get_timer_value();
    while(!window_should_close(&window) && !is_key_pressed(&window, KEY_ESCAPE))
    {
        time_val_t t2 = get_timer_value();
        if (t2 - t1 >= 1000 / FPS)
        {
            poll_window_events(&window);
            render(&rctx);
            draw_text(
                font_stash, font,
                "Ninja cow", 0, 0,
                0.0f, 0.0f, 1.0f
            );
            swap_buffers(&window);
        }
        else
            sleep((long)((1000 / FPS) - (t2 - t1)));
        t1 = t2;
    }

    /* Release font resources */
    free(font_data_buf);
    free_fontstash(font_stash);

    /* Shutdown */
    destroy_renderer(&rctx);
    close_window(&window);

    return 0;
}
