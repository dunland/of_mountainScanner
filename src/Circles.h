#pragma once

#include "ofMain.h"
#include "settings.h"

class Circle
{
public:

    Circle(int x_pos, int y_pos)
    {
        xpos = x_pos;
        ypos = y_pos;
    }


    int xpos, ypos;
    float radius = CIRCLES_CREATION_STEP;
    bool bKillMe = false;

    void draw();

    void update_radius();
};