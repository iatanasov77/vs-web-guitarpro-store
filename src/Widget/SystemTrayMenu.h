#ifndef SYSTEMTRAYMENU_H
#define SYSTEMTRAYMENU_H

#include <QWidget>
#include <QIcon>
#include <QToolBar>
#include <QTreeWidgetItem>
#include <QJsonObject>

#include "ApiManager/HttpRequestWorker.h"
#include "Model/WgpFileSystemModel.h"
#include "Dialog/UserLoginDialog.h"

namespace Ui {
	class SystemTrayMenu;
}

class SystemTrayMenu : public QWidget
{
    Q_OBJECT

	public:
		SystemTrayMenu( QWidget *_parent = nullptr );
		~SystemTrayMenu();

	private:
		Ui::SystemTrayMenu *ui;
		UserLoginDialog *loginDialog;
		QToolBar *toolBar;
		WgpFileSystemModel *dirModel;

		void loginToWebGuitarPro();
		void _createToolBar();
		QIcon _createProfileIcon();
		void _syncFileSystem();
		void _displayMyTablatures();
		void _setTopLevelItems( QList<QTreeWidgetItem *> items );
		QTreeWidgetItem *_createTreeWidgetItems( QJsonObject jc, QTreeWidgetItem *parentItem = nullptr );

	private slots:
		void logout();
		void handleMyCategoriesResult( HttpRequestWorker *worker );
		void handleMyTablaturesResult( HttpRequestWorker *worker );
		void openWebGuitarProFolder();
};

#endif // SYSTEMTRAYMENU_H
