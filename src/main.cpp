#include "ui/MainWindow.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("HalbyFbxEditor"));
    app.setOrganizationName(QStringLiteral("Halby"));

    MainWindow window;
    window.show();

    return app.exec();
}
