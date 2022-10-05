#pragma once


struct int_vec2{
    
    
    union{
        
        struct{
            float r;
            float g;
            
            
        };
        
        struct{
            
            float x;
            float y;
            
        };
        
    };
    
    bool operator==(int_vec2 other){
        
        return r == other.r && g == other.g;
    }
};

struct vec2{
    
    float x;
    float y;
};


struct vec4{
    
    
    union{
        
        struct{
            float r;
            float g;
            float b;
            float a;
            
            
        };
        
        struct{
            
            float x;
            float y;
            float z;
            float w;
            
        };
        
    };
    
    bool operator==(vec4 other){
        
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }
};

