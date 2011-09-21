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


#include "animation.h"

Animation::Animation()
{
}

void Animation::setAnimationCurve(int animationCurve)
{
    this->animationCurve=animationCurve;
}

void Animation::setDirection(int direction)
{
    this->direction=direction;
}

void Animation::setDuration(int duration)
{
    this->duration=duration;
}

void Animation::setPrameters(int duration,int direction,int animationCurve)
{
    this->animationCurve=animationCurve;
    this->direction=direction;
    this->duration=duration;
}

void Animation::start(QWidget *target,QWidget *current)
{
    if (this->isRunning())
        this->End();

    target->setVisible(true);

    animation.setDuration(duration);
    animation2.setDuration(duration);

    animation.setPropertyName("geometry");
    animation2.setPropertyName("geometry");

    animation2.setTargetObject(target);
    animation.setTargetObject(current);

    if (this->direction==UP_TO_DOWN)
    {
        animation2.setStartValue(current->geometry().translated(0,-current->height()));
        animation2.setEndValue(current->geometry());

        animation.setStartValue(current->geometry());
        animation.setEndValue(current->geometry().translated(0,current->height()));
    }
    else if (this->direction==DOWN_TO_UP)
    {
        animation2.setStartValue(current->geometry().translated(0,current->height()));
        animation2.setEndValue(current->geometry());

        animation.setStartValue(current->geometry());
        animation.setEndValue(current->geometry().translated(0,-current->height()));
    }
    else if (this->direction==LEFT_TO_RIGHT)
    {
        animation2.setStartValue(current->geometry().translated(-current->width(),0));
        animation2.setEndValue(current->geometry());

        animation.setStartValue(current->geometry());
        animation.setEndValue(current->geometry().translated(current->width(),0));
    }
    else if (this->direction==RIGHT_TO_LEFT)
    {
        animation2.setStartValue(current->geometry().translated(current->width(),0));
        animation2.setEndValue(current->geometry());

        animation.setStartValue(current->geometry());
        animation.setEndValue(current->geometry().translated(-current->width(),0));
    }

    animation.setEasingCurve((QEasingCurve::Type)(animationCurve));
    animation2.setEasingCurve((QEasingCurve::Type)(animationCurve));

    animation.start();
    animation2.start();
}

bool Animation::isRunning()
{
    if ((this->animation.state() == QAbstractAnimation::Running) || (this->animation2.state() == QAbstractAnimation::Running))
        return true;
    return false;
}

void Animation::End()
{
    animation.setCurrentTime(animation.duration());
    animation2.setCurrentTime(animation.duration());
}
