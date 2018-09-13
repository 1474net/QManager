#ifndef PANE_H
#define PANE_H

//#include <QtGui>
#include <QTreeView>
#include <QListView>
#include <QLineEdit>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QUrl>
#include <QDesktopServices>
#include <QDebug>
#include <QComboBox>

class MainWindow;

class Pane : public QFrame
{
    Q_OBJECT
public:
    explicit Pane(QWidget* parent=0);

    MainWindow *mainWindow;
    QListView *listView;
    QTreeView *treeView;
    QLineEdit *pathLineEdit;
    QStackedWidget *stackedWidget;
    QComboBox *combox;
    QString currentPath;

    enum ViewMode
    {
        TreeViewMode,
        ListViewMode
    };

    void moveTo(const QString& path);
    void setActive(bool active);
    bool isActive() const;
    void setViewTo(const ViewMode viewMode);

protected:

private:
    QHBoxLayout* hBoxLayout;
    QVBoxLayout* vBoxLayout;
    bool active;

signals:

private slots:
    void doubleClickedOnEntry(QModelIndex index);
    void pathLineEditChanged();
    void showContextMenu(const QPoint&);
    void OpenDisc(QString Disc);

};

#endif // PANE_H
