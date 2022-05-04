#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ServiceManager.h"

class ServiceManager : public QMainWindow
{
    Q_OBJECT

public:
    ServiceManager(QWidget *parent = Q_NULLPTR);

private slots:
    void on_pushButton_clicked();

private:
    Ui::ServiceManagerClass ui;
};
