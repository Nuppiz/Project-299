#include "Common.h"
#include "Defines.h"
#include "Enums.h"
#include "Structs.h"

/* Object movement and collision detection */

extern Object object_array [];
extern Map map1;
extern Vec2 camera_offset;

void checkGridLoc(Object* obj) // circle's location on the grid
{   
    // calculated by dividing the circle's x/y location by square size
    obj->grid_loc.x = obj->position.x / SQUARE_SIZE;
    obj->grid_loc.y = obj->position.y / SQUARE_SIZE;
}

Vec2 checkPlayerLoc(Vec2 pos)
{
    // for camera offset
    Vec2 camera_loc;

    camera_loc.x = pos.x - (SCREEN_WIDTH / 2) - SQUARE_SIZE / 2;
    camera_loc.y = pos.y;

    return camera_loc;
}

void calcCameraOffset(Object* target)
{
    camera_offset = checkPlayerLoc(target->position);
    checkGridLoc(target);
}

int tileDetectColor(Vec2 pos, Map* map)
{
    int object_tile; // tile which the object is on (or attempting to be), i.e. array index number from grid_array
    uint8_t tile_colour;
    
    // calculate current grid position
    pos.x /= SQUARE_SIZE;
    pos.y /= SQUARE_SIZE;
    
    // check which grid_array index it corresponds to
    object_tile = (int)pos.y * map->width + (int)pos.x;
    
    tile_colour = map->collision[object_tile]; // check which colour is at that index
    
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

void moveCircle(Object* obj, Vec2 movement, Map* map)
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
        if (tileDetectColor(test_point_a, map) == WALL || tileDetectColor(test_point_b, map) == WALL)
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
        
        if (tileDetectColor(test_point_a, map) == WALL || tileDetectColor(test_point_b, map) == WALL)
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
        
        if (tileDetectColor(test_point_a, map) == WALL || tileDetectColor(test_point_b, map) == WALL)
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
        
        if (tileDetectColor(test_point_a, map) == WALL || tileDetectColor(test_point_b, map) == WALL)
        {
            obj->position.y = (obj->grid_loc.y + 1) * SQUARE_SIZE - obj->radius - 1;
            obj->velocity.y = 0.0;
        }
    }
    checkGridLoc(obj);
}

void calculateMovements(Map* map)
{
    int i = 0;
    
    // iterate through the object array
    while (i < Num_Objects)
    {
        moveCircle(&object_array[i], object_array[i].velocity, map); // check each circle for wall collisions
        // reduce object velocity with aerial drag
        object_array[i].velocity.y /= DRAG;
        object_array[i].velocity.x /= DRAG;
        i++;
    }
}

void collisionDetect(Object* object_a, Object* object_b, Map* map)
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
        moveCircle(object_a, u, map);
        
        // ...and for the second object they are flipped
        u.x = -u.x;
        u.y = -u.y;
        moveCircle(object_b, u, map);
    }
}

void collision(Map* map)
{
    int x;
    int i;
    
    // iterate through each object pair to see if they collide
    for (i = 0; i < Num_Objects; i++)
    {
        for (x = i; x < Num_Objects-1; x++)
        {
            collisionDetect(&object_array[i], &object_array[x+1], map);
        }
    }
    
    // also check that none of the objects are going beyond the screen boundaries
    // edgeDetect();
}