#ifndef SYSTEMTRAYMENU_H
#define SYSTEMTRAYMENU_H

#include <QWidget>
#include <QIcon>
#include <QToolBar>
#include <QTreeWidgetItem>
#include <QJsonObject>

#include "ApiManager/HttpRequestWorker.h"
#include "Model/WgpFileSystemModel.h"

namespace Ui {
	class SystemTrayMenu;
}

class SystemTrayMenu : public QWidget
{
    Q_OBJECT

	public:
		SystemTrayMenu( QWidget *_parent = nullptr );
		~SystemTrayMenu();

		void displayMyTablatures();
		QTreeWidgetItem *createTreeWidgetItems( QJsonObject jc, QTreeWidgetItem *parentItem = nullptr );
		void setTopLevelItems( QList<QTreeWidgetItem *> items );

	private:
		Ui::SystemTrayMenu *ui;
		QToolBar *toolBar;
		WgpFileSystemModel *dirModel;

		void loginToWebGuitarPro();
		void createToolBar();
		QIcon createProfileIcon();
		void syncFileSystem();

	public slots:
		void logout();
		void handleMyCategoriesResult( HttpRequestWorker *worker );
		void handleMyTablaturesResult( HttpRequestWorker *worker );
};

#endif // SYSTEMTRAYMENU_H
