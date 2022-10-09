#pragma once

#include "defines.h"

#define MAX_LABELS 100

struct Label{
    
    char * text;
    PVec2 position;
    
};

struct UIID{
    
    u32 actualID;
    u32 layer;
    
    
};

struct UIstate{
    UIID active;
    UIID hotThisFrame;
    UIID hotLadtFrame;
    u32 labelCount;
    Label labels[MAX_LABELS];
    
};

struct UIElement{
    
    UIID ID;
    Rect rectangle;
    asset_type_id assetTypeId;
    u32 animationIndex;
    char * text;
};

PINLINE void update_ui(UIstate * uiState){
    
    uiState->labelCount = 0;
};

PINLINE void do_text(UIstate * uiState, PVec2 position,  char * text){
    if(uiState->labelCount < MAX_LABELS){
        
        Label * l = &uiState->labels[uiState->labelCount++];
        l->text = text;
        l->position = position;
    }
    else{
        
        //update_ui();
        //uiState->labelCount = 0;
        printf("reaches amxium amountof albels");
        
    }
};

/*PINLINE bool do_button(){
    
    bool result = false;
    
    if(is_active(ID)){
        
        if(key_released_this_frame(input, KEY_LEFT_MOUSE)){
            
            
            if(point_in_rect(mousePos, rect)){
                
                result = true;
                
            }
            
        }
        
        
        
    }
    
    else if(is_hot(ID){
                
                if(key_pressed_this_frame(input, KEY_LEFT_MOUSE)){
                    
                    set_active(ID);
                    
                }
            }
            
            return result;
            if(optionA){
                
                if(do_button()){
                    
                    
                    
                }
                
                
            }else{
                
                
            }
            
            
};*/