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
#ifndef LIFETIMECONFIG_H
#define LIFETIMECONFIG_H
#define GROUP chrm
#define FILENAME lifetime3

#include <QDialog>
#include <QDateTime>
#include "LifetimeMainWindow.h"
#include "ui_lifetimesettingsview.h"

class LifetimeMainWindow;

/**
Handles the current configuaration

    @author Christoph Maciejewski <chrmac@gmx.de>
*/
class LifetimeConfig : public QDialog {
    Q_OBJECT
public:
    LifetimeConfig(LifetimeMainWindow *parent);
    ~LifetimeConfig();
    bool writeConfig();
    bool readConfig();
    QString jobDescription;
    bool usePause;
    bool useTheme;
    QString themeDir;
    QDateTime startTime;
    int stateFromConfig;
    long pauseFromConfig;

    QString protocolFile;
    QChar protocolSeparator;
    bool useAutostart;
    bool trayAfterComing;
    QDateTime pauseSince;
    bool inPause;
    int onExitEvent;
    double getWorkPerDay(int day);
    double getPausePerDay(int day);
    void showSettingsDialog();
    void setProtocol(QString newName);
    QTime autoStopDuration;

private:
    LifetimeMainWindow* parentGui;

protected:
    QList<QString> workPerDay;
    QList<QString> pausePerDay;
    Ui::LifetimeSettingsDialog *settingsUi;
    //QDialog* settingsDialog;
private slots:
    void selectProtocolFile();
    void selectThemeDir();
    void updateSettingsFromGui();
    void updateGui();

protected slots:
    void cancelClicked();
    void okClicked();

};

#endif
