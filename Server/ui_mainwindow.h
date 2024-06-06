/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
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
    QLineEdit *PortEdit;
    QPushButton *PortButton;
    QLineEdit *ShowClient1;
    QLabel *label;
    QLabel *label_2;
    QLineEdit *ShowClient2;
    QPushButton *restart;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(378, 212);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        PortEdit = new QLineEdit(centralwidget);
        PortEdit->setObjectName(QString::fromUtf8("PortEdit"));
        PortEdit->setGeometry(QRect(60, 20, 111, 21));
        PortEdit->setAlignment(Qt::AlignCenter);
        PortButton = new QPushButton(centralwidget);
        PortButton->setObjectName(QString::fromUtf8("PortButton"));
        PortButton->setGeometry(QRect(200, 20, 111, 21));
        ShowClient1 = new QLineEdit(centralwidget);
        ShowClient1->setObjectName(QString::fromUtf8("ShowClient1"));
        ShowClient1->setGeometry(QRect(60, 60, 111, 20));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(200, 60, 111, 20));
        label->setAlignment(Qt::AlignCenter);
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(200, 100, 111, 20));
        label_2->setAlignment(Qt::AlignCenter);
        ShowClient2 = new QLineEdit(centralwidget);
        ShowClient2->setObjectName(QString::fromUtf8("ShowClient2"));
        ShowClient2->setGeometry(QRect(60, 100, 111, 20));
        restart = new QPushButton(centralwidget);
        restart->setObjectName(QString::fromUtf8("restart"));
        restart->setGeometry(QRect(60, 140, 251, 20));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 378, 17));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        PortEdit->setText(QCoreApplication::translate("MainWindow", "10086", nullptr));
        PortButton->setText(QCoreApplication::translate("MainWindow", "Listen Port", nullptr));
        ShowClient1->setText(QString());
        label->setText(QCoreApplication::translate("MainWindow", "Client 1", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Client 2", nullptr));
        ShowClient2->setText(QString());
        restart->setText(QCoreApplication::translate("MainWindow", "Restart", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
