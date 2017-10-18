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
#ifndef LIFETIMETHEMEMANAGER_H
#define LIFETIMETHEMEMANAGER_H

#include <QtGui>
#include <QString>
#include "LifetimeMainWindow.h"

/**
	@author Christoph Maciejewski <chrmac@gmx.de>
*/
class LifetimeThemeManager{
public:
    LifetimeThemeManager(LifetimeMainWindow* _parent, QString _dir);
    ~LifetimeThemeManager();

    bool hasValidTheme();
    bool setThemeDir(QString _dir);
    QPixmap* getPixmap(int _pixmapType);
    bool redrawMainWindow();
    QString getThemeDir();

    static const int PIXMAP_ACTION_BUTTON = 0;
    static const int PIXMAP_BACKGROUND = 1;
    static const int PIXMAP_PAUSE_BUTTON = 2;
    static const int PIXMAP_INFO_BUTTON = 3;

private:
    QPixmap* pic_action_button;
    QPixmap* pic_pause_button;
    QPixmap* pic_info_button;
    QPixmap* pic_background;
    bool themeIsValid;
    QDir themeDir;

protected:
    LifetimeMainWindow* parent;
private:
    bool parseThemeSettings();
};

#endif
