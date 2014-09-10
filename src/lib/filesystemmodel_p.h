#ifndef FILESYSTEMMODEL_P_H
#define FILESYSTEMMODEL_P_H

#include "filesystemmodel.h"

class FileSystemModelPrivate
{
    Q_DECLARE_PUBLIC(FileSystemModel)
public:
    struct TreeItem;

    explicit FileSystemModelPrivate(FileSystemModel *qq);

    QModelIndex index(TreeItem *item) const;
    TreeItem *item(const QModelIndex &index) const;

    TreeItem *root;
    QUrl rootUrl;
    QHash<QUrl, TreeItem *> itemHash;

protected:
    FileSystemModel *q_ptr;
};

struct FileSystemModelPrivate::TreeItem
{
    explicit TreeItem(TreeItem *parent = 0);
    ~TreeItem();

    inline TreeItem *child(int row) const { return m_children.at(row); }
    inline int childCount() const { return m_children.count(); }
    inline QList<TreeItem *> &children() { return m_children; }
//    inline const QList<TreeItem *> &children() const { return m_children; }
    inline TreeItem *parent() const { return m_parent; }
    inline int row() const { return m_parent ? m_parent->m_children.indexOf((TreeItem *)this) : 0; }

    enum State { NoState = 0, PopulatingState, PopulatedState };

private:
    TreeItem *m_parent;
    QList<TreeItem *> m_children;

public:
    FileInfo info;
    State state;
};

#endif // FILESYSTEMMODEL_P_H
