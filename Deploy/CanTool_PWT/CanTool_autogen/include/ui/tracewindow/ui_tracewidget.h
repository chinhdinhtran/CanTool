/********************************************************************************
** Form generated from reading UI file 'tracewidget.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRACEWIDGET_H
#define UI_TRACEWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TraceWidget
{
public:
    QPushButton *homeButton;

    void setupUi(QWidget *TraceWidget)
    {
        if (TraceWidget->objectName().isEmpty())
            TraceWidget->setObjectName("TraceWidget");
        TraceWidget->resize(400, 300);
        homeButton = new QPushButton(TraceWidget);
        homeButton->setObjectName("homeButton");
        homeButton->setGeometry(QRect(0, 0, 75, 24));

        retranslateUi(TraceWidget);

        QMetaObject::connectSlotsByName(TraceWidget);
    } // setupUi

    void retranslateUi(QWidget *TraceWidget)
    {
        TraceWidget->setWindowTitle(QCoreApplication::translate("TraceWidget", "Form", nullptr));
        homeButton->setText(QCoreApplication::translate("TraceWidget", "Home", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TraceWidget: public Ui_TraceWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRACEWIDGET_H
