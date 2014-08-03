#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include "fileinfo.h"
#include <QtCore/QAbstractItemModel>

class FileSystemModelPrivate;
class FileSystemModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FileSystemModel)
    Q_DISABLE_COPY(FileSystemModel)

    Q_PROPERTY(QUrl rootUrl READ rootUrl WRITE setRootUrl NOTIFY rootUrlChanged)

public:
    enum Columns {
        ColumnName = 0,
        ColumnSize,
        ColumnLastModified,
        ColumnMimeTypeName,
        ColumnCount };

    explicit FileSystemModel(QObject *parent = 0);
    ~FileSystemModel();

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;

    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    bool canFetchMore(const QModelIndex &parent) const;
    void fetchMore(const QModelIndex &parent);

    QUrl rootUrl() const;

public slots:
    void setRootUrl(const QUrl &url);
    void refresh();

signals:
    void rootUrlChanged(const QUrl &url);

protected:
    FileSystemModelPrivate *d_ptr;

private slots:
    void onFinished();
};

#endif // FILESYSTEMMODEL_H
