#ifndef SETTINGSMENU_TREEITEM_H
#define SETTINGSMENU_TREEITEM_H

#include <QVector>
#include <QVariant>

class SettingsTreeItem
{
	public:
		explicit SettingsTreeItem( const QVector<QVariant> &data, SettingsTreeItem *parentItem = nullptr );
		~SettingsTreeItem();

		void appendChild( SettingsTreeItem *child );

		SettingsTreeItem *child( int row );
		int childCount() const;
		int columnCount() const;
		QVariant data( int column ) const;
		int row() const;
		SettingsTreeItem *parentItem();

	private:
		QVector<SettingsTreeItem*> m_childItems;
		QVector<QVariant> m_itemData;
		SettingsTreeItem *m_parentItem;
};

#endif	// SETTINGSMENU_TREEITEM_H
