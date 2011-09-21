/***********************************************************************
*Copyright 2010-20XX by 7ymekk
*
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*
*   @author 7ymekk (7ymekk@gmail.com)
*
************************************************************************/


#ifndef ANIMATION_H
#define ANIMATION_H

#include <QtGui>

#define UP_TO_DOWN 0
#define DOWN_TO_UP 1
#define LEFT_TO_RIGHT 2
#define RIGHT_TO_LEFT 3

class Animation
{
public:
    Animation();
    void setDuration(int duration);
    void setDirection(int direction);
    void setAnimationCurve(int animationCurve);
    bool isRunning();
    void End();
    void setPrameters(int duration,int direction,int animationCurve);
    void start(QWidget *target,QWidget *current);
    QPropertyAnimation animation;
    QPropertyAnimation animation2;

private:
    int duration;
    int direction;
    int animationCurve;
};

#endif // ANIMATION_H
