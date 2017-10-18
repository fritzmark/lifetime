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
#include "lifetimethememanager.h"
#include <QDir>

LifetimeThemeManager::LifetimeThemeManager(LifetimeMainWindow* _parent, QString _dir)
{
    this->parent = _parent;
    this->themeIsValid = false;
    this->pic_background = NULL;
    this->pic_action_button = NULL;
    this->pic_pause_button = NULL;
    this->pic_info_button = NULL;
    this->themeDir = NULL;
    this->setThemeDir(_dir);
}


LifetimeThemeManager::~LifetimeThemeManager()
{
}


/*!
    \fn LifetimeThemeManager::getIcon(int _IconType)
 */
QPixmap* LifetimeThemeManager::getPixmap(int _pixmapType)
{
    switch(_pixmapType) {
        case PIXMAP_ACTION_BUTTON:
            return this->pic_action_button;
        case PIXMAP_PAUSE_BUTTON:
            return this->pic_pause_button;
        case PIXMAP_INFO_BUTTON:
            return this->pic_info_button;
        case PIXMAP_BACKGROUND:
            return this->pic_background;
    }

    return NULL;
}


/*!
    \fn LifetimeThemeManager::setThemeDir()
 */
bool LifetimeThemeManager::setThemeDir(QString _dir)
{
    QDir dir(_dir);

    if(!dir.isReadable() || _dir == "") {
        this->themeIsValid = false;
        return false;
    }

    this->themeDir = dir;

    // Check for needed pixmaps
    QString tmpName = dir.absolutePath()+"/action_button.png";
    QFileInfo tmpInfo(tmpName);

    if(tmpInfo.isReadable()) {
        this->pic_action_button = new QPixmap(tmpName);
        this->themeIsValid = true;
    }

    tmpName = dir.absolutePath()+"/background.png";
    tmpInfo.setFile(tmpName);

    if(tmpInfo.isReadable()) {
        this->pic_background = new QPixmap(tmpName);
        this->themeIsValid = true;
    }

    tmpName = dir.absolutePath()+"/pause_button.png";
    tmpInfo.setFile(tmpName);

    if(tmpInfo.isReadable()) {
        this->pic_pause_button = new QPixmap(tmpName);
        this->themeIsValid = true;
    }

    tmpName = dir.absolutePath()+"/info_button.png";
    tmpInfo.setFile(tmpName);

    if(tmpInfo.isReadable()) {
        this->pic_info_button = new QPixmap(tmpName);
        this->themeIsValid = true;
    }

    return true;
}


/*!
    \fn LifetimeThemeManager::hasValidTheme()
 */
bool LifetimeThemeManager::hasValidTheme()
{
    return this->themeIsValid;
}


/*!
    \fn LifetimeThemeManager::redrawMainWindow()
 */
bool LifetimeThemeManager::redrawMainWindow()
{
    if(this->hasValidTheme()) {

        // ActionButton
        QPixmap *pic = this->getPixmap(LifetimeThemeManager::PIXMAP_ACTION_BUTTON);
        if(pic != NULL) {
            this->parent->getUi().pushButton_state->setMinimumSize(pic->width(), pic->height());
            this->parent->getUi().pushButton_state->setMaximumSize(pic->width(), pic->height());
            this->parent->getUi().pushButton_state->resize(pic->width(), pic->height());
            this->parent->getUi().pushButton_state->setFlat(true);
            this->parent->getUi().pushButton_state->setDefault(false);
            this->parent->getUi().pushButton_state->setAutoFillBackground(true);
            QPalette pal(this->parent->getUi().pushButton_state->palette());
            pal.setBrush(QPalette::Button, *(pic));
            this->parent->getUi().pushButton_state->setPalette(pal);
            this->parent->getUi().pushButton_state->setFocusPolicy(Qt::NoFocus);

            this->parent->getUi().frame_state->setMinimumHeight(pic->height() + 4);
        }

        // PauseButton
        pic = this->getPixmap(LifetimeThemeManager::PIXMAP_PAUSE_BUTTON);
        if(pic != NULL) {
            this->parent->getUi().pushButton_pause->setMinimumSize(pic->width(), pic->height());
            this->parent->getUi().pushButton_pause->setMaximumSize(pic->width(), pic->height());
            this->parent->getUi().pushButton_pause->resize(pic->width(), pic->height());
            this->parent->getUi().pushButton_pause->setFlat(true);
            this->parent->getUi().pushButton_pause->setDefault(false);
            this->parent->getUi().pushButton_pause->setFocusPolicy(Qt::NoFocus);
            this->parent->getUi().pushButton_pause->setAutoFillBackground(true);
            QPalette pal(this->parent->getUi().pushButton_pause->palette());
            pal.setBrush(QPalette::Button, *(pic));
            this->parent->getUi().pushButton_pause->setPalette(pal);
        }

        // InfoButton
        pic = this->getPixmap(LifetimeThemeManager::PIXMAP_INFO_BUTTON);
        if(pic != NULL) {
            this->parent->getUi().pushButton_info->setMinimumSize(pic->width(), pic->height());
            this->parent->getUi().pushButton_info->setMaximumSize(pic->width(), pic->height());
            this->parent->getUi().pushButton_info->resize(pic->width(), pic->height());
            this->parent->getUi().pushButton_info->setFlat(true);
            this->parent->getUi().pushButton_info->setDefault(false);
            this->parent->getUi().pushButton_info->setFocusPolicy(Qt::NoFocus);
            this->parent->getUi().pushButton_info->setAutoFillBackground(true);
            QPalette pal(this->parent->getUi().pushButton_info->palette());
            pal.setBrush(QPalette::Button, *(pic));
            this->parent->getUi().pushButton_info->setPalette(pal);

            this->parent->getUi().frame_info->setMinimumHeight(pic->height() + 4);
        }

        int tmpOffset = 0;
        int tmpMargin = 18;

        if(this->parent->getUi().pushButton_pause->isVisible()) {
            tmpOffset = this->parent->getUi().pushButton_pause->width();
            tmpMargin = 24;
        }

        int tmpWidth = this->parent->getUi().pushButton_state->width() + \
                tmpOffset + \
                this->parent->getUi().label_time->width() + \
                tmpMargin;

        this->parent->setMinimumWidth(tmpWidth);
        this->parent->resize(tmpWidth, this->parent->height());

        // Background
        QPixmap *back = this->getPixmap(LifetimeThemeManager::PIXMAP_BACKGROUND);
        QPalette palB(this->parent->palette());
        palB.setBrush(QPalette::Background, *(back));
        this->parent->setPalette(palB);
    } else {
        // Reset all crazy theme stuff at runtime
        // TODO: make it working and clean!
        this->parent->getUi().pushButton_state->setFocusPolicy(Qt::StrongFocus);
        this->parent->getUi().pushButton_state->setDefault(true);
        //this->parent->getUi().pushButton_state->setAutoFillBackground(false);
        this->parent->centralWidget()->setPalette(QPalette());
        this->parent->getUi().pushButton_state->setPalette(QPalette());
        this->parent->getUi().pushButton_pause->setPalette(QPalette());
        this->parent->getUi().pushButton_info->setPalette(QPalette());
    }

    this->parseThemeSettings();
    return true;
}


/*!
    \fn LifetimeThemeManager::getThemeDir()
 */
QString LifetimeThemeManager::getThemeDir()
{
    if(this->hasValidTheme())
        return this->themeDir.absolutePath();
    else
        return "";
}


/*!
    \fn LifetimeThemeManager::parseThemeSettings()
 */
bool LifetimeThemeManager::parseThemeSettings()
{
    QString tmpName = this->themeDir.absolutePath()+"/theme_settings";
    QFile file(tmpName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "*** Can't read theme settings: " << tmpName;
        return false;
    }

    QTextStream in(&file);
    QString key, value, foregroundColor, backgroundColor;
    QString line;

    while (!in.atEnd()) {
        line = in.readLine();
        if(line.split(QChar('=')).size() == 2) {
            key = line.split(QChar('=')).at(0);
            value = line.split(QChar('=')).at(1);

            // Color like "#RRGGBB"
            if(key == "FOREGROUND")
                foregroundColor = value;
            else if(key == "BACKGROUND")
                backgroundColor = value;
        }
    }

    QPalette palParent(this->parent->palette());
    // Set foreground
    palParent.setColor(QPalette::WindowText, QColor(foregroundColor));
    palParent.setColor(QPalette::ButtonText, QColor(foregroundColor));
    // TODO: set background

    // use this colors
    this->parent->setPalette(palParent);
}
