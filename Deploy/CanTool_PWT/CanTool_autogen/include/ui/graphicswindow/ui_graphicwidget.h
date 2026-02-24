/********************************************************************************
** Form generated from reading UI file 'graphicwidget.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GRAPHICWIDGET_H
#define UI_GRAPHICWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GraphicWidget
{
public:

    void setupUi(QWidget *GraphicWidget)
    {
        if (GraphicWidget->objectName().isEmpty())
            GraphicWidget->setObjectName("GraphicWidget");
        GraphicWidget->resize(400, 300);

        retranslateUi(GraphicWidget);

        QMetaObject::connectSlotsByName(GraphicWidget);
    } // setupUi

    void retranslateUi(QWidget *GraphicWidget)
    {
        GraphicWidget->setWindowTitle(QCoreApplication::translate("GraphicWidget", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GraphicWidget: public Ui_GraphicWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GRAPHICWIDGET_H
