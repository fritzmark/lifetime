/***************************************************************************
 *   Copyright (C) 2007 by Christoph Maciejewski (chrmac@gmx.de)           *
 *   Copyright (C) 2007 by Michael Schulz ()                               *
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
#include <QMessageBox>
#include <QMenuBar>
#include <QTextEdit>
#include <QCloseEvent>
#include <QFileDialog>
#include <QSystemTrayIcon>
#include <QStatusBar>
#include "LifetimeMainWindow.h"
#include "lifetimecore.h"
#include "lifetimeconfig.h"
#include "ui_lifetimemainwidget.h"



LifetimeMainWindow::LifetimeMainWindow()
    : QMainWindow((QWidget*)0, Qt::WindowStaysOnTopHint)
{

    // Fixed Bug # 1707129 (Reporter: Dogo, Assignee: Dogo)
    // Bug message: Lifetime freeze on maximize (Win32)
    Qt::WindowFlags wflags = windowFlags() & ~Qt::WindowMaximizeButtonHint;
    this->overrideWindowFlags(wflags);

    // create complete UI first
    this->createActions();
    this->createMenus();
    this->createToolBars();
    this->createStatusBar();

    this->theTheme = new LifetimeThemeManager(this, "");

    this->createMainWidget();
    this->createSystemTrayIcon();

    this->state = WORKING_IDLE;
    this->theCore = new LifetimeCore(this);
    this->theConfig = new LifetimeConfig(this);
    this->theConfig->readConfig();

    this->theProtocol = new LifetimeProtocol(this);

    this->updateElements();
    this->trayIcon->show();

    this->setWindowTitle("Lifetime " VERSION);
    this->setWindowIcon(QIcon(QString::fromUtf8(":/img/lifeline_blue_48x48.png")));

    this->timer = new QTimer(this);
    QObject::connect(this->timer, SIGNAL(timeout()), this, SLOT(update()));

    if(this->theConfig->stateFromConfig > 0) {
        this->statusButtonClicked();
        this->theCore->updateStartTime(this->theConfig->startTime.toTime_t());

        if(this->theConfig->usePause) {
            this->theCore->sumOfPauses = this->theConfig->pauseFromConfig;
            if(this->theConfig->inPause) {
                this->theCore->sumOfPauses += this->theConfig->pauseSince.secsTo(QDateTime::currentDateTime());
                this->ui.pushButton_pause->setChecked(true);
            }
        }

    } else {
        if(this->theConfig->useAutostart) {
            this->statusButtonClicked();
        }
    }

    if(this->theConfig->useTheme) {
        this->theTheme->setThemeDir(this->theConfig->themeDir);
        this->redrawTheme();
    }

    // Added toggled info button to default view.
    // Change made by Dogo, 17.03.2008 23:00
    this->ui.pushButton_info->setChecked(true);
}

void LifetimeMainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    this->trayIcon->showMessage(tr("Lifetime"), tr("running minimized!"),QSystemTrayIcon::Information,2000);
    this->theConfig->writeConfig();
    this->hide();
}

void LifetimeMainWindow::changeEvent(QEvent *event) {
    event->ignore();

    if(event->type() == QEvent::WindowStateChange) {
        this->toggleVisibility();
    }
}

void LifetimeMainWindow::about()
{
      QMessageBox::about(this, tr("About Lifetime"),
            tr("<b>Lifetime " VERSION "</b><br> is a little working time control tool. <br>"
                  "Lifetime is using Qt 4.x. <br>"
                  "<br>"
                  "<i>This program is free software.<br> You can redistribute it and/or modify<br>"
                  "it under the terms of the<br> GNU General Public License</i><br>"
                  "<br>"
                  "<a href=\"http://sourceforge.net/projects/lifetime/\">http://sourceforge.net/projects/lifetime/</a>"));
}

void LifetimeMainWindow::createActions()
{
    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
    exitAct->setIcon(QIcon(QString::fromUtf8(":/img/exit.png")));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(exitApplication()));

    settingsAct = new QAction(tr("S&ettings..."), this);
    settingsAct->setShortcut(tr("Ctrl+S"));
    settingsAct->setStatusTip(tr("Show settings dialog"));
    settingsAct->setIcon(QIcon(QString::fromUtf8(":/img/configure.png")));
    connect(settingsAct, SIGNAL(triggered()), this, SLOT(showSettings()));

    statusAct = new QAction(tr("Coming/&Going"), this);
    statusAct->setShortcut(tr("Ctrl+G"));
    statusAct->setStatusTip(tr("Change status"));
    connect(statusAct, SIGNAL(triggered()), this, SLOT(statusButtonClicked()));

    protocolAct = new QAction(tr("&Protocol..."), this);
    protocolAct->setShortcut(tr("Ctrl+P"));
    protocolAct->setStatusTip(tr("Show protocol"));
    protocolAct->setIcon(QIcon(QString::fromUtf8(":/img/view_text.png")));
    connect(protocolAct, SIGNAL(triggered()), this, SLOT(showProtocol()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    aboutAct->setIcon(QIcon(QString::fromUtf8(":/img/help.png")));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    //TrayIcon actions
    pauseAct = new QAction(tr("&Pause"),this);
    pauseAct->setCheckable(true);
    connect(pauseAct,SIGNAL(toggled(bool)), this, SLOT(pauseButtonToggle(bool)));

    showMainWndAct = new QAction(tr("&Restore"),this);
    connect(showMainWndAct,SIGNAL(triggered()), this, SLOT(show()));

}

void LifetimeMainWindow::createMenus()
{
    lifetimeMenu = menuBar()->addMenu(tr("&Lifetime"));
    lifetimeMenu->addAction(settingsAct);

    lifetimeMenu->addSeparator();
    lifetimeMenu->addAction(exitAct);

    statusMenu = menuBar()->addMenu(tr("&Status"));
    statusMenu->addAction(statusAct);
    statusMenu->addSeparator();
    statusMenu->addAction(protocolAct);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    //Construct the trayIconMenu
    trayMenu = new QMenu();
    trayMenu->addAction(statusAct);
    trayMenu->addAction(pauseAct);
    trayMenu->addSeparator();
    trayMenu->addAction(showMainWndAct);
    trayMenu->addAction(protocolAct);
    trayMenu->addAction(settingsAct);


}

void LifetimeMainWindow::createToolBars()
{
}

void LifetimeMainWindow::createStatusBar() {
    statusBar()->showMessage(tr("Ready"));
}

void LifetimeMainWindow::writeSettings() {
}


LifetimeMainWindow::~LifetimeMainWindow() {
}



/*!
    \fn LifetimeMainWindow::getTheCore()
 */
LifetimeCore* LifetimeMainWindow::getTheCore()
{
    return this->theCore;
}


/*!
    \fn LifetimeMainWindow::getTheConfig()
 */
LifetimeConfig* LifetimeMainWindow::getTheConfig()
{
    return this->theConfig;
}

/*!
    \fn LifetimeMainWindow::getTheTheme()
 */
LifetimeThemeManager* LifetimeMainWindow::getTheTheme()
{
    return this->theTheme;
}

/*!
    \fn LifetimeMainWindow::getTheProtocol()
 */
LifetimeProtocol* LifetimeMainWindow::getTheProtocol()
{
    return this->theProtocol;
}


/*!
    \fn LifetimeMainWindow::createSystemTrayIcon()
 */
void LifetimeMainWindow::createSystemTrayIcon()
{
    this->trayIcon = new QSystemTrayIcon(this);

    connect(this->trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
        this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    this->trayIcon->setContextMenu(trayMenu);
}

void LifetimeMainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        this->toggleVisibility();
        break;
    case QSystemTrayIcon::DoubleClick:
        break;
    case QSystemTrayIcon::MiddleClick:
        break;
    default:
        ;
    }
}

/*!
    \fn LifetimeMainWindow::createMainWidget()
 */
bool LifetimeMainWindow::createMainWidget()
{
    QWidget* w = new QWidget();
    Ui::lifetimeMainWidget newUi;
    newUi.setupUi(w);

    this->ui = newUi;

    QObject::connect(this->ui.pushButton_state, SIGNAL(clicked()), this, SLOT(statusButtonClicked()));
    QObject::connect(this->ui.pushButton_pause, SIGNAL(toggled(bool)), this, SLOT(pauseButtonToggle(bool)));
    QObject::connect(this->ui.pushButton_info, SIGNAL(toggled(bool)), this, SLOT(infoButtonToggle(bool)));

    this->ui.label_overview->setVisible(false);

    setCentralWidget(w);

    return true;
}

/*!
    \fn LifetimeMainWindow::statusButtonClicked()
 */
void LifetimeMainWindow::statusButtonClicked()
{
    if(this->state > WORKING_IDLE) {
        // Stop
        this->theCore->stopWorking();
        this->timer->stop();
        this->setState(WORKING_IDLE);
        this->ui.pushButton_state->setIcon(QIcon(QString::fromUtf8(":/img/kommt.xpm")));
        this->ui.pushButton_state->setText(tr(" Coming"));
        this->writeWorkToProtocol();

    } else {
        // Start
        this->theCore->startWorking();
        this->timer->start(1000);
        this->setState(WORKING);
        this->ui.pushButton_state->setIcon(QIcon(QString::fromUtf8(":/img/geht.xpm")));
        this->ui.pushButton_state->setText(tr(" Going"));

        if(this->theConfig->trayAfterComing) {
            if(this->isVisible()) {
                this->trayIcon->showMessage(tr("Lifetime"), tr("running minimized!"),QSystemTrayIcon::Information,2000);
                this->hide();
            }
        }
    }

    this->theConfig->writeConfig();
    this->updateElements();
}


/*!
    \fn LifetimeMainWindow::pauseButtonClicked()
 */
void LifetimeMainWindow::pauseButtonToggle(bool checked)
{
    if(this->state == WORKING_IDLE)
        return;

    if (checked) {
       this->setState(WORKING_PAUSE);
       this->getTheCore()->startPause();
    } else {
        this->setState(WORKING);
        this->getTheCore()->stopPause();
    }

    this->updateElements();
    this->theConfig->writeConfig();
}


/*!
    \fn LifetimeMainWindow::infoButtonClicked()
 */
void LifetimeMainWindow::infoButtonToggle(bool checked)
{

    int h = this->ui.label_overview->height();

    if (checked) {
        // Show Info label and raise main window
        this->resize(this->width(), this->height() + h);
        this->ui.label_overview->setVisible(checked);
	    this->adjustSize();
    } else {
        this->ui.label_overview->setVisible(checked);
        //this->adjustSize();
        this->resize(this->width(), this->height() - h);
    }

    //this->ui.gridLayout->invalidate();
}


/*!
    \fn LifetimeMainWindow::updateElements()
 */
void LifetimeMainWindow::updateElements()
{
    QPalette pal = QApplication::palette();
    this->ui.pushButton_pause->setEnabled(true);
    this->pauseAct->setEnabled(true);

    switch (this->state) {
        case WORKING_IDLE:
            trayIcon->setIcon(QIcon(QString::fromUtf8(":/img/working_idle.png")));
            pal.setColor(QPalette::Background, QColor(255,255,0));
            this->ui.pushButton_pause->setEnabled(false);
            this->pauseAct->setEnabled(false);
            this->ui.pushButton_pause->setChecked(false);
            this->pauseAct->setChecked(false);
            break;
        case WORKING:
            trayIcon->setIcon(QIcon(QString::fromUtf8(":/img/working.png")));
            pal.setColor(QPalette::Background, QColor(255,0,0));
            this->ui.pushButton_pause->setChecked(false);
            this->pauseAct->setChecked(false);
            break;
        case WORKING_PAUSE:
            trayIcon->setIcon(QIcon(QString::fromUtf8(":/img/working_pause.png")));
            pal.setColor(QPalette::Background, QColor(0,0,255));
            this->ui.pushButton_pause->setChecked(true);
            this->pauseAct->setChecked(true);
            break;
        case WORKING_OVERTIME:
            trayIcon->setIcon(QIcon(QString::fromUtf8(":/img/working_overtime.png")));
            pal.setColor(QPalette::Background, QColor(0,255,0));
            this->ui.pushButton_pause->setChecked(false);
            this->pauseAct->setChecked(false);
            break;
        default:
            trayIcon->setIcon(QIcon(QString::fromUtf8(":/img/working_idle.png")));
            pal.setColor(QPalette::Background, QColor(100,100,100));
            this->ui.pushButton_pause->setChecked(false);
            this->pauseAct->setChecked(false);
    }

    this->ui.label_state_info->setText(this->getTheCore()->getStatusString());
    this->ui.label_state_info->setPalette(pal);
    this->ui.progressBar->setValue((int)(this->getTheCore()->percentOfWork));
    this->trayIcon->setToolTip("Lifetime: " + this->getReadableState() + "\n" + this->getTheCore()->getStatusString());
    this->ui.label_time->setText(QDateTime::currentDateTime().toString("hh:mm:ss"));

    if(this->state > 0) {
        QString s("");
        s += (tr("Begin: ") + this->theCore->startTime->toString("hh:mm:ss (ddd)")) + "\n";
        s += (tr("End: ") + this->theCore->plannedEndTime->toString("hh:mm:ss (ddd)")) + "\n";
        s += (tr("Breaks: ") + QString::number(this->theCore->sumOfPauses / 60) + " min");

        this->ui.label_overview->setText(s);
    }

    if(!this->theConfig->usePause) {
        this->ui.pushButton_pause->setVisible(false);
        this->pauseAct->setVisible(false);
    } else {
        this->ui.pushButton_pause->setVisible(true);
        this->pauseAct->setVisible(true);
    }
}


/*!
    \fn LifetimeMainWindow::getReadableState()
 */
QString LifetimeMainWindow::getReadableState()
{
    switch (this->state) {
        case WORKING_IDLE:
            return tr("idle");
            break;
        case WORKING:
            return tr("working");
            break;
        case WORKING_PAUSE:
            return tr("pause");
            break;
        case WORKING_OVERTIME:
            return tr("overtime");
            break;
        default:
            return tr("unknown");
            ;
    }
}


/*!
    \fn LifetimeMainWindow::update()
 */
void LifetimeMainWindow::update()
{
    this->getTheCore()->update();

    if(this->state > WORKING_IDLE && this->state != WORKING_PAUSE) {
       if(this->getTheCore()->isOvertime())
           this->setState(WORKING_OVERTIME);
       else if(this->state == WORKING_OVERTIME && this->theCore->restDuration > 0)
           this->setState(WORKING);
    }

    // save settings every 2min if on break
    if(this->state == WORKING_PAUSE && !(this->theCore->sumOfPauses%120)) {
        this->theConfig->writeConfig();
    }

    this->updateElements();
    //qDebug() << "End:" << this->theCore->plannedEndTime->toString("hh:mm");
    //qDebug() << "Rest:" << this->theCore->restDuration;
    //qDebug() << "Pause:" << this->theCore->sumOfPauses;
}


/*!
    \fn LifetimeMainWindow::getState()
 */
int LifetimeMainWindow::getState()
{
    return this->state;
}


/*!
    \fn LifetimeMainWindow::showProtocol()
 */
void LifetimeMainWindow::showProtocol()
{
    this->theProtocol->openFile(this->getTheConfig()->protocolFile);
    this->theProtocol->show();
}


/*!
    \fn LifetimeMainWindow::showSettings()
 */
void LifetimeMainWindow::showSettings()
{
    this->theConfig->showSettingsDialog();
    this->updateElements();
    this->redrawTheme();
}


/*!
    \fn LifetimeMainWindow::writeWorkToProtocol()
 */
bool LifetimeMainWindow::writeWorkToProtocol()
{
    this->theProtocol->writeToFile();
    return true;
}


/*!
    \fn LifetimeMainWindow::exitAppliaction()
 */
void LifetimeMainWindow::exitApplication()
{
    int onExit = this->theConfig->onExitEvent;

    if(this->state == WORKING_IDLE)
        onExit = 0;

    if(onExit == 0) {
        // Do nothing
    } else if(onExit == 1) {
        // Do "Going"
        if(this->state > WORKING_IDLE) {
            this->statusButtonClicked();
        }
    } else {
        // Ask for action
        switch( QMessageBox::question(
                this,
                tr("Exit Lifetime"),
                tr("Do \"Going\" and save current state in the protocol?\n"),
                tr("&Yes"),
                tr("&No, let the horology go"),
                tr("&Cancel"),
                0,      // Enter == button 0
                2 ) ) { // Escape == button 2
        case 0: // Yes or Enter pressed.
            // stop & write to file
            if(this->state > WORKING_IDLE) {
                this->statusButtonClicked();
            }
            break;
        case 1: // No clicked or Alt-N pressed
            // Do nothing
            break;
        case 2: // Cancel clicked, Alt-A or Escape pressed
            // don't exit
            return;
            break;
        }
    }

    this->trayIcon->hide();
    this->theConfig->writeConfig();
    qApp->quit();
}


/*!
    \fn LifetimeMainWindow::autoStop()
 */
void LifetimeMainWindow::autoStop()
{
    qDebug("!!! AUTOSTOP !!!");

    if(this->state > WORKING_IDLE) {
        this->show();
        this->showNormal();
        this->statusButtonClicked();
        QString time = QDateTime::currentDateTime().toString("hh:mm:ss (ddd)");
        QMessageBox::information(this, tr("Lifetime autostop"), tr("Lifetime stopped at ") + time);
    }
}

/*!
    \fn LifetimeMainWindow::toggleVisibility()
 */
void LifetimeMainWindow::toggleVisibility()
{
    if(this->isHidden())
    {
        this->show();
        if(this->isMinimized())
        {
            if(this->isMaximized())
                this->showMaximized();
            else
                this->showNormal();
        }
        this->raise();
        this->activateWindow();
    }
    else
    {
        this->hide();
    }
}


/*!
    \fn LifetimeMainWindow::redrawTheme()
 */
bool LifetimeMainWindow::redrawTheme()
{
    return this->theTheme->redrawMainWindow();
}


/*!
    \fn LifetimeMainWindow::getUi();
 */
Ui::lifetimeMainWidget LifetimeMainWindow::getUi()
{
    return this->ui;
}

/*!
    \fn LifetimeMainWindow::setState(int _newState)
 */
void LifetimeMainWindow::setState(int _newState)
{
    this->state = _newState;
}
