
#pragma once

#include <QList>
#include <QString>

class QLabel;
class QListWidget;
class QListWidgetItem;

namespace aeroarch::shell {

struct DesktopApp {
    QString name;
    QString exec;
    QString desktopFile;
    QString icon;
};

class AppSearch {
public:
    AppSearch();

    const QList<DesktopApp>& applications() const;

    void populate(QListWidget* list) const;
    void filter(
        QListWidget* list,
        QLabel* emptyLabel,
        const QString& query) const;

    bool launch(const QListWidgetItem* item) const;

private:
    QList<DesktopApp> discoverApplications() const;

    QList<DesktopApp> m_applications;
};

} // namespace aeroarch::shell
