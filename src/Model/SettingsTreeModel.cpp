#include "SettingsTreeModel.h"
#include "SettingsTreeItem.h"

#include <QStringList>
#include <QDomDocument>
#include <QApplication>

SettingsTreeModel::SettingsTreeModel( const QString &data, QObject *parent )
    : QAbstractItemModel( parent )
{
	rootItem = new SettingsTreeItem( { tr( "Title" ), tr( "Id" ) } );
	setupModelData( data, rootItem );
}

SettingsTreeModel::~SettingsTreeModel()
{
    delete rootItem;
}

QModelIndex SettingsTreeModel::index( int row, int column, const QModelIndex &parent ) const
{
    if ( ! hasIndex( row, column, parent ) )
        return QModelIndex();

    SettingsTreeItem *parentItem;

    if ( ! parent.isValid() )
        parentItem = rootItem;
    else
        parentItem = static_cast<SettingsTreeItem*>( parent.internalPointer() );

    SettingsTreeItem *childItem = parentItem->child( row );
    if ( childItem )
        return createIndex( row, column, childItem );
    return QModelIndex();
}

QModelIndex SettingsTreeModel::parent( const QModelIndex &index ) const
{
    if ( ! index.isValid() )
        return QModelIndex();

    SettingsTreeItem *childItem = static_cast<SettingsTreeItem*>( index.internalPointer() );
    SettingsTreeItem *parentItem = childItem->parentItem();

    if ( parentItem == rootItem )
        return QModelIndex();

    return createIndex( parentItem->row(), 0, parentItem );
}

int SettingsTreeModel::rowCount( const QModelIndex &parent ) const
{
    SettingsTreeItem *parentItem;
    if ( parent.column() > 0 )
        return 0;

    if ( ! parent.isValid() )
        parentItem = rootItem;
    else
        parentItem = static_cast<SettingsTreeItem*>( parent.internalPointer() );

    return parentItem->childCount();
}

int SettingsTreeModel::columnCount( const QModelIndex &parent ) const
{
    if ( parent.isValid() )
        return static_cast<SettingsTreeItem*>( parent.internalPointer() )->columnCount();
    return rootItem->columnCount();
}

QVariant SettingsTreeModel::data( const QModelIndex &index, int role ) const
{
    if ( ! index.isValid() )
        return QVariant();

    if ( role != Qt::DisplayRole )
        return QVariant();

    SettingsTreeItem *item = static_cast<SettingsTreeItem*>( index.internalPointer() );

    return index.column() == 0 ?
    		QVariant( qApp->translate( "SettingsMenu", item->data( index.column() ).toString().toStdString().c_str() ) ) :
    		item->data( index.column() );
}

Qt::ItemFlags SettingsTreeModel::flags( const QModelIndex &index ) const
{
    if ( ! index.isValid() )
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags( index );
}

QVariant SettingsTreeModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal && role == Qt::DisplayRole )
        return rootItem->data( section );

    return QVariant();
}

void SettingsTreeModel::setupModelData( const QString data, SettingsTreeItem *parent )
{
	QDomDocument* xml	= new QDomDocument();
	xml->setContent( data );

	// First Child
	QDomElement item	= xml->elementsByTagName( "MenuItems" ).at( 0 ).firstChild().toElement();
	iterateChildItems( parent, item );
}

void SettingsTreeModel::iterateChildItems( SettingsTreeItem* parent, QDomElement item )	// item passed is first child of the parent
{
	while( ! item.isNull() )
	{
		QString id		= item.attribute( "id" );
		// Title translations are in `DynamicTranslations.h`
		QString title	= item.elementsByTagName( "Title" ).at( 0 ).toElement().text();

		QVector<QVariant> columnData;
		columnData << title << id;

		parent->appendChild( new SettingsTreeItem( columnData, parent ) );

		QDomNode	firstChild	= item.elementsByTagName( "MenuItems" ).at( 0 ).firstChild();
		if ( ! firstChild.isNull() ) {	// Has Childs
			iterateChildItems( parent->child( parent->childCount() -1 ), firstChild.toElement() );
		}

		item	= item.nextSibling().toElement();
	}
}
