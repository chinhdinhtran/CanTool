#include "CanSignalFilterModel.h"

CanSignalFilterModel::CanSignalFilterModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

bool CanSignalFilterModel::filterAcceptsRow(int sourceRow,
                                            const QModelIndex& sourceParent) const
{
    QModelIndex idx = sourceModel()->index(sourceRow, 0, sourceParent);
    QString signalName = sourceModel()
                             ->data(idx, Qt::UserRole)
                             .toString();

    return signalName.contains(filterRegularExpression());
}
