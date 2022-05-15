#include "Common.h"
#include "Defines.h"
#include "Enums.h"
#include "Structs.h"

/* Object movement and collision detection */

extern Object object_array [];
extern uint8_t grid_array [];

void checkGridLoc(Object* obj) // circle's location on the grid
{   
    // calculated by dividing the circle's x/y location by square size
    obj->grid_loc.x = obj->position.x / SQUARE_SIZE;
    obj->grid_loc.y = obj->position.y / SQUARE_SIZE;
}

Vec2_int checkPlayerLoc(Vec2 pos)
{
    // for camera offset
    Vec2_int player_location;

    return player_location;
}

int tileDetectColor(Vec2 pos)
{
    int object_tile; // tile which the object is on (or attempting to be), i.e. array index number from grid_array
    uint8_t tile_colour;
    
    // calculate current grid position
    pos.x /= SQUARE_SIZE;
    pos.y /= SQUARE_SIZE;
    
    // check which grid_array index it corresponds to
    object_tile = (int)pos.y * NUM_COLUMNS + (int)pos.x;
    
    tile_colour = grid_array[object_tile]; // check which colour is at that index
    
    return tile_colour; // return said colour
}

void edgeDetect()
{
    int i = 0;
    while (i < Num_Objects)
    {
        if (object_array[i].position.x - object_array[i].radius <= 0) // left edge
        {
            object_array[i].position.x = 0 + object_array[i].radius;
            object_array[i].velocity.x = 0.0;
        }
        
        if (object_array[i].position.y - object_array[i].radius <= 0) // top edge
        {
            object_array[i].position.y = 0 + object_array[i].radius;
            object_array[i].velocity.y = 0.0;
        }
        
        if (object_array[i].position.x + object_array[i].radius >= (SCREEN_WIDTH - 1)) // right edge
        {
            object_array[i].position.x = (SCREEN_WIDTH - 1) - object_array[i].radius;
            object_array[i].velocity.x = 0.0;
        }
        
        if (object_array[i].position.y + object_array[i].radius >= (SCREEN_HEIGHT - 1)) // bottom edge
        {
            object_array[i].position.y = (SCREEN_HEIGHT - 1) - object_array[i].radius;
            object_array[i].velocity.y = 0.0;
        }
        i++;
    }
}

void moveCircle(Object* obj, Vec2 movement)
{
    // collision box around the object
    Vec2 test_point_a;
    Vec2 test_point_b;
    
    checkGridLoc(obj);
    
    if (movement.x > 0) // if moving to the right
    {
        obj->position.x += movement.x; // temporarily move the object
        // test_point_a = top right corner
        test_point_a.x = obj->position.x + obj->radius;
        test_point_a.y = obj->position.y - obj->radius;
        // test_point_b = bottom right corner
        test_point_b.x = obj->position.x + obj->radius;
        test_point_b.y = obj->position.y + obj->radius;
        
        // if the movement would result in the object moving inside of a wall...
        if (tileDetectColor(test_point_a) == WALL || tileDetectColor(test_point_b) == WALL)
        {
            // ...cancel movement and set velocity to 0
            obj->position.x = (obj->grid_loc.x + 1) * SQUARE_SIZE - obj->radius - 1;
            obj->velocity.x = 0.0;
        }
    }
    else if (movement.x < 0) // if moving to the left
    {
        obj->position.x += movement.x;
        // test_point_a = top left corner
        test_point_a.x = obj->position.x - obj->radius;
        test_point_a.y = obj->position.y - obj->radius;
        // test_point_b = bottom left corner
        test_point_b.x = obj->position.x - obj->radius;
        test_point_b.y = obj->position.y + obj->radius;
        
        if (tileDetectColor(test_point_a) == WALL || tileDetectColor(test_point_b) == WALL)
        {
            obj->position.x = obj->grid_loc.x * SQUARE_SIZE + obj->radius;
            obj->velocity.x = 0.0;
        }
    }

    if (movement.y < 0) // if moving towards the top
    {
        obj->position.y += movement.y;
        // test_point_a = top right corner
        test_point_a.x = obj->position.x + obj->radius;
        test_point_a.y = obj->position.y - obj->radius;
        // test_point_b = top left corner
        test_point_b.x = obj->position.x - obj->radius;
        test_point_b.y = obj->position.y - obj->radius;
        
        if (tileDetectColor(test_point_a) == WALL || tileDetectColor(test_point_b) == WALL)
        {
            obj->position.y = obj->grid_loc.y * SQUARE_SIZE + obj->radius;
            obj->velocity.y = 0.0;
        }
    }
    else if (movement.y > 0) // if moving towards the bottom
    {
        obj->position.y += movement.y;
        // test_point_a = bottom right corner
        test_point_a.x = obj->position.x + obj->radius;
        test_point_a.y = obj->position.y + obj->radius;
        // test_point_b = bottom left corner
        test_point_b.x = obj->position.x - obj->radius;
        test_point_b.y = obj->position.y + obj->radius;
        
        if (tileDetectColor(test_point_a) == WALL || tileDetectColor(test_point_b) == WALL)
        {
            obj->position.y = (obj->grid_loc.y + 1) * SQUARE_SIZE - obj->radius - 1;
            obj->velocity.y = 0.0;
        }
    }
    checkGridLoc(obj);
}

void calculateMovements()
{
    int i = 0;
    
    // iterate through the object array
    while (i < Num_Objects)
    {
        moveCircle(&object_array[i], object_array[i].velocity); // check each circle for wall collisions
        // reduce object velocity with aerial drag
        object_array[i].velocity.y /= DRAG;
        object_array[i].velocity.x /= DRAG;
        i++;
    }
}

void collisionDetect(Object* object_a, Object* object_b)
{
    float distance_x;
    float distance_y;
    float distance;
    float collision_depth;
    int tile_colour;
    Vec2 u; // how much each object moves in case of a collision
    
    distance_x = object_a->position.x - object_b->position.x;  // x-distance between the two objects
    distance_y = object_a->position.y - object_b->position.y;  // y-distance between the two objects
    
    // actual distance between the two objects (hypotenuse of the x and y distances)
    distance = sqrt((distance_x * distance_x) + (distance_y * distance_y));
    
    // if distance is less than combined radiuses
    if (distance < (object_a->radius + object_b->radius))
    {
        // calculate how much the objects are "inside" each other
        collision_depth = (object_a->radius + object_b->radius) - distance;
        
        //each object is moved for half of that
        u.x = (distance_x/distance) * (collision_depth/2);
        u.y = (distance_y/distance) * (collision_depth/2);
        
        // first object gets the values as is...
        moveCircle(object_a, u);
        
        // ...and for the second object they are flipped
        u.x = -u.x;
        u.y = -u.y;
        moveCircle(object_b, u);
    }
}

void collision()
{
    int x;
    int i;
    
    // iterate through each object pair to see if they collide
    for (i = 0; i < Num_Objects; i++)
    {
        for (x = i; x < Num_Objects-1; x++)
        {
            collisionDetect(&object_array[i], &object_array[x+1]);
        }
    }
    
    // also check that none of the objects are going beyond the screen boundaries
    edgeDetect();
}