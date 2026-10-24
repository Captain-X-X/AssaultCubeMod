#include "cube.h"
#include "vert_animation.h"

/*
* ==========
* anpos
* ==========
*/
void anpos::setframes(const animstate &as)
{
    int time = lastmillis-as.basetime;
    fr1 = (int)(time/as.speed); // round to full frames
    t = (time-fr1*as.speed)/as.speed; // progress of the frame, value from 0.0f to 1.0f
    ASSERT(t >= 0.0f);
    if(as.anim&ANIM_LOOP)
    {
        fr1 = fr1%as.range+as.frame;
        fr2 = fr1+1;
        if(fr2>=as.frame+as.range) fr2 = as.frame;
    }
    else
    {
        fr1 = Maths::min(fr1, as.range-1)+as.frame;
        fr2 = Maths::min(fr1+1, as.frame+as.range-1);
    }
    if(as.anim&ANIM_REVERSE)
    {
        fr1 = (as.frame+as.range-1)-(fr1-as.frame);
        fr2 = (as.frame+as.range-1)-(fr2-as.frame);
    }
}
