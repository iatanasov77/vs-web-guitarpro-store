/*
 * FROM THIS EXAMPLE BUT CHANGED SOURCE FILE FORMAT FROM TXT TO XML:
 * ===========================================================================
 * https://doc.qt.io/qt-5/qtwidgets-itemviews-simpleSettingsTreeModel-example.html
 */
#ifndef SETTINGSMENU_SettingsTreeModel_H
#define SETTINGSMENU_SettingsTreeModel_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QDomElement>

class SettingsTreeItem;

#include "GlobalTypes.h"

class SettingsTreeModel : public QAbstractItemModel
{
    Q_OBJECT

	public:
		explicit SettingsTreeModel( const QString &data, QObject *parent = nullptr );
		~SettingsTreeModel();

		QVariant data( const QModelIndex &index, int role ) const override;
		Qt::ItemFlags flags( const QModelIndex &index ) const override;
		QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;
		QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const override;
		QModelIndex parent( const QModelIndex &index ) const override;
		int rowCount( const QModelIndex &parent = QModelIndex() ) const override;
		int columnCount( const QModelIndex &parent = QModelIndex() ) const override;

		QStringList ids();

	private:
		void setupModelData( const QString data, SettingsTreeItem *parent );
		void iterateChildItems( SettingsTreeItem* parent, QDomElement item );

		SettingsTreeItem *rootItem;
		QStringList _ids;
};

#endif	// SETTINGSMENU_SettingsTreeModel_H
