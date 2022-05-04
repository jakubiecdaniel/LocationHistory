#include "ServiceView.h"
#include "ServiceControls.h"
#include "Config.h"

ServiceView::ServiceView(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->ui.errorLabel->hide();

	UpdateStatus();
}

ServiceView::~ServiceView()
{
}

void ServiceView::on_serviceStartButton_clicked() {
	std::vector<KeyValue> kvs = GetConfig();

	if (kvs.size() != 2) {
		this->ui.errorLabel->setText("Error retrieving config...");
		return;
	}

	KeyValue kv1 = kvs.at(0);
	KeyValue kv2 = kvs.at(1);

	// Arg format: USER_ID API_KEY
	LPCWSTR szArgs[2];

	std::wstring wkv1 = kv1.GetValue();
	std::wstring wkv2 = kv2.GetValue();

	szArgs[0] = wkv1.c_str();
	szArgs[1] = wkv2.c_str();

	//std::wstring args = L" " + kv1.GetValue() + L"\0 " + kv2.GetValue() + L"\0";

	this->ui.errorLabel->show();
	if (!doStartService(2, szArgs)) {
		this->ui.errorLabel->setText("Error starting service");
	}
	else {
		this->ui.errorLabel->setText("Service started...");
	}
	UpdateStatus();

}

void ServiceView::on_serviceStopButton_clicked() {
	this->ui.errorLabel->show();
	if (!StopService()) {
		this->ui.errorLabel->setText("Error stopping service");
	}
	else {
		this->ui.errorLabel->setText("Service stopped...");
	}
	UpdateStatus();
}

void ServiceView::on_serviceUninstallButton_clicked() {
	this->ui.errorLabel->show();
	if (!DeleteService()) {
		this->ui.errorLabel->setText("Error uninstalling service");
	}
	else {
		this->ui.errorLabel->setText("Service uninstalled...");
	}
	UpdateStatus();
}

void ServiceView::on_serviceRefreshButton_clicked() {
	this->ui.errorLabel->hide();
	UpdateStatus();
}


void ServiceView::UpdateStatus() {
	ServiceStatus status = GetServiceStatus();

	QString statusText = "";

	switch (status) {
	case ERROR_GETTING_STATUS:
		statusText = "Error retrieving status...";
		break;
	case RUNNING:
		statusText = "Service Running...";
		break;
	case STOPPED:
		statusText = "Service Stopped...";
		break;
	case START_PENDING:
		statusText = "Service Start Pending...";
		break;
	case STOP_PENDING:
		statusText = "Service Stop Pending...";
		break;
	case UNKNOWN_STATUS:
		statusText = "Unknown Status...";
		break;

	}

	this->ui.ServiceStatusLabel->setText(statusText);
}