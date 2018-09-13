#include "MainWindow.h"
#include "Pane.h"


Pane::Pane(QWidget *parent) : QFrame (parent)
{
    mainWindow = static_cast<MainWindow*>(parent->parent());
    pathLineEdit = new QLineEdit;
    connect(pathLineEdit, SIGNAL(editingFinished()), this, SLOT(pathLineEditChanged()));
    combox = new QComboBox(this);
    combox->setModel(mainWindow->fileSystemModel);
    connect(combox, SIGNAL(activated(QString)),this, SLOT(OpenDisc(QString)));

    treeView = new QTreeView(this);
    treeView->setRootIsDecorated(false);
    treeView->setModel(mainWindow->fileSystemModel);
    treeView->setFrameStyle(QFrame::NoFrame | QFrame::Plain);
    treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    treeView->setItemsExpandable(false);
    treeView->setDragDropMode(QAbstractItemView::DragDrop);
    treeView->setDefaultDropAction(Qt::MoveAction);
    treeView->setDropIndicatorShown(true);
    //treeView->setMouseTracking(true);
    //treeView->setSortingEnabled(true);
    treeView->setEditTriggers(QAbstractItemView::EditKeyPressed | QAbstractItemView::SelectedClicked);
    treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(treeView, SIGNAL(activated(QModelIndex)), this, SLOT(doubleClickedOnEntry(QModelIndex)));
    connect(treeView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));


    listView = new QListView(this);
    listView->setWrapping(true);
    listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    listView->setDragDropMode(QAbstractItemView::DragDrop);
    listView->setDefaultDropAction(Qt::MoveAction);
    listView->setDropIndicatorShown(true);
    listView->setEditTriggers(QAbstractItemView::EditKeyPressed | QAbstractItemView::SelectedClicked);
    connect(listView, SIGNAL(activated(QModelIndex)), this, SLOT(doubleClickedOnEntry(QModelIndex)));
    listView->setModel(mainWindow->fileSystemModel);
    listView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(listView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));

    stackedWidget = new QStackedWidget();
    stackedWidget->addWidget(treeView);
    stackedWidget-> addWidget(listView);

    hBoxLayout = new QHBoxLayout();
    hBoxLayout->addWidget(combox);
    hBoxLayout->addWidget(pathLineEdit);

    vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addLayout(hBoxLayout);
    vBoxLayout->addWidget(stackedWidget);


    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    vBoxLayout->setSpacing(0);
    setLayout(vBoxLayout);
    setFrameStyle(QFrame::Box | QFrame::Plain);
    moveTo("C:/");

}

void Pane::doubleClickedOnEntry(QModelIndex index)
{
    Qt::KeyboardModifiers keybMod = QApplication::keyboardModifiers();
    if(keybMod == Qt::ControlModifier || keybMod == Qt::ShiftModifier)
        return;

    QFileInfo fileInfo(mainWindow->fileSystemModel->filePath(index));

    if(fileInfo.isDir())
        moveTo(fileInfo.absoluteFilePath());
    else if (fileInfo.isExecutable()) {
        QProcess *process = new QProcess(this);
        process->startDetached(fileInfo.absoluteFilePath());
    }
    else {

        QFileInfo info(mainWindow->fileSystemModel->filePath(index));
        QString filepath = info.absoluteFilePath();
        QUrl fileurl = QUrl::fromUserInput(filepath);
        QDesktopServices::openUrl(fileurl);

    }
}

void Pane::moveTo(const QString& path)
{
    currentPath=path;
    pathLineEdit->setText(path);
    QModelIndex index(mainWindow->fileSystemModel->index(path));
    listView->setRootIndex(index);
    treeView->setRootIndex(index);

}

void Pane::pathLineEditChanged()
{
    QFileInfo file(pathLineEdit->text());
    if(file.isDir())
        moveTo(pathLineEdit->text());
    else
        pathLineEdit->setText(mainWindow->fileSystemModel->filePath(treeView->rootIndex()));
}

void Pane::showContextMenu(const QPoint& position)
{
    mainWindow->contextMenu->exec(listView->mapToGlobal(position));
}

void Pane::OpenDisc(QString Disc)
{
    int i=Disc.indexOf(':');
    QString cDisc="";
    cDisc = Disc.at(i-1);
    cDisc += Disc.at(i);
    qDebug() << cDisc;
    moveTo(cDisc+"/");
}

void Pane::setViewTo(const ViewMode viewMode)
{
    stackedWidget->setCurrentIndex(viewMode);
}

bool Pane::isActive() const
{
    return active;
}

void Pane::setActive(bool active)
{
    if (this->active == active) {
        return;
    }

    this->active = active;
    treeView->setAlternatingRowColors(active);
    listView->setAlternatingRowColors(active);
}
