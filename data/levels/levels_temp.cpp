#include <data/levels/levels_temp.hpp>

/* Test Level 01 - Just a box! */
uint8_t level_temp_01[] = 
{ 
    16, 16,                                                                                         // Size
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,             // Line0
    1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1,             // Line1
    1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1,             // Line2                 
    1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1,             // Line3
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1,             // Line4
    1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1,             // Line5
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1,             // Line6
    1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1,             // Line7
    1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1,             // Line8
    1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1,             // Line9
    1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1,             // Line10
    1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,             // Line11
    1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1,             // Line12
    1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,             // Line13
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,             // Line14
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              // Line15
    
};


/* Test Level 02 - Some obstacles! */
uint8_t level_temp_02[] = 
{ 
    8, 8,                                                       // Size
    1, 1, 1, 1, 1, 1, 1, 1, 1,     // Line0
    1, 1, 1, 1, 0, 0, 0, 1,     // Line1
    1, 1, 0, 0, 0, 0, 0, 1,     // Line2
    1, 0, 0, 0, 0, 1, 1, 1,     // Line3
    1, 0, 0, 0, 0, 0, 0, 1,     // Line4
    1, 0, 0, 0, 1, 1, 1, 1,     // Line5
    1, 0, 0, 0, 1, 1, 1, 1,     // Line6
    1, 1, 1, 1, 1, 1, 1, 1,     // Line7
};

/* Temporary function to get a level data array */
uint8_t* LevelTemp_GetLevelDataArray()
{
    uint8_t* level_data = level_temp_01;

    if (game.info.level_id == 1)
        level_data = level_temp_02;

    return level_data;
}

// Get index for level position
uint32_t Level_GetIndexForPosition(int32_t x, int32_t y)
{
    uint8_t* level_data = LevelTemp_GetLevelDataArray();

    // do something if were out of bounds
    if (x < 0
    || y < 0
    || x >= level_data[LEVEL_HEADER_SIZE_X]
    || y >= level_data[LEVEL_HEADER_SIZE_Y])
    {
        return 0;
    }

    return (level_data[LEVEL_HEADER_SIZE_Y] * y) + x + LEVEL_HEADER_SIZE;
}
