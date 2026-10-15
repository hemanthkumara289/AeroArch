
#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QWidget>

#include "aeroarch/core/version.hpp"
#include "aeroarch/system/system_info.hpp"
#include "aeroarch/system/system_info_format.hpp"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QMainWindow window;
    window.setWindowTitle("AeroArch");
    window.resize(850, 550);

    auto* central = new QWidget(&window);
    auto* layout = new QVBoxLayout(central);

    auto* title = new QLabel("AeroArch OS");
    title->setObjectName("title");

    auto* subtitle = new QLabel(QString("Version %1.%2.%3 — Development")
                                    .arg(aeroarch::VERSION_MAJOR)
                                    .arg(aeroarch::VERSION_MINOR)
                                    .arg(aeroarch::VERSION_PATCH));

    const auto info = aeroarch::system::get_system_info();

    auto* system_info =
        new QLabel(QString::fromStdString(aeroarch::system::format_system_info(info)));
    system_info->setObjectName("systemInfo");
    system_info->setTextInteractionFlags(Qt::TextSelectableByMouse);

    layout->addWidget(title);
    layout->addWidget(subtitle);
    layout->addSpacing(20);
    layout->addWidget(system_info);
    layout->addStretch();

    central->setStyleSheet(R"(
        QWidget {
            background-color: #101c30;
            color: #e8f1ff;
            font-family: "Ubuntu";
            font-size: 14px;
        }

        QLabel#title {
            font-size: 34px;
            font-weight: bold;
            color: #8dcaff;
        }

        QLabel#systemInfo {
            background-color: #192b43;
            border: 1px solid #355574;
            border-radius: 8px;
            padding: 18px;
            font-family: monospace;
            font-size: 13px;
        }
    )");

    window.setCentralWidget(central);
    window.show();

    return app.exec();
}
