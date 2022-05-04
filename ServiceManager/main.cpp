#include <QtWidgets/QApplication>

#include "ServiceManager.h"
#include "ServiceView.h"
#include "ServiceControls.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    ServiceManager w;

    if (SERVICE_NOT_INSTALLED == isServiceInstalled()) {

        w.setWindowTitle(QApplication::translate("toplevel", "Location History Setup"));
        w.show();

    }
    else {
        ServiceView* sv = new ServiceView();

        sv->setWindowTitle(QApplication::translate("toplevel", "Location History"));
        sv->show();
    }

    return a.exec();

}
