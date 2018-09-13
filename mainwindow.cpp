#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pane.h"
#include "properties.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle(tr("SF manager"));
    setUnifiedTitleAndToolBarOnMac(true);
    createActionsAndMenus();

    settings = new QSettings("Free Software", "Gento");
    splitter = new QSplitter(this);

    fileSystemModel = new QFileSystemModel;
    fileSystemModel->setFilter(QDir::NoDot | QDir::AllEntries | QDir::System);
    fileSystemModel->setRootPath("");
    fileSystemModel->setReadOnly(false);


    leftPane = new Pane(splitter);
    rightPane = new Pane(splitter);

    connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), SLOT(focusChangedSlot(QWidget*, QWidget*)));

    modell = new QSortFilterProxyModel();
    modell->setSourceModel(fileSystemModel);
    modell->setSortCaseSensitivity(Qt::CaseInsensitive);

    splitter->addWidget(leftPane);
    splitter->addWidget(rightPane);
    splitter->setHandleWidth(3);
    this->setCentralWidget(splitter);
    connect(QApplication::clipboard(), SIGNAL(changed(QClipboard::Mode)), this, SLOT(clipboardChanged()));

}
void MainWindow::focusChangedSlot(QWidget *, QWidget *now)
{
    if (now == rightPane->pathLineEdit || now == rightPane->treeView || now == rightPane->listView)
        setActivePane(rightPane);
    else if (now == leftPane->pathLineEdit || now == leftPane->treeView || now == leftPane->listView)
        setActivePane(leftPane);
}

void MainWindow::setActivePane(Pane* pane)
{
    pane->setActive(true);
    if (pane == leftPane)
        rightPane->setActive(false);
    else
        leftPane->setActive(false);
    activePane = pane;
    updateViewActions();
}


void MainWindow::clipboardChanged()
{
    if(QApplication::clipboard()->mimeData()->hasUrls())
        pasteAction->setEnabled(true);
    else
    {
        pasteAction->setEnabled(false);
    }
}


Pane* MainWindow::getActivePane()
{
    return(activePane);
}


void MainWindow::updateViewActions()
{
    switch (activePane->stackedWidget->currentIndex())
    {
    case Pane::TreeViewMode:
        detailViewAction->setChecked(true);
        break;
    case Pane::ListViewMode:
        iconViewAction->setChecked(true);
        break;
    }
}


void MainWindow::cut()
{
    QModelIndexList selectionList;

    QWidget* focus(focusWidget());
    QAbstractItemView* view;
    if (focus == leftPane->treeView || focus == leftPane->listView || focus == rightPane->treeView || focus == rightPane->listView){
        view =  qobject_cast<QAbstractItemView *>(focus);
        selectionList = view->selectionModel()->selectedIndexes();
    }

    if(selectionList.count() == 0)
        return;

    QApplication::clipboard()->setMimeData(fileSystemModel->mimeData(selectionList));
    pasteAction->setData(true);

    view->selectionModel()->clear();
}


void MainWindow::copy()
{
    QModelIndexList selectionList;

    QWidget* focus(focusWidget());
    QAbstractItemView* view;
    if ( focus == leftPane->treeView || focus == leftPane->listView || focus == rightPane->treeView || focus == rightPane->listView){
        view =  qobject_cast<QAbstractItemView *>(focus);
        selectionList = view->selectionModel()->selectedIndexes();
    }

    if(selectionList.count() == 0)
        return;

    QApplication::clipboard()->setMimeData(fileSystemModel->mimeData(selectionList));
    pasteAction->setData(false);
}

void MainWindow::paste()
{

    QWidget* focus(focusWidget());
    Qt::DropAction cutOrCopy(pasteAction->data().toBool() ? Qt::MoveAction : Qt::CopyAction);

    if (focus == leftPane->treeView || focus == leftPane->listView || focus == rightPane->treeView || focus == rightPane->listView){
        fileSystemModel->dropMimeData(QApplication::clipboard()->mimeData(), cutOrCopy, 0, 0, qobject_cast<QAbstractItemView *>(focus)->rootIndex());
    }
}

void MainWindow::newFolder()
{
    QAbstractItemView* currentView = qobject_cast<QAbstractItemView *>(getActivePane()->stackedWidget->currentWidget());

    int num = 1;
    QString name;
    QString path;
    path = fileSystemModel->filePath(currentView->rootIndex()) ;
    name = "New folder";
    while (hasItem(name, path)) {
        name = QString(tr("New folder %1")).arg(num);
        num++;
    }

    qDebug()<< path;
    QModelIndex newDir = fileSystemModel->mkdir(currentView->rootIndex(), name);

    if (newDir.isValid()) {
        currentView->selectionModel()->setCurrentIndex(newDir, QItemSelectionModel::ClearAndSelect);
        currentView->edit(newDir);
    }
}

void MainWindow::del()
{
    QModelIndexList selectionList;
    bool yesToAll = false;
    bool ok = false;
    bool confirm = true;

    QWidget* focus(focusWidget());
    QAbstractItemView* view;
    if ( focus == leftPane->treeView || focus == leftPane->listView || focus == rightPane->treeView || focus == rightPane->listView){
        view = qobject_cast<QAbstractItemView *>(focus);
        selectionList = view->selectionModel()->selectedIndexes();
    }

    for(int i = 0; i < selectionList.count(); ++i)
    {
        QFileInfo file(fileSystemModel->filePath(selectionList.at(i)));
        if(file.isWritable())
        {
            if(file.isSymLink()) ok = QFile::remove(file.filePath());
            else
            {
                if(!yesToAll)
                {
                    if(confirm)
                    {
                        int answer;
                        if(selectionList.count() > 1)
                           answer= QMessageBox::information(this, tr("Delete file"), "Are you sure you want to delete <p><b>\"" + file.filePath() + "</b>?",QMessageBox::Yes | QMessageBox::No | QMessageBox::YesToAll);
                        else
                           answer = QMessageBox::information(this, tr("Delete file"), "Are you sure you want to delete <p><b>\"" + file.filePath() + "</b>?",QMessageBox::Yes | QMessageBox::No);
                        if(answer == QMessageBox::YesToAll)
                            yesToAll = true;
                        if(answer == QMessageBox::No)
                            return;


                    }
                }
                ok = fileSystemModel->remove(selectionList.at(i));
            }
        }
        else if(file.isSymLink())
            ok = QFile::remove(file.filePath());
    }

    if(!ok)
        QMessageBox::information(this, tr("Delete Failed"), tr("Some files could not be deleted."));
}



void MainWindow::toggleHidden()
{
    if(hiddenAction->isChecked())
        fileSystemModel->setFilter(QDir::NoDot | QDir::AllEntries | QDir::System | QDir::Hidden);
    else
        fileSystemModel->setFilter(QDir::NoDot | QDir::AllEntries | QDir::System);
}

void MainWindow::closeEvent(QCloseEvent *event)
{

    /*event->accept();*/
}
bool MainWindow::hasItem(QString name, QString path)
{
    QModelIndexList items;

    for(int x = 0; x < fileSystemModel->rowCount(fileSystemModel->index(path)); ++x)
        items.append(fileSystemModel->index(x,0,fileSystemModel->index(path)));

    foreach(QModelIndex theItem,items)
    {
        if(fileSystemModel->fileName(theItem) == name)
            return true;
    }
    return false;
}

void MainWindow::toggleToDetailView()
{
    getActivePane()->setViewTo(Pane::TreeViewMode);
    qDebug() << "TreeViewMode";
}

void MainWindow::toggleToIconView()
{
    getActivePane()->setViewTo(Pane::ListViewMode);
    qDebug() << "ListViewMode";
}


bool MainWindow::MakeFile()
{
    QAbstractItemView* currentView = qobject_cast<QAbstractItemView *>(getActivePane()->stackedWidget->currentWidget());
    int num = 1;
    QString name;
    QString path;
        QModelIndex fileIndex;
    path = fileSystemModel->filePath(currentView->rootIndex()) ;
    name = "New_file.txt";
    while (hasItem(name, path)) {
        name = QString(tr("New_file_%1.txt")).arg(num);
        num++;
    }

    QFile temp(path + QDir::separator() + name);
    if(!temp.open(QIODevice::WriteOnly)) return false;
    QFileInfo tmpInfo(temp);
    temp.close();
    qDebug()<<fileSystemModel->index(tmpInfo.absoluteFilePath());
    fileIndex = fileSystemModel->index(tmpInfo.absoluteFilePath());
    currentView->selectionModel()->setCurrentIndex(fileIndex,QItemSelectionModel::ClearAndSelect);
    currentView->edit(fileIndex);
    return true;
}

void MainWindow::rename()
{
    QModelIndex fileIndex;
    QWidget* focus(focusWidget());
    QAbstractItemView* view;
    if (focus == leftPane->treeView || focus == leftPane->listView || focus == rightPane->treeView || focus == rightPane->listView){
        view =  qobject_cast<QAbstractItemView *>(focus);
}
        fileIndex = view->selectionModel()->selectedIndexes().at(0);
        view->selectionModel()->setCurrentIndex(fileIndex, QItemSelectionModel::ClearAndSelect);
        view->edit(fileIndex);
}

void MainWindow::createActionsAndMenus()
{
    deleteAction = new QAction(QIcon::fromTheme("edit-delete", QIcon(":/Images/Delete.ico")), tr("Delete"), this );
    deleteAction->setStatusTip(tr("Delete file"));
    deleteAction->setShortcut(QKeySequence::Delete);
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(del()));

    newFolderAction = new QAction(QIcon::fromTheme("edit-new", QIcon(":/Images/NewFolder.ico")), tr("New Folder"), this );
    newFolderAction->setStatusTip(tr("Create New Folder"));
    newFolderAction->setShortcut(QKeySequence::New);
    connect(newFolderAction, SIGNAL(triggered()), this, SLOT(newFolder()));

    renameAction = new QAction(QIcon::fromTheme("edit-name", QIcon(":/Images/NewFolder.ico")), tr("Rename"), this );
    renameAction->setStatusTip(tr("Rename file"));
    connect(renameAction, SIGNAL(triggered()), this, SLOT(rename()));

    exitAction = new QAction(QIcon::fromTheme("application-exit", QIcon(":/Images/Exit.png")), tr("&Exit"), this );
    //exitAction->setShortcuts(QKeySequence::QuitRole);
    exitAction->setMenuRole(QAction::QuitRole);
    exitAction->setStatusTip(tr("Quit Synopson"));
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    cutAction = new QAction(QIcon::fromTheme("edit-cut", QIcon(":/Images/Cut.png")), tr("Cut"), this );
    cutAction->setStatusTip(tr("Cut file"));
    cutAction->setShortcut(QKeySequence::Cut);
    connect(cutAction, SIGNAL(triggered()), this, SLOT(cut()));

    copyAction = new QAction(QIcon::fromTheme("edit-copy", QIcon(":/Images/Copy.png")), tr("Copy"), this );
    copyAction->setStatusTip(tr("Copy file"));
    copyAction->setShortcut(QKeySequence::Copy);
    connect(copyAction, SIGNAL(triggered()), this, SLOT(copy()));

    makeFile = new QAction(QIcon::fromTheme("edit-copy", QIcon(":/Images/Copy.png")), tr("MakeFile"), this );
    makeFile->setStatusTip(tr("Make File"));
    connect(makeFile, SIGNAL(triggered()), this, SLOT(MakeFile()));//copy

    pasteAction = new QAction(QIcon::fromTheme("edit-paste", QIcon(":/Images/Paste.png")), tr("Paste"), this );
    pasteAction->setStatusTip(tr("Paste file"));
    //pasteAction->setEnabled(false);
    pasteAction->setShortcut(QKeySequence::Paste);
    connect(pasteAction, SIGNAL(triggered()), this, SLOT(paste()));

    detailViewAction = new QAction(QIcon::fromTheme("view-list-details", QIcon(":/Images/DetailView.ico")), tr("Detail View"), this );
    detailViewAction->setStatusTip(tr("Show list in detail"));
    detailViewAction->setCheckable(true);
    connect(detailViewAction, SIGNAL(triggered()), this, SLOT(toggleToDetailView()));

    iconViewAction = new QAction(QIcon::fromTheme("view-list-icons", QIcon(":/Images/IconView.ico")), tr("Icon View"), this );
    iconViewAction->setStatusTip(tr("Show list as icons"));
    iconViewAction->setCheckable(true);
    connect(iconViewAction, SIGNAL(triggered()), this, SLOT(toggleToIconView()));

    hiddenAction = new QAction(QIcon::fromTheme("folder-saved-search"), tr("Show Hidden"), this );
    hiddenAction->setStatusTip(tr("Show hidden items"));
    hiddenAction->setCheckable(true);
    connect(hiddenAction, SIGNAL(triggered()), this, SLOT(toggleHidden()));

    viewActionGroup = new QActionGroup(this);
    viewActionGroup->addAction(detailViewAction);
    viewActionGroup->addAction(iconViewAction);

    aboutAction = new QAction(QIcon::fromTheme("help-about", QIcon(":/Images/About.ico")), tr("&About"), this );
    aboutAction->setStatusTip(tr("About Synopson"));
    aboutAction->setMenuRole (QAction::AboutRole);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAboutBox()));

    propertiesAction = new QAction(QIcon::fromTheme("document-properties", QIcon(":/Images/Properties.ico")), tr("P&roperties"), this );
    propertiesAction->setStatusTip(tr("Properties"));
    propertiesAction->setShortcut(Qt::CTRL + Qt::Key_R);
    connect(propertiesAction, SIGNAL(triggered()), this, SLOT(showProperties()));

    menuBar = new QMenuBar(0);

    fileMenu = menuBar->addMenu(tr("&File"));
    fileMenu->addAction(newFolderAction);
    fileMenu->addAction(deleteAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);
    fileMenu->addAction(makeFile);

    editMenu = menuBar->addMenu(tr("&Edit"));
    editMenu->addAction(cutAction);
    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);
    editMenu->addAction(renameAction);


    viewMenu = menuBar->addMenu(tr("&View"));
    viewMenu->addAction(detailViewAction);
    viewMenu->addAction(iconViewAction);
    viewMenu->addAction(hiddenAction);

    helpMenu = menuBar->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);

    setMenuBar(menuBar);

    contextMenu = new QMenu(this);

    contextMenu->addAction(cutAction);
    contextMenu->addAction(copyAction);
    contextMenu->addAction(pasteAction);
    contextMenu->addSeparator();
    contextMenu->addAction(deleteAction);
    contextMenu->addSeparator();
    contextMenu->addAction(newFolderAction);
    contextMenu->addAction(makeFile);
    contextMenu->addSeparator();
    contextMenu->addAction(renameAction);
    contextMenu->addAction(propertiesAction);
}
void MainWindow::showProperties()
{
    Properties properties(this);
    properties.exec();
}

void MainWindow::showAboutBox()
{
    QMessageBox::about(this, tr("About Synopson"),
                       tr("<h2>SF manager</h2>"
                          // "<h2>&Sigma;&upsilon;&nu;&omicron;&pi;&sigma;&omicron;&nu;</h2>"
                          "<p><em>Version 0.2.9</em>"
                          "<p>File Manager<br>"
                          "2015 - 2016 by Minkin Stepan<br>"));
}

