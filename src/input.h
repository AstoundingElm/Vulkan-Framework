#pragma once
#include "defines.h"
enum KEY_ID{
    
    W_KEY,
    A_KEY,
    S_KEY,
    D_KEY,
    KEY_COUNT,
};


struct Key{
    
    u32 half_transition_count;
    bool is_down;
};

struct Input_State{
    
    Key keys[KEY_COUNT];
    
};

PINLINE bool key_pressed_this_frame(Input_State * input, KEY_ID key){
    Key * k = &input->keys[key];
    bool result = (k->is_down && k->half_transition_count == 1) ||
    (k->half_transition_count > 1);
    return result;
    
};


PINLINE bool key_released_this_frame(Input_State * input, KEY_ID key){
    Key * k = &input->keys[key];
    bool result = (!k->is_down && k->half_transition_count == 1) ||
    (k->half_transition_count > 1);
    return result;
    
    
};


PINLINE bool key_is_down(Input_State * input, KEY_ID key){
    
    return input->keys[key].is_down;
    
};

