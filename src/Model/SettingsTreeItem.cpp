#include "SettingsTreeItem.h"

SettingsTreeItem::SettingsTreeItem( const QVector<QVariant> &data, SettingsTreeItem *parent )
    : m_itemData(data), m_parentItem(parent)
{}

SettingsTreeItem::~SettingsTreeItem()
{
    qDeleteAll( m_childItems );
}

void SettingsTreeItem::appendChild( SettingsTreeItem *item )
{
    m_childItems.append( item );
}

SettingsTreeItem *SettingsTreeItem::child( int row )
{
    if ( row < 0 || row >= m_childItems.size() )
        return nullptr;
    return m_childItems.at( row );
}

int SettingsTreeItem::childCount() const
{
    return m_childItems.count();
}

int SettingsTreeItem::row() const
{
    if ( m_parentItem )
        return m_parentItem->m_childItems.indexOf( const_cast<SettingsTreeItem*>( this ) );

    return 0;
}

int SettingsTreeItem::columnCount() const
{
    return m_itemData.count();
}

QVariant SettingsTreeItem::data( int column ) const
{
    if ( column < 0 || column >= m_itemData.size() )
        return QVariant();
    return m_itemData.at( column );
}

SettingsTreeItem *SettingsTreeItem::parentItem()
{
    return m_parentItem;
}
