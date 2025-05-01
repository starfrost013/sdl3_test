/* Player code */

#include <ddatest.hpp>
#include <core/core.hpp>

struct Player 
{
    float x;
    float y; 

    float spawn_x;
    float spawn_y; 

    float dir_x;
    float dir_y; 

    float plane_x;
    float plane_y; 

    float move_speed;               // squares per second
    float rotation_speed;           // radians per second
};

/* For test purposes */
extern Player player;