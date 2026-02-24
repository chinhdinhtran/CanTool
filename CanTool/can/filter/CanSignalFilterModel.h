#ifndef CANSIGNALFILTERMODEL_H
#define CANSIGNALFILTERMODEL_H
#include <QSortFilterProxyModel>


class CanSignalFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit CanSignalFilterModel(QObject* parent = nullptr);

protected:
    bool filterAcceptsRow(int sourceRow,
                          const QModelIndex& sourceParent) const override;
};

#endif // CANSIGNALFILTERMODEL_H
