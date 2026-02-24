#ifndef CANSIGNALSEARCH_H
#define CANSIGNALSEARCH_H

#include <QAbstractItemModel>
#include <QVector>
#include "can/model/CanInformation.h"

struct CanSignalSearchItem
{
    uint32_t messageId;
    QString  messageName;
    CanSignalInfo signal;
};

class CanSignalSearch : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit CanSignalSearch(const CanInformation* canInfo,
                             QObject* parent = nullptr);

    QModelIndex index(int row, int column,
                      const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role) const override;
    void reload();
    void clear();

private:
    void buildIndex();

private:
    const CanInformation* m_canInfo;
    QVector<CanSignalSearchItem> m_items;
};

#endif // CANSIGNALSEARCH_H



