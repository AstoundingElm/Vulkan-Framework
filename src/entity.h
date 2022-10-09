#pragma once

#include "defines.h"
#include "shared_render_types.h"
#include "assets.h"
#include "input.h"
#include "ui.h"

#define MAX_ENTITIES 100
#define MAX_MATERIALS 100

enum components{
    
    COMPONENT_BALL = BIT(1),
    COMPONENT_LEFT_PADDLE = BIT(2),
    COMPONENT_RIGHT_PADDLE = BIT(3),
    
};

struct Entity{
    u32 component_mask;
    uint32_t material_index;
    PVec2 origin;
    PVec2 spriteOffset;
    PVec2 velocity;
    Transform transform;
    asset_type_id asset_id;
    
};

struct Game_State{
    
    u32 entity_count;
    Entity entities[MAX_ENTITIES];
    u32 material_count;
    Material materials[MAX_MATERIALS];
};

PINLINE bool has_component(Entity * e, components c){
    
    
    return e->component_mask & c;
    
};


PINLINE void add_component(Entity * e, components c){
    
    e->component_mask |=  c;
    
};


PINLINE void remove_component(Entity * e, components c){
    
    e->component_mask &= ~c;
    
};

//forward declares
u32 create_material(Game_State * game_state, asset_type_id asset_id, vec4 colour);

PINLINE u32 get_material(Game_State * game_state, asset_type_id asset_id, vec4 colour = {1.0f, 1.0f, 1.0f, 1.0f});


PINLINE Entity * create_entity(Game_State * game_state,Transform transform ){
    Entity * e = 0;
    if(game_state->entity_count < MAX_ENTITIES){
        
        e = &game_state->entities[game_state->entity_count++];
        e->transform = transform;
    }else {
        
        printf("entity debug");
        
    }
    
    return e;
};

PINLINE void update_game(Game_State * gameState, Input_State * input, f64 delta, UIstate * int_ui){
    
    f32 speed = 0.1f;
    
    do_text(int_ui, {100.0f, 100.0f}, "Pong from scratch"); 
    
    // Does nothing
    for (uint32_t i = 0; i < gameState->entity_count; i++)
    {
        
        Entity *e = &gameState->entities[i];
        //e->velocity = {};
        // This is frame rate dependent
        e->velocity.x = speed;
        
        e->transform.xPos += e->velocity.x * delta;
        
        
        if (has_component(e, COMPONENT_BALL))
        {
            if (e->transform.xPos + e->transform.sizeX > 900)
            {
                e->velocity.x = -e->velocity.x;
                
                e->transform.xPos -= 2 * (e->transform.xPos + e->transform.sizeX - input->screen_size.x);
            }
            
            if (e->transform.xPos < 0.0f)
            {
                e->velocity.x = -e->velocity.x;
                
                e->transform.xPos = -e->transform.xPos;
            }
            
            if (e->transform.yPos + e->transform.sizeX > 600)
            {
                e->velocity.y = -e->velocity.y;
                
                e->transform.yPos -= 2 * (e->transform.yPos + e->transform.sizeY - input->screen_size.y);
            }
            
            if (e->transform.yPos < 0.0f)
            {
                e->velocity.y = -e->velocity.y;
                
                e->transform.yPos = -e->transform.yPos;
            }
        }
        
        e->transform.xPos += e->velocity.x * delta;
        e->transform.yPos += e->velocity.y * delta;
        
    }
    
};

PINLINE bool init_game(Game_State * game_state){
    
    /*u32 counter = 0;
    for (uint32_t i = 0; i < 10; i++)
    {
        for (uint32_t j = 0; j < 10; j++)
        {
            
            float r  = (float)counter /100.0f;
            float g  = r;
            float b  = 1.0f - r;
            float a  = r;
            Entity *e = create_entity(gameState, {i * 120.0f, j * 60.0f, 70.0f, 70.0f, 0});
            e->transform.material_index = create_material(gameState, ASSET_SPRITE_DEFAULT, {r, g, b, a});
            
            counter++;
        }
    }
    */
    
    float paddle_size_x = 50.0f, paddle_size_y = 100.0f, ball_size = 50.0f;
    Entity * internal_e  = create_entity(game_state,{10.0f, 10.0f, paddle_size_x, paddle_size_y} );
    add_component(internal_e, COMPONENT_LEFT_PADDLE);
    internal_e->transform.material_index = get_material(game_state, ASSET_SPRITE_WHITE);
    
    internal_e = create_entity(game_state, {700.0f - paddle_size_x - 10.0f, 100.0f, paddle_size_x, paddle_size_y});
    add_component(internal_e, COMPONENT_RIGHT_PADDLE);
    internal_e->transform.material_index = get_material(game_state, ASSET_SPRITE_WHITE);
    
    
    internal_e= create_entity(game_state, {1000.0f/2.0f, 400.0f, ball_size, ball_size});
    
    add_component(internal_e, COMPONENT_BALL);
    internal_e->transform.material_index = get_material(game_state, ASSET_SPRITE_WHITE);
    internal_e->velocity = {1.0f, 1.0f};
    
    return true;
    
};