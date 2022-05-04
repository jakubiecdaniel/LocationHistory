#include "ServiceManager.h"
#include "ServiceView.h"
#include "ServiceControls.h"
#include "Config.h"

ServiceManager::ServiceManager(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.errorLabel->hide();
    this->ui.FilePath->setText("C:\\Users\\Admin\\Source\\Repos\\WindowsLocation\\Debug\\WindowsLocation.exe");
}

// For the application to react appropriately when the Add button is clicked, you must implement a slot called on_addButton_clicked().


void ServiceManager::on_pushButton_clicked() {

    //C:\Users\Admin\Source\Repos\WindowsLocation\Debug
    QString ApiKey = this->ui.ApiKey->text();
    QString UserId = this->ui.UserId->text();



    QString FilePath = this->ui.FilePath->text();

    if (ApiKey.isEmpty() || UserId.isEmpty()) {
        this->ui.errorLabel->setText("API Key or User ID must not be empty");
        this->ui.errorLabel->show();
    }
    else {
        std::vector<KeyValue> kvs;

        KeyValue id(L"USER_ID", UserId.toStdWString());
        KeyValue api(L"API_KEY", ApiKey.toStdWString());

        kvs.push_back(id);
        kvs.push_back(api);

        ConfigFile(kvs);

        std::wstring szPath = FilePath.toStdWString();

        if (!InstallService(szPath.data())) {
            this->ui.errorLabel->setText("Error installing service");
            this->ui.errorLabel->show();
        }
        else {
            this->ui.errorLabel->hide();
            this->close();

            ServiceView* sv = new ServiceView();

            sv->show();
        }


    }


}