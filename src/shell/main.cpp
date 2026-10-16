#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMainWindow>
#include <QMenu>
#include <QProcess>
#include <QPushButton>
#include <QSettings>
#include <QStandardPaths>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <QMap>
#include <QStyle>
#include <QDesktopServices>
#include <QUrl>
#include <QKeySequence>
#include <QShortcut>
#include <QIcon>
#include <QScrollBar>
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QColor>
#include <QPixmap>
#include <QPen>
#include <QAbstractItemView>
#include <QPainter>
#include <QPainterPath>
#include <algorithm>
struct DesktopApp {
    QString name;
    QString exec;
    QString desktopFile;
    QString icon;
};
static QList<DesktopApp> discoverApplications()
{
    QList<DesktopApp> apps;
    QMap<QString, bool> seen;
    const auto locations =
        QStandardPaths::standardLocations(
            QStandardPaths::GenericDataLocation);
    for (const QString& location : locations) {
        const QString appDir = location + "/applications";
        QDirIterator it(
            appDir, QStringList{"*.desktop"},
            QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            const QString path = it.next();
            QSettings desktop(path, QSettings::IniFormat);
            desktop.beginGroup("Desktop Entry");
            const QString name =
                desktop.value("Name").toString().trimmed();
            const QString exec =
                desktop.value("Exec").toString().trimmed();
            const QString icon =
                desktop.value("Icon").toString();
            const QString type =
                desktop.value("Type", "Application").toString();
            const bool hidden =
                desktop.value("Hidden", false).toBool();
            const bool noDisplay =
                desktop.value("NoDisplay", false).toBool();
            desktop.endGroup();
            const QString id = QFileInfo(path).fileName();
            if (name.isEmpty() || exec.isEmpty() ||
                type != "Application" || hidden || noDisplay ||
                seen.contains(id))
                continue;
            seen.insert(id, true);
            apps.append({name, exec, path, icon});
        }
    }
    std::sort(apps.begin(), apps.end(),
        [](const DesktopApp& a, const DesktopApp& b) {
            return a.name.toCaseFolded() <
                   b.name.toCaseFolded();
        });
    return apps;
}
static bool launchApplication(const DesktopApp& app)
{
    QStringList args = QProcess::splitCommand(app.exec);
    for (int i = args.size() - 1; i >= 0; --i) {
        if (args[i].startsWith('%') ||
            args[i].contains('%'))
            args.removeAt(i);
    }
    if (args.isEmpty())
        return false;
    const QString program = args.takeFirst();
    return QProcess::startDetached(
        program, args, QDir::homePath());
}
static void openFolder(const QString& folder)
{
    QDesktopServices::openUrl(
        QUrl::fromLocalFile(
            QDir::homePath() + "/" + folder));
}
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("AeroShell");
    const auto applications = discoverApplications();
    QMainWindow window;
    window.setWindowTitle("AeroShell - AeroArch");
    window.resize(1000, 650);
    window.setMinimumSize(700, 450);
    // Desktop
    auto* desktop = new QWidget;
    desktop->setObjectName("desktop");
    auto* root = new QVBoxLayout(desktop);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);
    auto* workspace = new QWidget;
    auto* workspaceLayout = new QVBoxLayout(workspace);
    workspaceLayout->setContentsMargins(32, 28, 32, 28);
    auto* title = new QLabel("AeroArch");
    title->setObjectName("desktopTitle");
    auto* subtitle = new QLabel(
        "AeroShell desktop environment — Development");
    subtitle->setObjectName("desktopSubtitle");
    workspaceLayout->addWidget(title);
    workspaceLayout->addWidget(subtitle);
    workspaceLayout->addStretch();
    // Bottom panel
    auto* panel = new QFrame;
    panel->setObjectName("panel");
    panel->setFixedHeight(54);
    auto* panelLayout = new QHBoxLayout(panel);
    panelLayout->setContentsMargins(10, 5, 10, 5);
    panelLayout->setSpacing(10);
    auto* startButton = new QPushButton("AeroArch");
    startButton->setObjectName("startButton");
    auto* taskArea = new QLabel("AeroShell");
    taskArea->setObjectName("taskArea");
    auto* tray = new QLabel("Network  •  Sound");
    tray->setObjectName("tray");
    auto* clock = new QLabel;
    clock->setObjectName("clock");
    clock->setMinimumWidth(145);
    clock->setAlignment(Qt::AlignCenter);
    panelLayout->addWidget(startButton);
    panelLayout->addWidget(taskArea);
    panelLayout->addStretch();
    panelLayout->addWidget(tray);
    panelLayout->addWidget(clock);
    root->addWidget(workspace, 1);
    root->addWidget(panel);
    // Windows 7-inspired Start menu
    auto* menu = new QWidget(nullptr,
        Qt::Window | Qt::FramelessWindowHint |
        Qt::WindowStaysOnTopHint);
    menu->setObjectName("startMenu");
    menu->setWindowTitle("AeroArch Start");
    menu->resize(500, 550);
    auto* menuRoot = new QVBoxLayout(menu);
    menuRoot->setContentsMargins(8, 8, 8, 8);
    menuRoot->setSpacing(0);
    // User header
    auto* userHeader = new QFrame;
    userHeader->setObjectName("userHeader");
    userHeader->setFixedHeight(66);
    auto* userLayout = new QHBoxLayout(userHeader);
    userLayout->setContentsMargins(8, 6, 8, 6);
    auto* avatar = new QLabel;
    avatar->setFixedSize(48, 48);
    avatar->setAlignment(Qt::AlignCenter);
    // Draw a self-contained circular user avatar (no external image needed).
    QPixmap avatarPixmap(48, 48);
    avatarPixmap.fill(Qt::transparent);
    {
        QPainter painter(&avatarPixmap);
        painter.setRenderHint(QPainter::Antialiasing, true);

        QLinearGradient background(0, 0, 0, 48);
        background.setColorAt(0.0, QColor("#eaf7ff"));
        background.setColorAt(0.48, QColor("#8fc8f1"));
        background.setColorAt(1.0, QColor("#3d78ad"));
        painter.setBrush(background);
        painter.setPen(QPen(QColor("#ffffff"), 2));
        painter.drawEllipse(QRectF(2, 2, 44, 44));

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor("#315a7d"));
        painter.drawEllipse(QRectF(17, 10, 14, 14));
        painter.drawRoundedRect(QRectF(10, 26, 28, 15), 8, 8);

        painter.setBrush(QColor(255, 255, 255, 55));
        painter.drawEllipse(QRectF(7, 5, 34, 13));
    }
    avatar->setPixmap(avatarPixmap);
    avatar->setObjectName("avatar");
    auto* userName = new QLabel("AeroArch User");
    userName->setObjectName("userName");
    userLayout->addWidget(avatar);
    userLayout->addWidget(userName);
    userLayout->addStretch();
    // Two-column content
    auto* columns = new QHBoxLayout;
    columns->setSpacing(0);
    auto* leftColumn = new QWidget;
    leftColumn->setObjectName("leftColumn");
    auto* leftLayout = new QVBoxLayout(leftColumn);
    leftLayout->setContentsMargins(8, 10, 8, 8);
    leftLayout->setSpacing(7);
    auto* search = new QLineEdit;
    search->setPlaceholderText("Search programs and files");
    search->setClearButtonEnabled(true);
    search->setObjectName("menuSearch");
    auto* appList = new QListWidget;
    appList->setObjectName("appList");
    appList->setIconSize(QSize(28, 28));
    appList->setSpacing(2);
    appList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    appList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    auto* emptyLabel = new QLabel("No programs found");
    emptyLabel->setAlignment(Qt::AlignCenter);
    emptyLabel->hide();
    for (const auto& item : applications) {
        auto* row = new QListWidgetItem(
            QIcon::fromTheme(item.icon), item.name);
        row->setData(Qt::UserRole, item.desktopFile);
        row->setData(Qt::UserRole + 1, item.exec);
        row->setData(Qt::UserRole + 2, item.icon);
        appList->addItem(row);
    }
    auto* allPrograms = new QPushButton("All Programs  ›");
    allPrograms->setObjectName("allPrograms");
    leftLayout->addWidget(appList, 1);
    leftLayout->addWidget(emptyLabel);
    leftLayout->addWidget(allPrograms);
    leftLayout->addWidget(search);
    auto* rightColumn = new QFrame;
    rightColumn->setObjectName("rightColumn");
    rightColumn->setFixedWidth(185);
    auto* rightLayout = new QVBoxLayout(rightColumn);
    rightLayout->setContentsMargins(8, 12, 8, 8);
    rightLayout->setSpacing(3);
    auto addSystemLink = [&](const QString& text,
                             const QString& folder,
                             const QIcon& icon) {
        auto* button = new QPushButton(icon, text);
        button->setObjectName("systemLink");
        button->setCursor(Qt::PointingHandCursor);
        QObject::connect(button, &QPushButton::clicked,
            [folder, menu]() {
                menu->hide();
                if (folder == "computer") {
                    QDesktopServices::openUrl(
                        QUrl("file:///"));
                } else if (folder == "settings") {
                    QProcess::startDetached(
                        "systemsettings", {});
                } else {
                    openFolder(folder);
                }
            });
        rightLayout->addWidget(button);
    };
    QIcon folderIcon =
        window.style()->standardIcon(QStyle::SP_DirIcon);
    addSystemLink("Documents", "Documents", folderIcon);
    addSystemLink("Pictures", "Pictures", folderIcon);
    addSystemLink("Music", "Music", folderIcon);
    addSystemLink("Videos", "Videos", folderIcon);
    auto* separator = new QFrame;
    separator->setFrameShape(QFrame::HLine);
    separator->setObjectName("menuSeparator");
    rightLayout->addWidget(separator);
    addSystemLink("Computer", "computer",
        window.style()->standardIcon(
            QStyle::SP_ComputerIcon));
    addSystemLink("Control Panel", "settings",
        window.style()->standardIcon(
            QStyle::SP_FileDialogDetailedView));
    rightLayout->addStretch();
    columns->addWidget(leftColumn, 1);
    columns->addWidget(rightColumn);
    // Shutdown area
    auto* bottom = new QFrame;
    bottom->setObjectName("shutdownArea");
    bottom->setFixedHeight(52);
    auto* bottomLayout = new QHBoxLayout(bottom);
    bottomLayout->setContentsMargins(8, 4, 8, 4);
    auto* shutdownButton = new QPushButton("Shut down");
    shutdownButton->setObjectName("shutdownButton");
    auto* shutdownArrow = new QPushButton("▼");
    shutdownArrow->setObjectName("shutdownArrow");
    shutdownArrow->setFixedWidth(30);
    auto* shutdownMenu = new QMenu(menu);
    shutdownMenu->addAction("Sleep");
    shutdownMenu->addAction("Restart", []() {
        QProcess::startDetached(
            "systemctl", {"reboot"});
    });
    shutdownMenu->addAction("Shut down", []() {
        QProcess::startDetached(
            "systemctl", {"poweroff"});
    });
    QObject::connect(shutdownButton, &QPushButton::clicked,
        [&shutdownMenu, &shutdownArrow]() {
            shutdownMenu->exec(
                shutdownArrow->mapToGlobal(
                    QPoint(0, -shutdownMenu->sizeHint().height())));
        });
    QObject::connect(shutdownArrow, &QPushButton::clicked,
        [&shutdownMenu, shutdownArrow]() {
            shutdownMenu->exec(
                shutdownArrow->mapToGlobal(
                    QPoint(0, -shutdownMenu->sizeHint().height())));
        });
    bottomLayout->addStretch();
    bottomLayout->addWidget(shutdownButton);
    bottomLayout->addWidget(shutdownArrow);
    menuRoot->addWidget(userHeader);
    menuRoot->addLayout(columns, 1);
    menuRoot->addWidget(bottom);
    // Search filtering
    auto filterApps = [appList, emptyLabel](const QString& query) {
        int count = 0;
        for (int i = 0; i < appList->count(); ++i) {
            auto* item = appList->item(i);
            const bool match =
                item->text().contains(
                    query, Qt::CaseInsensitive);
            item->setHidden(!match);
            if (match)
                ++count;
        }
        emptyLabel->setVisible(count == 0);
    };
    QObject::connect(search, &QLineEdit::textChanged,
                     filterApps);
    auto launchSelected = [appList, menu]() {
        auto* item = appList->currentItem();
        if (!item || item->isHidden())
            return;
        DesktopApp selected {
            item->text(),
            item->data(Qt::UserRole + 1).toString(),
            item->data(Qt::UserRole).toString(),
            item->data(Qt::UserRole + 2).toString()
        };
        if (launchApplication(selected))
            menu->hide();
    };
    QObject::connect(appList, &QListWidget::itemDoubleClicked,
        [launchSelected](QListWidgetItem*) {
            launchSelected();
        });
    QObject::connect(search, &QLineEdit::returnPressed,
        [appList, launchSelected]() {
            if (!appList->currentItem()) {
                for (int i = 0; i < appList->count(); ++i) {
                    if (!appList->item(i)->isHidden()) {
                        appList->setCurrentRow(i);
                        break;
                    }
                }
            }
            launchSelected();
        });
    QObject::connect(allPrograms, &QPushButton::clicked,
        [search, appList]() {
            search->clear();
            appList->setCurrentRow(-1);
            appList->setFocus();
        });
    QObject::connect(startButton, &QPushButton::clicked,
        [&window, menu, search, appList]() {
            if (menu->isVisible()) {
                menu->hide();
                return;
            }
            search->clear();
            appList->setCurrentRow(-1);
            const QPoint pos = window.mapToGlobal(
                QPoint(0, window.height() - 54 - menu->height()));
            menu->move(pos);
            menu->show();
            menu->raise();
            menu->activateWindow();
            search->setFocus();
        });
    auto* shortcut = new QShortcut(
        QKeySequence("Ctrl+Space"), &window);
    QObject::connect(shortcut, &QShortcut::activated,
        [startButton]() {
            startButton->click();
        });
    // Clock
    auto* timer = new QTimer(&window);
    auto updateClock = [clock]() {
        clock->setText(
            QDateTime::currentDateTime().toString(
                "HH:mm:ss\ndd MMM yyyy"));
    };
    QObject::connect(timer, &QTimer::timeout, updateClock);
    updateClock();
    timer->start(1000);
    // Desktop and taskbar styling
    desktop->setStyleSheet(R"(
        QWidget#desktop {
            background-color: #10233d;
            color: #edf5ff;
            font-family: "Noto Sans";
        }
        QLabel#desktopTitle {
            font-size: 38px;
            font-weight: bold;
            color: #a6d8ff;
        }
        QLabel#desktopSubtitle {
            font-size: 14px;
            color: #c0d3e8;
        }
        QFrame#panel {
            background-color: #172b43;
            border-top: 1px solid #496a89;
        }
        QPushButton#startButton {
            background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                stop:0 #719dc4, stop:0.5 #426f9d, stop:1 #31577c);
            color: white;
            border: 1px solid #9abbd8;
            border-radius: 5px;
            padding: 8px;
            font-weight: bold;
        }
        QPushButton#startButton:hover {
            background: #6598c5;
        }
        QLabel#taskArea {
            background-color: #203952;
            border: 1px solid #355573;
            border-radius: 4px;
            padding: 7px 14px;
        }
        QLabel#tray, QLabel#clock {
            color: #edf5ff;
            padding: 5px;
        }
    )");
    // Glossy blue Start menu styling
    menu->setStyleSheet(R"(
        QWidget#startMenu {
            background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
                stop:0 #f7fbff, stop:0.58 #e8f2fc,
                stop:0.59 #d8e8f7, stop:1 #c4dbef);
            border: 1px solid #7899b9;
            border-radius: 6px;
            font-family: "Noto Sans";
            font-size: 12px;
        }
        QFrame#userHeader {
            background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                stop:0 #8fc5ed, stop:0.12 #5c9ed2,
                stop:0.48 #3976a8, stop:1 #1d456b);
            border: 1px solid #173c60;
            border-radius: 5px;
        }
        QLabel#userName {
            color: white;
            font-size: 16px;
            font-weight: bold;
        }
        QLabel#avatar {
            background: transparent;
            border: none;
            border-radius: 24px;
        }
        QWidget#leftColumn {
            background-color: #f9fcff;
            border-right: 1px solid #b4cce3;
        }
        QFrame#rightColumn {
            background: transparent;
        }
        QListWidget#appList {
            background: transparent;
            color: #172c43;
            border: none;
            outline: none;
        }
        QListWidget#appList::item {
            padding: 6px;
            border: 1px solid transparent;
        }
        QListWidget#appList::item:hover {
            background: #dcecfb;
            border: 1px solid #a6c8e8;
            border-radius: 3px;
        }
        QListWidget#appList::item:selected {
            background: #c6e1fa;
            color: #142c46;
            border: 1px solid #8bb8e3;
            border-radius: 3px;
        }
        QListWidget#appList QScrollBar:vertical {
            background: #edf5fc;
            width: 10px;
            margin: 2px;
            border: none;
        }

        QListWidget#appList QScrollBar::handle:vertical {
            background: #b6cde3;
            min-height: 28px;
            border: 1px solid #9bb8d3;
            border-radius: 4px;
        }

        QListWidget#appList QScrollBar::handle:vertical:hover {
            background: #8fb4d8;
            border: 1px solid #6f9fc9;
        }

        QListWidget#appList QScrollBar::add-line:vertical,
        QListWidget#appList QScrollBar::sub-line:vertical {
            height: 0;
            border: none;
        }

        QListWidget#appList QScrollBar::add-page:vertical,
        QListWidget#appList QScrollBar::sub-page:vertical {
            background: transparent;
        }

        QLineEdit#menuSearch {
            background: white;
            border: 1px solid #8fa9c3;
            border-radius: 3px;
            padding: 7px;
            color: #172c43;
        }
        QPushButton#allPrograms {
            background: transparent;
            border: none;
            color: #1a4269;
            font-weight: bold;
            padding: 8px;
        }
        QPushButton#allPrograms:hover {
            background: #dcecfb;
        }
        QPushButton#systemLink {
            background: transparent;
            border: 1px solid transparent;
            color: #183956;
            text-align: left;
            padding: 7px 4px;
            border-radius: 3px;
        }
        QPushButton#systemLink:hover {
            background: #dcecfb;
            border: 1px solid #b0cee9;
        }
        QFrame#menuSeparator {
            color: #b4cce3;
            margin-top: 6px;
            margin-bottom: 6px;
        }
        QFrame#shutdownArea {
            background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                stop:0 #d7e8f8, stop:1 #aecbe5);
            border-top: 1px solid #a2bfdc;
        }
        QPushButton#shutdownButton {
            background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                stop:0 #75a0c5, stop:0.5 #4779a5, stop:1 #315c83);
            color: white;
            font-weight: bold;
            border: 1px solid #284d70;
            border-radius: 3px;
            padding: 8px 14px;
        }
        QPushButton#shutdownArrow {
            background: #4779a5;
            color: white;
            border: 1px solid #284d70;
            border-radius: 3px;
        }
        QPushButton#shutdownButton:hover,
        QPushButton#shutdownArrow:hover {
            background: #6697c2;
        }
    )");
    window.setCentralWidget(desktop);
    window.show();
    return app.exec();
}
