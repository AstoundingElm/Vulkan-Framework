#pragma once

#include "defines.h"
#include "render_loop.h"
#include "assets.h"
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

bool is_running;

struct xcb_types {
    Display *display;
    xcb_connection_t *connection;
    xcb_window_t window;
    xcb_screen_t *screen;
    xcb_atom_t wm_protocols;
    xcb_atom_t wm_delete_win;
    u32 WINDOW_WIDTH = 800;
    u32 WINDOW_HEIGHT = 500;
    bool running = false;
};

static xcb_types xcb_context;

PINLINE void init_xwindow() {
    int screenp = 0;
    xcb_context.connection = xcb_connect(NULL, &screenp);
    if (xcb_connection_has_error(xcb_context.connection))
        printf("Failed to connect to X server using XCB.");
    
    xcb_screen_iterator_t iter =
        xcb_setup_roots_iterator(xcb_get_setup(xcb_context.connection));
    
    for (int s = screenp; s > 0; s--)
        xcb_screen_next(&iter);
    xcb_context.screen = iter.data;
    xcb_context.window = xcb_generate_id(xcb_context.connection);
    u32 eventMask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    u32 valueList[] = {xcb_context.screen->black_pixel, 0, XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_STRUCTURE_NOTIFY};
    xcb_create_window(xcb_context.connection, XCB_COPY_FROM_PARENT, xcb_context.window, xcb_context.screen->root, 0, 0,
                      xcb_context.WINDOW_WIDTH, xcb_context.WINDOW_HEIGHT, 0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, xcb_context.screen->root_visual, eventMask, valueList);
    const char *applicationName = "Vk-Framework";
    xcb_change_property(
                        xcb_context.connection,
                        XCB_PROP_MODE_REPLACE,
                        xcb_context.window,
                        XCB_ATOM_WM_NAME,
                        XCB_ATOM_STRING,
                        8,
                        strlen(applicationName),
                        applicationName);
    
    xcb_map_window(xcb_context.connection, xcb_context.window);
    xcb_flush(xcb_context.connection);
    xcb_intern_atom_cookie_t wmDeleteCookie = xcb_intern_atom(
                                                              xcb_context.connection, 0, strlen("WM_DELETE_WINDOW"), "WM_DELETE_WINDOW");
    
    xcb_intern_atom_cookie_t wmProtocolsCookie =
        xcb_intern_atom(xcb_context.connection, 0, strlen("WM_PROTOCOLS"), "WM_PROTOCOLS");
    xcb_intern_atom_reply_t *wmDeleteReply =
        xcb_intern_atom_reply(xcb_context.connection, wmDeleteCookie, NULL);
    xcb_intern_atom_reply_t *wmProtocolsReply =
        xcb_intern_atom_reply(xcb_context.connection, wmProtocolsCookie, NULL);
    xcb_context.wm_delete_win = wmDeleteReply->atom;
    
    xcb_context.wm_protocols = wmProtocolsReply->atom;
    
    xcb_change_property(xcb_context.connection, XCB_PROP_MODE_REPLACE, xcb_context.window,
                        wmProtocolsReply->atom, 4, 32, 1, &wmDeleteReply->atom);
    xcb_map_window(xcb_context.connection, xcb_context.window);
    xcb_flush(xcb_context.connection);
};

PINLINE void xwindow_main_loop() {
    xcb_context.running = true;
    xcb_generic_event_t *event;
    xcb_client_message_event_t *cm;
    
    //while (xcb_context.running) {
    while(event != 0){
        event = xcb_poll_for_event(xcb_context.connection);
        if(event == 0){break;}
        
        switch (event->response_type & ~0x80) {
            case XCB_CLIENT_MESSAGE: {
                cm = (xcb_client_message_event_t *)event;
                
                if (cm->data.data32[0] == xcb_context.wm_delete_win)
                    vulkan_running = false;
                xcb_context.running = false;
                
                break;
            }
            
            
            free(event);
            
            
        }
        xcb_destroy_window(xcb_context.connection, xcb_context.window);
        
        is_running = false;
    }
};


PINLINE VkXcbSurfaceCreateInfoKHR create_xcb_surface(const void * xcb_surface_info_pnext, VkXcbSurfaceCreateFlagsKHR xcb_surface_flags,
                                                     xcb_connection_t *connection, xcb_window_t window) {
    VkXcbSurfaceCreateInfoKHR surface_create_info = {};
    surface_create_info.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    surface_create_info.pNext = xcb_surface_info_pnext;
    surface_create_info.flags = xcb_surface_flags;
    surface_create_info.connection = connection;
    surface_create_info.window = window;
    
    return surface_create_info;
};