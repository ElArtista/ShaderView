#include "window.h"
#include "renderer.h"
#include "timer.h"

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

    /* Main loop */
    time_val_t t1 = get_timer_value();
    while(!window_should_close(&window) && !is_key_pressed(&window, KEY_ESCAPE))
    {
        time_val_t t2 = get_timer_value();
        if (t2 - t1 >= 1000 / FPS)
        {
            poll_window_events(&window);
            render(&rctx);
            swap_buffers(&window);
        }
        else
            sleep((long)((1000 / FPS) - (t2 - t1)));
        t1 = t2;
    }

    /* Shutdown */
    destroy_renderer(&rctx);
    close_window(&window);

    return 0;
}
