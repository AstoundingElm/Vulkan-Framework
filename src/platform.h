#pragma once

#include "defines.h"
#include "render_loop.h"
#include "assets.h"
#include "input.h"
#include <vulkan/vulkan_xcb.h>
#include <xcb/xcb.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <sys/time.h>
#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-x11.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include "keys.h"

struct xcb_types{
    
    xcb_screen_t *screen;
    xcb_window_t window;
    xcb_connection_t *connection;
    xcb_atom_t wm_delete_win;
};

static xcb_types xcb_context;

static xcb_gc_t gc_font_get (xcb_types * xcb_font_context,
                             const char       *font_name);

static void text_draw (xcb_types * xcb_text_context,
                       int16_t           x1,
                       int16_t           y1,
                       const char       *label);
static void
text_draw (xcb_types * xcb_text_context,
           int16_t           x1,
           int16_t           y1,
           const char       *label)
{
    xcb_void_cookie_t    cookie_gc;
    xcb_void_cookie_t    cookie_text;
    xcb_generic_error_t *error;
    xcb_gcontext_t       gc;
    uint8_t              length;
    
    length = strlen (label);
    
    gc = gc_font_get(&xcb_context, "7x13");
    
    cookie_text = xcb_image_text_8_checked (xcb_text_context->connection, length, xcb_text_context->window, gc,
                                            x1,
                                            y1, label);
    error = xcb_request_check (xcb_text_context->connection, cookie_text);
    if (error) {
        fprintf (stderr, "ERROR: can't paste text : %d\n", error->error_code);
        xcb_disconnect (xcb_text_context->connection);
        exit (-1);
    }
    
    cookie_gc = xcb_free_gc (xcb_text_context->connection, gc);
    error = xcb_request_check (xcb_text_context->connection, cookie_gc);
    if (error) {
        fprintf (stderr, "ERROR: can't free gc : %d\n", error->error_code);
        xcb_disconnect (xcb_text_context->connection);
        exit (-1);
    }
}

static xcb_gc_t
gc_font_get (xcb_types * xcb_font_context,
             const char       *font_name)
{
    uint32_t             value_list[3];
    xcb_void_cookie_t    cookie_font;
    xcb_void_cookie_t    cookie_gc;
    xcb_generic_error_t *error;
    xcb_font_t           font;
    xcb_gcontext_t       gc;
    uint32_t             mask;
    
    font = xcb_generate_id (xcb_font_context->connection);
    cookie_font = xcb_open_font_checked (xcb_font_context->connection, font,
                                         strlen (font_name),
                                         font_name);
    
    error = xcb_request_check (xcb_font_context->connection, cookie_font);
    if (error) {
        fprintf (stderr, "ERROR: can't open font : %d\n", error->error_code);
        xcb_disconnect (xcb_font_context->connection);
        
    }
    
    gc = xcb_generate_id (xcb_font_context->connection);
    mask = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND | XCB_GC_FONT;
    value_list[0] = xcb_font_context->screen->black_pixel;
    value_list[1] = xcb_font_context->screen->white_pixel;
    value_list[2] = font;
    cookie_gc = xcb_create_gc_checked (xcb_font_context->connection, gc, xcb_font_context->window, mask, value_list);
    error = xcb_request_check (xcb_font_context->connection, cookie_gc);
    if (error) {
        fprintf (stderr, "ERROR: can't create gc : %d\n", error->error_code);
        xcb_disconnect (xcb_font_context->connection);
        exit (-1);
    }
    
    cookie_font = xcb_close_font_checked (xcb_font_context->connection, font);
    error = xcb_request_check (xcb_font_context->connection, cookie_font);
    if (error) {
        fprintf (stderr, "ERROR: can't close font : %d\n", error->error_code);
        xcb_disconnect (xcb_font_context->connection);
        exit (-1);
    }
    
    return (xcb_gc_t)gc;
}
PINLINE void init_xwindow(xcb_types * xcb_context) {
    u32 WINDOW_WIDTH = 800;
    u32 WINDOW_HEIGHT = 500;
    xcb_atom_t wm_protocols;
    int screenp = 0;
    xcb_context->connection = xcb_connect(NULL, &screenp);
    if (xcb_connection_has_error(xcb_context->connection))
        printf("Failed to connect to X server using XCB.");
    
    xcb_screen_iterator_t iter =
        xcb_setup_roots_iterator(xcb_get_setup(xcb_context->connection));
    
    for (int s = screenp; s > 0; s--)
        xcb_screen_next(&iter);
    xcb_context->screen = iter.data;
    xcb_context->window = xcb_generate_id(xcb_context->connection);
    
    u32 eventMask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    
    u32 valueList[] = { 0, XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_STRUCTURE_NOTIFY};
    
    xcb_create_window(xcb_context->connection, XCB_COPY_FROM_PARENT, xcb_context->window, xcb_context->screen->root, 0, 0,
                      WINDOW_WIDTH, WINDOW_HEIGHT, 0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, xcb_context->screen->root_visual, eventMask, valueList);
    
    const char *applicationName = "Vk-Framework";
    
    xcb_change_property(
                        xcb_context->connection,
                        XCB_PROP_MODE_REPLACE,
                        xcb_context->window,
                        XCB_ATOM_WM_NAME,
                        XCB_ATOM_STRING,
                        8,
                        strlen(applicationName),
                        applicationName);
    
    xcb_map_window(xcb_context->connection, xcb_context->window);
    
    xcb_flush(xcb_context->connection);
    
    xcb_intern_atom_cookie_t wmDeleteCookie = xcb_intern_atom(xcb_context->connection, 0, strlen("WM_DELETE_WINDOW"), "WM_DELETE_WINDOW");
    
    xcb_intern_atom_cookie_t wmProtocolsCookie =
        xcb_intern_atom(xcb_context->connection, 0, strlen("WM_PROTOCOLS"), "WM_PROTOCOLS");
    
    xcb_intern_atom_reply_t *wmDeleteReply =
        xcb_intern_atom_reply(xcb_context->connection, wmDeleteCookie, NULL);
    
    xcb_intern_atom_reply_t *wmProtocolsReply =
        xcb_intern_atom_reply(xcb_context->connection, wmProtocolsCookie, NULL);
    
    xcb_context->wm_delete_win = wmDeleteReply->atom;
    
    wm_protocols = wmProtocolsReply->atom;
    
    xcb_change_property(xcb_context->connection, XCB_PROP_MODE_REPLACE, xcb_context->window,
                        wmProtocolsReply->atom, 4, 32, 1, &wmDeleteReply->atom);
    
    xcb_map_window(xcb_context->connection, xcb_context->window);
    
    xcb_flush(xcb_context->connection);
};



#define HEIGHT 100

PINLINE void xwindow_main_loop(xcb_types * xcb_loop_context) {
    xcb_generic_event_t *event;
    Display *display;
    xcb_client_message_event_t *cm;
    
    event = xcb_poll_for_event(xcb_loop_context->connection);
    if (event) {
        
        
        switch (event->response_type & ~0x80) {
            case XCB_KEY_PRESS:{ xcb_key_press_event_t *ev = (xcb_key_press_event_t *)event;
                
                break;};
            case XCB_KEY_RELEASE: {
                /*xcb_key_release_event_t *ev;
                
                ev = (xcb_key_release_event_t *)event;
                
                switch (ev->detail) {
                    
                case 9:
                
                */
                
                // Key press event - xcb_key_press_event_t and xcb_key_release_event_t are the same
                
                xcb_key_press_event_t *kb_event = (xcb_key_press_event_t *)event;
                bool pressed = event->response_type == XCB_KEY_PRESS;
                xcb_keycode_t code = kb_event->detail;
                KeySym key_sym = XkbKeycodeToKeysym(
                                                    display,
                                                    (KeyCode)code,  //event.xkey.keycode,
                                                    0,
                                                    code & ShiftMask ? 1 : 0);
                
                keys key = translate_keycode(key_sym);
                
                // Pass to the input subsystem for processing.
                //input_process_key(key, pressed);
                
            }
            case XCB_EXPOSE: {
                
                
            }
            
            case XCB_CLIENT_MESSAGE: {
                cm = (xcb_client_message_event_t *)event;
                
                if (cm->data.data32[0] == xcb_loop_context->wm_delete_win){
                    vulkan_running = false;
                    
                }
                break;
                
            };
            
            free(event);
            xcb_destroy_window(xcb_loop_context->connection, xcb_loop_context->window);
            
        }
    }
    
}

PINLINE VkXcbSurfaceCreateInfoKHR create_xcb_surface(const void * xcb_surface_info_pnext, VkXcbSurfaceCreateFlagsKHR xcb_surface_flags,
                                                     xcb_types * xcb_context) {
    VkXcbSurfaceCreateInfoKHR surface_create_info = {};
    surface_create_info.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    surface_create_info.pNext = xcb_surface_info_pnext;
    surface_create_info.flags = xcb_surface_flags;
    surface_create_info.connection = xcb_context->connection;
    surface_create_info.window = xcb_context->window;
    
    return surface_create_info;
};