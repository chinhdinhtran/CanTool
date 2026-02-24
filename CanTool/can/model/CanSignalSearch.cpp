#include "CanSignalSearch.h"
#include <QString>
#include <algorithm>

CanSignalSearch::CanSignalSearch(const CanInformation* canInfo,
                                 QObject* parent)
    : QAbstractItemModel(parent)
    , m_canInfo(canInfo)
{
    buildIndex();
}

void CanSignalSearch::buildIndex()
{
    m_items.clear();

    if (!m_canInfo)
        return;

    const auto& messages = m_canInfo->messages();
    for (auto it = messages.begin(); it != messages.end(); ++it)
    {
        const CanMessageInfo& msg = it.value();
        for (const CanSignalInfo& sig : msg.signalList)
        {
            CanSignalSearchItem item;
            item.messageId   = msg.id;
            item.messageName = msg.name;
            item.signal      = sig;

            m_items.push_back(item);
        }
    }

    std::sort(m_items.begin(), m_items.end(),
              [](const CanSignalSearchItem& a,
                 const CanSignalSearchItem& b)
              {
                  return a.signal.name.compare(
                             b.signal.name,
                             Qt::CaseInsensitive) < 0;
              });
}

QModelIndex CanSignalSearch::index(int row, int column,
                                   const QModelIndex& parent) const
{
    if (parent.isValid())
        return QModelIndex();

    if (column != 0)
        return QModelIndex();

    if (row < 0 || row >= m_items.size())
        return QModelIndex();

    return createIndex(row, column);
}

QModelIndex CanSignalSearch::parent(const QModelIndex&) const
{
    return QModelIndex();
}

int CanSignalSearch::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return m_items.size();
}

int CanSignalSearch::columnCount(const QModelIndex&) const
{
    return 1;
}

QVariant CanSignalSearch::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return {};

    if (index.column() != 0)
        return {};

    const auto& item = m_items.at(index.row());

    switch (role)
    {
    case Qt::DisplayRole:
        return item.signal.name;

    case Qt::UserRole:
        return item.signal.name;
    }

    return {};
}

void CanSignalSearch::reload()
{
    beginResetModel();
    buildIndex();
    endResetModel();
    qDebug() << "Signal count:" << m_items.size();
}

void CanSignalSearch::clear()
{
    beginResetModel();
    m_items.clear();
    endResetModel();
    qDebug() << "Signal count:" << m_items.size();
}
