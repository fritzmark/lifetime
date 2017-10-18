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
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QFileDialog>
#include "lifetimeconfig.h"

LifetimeConfig::LifetimeConfig(LifetimeMainWindow *parent)
    : QDialog(parent)
{
    // Presets
    //this->settingsDialog = (QDialog*)0;
    this->usePause = false;
    this->inPause = false;
    this->pauseSince = QDateTime::currentDateTime();
    this->startTime = QDateTime::currentDateTime();
    this->workPerDay << "07:00:00" << "07:00:00" << "07:00:00" << "07:00:00" << "07:00:00" << "07:00:00" << "04:00:00";
    this->pausePerDay << "00:30:00" << "00:30:00" << "00:30:00" << "00:30:00" << "00:30:00" << "00:30:00" << "00:20:00";
    this->jobDescription = "Working hard...";
    this->useTheme = false;
    this->themeDir = "";
    this->protocolFile = QDir::homePath() + QDir::separator() + "times.csv";
    this->stateFromConfig = -1;
    this->protocolSeparator = '|';
    this->useAutostart = false;
    this->trayAfterComing = false;
    this->onExitEvent = 2; // Default: ask for action
    this->autoStopDuration = QTime(10, 0, 0);

    Ui::LifetimeSettingsDialog* newUi = new Ui::LifetimeSettingsDialog();
    newUi->setupUi(this);

    this->settingsUi = newUi;
    this->setModal(true);

    QObject::connect(this->settingsUi->pushButton_openProtocol, SIGNAL(clicked()), this, SLOT(selectProtocolFile()));
    QObject::connect(this->settingsUi->pushButton_openTheme, SIGNAL(clicked()), this, SLOT(selectThemeDir()));
    QObject::connect(this->settingsUi->pushButton_OK, SIGNAL(clicked()), this, SLOT(okClicked()));
    QObject::connect(this->settingsUi->pushButton_Cancel, SIGNAL(clicked()), this, SLOT(cancelClicked()));

    this->parentGui = parent;
    //this->readConfig();
}


LifetimeConfig::~LifetimeConfig()
{
}



/*!
    \fn LifetimeConfig::readConfig()
 */
bool LifetimeConfig::readConfig()
{
    QSettings settings("chrm", "lifetime3");

    if(settings.status() != QSettings::NoError) {
        // Error occured.
        // Starting for the first time?
        qDebug("No config file found");
    }



    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    QDateTime touch = QDateTime::fromTime_t(settings.value("touch", "0").toUInt());

    this->workPerDay[0] = settings.value("workMonday", "07:30:00").toString();
    this->workPerDay[1] = settings.value("workTuesday", "07:30:00").toString();
    this->workPerDay[2] = settings.value("workWednesday", "07:30:00").toString();
    this->workPerDay[3] = settings.value("workThursday", "07:30:00").toString();
    this->workPerDay[4] = settings.value("workFriday", "07:30:00").toString();
    this->workPerDay[5] = settings.value("workSaturday", "07:30:00").toString();
    this->workPerDay[6] = settings.value("workSunday", "03:30:00").toString();

    this->pausePerDay[0] = settings.value("pauseMonday", "00:30:00").toString();
    this->pausePerDay[1] = settings.value("pauseTuesday", "00:30:00").toString();
    this->pausePerDay[2] = settings.value("pauseWednesday", "00:30:00").toString();
    this->pausePerDay[3] = settings.value("pauseThursday", "00:30:00").toString();
    this->pausePerDay[4] = settings.value("pauseFriday", "00:30:00").toString();
    this->pausePerDay[5] = settings.value("pauseSaturday", "00:30:00").toString();
    this->pausePerDay[6] = settings.value("pauseSunday", "00:30:00").toString();

    this->usePause = settings.value("usePause", false).toBool();

    this->startTime = QDateTime::fromTime_t(settings.value("startTime", this->startTime.toTime_t()).toUInt());
    this->jobDescription = settings.value("jobDescription", this->jobDescription).toString();

    this->useTheme = settings.value("useTheme", false).toBool();
    this->themeDir = settings.value("themeDir", this->themeDir).toString();

    this->protocolFile = settings.value("protocolFile", this->protocolFile).toString();
    this->protocolSeparator = settings.value("protocolSeparator", this->protocolSeparator).toChar();

    this->stateFromConfig = settings.value("state", "0").toInt();
    this->pauseFromConfig = settings.value("pauseSum", "0").toUInt();
    this->inPause = settings.value("inPause", false).toBool();
    this->pauseSince = QDateTime::fromTime_t(settings.value("pauseSince", "0").toUInt());

    this->useAutostart = settings.value("useAutostart", false).toBool();
    this->trayAfterComing = settings.value("trayAfterComing", false).toBool();
    this->onExitEvent = settings.value("onExitEvent", 2).toInt();
    this->autoStopDuration = QTime::fromString(settings.value("autoStopDuration", "10:00:00").toString(), "hh:mm:ss");
    bool bFirstStart = settings.value("firstStart", true).toBool();

    // Set GUI position
    this->parentGui->resize(size);
    this->parentGui->move(pos);

    if(bFirstStart)
    {
        this->showSettingsDialog();
    }

    return true;
}


/*!
    \fn LifetimeConfig::writeConfig()
 */
bool LifetimeConfig::writeConfig()
{
//    qDebug("+ Writing configuration");
    QSettings settings("chrm", "lifetime3");

    settings.setValue("touch", QDateTime::currentDateTime().toTime_t());
    if(this->parentGui->pos().x() != -1) {
        settings.setValue("pos", this->parentGui->pos());
    }
    settings.setValue("size", this->parentGui->size());
    settings.setValue("state", this->parentGui->getState());

    settings.setValue("workMonday", this->workPerDay[0]);
    settings.setValue("workTuesday", this->workPerDay[1]);
    settings.setValue("workWednesday", this->workPerDay[2]);
    settings.setValue("workThursday", this->workPerDay[3]);
    settings.setValue("workFriday", this->workPerDay[4]);
    settings.setValue("workSaturday", this->workPerDay[5]);
    settings.setValue("workSunday", this->workPerDay[6]);

    settings.setValue("pauseMonday", this->pausePerDay[0]);
    settings.setValue("pauseTuesday", this->pausePerDay[1]);
    settings.setValue("pauseWednesday", this->pausePerDay[2]);
    settings.setValue("pauseThursday", this->pausePerDay[3]);
    settings.setValue("pauseFriday", this->pausePerDay[4]);
    settings.setValue("pauseSaturday", this->pausePerDay[5]);
    settings.setValue("pauseSunday", this->pausePerDay[6]);

    settings.setValue("usePause", this->usePause);
    if(this->parentGui->getTheCore()->startTime != NULL) {
        settings.setValue("startTime", this->parentGui->getTheCore()->startTime->toTime_t());
    }
    settings.setValue("pauseSum", this->parentGui->getTheCore()->sumOfPauses);
    settings.setValue("inPause", this->inPause);
    settings.setValue("pauseSince", this->pauseSince.toTime_t());
    settings.setValue("jobDescription", this->jobDescription);
    settings.setValue("useTheme", this->useTheme);
    settings.setValue("themeDir", this->themeDir);
    settings.setValue("protocolFile", this->protocolFile);
    settings.setValue("protocolSeparator", this->protocolSeparator);

    settings.setValue("useAutostart", this->useAutostart);
    settings.setValue("trayAfterComing", this->trayAfterComing);
    settings.setValue("onExitEvent", this->onExitEvent);
    settings.setValue("autoStopDuration", this->autoStopDuration.toString("hh:mm:ss"));
    settings.setValue("firstStart", false);

    return true;
}


/*!
    \fn LifetimeConfig::getWorkPerDay(int day)
 */
double LifetimeConfig::getWorkPerDay(int day)
{
    return LifetimeCore::timeToDecimal(QTime::fromString(this->workPerDay[day], "hh:mm:ss"), false);
}


/*!
    \fn LifetimeConfig::getPausePerDay(int day)
 */
double LifetimeConfig::getPausePerDay(int day)
{
    return LifetimeCore::timeToDecimal(QTime::fromString(this->pausePerDay[day], "hh:mm:ss"), false);
}


/*!
    \fn LifetimeConfig::showSettingsDialog()
 */
void LifetimeConfig::showSettingsDialog()
{
    this->updateGui();
    this->exec();
    this->raise();
}

void LifetimeConfig::updateGui()
{
    // Preset all values
    this->settingsUi->lineEdit_job->setText(this->jobDescription);

    if(this->parentGui->getTheCore()->startTime == NULL || this->parentGui->getState() == 0) {
        this->settingsUi->timeEdit_start->setEnabled(false);
    } else {
        this->settingsUi->timeEdit_start->setEnabled(true);
        this->settingsUi->timeEdit_start->setTime(this->parentGui->getTheCore()->startTime->time());
    }

    this->settingsUi->timeEdit_Mo->setTime(QTime::fromString(this->workPerDay[0], "hh:mm:ss"));
    this->settingsUi->timeEdit_Tue->setTime(QTime::fromString(this->workPerDay[1], "hh:mm:ss"));
    this->settingsUi->timeEdit_Wen->setTime(QTime::fromString(this->workPerDay[2], "hh:mm:ss"));
    this->settingsUi->timeEdit_Thu->setTime(QTime::fromString(this->workPerDay[3], "hh:mm:ss"));
    this->settingsUi->timeEdit_Fri->setTime(QTime::fromString(this->workPerDay[4], "hh:mm:ss"));
    this->settingsUi->timeEdit_Sat->setTime(QTime::fromString(this->workPerDay[5], "hh:mm:ss"));
    this->settingsUi->timeEdit_Sun->setTime(QTime::fromString(this->workPerDay[6], "hh:mm:ss"));

    if(this->parentGui->getState() == 0)
        this->settingsUi->checkBox_usePause->setEnabled(true);
    else
        this->settingsUi->checkBox_usePause->setEnabled(false);

    this->settingsUi->checkBox_usePause->setChecked(this->usePause);

    this->settingsUi->timeEdit_Mo_pause->setTime(QTime::fromString(this->pausePerDay[0], "hh:mm:ss"));
    this->settingsUi->timeEdit_Tue_pause->setTime(QTime::fromString(this->pausePerDay[1], "hh:mm:ss"));
    this->settingsUi->timeEdit_Wen_pause->setTime(QTime::fromString(this->pausePerDay[2], "hh:mm:ss"));
    this->settingsUi->timeEdit_Thu_pause->setTime(QTime::fromString(this->pausePerDay[3], "hh:mm:ss"));
    this->settingsUi->timeEdit_Fri_pause->setTime(QTime::fromString(this->pausePerDay[4], "hh:mm:ss"));
    this->settingsUi->timeEdit_Sat_pause->setTime(QTime::fromString(this->pausePerDay[5], "hh:mm:ss"));
    this->settingsUi->timeEdit_Sun_pause->setTime(QTime::fromString(this->pausePerDay[6], "hh:mm:ss"));

    this->settingsUi->lineEdit_protocolPath->setText(this->protocolFile);
    this->settingsUi->lineEdit_delim->setText(this->protocolSeparator);

    this->settingsUi->checkBox_come_on_startup->setChecked(this->useAutostart);
    this->settingsUi->checkBox_tray_on_come->setChecked(this->trayAfterComing);
    this->settingsUi->radioButton_close_but_run->setChecked(this->onExitEvent == 0);
    this->settingsUi->radioButton_do_going->setChecked(this->onExitEvent == 1);
    this->settingsUi->radioButton_ask_on_close->setChecked(this->onExitEvent == 2);

    this->settingsUi->timeEdit_auto_stop->setTime(this->autoStopDuration);

    if(this->useTheme)
        this->settingsUi->checkBox_useTheme->setChecked(true);
    else
        this->settingsUi->checkBox_useTheme->setChecked(false);

    this->settingsUi->lineEdit_themePath->setText(this->themeDir);
}

void LifetimeConfig::updateSettingsFromGui()
{
    // Read and set new values

    this->jobDescription = this->settingsUi->lineEdit_job->text();
    if ( this->parentGui->getTheCore()->startTime != NULL )
    {
        this->parentGui->getTheCore()->startTime->setTime ( this->settingsUi->timeEdit_start->time() );
    }
    this->workPerDay[0] = this->settingsUi->timeEdit_Mo->time().toString ( "hh:mm:ss" );
    this->workPerDay[1] = this->settingsUi->timeEdit_Tue->time().toString ( "hh:mm:ss" );
    this->workPerDay[2] = this->settingsUi->timeEdit_Wen->time().toString ( "hh:mm:ss" );
    this->workPerDay[3] = this->settingsUi->timeEdit_Thu->time().toString ( "hh:mm:ss" );
    this->workPerDay[4] = this->settingsUi->timeEdit_Fri->time().toString ( "hh:mm:ss" );
    this->workPerDay[5] = this->settingsUi->timeEdit_Sat->time().toString ( "hh:mm:ss" );
    this->workPerDay[6] = this->settingsUi->timeEdit_Sun->time().toString ( "hh:mm:ss" );

    this->usePause = this->settingsUi->checkBox_usePause->isChecked();

    this->pausePerDay[0] = this->settingsUi->timeEdit_Mo_pause->time().toString ( "hh:mm:ss" );
    this->pausePerDay[1] = this->settingsUi->timeEdit_Tue_pause->time().toString ( "hh:mm:ss" );
    this->pausePerDay[2] = this->settingsUi->timeEdit_Wen_pause->time().toString ( "hh:mm:ss" );
    this->pausePerDay[3] = this->settingsUi->timeEdit_Thu_pause->time().toString ( "hh:mm:ss" );
    this->pausePerDay[4] = this->settingsUi->timeEdit_Fri_pause->time().toString ( "hh:mm:ss" );
    this->pausePerDay[5] = this->settingsUi->timeEdit_Sat_pause->time().toString ( "hh:mm:ss" );
    this->pausePerDay[6] = this->settingsUi->timeEdit_Sun_pause->time().toString ( "hh:mm:ss" );

    if ( !this->usePause && this->parentGui->getState() > 0 )
    {
        this->parentGui->getTheCore()->sumOfPauses = ( int ) ( this->getPausePerDay ( this->parentGui->getTheCore()->startTime->date().dayOfWeek() - 1 ) * 3600 );
    }

    this->protocolFile = this->settingsUi->lineEdit_protocolPath->text();
    this->protocolSeparator = this->settingsUi->lineEdit_delim->text().at ( 0 );

    if ( this->protocolSeparator.isNull() )
    {
        // Set default separator
        this->protocolSeparator = '|';
    }

    this->useAutostart = this->settingsUi->checkBox_come_on_startup->isChecked();
    this->trayAfterComing = this->settingsUi->checkBox_tray_on_come->isChecked();
    this->onExitEvent = 2;
    this->onExitEvent = this->settingsUi->radioButton_close_but_run->isChecked() ? 0 : this->onExitEvent;
    this->onExitEvent = this->settingsUi->radioButton_do_going->isChecked() ? 1 : this->onExitEvent;
    this->onExitEvent = this->settingsUi->radioButton_ask_on_close->isChecked() ? 2 : this->onExitEvent;
    this->autoStopDuration = this->settingsUi->timeEdit_auto_stop->time();

    this->useTheme = this->settingsUi->checkBox_useTheme->isChecked();

    if(this->useTheme) {
        this->themeDir = this->settingsUi->lineEdit_themePath->text();
        this->parentGui->getTheTheme()->setThemeDir(this->themeDir);
    } else
        this->parentGui->getTheTheme()->setThemeDir("");

    // write all to config file
    this->writeConfig();
}


/*!
    \fn LifetimeConfig::setProtocolFileButtonClicked()
 */
void LifetimeConfig::setProtocol(QString newName)
{
    this->settingsUi->lineEdit_protocolPath->setText(newName);
    this->protocolFile = newName;
}


/*!
    \fn LifetimeConfig::okClicked()
 */
void LifetimeConfig::okClicked()
{
    this->updateSettingsFromGui();
    this->hide();
}


/*!
    \fn LifetimeConfig::cancelClicked()
 */
void LifetimeConfig::cancelClicked()
{
    this->hide();
}


/*!
    \fn LifetimeConfig::selectProtocolFile()
 */
void LifetimeConfig::selectProtocolFile()
{
    QWidget *parent = (QWidget*)this;

    if(!this->isVisible())
        parent = (QWidget*)0;

    QString file = QFileDialog::getSaveFileName(parent, tr("Select Protocol File"), this->protocolFile);

    if(!file.isNull()) {
        this->setProtocol(file);
    }
}


/*!
    \fn LifetimeConfig::selectThemeDir()
 */
void LifetimeConfig::selectThemeDir()
{
    QWidget *parent = (QWidget*)this;

    if(!this->isVisible())
        parent = (QWidget*)0;

    QString dir = QFileDialog::getExistingDirectory(parent, tr("Select Theme Directory"), this->themeDir);

    if(!dir.isNull()) {
        this->settingsUi->lineEdit_themePath->setText(dir);
    }
}
