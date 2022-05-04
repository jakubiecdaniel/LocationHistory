#pragma once

#include <QWidget>
#include "ui_ServiceView.h"

class ServiceView : public QWidget
{
	Q_OBJECT

public:
	ServiceView(QWidget *parent = Q_NULLPTR);
	~ServiceView();

private slots:
	void on_serviceStartButton_clicked();
	void on_serviceStopButton_clicked();
	void on_serviceUninstallButton_clicked();
	void on_serviceRefreshButton_clicked();

private:
	void UpdateStatus();
	Ui::ServiceView ui;
};
