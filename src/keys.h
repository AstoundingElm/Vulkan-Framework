#pragma once

#define DEFINE_KEY(name, code) KEY_##name = code

typedef enum keys {
    
    DEFINE_KEY(SPACE, 0x20),
    DEFINE_KEY(HOME, 0x24),
    DEFINE_KEY(LEFT, 0x25),
    DEFINE_KEY(UP, 0x26),
    DEFINE_KEY(RIGHT, 0x27),
    DEFINE_KEY(DOWN, 0x28),
    DEFINE_KEY(W, 0x57),
    DEFINE_KEY(A, 0x41),
    DEFINE_KEY(D, 0x44),
    
    DEFINE_KEY(S, 0x53),
    
    KEYS_MAX_KEYS
} keys;

keys translate_keycode(u32 x_keycode) {
    switch (x_keycode) {
        
        case XK_space:
        return KEY_SPACE;
        case XK_Left:
        return KEY_LEFT;
        case XK_Up:
        return KEY_UP;
        case XK_Right:
        return KEY_RIGHT;
        case XK_Down:
        return KEY_DOWN;
        
        case XK_a:
        case XK_A:
        return KEY_A;
        return KEY_D;
        case XK_s:
        case XK_S:
        return KEY_S;;
        case XK_w:
        case XK_W:
        return KEY_W;
        
        default:
        return KEY_DOWN;
    }
}