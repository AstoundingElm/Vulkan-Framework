#pragma once


struct PVec2
{
    float x;
    float y;
    
    PVec2 operator/(float scalar)
    {
        return PVec2{x / scalar, y / scalar};
    }
    
    PVec2 operator*(float scalar)
    {
        return PVec2{x * scalar, y * scalar};
    }
    
    PVec2 operator+(PVec2 other)
    {
        return PVec2{x + other.x, y + other.y};
    }
    
    PVec2 operator-(PVec2 other)
    {
        return PVec2{x - other.x, y - other.y};
    }
    
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

