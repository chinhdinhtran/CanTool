/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *btnOpenDbc;
    QLineEdit *editDbcPath;
    QPushButton *testDbcLog;
    QPushButton *btnClearDbc;
    QPushButton *disp_graphic;
    QPushButton *showTraceButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->setEnabled(true);
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        btnOpenDbc = new QPushButton(centralwidget);
        btnOpenDbc->setObjectName("btnOpenDbc");
        btnOpenDbc->setGeometry(QRect(30, 10, 81, 31));
        editDbcPath = new QLineEdit(centralwidget);
        editDbcPath->setObjectName("editDbcPath");
        editDbcPath->setEnabled(false);
        editDbcPath->setGeometry(QRect(140, 20, 151, 21));
        testDbcLog = new QPushButton(centralwidget);
        testDbcLog->setObjectName("testDbcLog");
        testDbcLog->setGeometry(QRect(30, 60, 81, 31));
        btnClearDbc = new QPushButton(centralwidget);
        btnClearDbc->setObjectName("btnClearDbc");
        btnClearDbc->setGeometry(QRect(140, 60, 81, 31));
        disp_graphic = new QPushButton(centralwidget);
        disp_graphic->setObjectName("disp_graphic");
        disp_graphic->setGeometry(QRect(540, 20, 101, 24));
        showTraceButton = new QPushButton(centralwidget);
        showTraceButton->setObjectName("showTraceButton");
        showTraceButton->setGeometry(QRect(540, 60, 101, 24));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        btnOpenDbc->setText(QCoreApplication::translate("MainWindow", "Open DBC", nullptr));
        editDbcPath->setText(QString());
        editDbcPath->setPlaceholderText(QCoreApplication::translate("MainWindow", "State load DBC file ", nullptr));
        testDbcLog->setText(QCoreApplication::translate("MainWindow", "Test DBC", nullptr));
        btnClearDbc->setText(QCoreApplication::translate("MainWindow", "Clear DBC", nullptr));
        disp_graphic->setText(QCoreApplication::translate("MainWindow", "Show graphic", nullptr));
        showTraceButton->setText(QCoreApplication::translate("MainWindow", "Show trace", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
