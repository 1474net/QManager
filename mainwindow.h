#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QFileSystemModel>
#include <QTreeView>
#include <QSortFilterProxyModel>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QSplitter>
#include <QSettings>
#include <QMainWindow>
#include <QClipboard>
#include <QMessageBox>
#include <QMimeData>
#include <QHeaderView>
#include <QCloseEvent>
#include <QProcess>
#include <QDebug>
#include <QSortFilterProxyModel>
#include <QMessageBox>
#include <QDialogButtonBox>

class Pane;
class PreviewPane;
class PathValidator;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);
    QFileSystemModel* fileSystemModel;
    QSortFilterProxyModel *modell;


    Pane* leftPane;
    Pane* rightPane;
    Pane* activePane;

    QMenu* contextMenu;
    QToolBar* toolBar;

    void setActivePane(Pane* pane);
    Pane* getActivePane();
    void moveTo(QString path);
    void updateViewActions();


public slots:
    void clipboardChanged();

protected:
    void closeEvent(QCloseEvent *event);

signals:

private:

      QMenuBar* menuBar;
      QMenu* fileMenu;
      QMenu* editMenu;
      QMenu* viewMenu;
      QMenu* helpMenu;
      QIcon* aboutIcon;
      QAction* exitAction;
      QAction* preferencesAction;
      QAction* cutAction;
      QAction* makeFile;
      QAction* copyAction;
      QAction* pasteAction;
      QAction* deleteAction;
      QAction* newFolderAction;
      QAction* detailViewAction;
      QAction* iconViewAction;
      QAction* hiddenAction;
      QAction* renameAction;
      QActionGroup* viewActionGroup;
      QAction* aboutAction;
      QAction* propertiesAction;
      QSplitter* splitter;
      QItemSelectionModel* treeSelectionModel;
      QSettings* settings;

      void createActionsAndMenus();
      bool hasItem(QString, QString);
      void saveState();
      void restoreState();
      void activeView();


private slots:
      void rename();
      void cut();
      void copy();
      void paste();
      void del();
      void newFolder();
      void toggleHidden();
      bool MakeFile();
      void showProperties();
      void showAboutBox();
      void toggleToDetailView();
      void toggleToIconView();
      void focusChangedSlot(QWidget *, QWidget *now);

};



#endif // MAINWINDOW_H
