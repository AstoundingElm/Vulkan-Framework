#pragma once

#include "defines.h"
#include "shared_render_types.h"
u32 constexpr MAX_ENTITIES = 100;

struct Entity{
    
    Transform transform;
    
};

struct Game_State{
    
    u32 entity_count;
    Entity entities[MAX_ENTITIES];
};

static Entity * create_entity(Game_State * game_state,Transform transform ){
    Entity * e = 0;
    if(game_state->entity_count < MAX_ENTITIES){
        
        e = &game_state->entities[game_state->entity_count++];
        e->transform = transform;
    }else {
        
        printf("entity debug");
        
    }
    
    return e;
};

PINLINE void update_game(Game_State * gameState){
    
    // Does nothing
    for (uint32_t i = 0; i < gameState->entity_count; i++)
    {
        Entity *e = &gameState->entities[i];
        
        // This is frame rate dependent
        e->transform.xPos += 0.01f;
    }
    
};

PINLINE bool init_game(Game_State * gameState){
    
    for (uint32_t i = 0; i < 10; i++)
    {
        for (uint32_t j = 0; j < 10; j++)
        {
            Entity *e = create_entity(gameState, {i * 120.0f, j * 60.0f, 70.0f, 70.0f});
        }
    }
    
    return true;
    
};