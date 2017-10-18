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
#ifndef LIFETIMECORE_H
#define LIFETIMECORE_H

#include <QDateTime>
#include "LifetimeMainWindow.h"

class LifetimeMainWindow;

/**
This class makes all the time logging and calculation jobs.

    @author Christoph Maciejewski <chrmac@gmx.de>
*/
class LifetimeCore {

public:
    LifetimeCore(LifetimeMainWindow* parent);
    ~LifetimeCore();
    void startPause();
    void stopWorking();
    void startWorking();
    void update();
    bool isOvertime();
    QString getStatusString(bool seconds=false);
    void stopPause();
    void updateStartTime(int ts);
    static QTime decimalToTime(double decimal);
    static double timeToDecimal(QTime qt, bool ignoreSeconds);
    static QString decimalToTimeString(double decimal, bool seconds=false);
    QDateTime* plannedEndTime;
    long restDuration;
    double percentOfWork;
    int sumOfPauses;
    QDateTime* startTime;

protected:
    LifetimeMainWindow* parentGui;
    bool inPause;
    QTime* pauseCounter;


};

#endif
