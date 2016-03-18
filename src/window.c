#include "window.h"
#include <windows.h>
#include <glad/glad.h>
#include <GL/wglext.h>

/* The window class unique identifier */
const char* window_class_name = "shader_view_window";

struct wgl_extension_funcs
{
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
    PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
    PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB;
    PFNWGLGETEXTENSIONSSTRINGEXTPROC wglGetExtensionsStringEXT;
    PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;
};

static void initialize_opengl_extensions(HWND hwnd, struct wgl_extension_funcs* funcs)
{
    /* Handle to device context */
    HDC hdc;

    /* Pixel format hint struct */
    PIXELFORMATDESCRIPTOR pfd = {0};

    /* Fill in the struct */
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA; /* The kind of framebuffer. RGBA or palette */
    pfd.cColorBits = 32;            /* Colordepth of the framebuffer */
    pfd.cDepthBits = 24;            /* Number of bits for the depthbuffer */
    pfd.cStencilBits = 8;           /* Number of bits for the stencilbuffer */
    pfd.iLayerType = PFD_MAIN_PLANE;

    /* Get the Device Context */
    hdc = GetDC(hwnd);

    /* Query for a matching pixel format */
    int pixel_fmt = ChoosePixelFormat(hdc, &pfd);

    /* Set window's pixel format to the above */
    SetPixelFormat(hdc, pixel_fmt, &pfd);

    /* Create opengl context and make it current */
    HGLRC temp_context = (HGLRC) wglCreateContext(hdc);
    wglMakeCurrent(hdc, temp_context);

    /* Load extension funcs */
    funcs->wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");
    funcs->wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress("wglSwapIntervalEXT");
    funcs->wglGetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC) wglGetProcAddress("wglGetPixelFormatAttribivARB");
    funcs->wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC) wglGetProcAddress("wglGetExtensionsStringEXT");
    funcs->wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC) wglGetProcAddress("wglGetExtensionsStringARB");

    /* Release temporary context resources */
    wglMakeCurrent(hdc, 0);
    wglDeleteContext(temp_context);
    ReleaseDC(hwnd, hdc);
}

static void create_opengl_context(struct window* wnd)
{
    /* Holder for the wgl extension functions */
    struct wgl_extension_funcs wgl = {0};

    /* Creates a temp context in order to load opengl functions */
    initialize_opengl_extensions(wnd->internal.hwnd, &wgl);

    /* Get the Device Context */
    wnd->internal.hdc = GetDC(wnd->internal.hwnd);

    /* Choose pixel format using extension */
    //wgl.wgtChoo

    /* Create context using extension and make it current */
    int ctx_attribs[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_FLAGS_ARB, 0,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };
    wnd->internal.context = (HGLRC) wgl.wglCreateContextAttribsARB(wnd->internal.hdc, 0, ctx_attribs);
    wglMakeCurrent(wnd->internal.hdc, wnd->internal.context);

    /* Load core profile functions */
    gladLoadGL();
    /* MessageBoxA(0, (char*) glGetString(GL_VERSION), "OPENGL VERSION", 0); */
}

int window_should_close(struct window* wnd)
{
    return wnd->should_close;
}

static LRESULT CALLBACK window_callback_func(HWND hh, UINT mm, WPARAM ww, LPARAM ll)
{
    struct window* wnd = (struct window*) GetWindowLongPtr(hh, GWLP_USERDATA);
    switch (mm)
    {
        case WM_CREATE:
        {
            struct window* window = (struct window*) ((CREATESTRUCT*)ll)->lpCreateParams;
            SetWindowLongPtr(hh, GWLP_USERDATA, (LONG_PTR) window);
            break;
        }
        case WM_CLOSE:
        {
            wnd->should_close = 1;
            break;
        }
        default:
            return DefWindowProc(hh, mm, ww, ll);
    }
    return 0;
}

static void register_window_class()
{
    /* The window class information structure */
    WNDCLASSEX wc;
    memset(&wc, 0, sizeof(WNDCLASSEX));

    /* Fill class info structure */
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = window_callback_func;
    wc.lpszClassName = window_class_name;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);

    /* Register the window class */
    RegisterClassEx(&wc);
}

static HWND create_window(struct window* wnd)
{
    /* The window handle */
    HWND hwnd;

    /* Register the window class */
    register_window_class();

    /* Create the Window */
    hwnd = CreateWindowExA(
        0,                                /* dwExStyle    */
        window_class_name,                /* lpClassName  */
        "ShaderView",                     /* lpWindowName */
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, /* dwStyle      */
        CW_USEDEFAULT,                    /* x            */
        CW_USEDEFAULT,                    /* y            */
        800,                              /* nWidth       */
        600,                              /* nHeight      */
        0,                                /* nWndParent   */
        0,                                /* nMenu        */
        GetModuleHandle(0),               /* hInstance    */
        wnd                               /* lpParam      */
    );

    return hwnd;
}

void open_window(struct window* window)
{
    /* Window handle and window message */
    HWND hwnd;

    /* Create the window instance */
    hwnd = create_window(window);

    /* Store the window handle */
    window->internal.hwnd = hwnd;

    /* Create opengl context */
    create_opengl_context(window);

    /* Keep it open */
    window->should_close = 0;
}

void poll_window_events(struct window* window)
{
    /* Window message var */
    MSG msg;

    /* Peek message loop, poll for events */
    while(PeekMessage(&msg, window->internal.hwnd, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void close_window(struct window* wnd)
{
    /* Release resources */
    wglMakeCurrent(wnd->internal.hdc, 0);
    wglDeleteContext(wnd->internal.context);
    ReleaseDC(wnd->internal.hwnd, wnd->internal.hdc);
}

void swap_buffers(struct window* wnd)
{
    SwapBuffers(wnd->internal.hdc);
}
