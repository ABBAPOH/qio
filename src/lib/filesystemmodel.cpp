#include "filesystemmodel.h"
#include "filesystemmodel_p.h"

#include "dir.h"

#include <QtCore/QFutureWatcher>

typedef FileSystemModelPrivate::TreeItem TreeItem;

FileSystemModelPrivate::FileSystemModelPrivate(FileSystemModel *qq) :
    q_ptr(qq)
{
    root = new TreeItem;
}

QModelIndex FileSystemModelPrivate::index(TreeItem *item) const
{
    Q_Q(const FileSystemModel);

    if (item == root)
        return QModelIndex();
    return q->createIndex(item->row(), 0, item);
}

FileSystemModelPrivate::TreeItem *FileSystemModelPrivate::item(const QModelIndex &index) const
{
    if (!index.isValid())
        return root;
    return static_cast<TreeItem*>(index.internalPointer());
}

TreeItem::TreeItem(TreeItem *parent) :
    m_parent(parent),
    state(NoState)
{
}

TreeItem::~TreeItem()
{
    foreach (TreeItem *item, m_children)
        delete item;

    if (m_parent)
        m_parent->m_children.removeAll(this);
}

/*!
    \class FileSystemModel
*/

FileSystemModel::FileSystemModel(QObject *parent) :
    QAbstractItemModel(parent),
    d_ptr(new FileSystemModelPrivate(this))
{
}

FileSystemModel::~FileSystemModel()
{
    delete d_ptr;
}

QModelIndex FileSystemModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_D(const FileSystemModel);

    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *childItem = d->item(parent)->child(row);
    if (!childItem)
        return QModelIndex();

    return createIndex(row, column, childItem);
}

QModelIndex FileSystemModel::parent(const QModelIndex &child) const
{
    Q_D(const FileSystemModel);

    TreeItem *parentItem = d->item(child)->parent();

    if (!parentItem || parentItem == d->root)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int FileSystemModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ColumnCount;
}

int FileSystemModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const FileSystemModel);
    return d->item(parent)->childCount();
}

bool FileSystemModel::hasChildren(const QModelIndex &parent) const
{
    Q_D(const FileSystemModel);
    TreeItem *item = d->item(parent);
    return item == d->root || item->info.isDir();
}

QVariant FileSystemModel::data(const QModelIndex &index, int role) const
{
    Q_D(const FileSystemModel);
    if (!index.isValid())
        return QVariant();

    const TreeItem *item = d->item(index);
    const int column = index.column();
    if (role == Qt::DisplayRole) {
        switch (column) {
        case ColumnName: return item->info.fileName();
        case ColumnSize: return item->info.size();
        case ColumnLastModified: return item->info.lastModified();
        case ColumnMimeTypeName: return item->info.mimeTypeName();
        default:
            break;
        }
    }

    return QVariant();
}

bool FileSystemModel::canFetchMore(const QModelIndex &parent) const
{
    Q_D(const FileSystemModel);
    TreeItem *item = d->item(parent);
//    qDebug() << "canFetchMore" << item->info.url() << item->populated;
//    if (item == d->root)
//        return !d->root->populated;
    return item->state == FileSystemModelPrivate::TreeItem::NoState;
}

void FileSystemModel::fetchMore(const QModelIndex &parent)
{
    refresh(parent);
}

QUrl FileSystemModel::rootUrl() const
{
    Q_D(const FileSystemModel);
    return d->rootUrl;
}

QString FileSystemModel::getNonExistentFileName(const QString &name) const
{
    QSet<QString> fileNames;
    const int rows = rowCount();
    for (int i = 0; i < rows; ++i)
        fileNames.insert(index(i, ColumnName).data().toString());

    QString result = name;
    int count = 0;
    while (fileNames.contains(result))
        result = tr("%1 %2").arg(name).arg(++count);
    return result;
}

void FileSystemModel::setRootUrl(const QUrl &url)
{
    Q_D(FileSystemModel);
    if (d->rootUrl == url)
        return;

    d->rootUrl = url;
    emit rootUrlChanged(url);
    refresh();
}

void FileSystemModel::refresh(const QModelIndex &index)
{
    Q_D(FileSystemModel);
    TreeItem *item = d->item(index);

    const QUrl url = item == d->root ? d->rootUrl : item->info.url();
    qDebug() << "fetchMore" << url << item->state;
    QFutureWatcher<FileInfo> *watcher = new QFutureWatcher<FileInfo>(this);
    watcher->setProperty("url", url);
    connect(watcher, &QFutureWatcherBase::finished, this, &FileSystemModel::onFinished);

    Dir dir(url);
    watcher->setFuture(dir.entryList());
    item->state = FileSystemModelPrivate::TreeItem::PopulatingState;
}

void FileSystemModel::refresh()
{
    Q_D(FileSystemModel);

    beginResetModel();
    d->itemHash.clear();
    delete d->root;
    d->root = new TreeItem;
    d->itemHash.insert(d->rootUrl, d->root);
    endResetModel();
//    fetchMore(QModelIndex());
}

void FileSystemModel::onFinished()
{
    Q_D(FileSystemModel);

    QFutureWatcher<FileInfo> *watcher = static_cast< QFutureWatcher<FileInfo> *>(sender());
    QFuture<FileInfo> future = watcher->future();
    if (future.isCanceled())
        return;

    const QUrl url = watcher->property("url").toUrl();
    TreeItem *parentItem = d->itemHash.value(url);
    if (!parentItem)
        return;

    QSet<FileInfo> oldSet;
    const int childCount = parentItem->childCount();
    for (int i = 0; i < childCount; ++i) {
        TreeItem *child = parentItem->child(i);
        oldSet.insert(child->info);
    }

    QSet<FileInfo> newSet;
    const int count = future.resultCount();
    for (int i = 0; i < count; ++i) {
        newSet.insert(future.resultAt(i));
    }

    QSet<FileInfo> toInsert = newSet;
    toInsert.subtract(oldSet);

    QSet<FileInfo> toRemove = oldSet;
    toRemove.subtract(newSet);

    const QModelIndex parentIndex = d->index(parentItem);
    foreach (const FileInfo &info, toRemove) {
        TreeItem *item = d->itemHash.take(info.url());
        const int row = item->row();
        beginRemoveRows(parentIndex, row, row);
        delete item;
        endRemoveRows();
    }

    const auto lessThan = [](TreeItem *lhs, TreeItem *rhs)
    {
        return lhs->info.fileName() < rhs->info.fileName();
    };

    foreach (const FileInfo &info, toInsert) {
        TreeItem *child = new TreeItem(parentItem);
        child->info = info;

        auto it = std::lower_bound(parentItem->children().begin(),
                                   parentItem->children().end(),
                                   child,
                                   lessThan);

        const int row = it - parentItem->children().begin();
        beginInsertRows(parentIndex, row, row);
        parentItem->children().insert(it, child);
        d->itemHash.insert(child->info.url(), child);
        endInsertRows();
    }

    parentItem->state = FileSystemModelPrivate::TreeItem::PopulatedState;
    Q_ASSERT(rowCount(d->index(parentItem)) == count);
}
