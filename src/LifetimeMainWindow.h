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


#ifndef LIFETIME3_H
#define LIFETIME3_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QTimer>

#include "lifetimecore.h"
#include "lifetimeconfig.h"
#include "lifetimeprotocol.h"
#include "ui_lifetimemainwidget.h"
#include "lifetimethememanager.h"

// State definition
#define WORKING_IDLE 0
#define WORKING 1
#define WORKING_OVERTIME 2
#define WORKING_PAUSE 3

#define VERSION "2.1.2"


class QAction;
class QMenu;
class QTextEdit;
class LifetimeCore;
class LifetimeConfig;
class LifetimeProtocol;
class LifetimeThemeManager;
class QSystemTrayIcon;

class LifetimeMainWindow:public QMainWindow
{
    Q_OBJECT

public:
    LifetimeMainWindow();
    ~LifetimeMainWindow();
    LifetimeCore* getTheCore();
    LifetimeConfig* getTheConfig();
    LifetimeThemeManager* getTheTheme();
    LifetimeProtocol* getTheProtocol();

    bool createMainWidget();
    void updateElements();
    QString getReadableState();
    int getState();
    bool writeWorkToProtocol();
    void autoStop();
    Ui::lifetimeMainWidget getUi();
    void setState(int _newState);

protected:
    void closeEvent(QCloseEvent *event);
    void changeEvent(QEvent *event);
    void createSystemTrayIcon();

private slots:
    void about();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void update();
    void statusButtonClicked();
    void pauseButtonToggle(bool checked);
    void infoButtonToggle(bool checked);
    void exitApplication();

public slots:
    void showSettings();
    void showProtocol();
    void toggleVisibility();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void writeSettings();
    bool redrawTheme();

    QMenu *lifetimeMenu;
    QMenu *statusMenu;
    QMenu *helpMenu;
    QMenu *trayMenu;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;

    QAction *exitAct;
    QAction *settingsAct;
    QAction *statusAct;
    QAction *protocolAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    QAction *pauseAct;
    QAction *comingCoingAct;
    QAction *showMainWndAct;
    QTimer *timer;

    /** Sate of the application: 0:idle 1:running 2: running, overtimeS*/
    int state;

protected:
    QSystemTrayIcon *trayIcon;
    LifetimeCore *theCore;
    LifetimeConfig *theConfig;
    Ui::lifetimeMainWidget ui;
    LifetimeProtocol *theProtocol;
    LifetimeThemeManager *theTheme;
};

#endif
