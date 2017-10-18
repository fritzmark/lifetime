/***************************************************************************
 *   Copyright (C) 2007 by Christoph Maciejewski   *
 *   chrmac@gmx.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <QtGui>
#include "lifetimecore.h"


LifetimeCore::LifetimeCore(LifetimeMainWindow *parent)
{
    this->parentGui = parent;
    this->pauseCounter = new QTime();
    this->sumOfPauses = 0;
    this->startTime = (QDateTime*)0;
    this->plannedEndTime = (QDateTime*)0;
    this->inPause = false;
}


LifetimeCore::~LifetimeCore()
{
}




/*!
    \fn LifetimeCore::startWorking()
 */
void LifetimeCore::startWorking()
{
    this->startTime = new QDateTime(QDateTime::currentDateTime());
    this->plannedEndTime = new QDateTime();

    if(this->parentGui->getTheConfig()->usePause) {
        this->sumOfPauses = 0;
    } else {
            this->sumOfPauses = qRound(this->parentGui->getTheConfig()->getPausePerDay(this->startTime->date().dayOfWeek() - 1) * 3600);
    }

    this->update();
}


/*!
    \fn LifetimeCore::stopWorking()
 */
void LifetimeCore::stopWorking()
{
    this->inPause = false;
}


/*!
    \fn LifetimeCore::pauseWorking()
 */
void LifetimeCore::startPause()
{
    //this->pauseCounter->restart();
    this->inPause = true;
    this->parentGui->getTheConfig()->pauseSince = QDateTime::currentDateTime();
    this->parentGui->getTheConfig()->inPause = true;
}


/*!
    \fn LifetimeCore::stopPause()
 */
void LifetimeCore::stopPause()
{
    this->inPause = false;
    this->parentGui->getTheConfig()->inPause = false;
}


/*!
    \fn LifetimeCore::update()
 */
void LifetimeCore::update()
{
    if(this->startTime == NULL)
        return;

    if(this->inPause) {
        // Since this will be called every second
        // simple add 1sec
        this->sumOfPauses++;
        this->parentGui->getTheConfig()->pauseSince = QDateTime::currentDateTime();
    }

    long tmpCurrTime = QDateTime::currentDateTime().toTime_t();
    int day = this->startTime->date().dayOfWeek();
    double workForToday = this->parentGui->getTheConfig()->getWorkPerDay(day - 1);

    this->plannedEndTime->setTime_t(this->startTime->toTime_t() + (long)(workForToday*3600) + this->sumOfPauses);
    this->restDuration = this->plannedEndTime->toTime_t() - tmpCurrTime;
    this->percentOfWork = 100 - (this->restDuration / ((workForToday*3600) + this->sumOfPauses))*100;
    if(this->percentOfWork > 100)
        this->percentOfWork = 100;

    long tmpSecsOfWork = tmpCurrTime - this->startTime->toTime_t() - this->sumOfPauses;
    double tmpSecsOfAutostop = LifetimeCore::timeToDecimal(this->parentGui->getTheConfig()->autoStopDuration, true)*3600;

    if((double)tmpSecsOfWork > tmpSecsOfAutostop)
        this->parentGui->autoStop();

}


/*!
    \fn LifetimeCore::isOvertime()
 */
bool LifetimeCore::isOvertime()
{
    if(this->restDuration < 0) {
        return true;
    }

    return false;
}


/*!
    \fn LifetimeCore::getStatusTring()
 */
QString LifetimeCore::getStatusString(bool seconds)
{
    QString out = "";
    if(this->startTime == NULL)
        return out;

    return LifetimeCore::decimalToTimeString(-1.0 * (double)restDuration / 3600, seconds);
}


/*!
    \fn LifetimeCore::updateStartTime(int ts)
 */
void LifetimeCore::updateStartTime(int ts)
{
    if(this->startTime == NULL)
        return;

    this->startTime->setTime_t(ts);
}


/*!
    \fn LifetimeCore::decimalToTime(double decimal)
 */
QTime LifetimeCore::decimalToTime(double decimal)
{
    int h = (int)decimal;
    int m = (int)((decimal - (double)h) * 60.0);
    int s = (int)((decimal - (double)h - ((double)m)/60.0) * 3600.0);
    int ms = (int)((decimal - (double)h - ((double)m)/60.0 - ((double)s)/3600.0) * 1000.0 * 3600.0);

    if(ms > 500) {
        s++;
        if(s > 59) {
            s = 0;
            m++;
            if(m > 59) {
                m = 0;
                h++;
            }
        }
    }

    QTime qt(h, m, s);
    return qt;
}


/*!
    \fn LifetimeCore::timeToDecimal(QTime qt)
 */
double LifetimeCore::timeToDecimal(QTime qt, bool ignoreSeconds)
{
    double h = (double)qt.hour();
    h += ((double)qt.minute()) / 60.0;

    if(!ignoreSeconds) {
        h += ((double)qt.second()) / 3600.0;
        h += ((double)qt.msec()) / 3600.0 / 1000.0;
    }

    return h;
}


/*!
    \fn LifetimeCore::decimalToTimeString(double decimal)
 */
QString LifetimeCore::decimalToTimeString(double decimal, bool seconds)
{
    QString out = "";
    QChar sign = '-';
    if(decimal >= 0)
        sign = '+';
    
    if(decimal < 0)
        decimal *= -1;

    int h = (int)decimal;
    int m = (int)((decimal - (double)h) * 60.0);
    int s = (int)((decimal - (double)h - ((double)m)/60.0) * 3600.0);
    int ms = (int)((decimal - (double)h - ((double)m)/60.0 - ((double)s)/3600.0) * 1000.0 * 3600.0);

    if(ms > 500) {
        s++;
        if(s > 59) {
            s = 0;
            m++;
            if(m > 59) {
                m = 0;
                h++;
            }
        }
    }
    
    QTime qt(0, m, s);    

    if(seconds)
        out += sign + QString::number(h) + ":" + qt.toString("mm:ss");
    else
        out += sign + QString::number(h) + ":" + qt.toString("mm");


    return out;
}
