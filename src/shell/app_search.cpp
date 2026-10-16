
#include "aeroarch/shell/app_search.hpp"

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>
#include <QIcon>

#include <algorithm>

namespace aeroarch::shell {

AppSearch::AppSearch()
    : m_applications(discoverApplications())
{
}

const QList<DesktopApp>& AppSearch::applications() const
{
    return m_applications;
}

QList<DesktopApp> AppSearch::discoverApplications() const
{
    QList<DesktopApp> apps;
    QMap<QString, bool> seen;

    const QStringList locations =
        QStandardPaths::standardLocations(
            QStandardPaths::GenericDataLocation);

    for (const QString& location : locations) {
        const QString appDir = location + "/applications";

        QDirIterator it(
            appDir,
            QStringList{"*.desktop"},
            QDir::Files,
            QDirIterator::Subdirectories);

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
                type != "Application" || hidden ||
                noDisplay || seen.contains(id)) {
                continue;
            }

            seen.insert(id, true);
            apps.append({name, exec, path, icon});
        }
    }

    std::sort(
        apps.begin(), apps.end(),
        [](const DesktopApp& a, const DesktopApp& b) {
            return a.name.toCaseFolded() <
                   b.name.toCaseFolded();
        });

    return apps;
}

void AppSearch::populate(QListWidget* list) const
{
    if (!list)
        return;

    list->clear();

    for (const DesktopApp& app : m_applications) {
        auto* item = new QListWidgetItem(
            QIcon::fromTheme(app.icon), app.name);

        item->setData(Qt::UserRole, app.desktopFile);
        item->setData(Qt::UserRole + 1, app.exec);
        item->setData(Qt::UserRole + 2, app.icon);

        list->addItem(item);
    }
}

void AppSearch::filter(
    QListWidget* list,
    QLabel* emptyLabel,
    const QString& query) const
{
    if (!list)
        return;

    int visibleCount = 0;

    for (int i = 0; i < list->count(); ++i) {
        auto* item = list->item(i);

        const bool matches =
            item->text().contains(
                query, Qt::CaseInsensitive);

        item->setHidden(!matches);

        if (matches)
            ++visibleCount;
    }

    if (emptyLabel)
        emptyLabel->setVisible(visibleCount == 0);
}

bool AppSearch::launch(const QListWidgetItem* item) const
{
    if (!item || item->isHidden())
        return false;

    QStringList args =
        QProcess::splitCommand(
            item->data(Qt::UserRole + 1).toString());

    // Remove desktop-entry field codes.
    for (int i = args.size() - 1; i >= 0; --i) {
        if (args[i].startsWith('%') ||
            args[i].contains('%')) {
            args.removeAt(i);
        }
    }

    if (args.isEmpty())
        return false;

    const QString program = args.takeFirst();

    return QProcess::startDetached(
        program, args, QDir::homePath());
}

} // namespace aeroarch::shell
