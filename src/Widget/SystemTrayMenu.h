#ifndef SYSTEMTRAYMENU_H
#define SYSTEMTRAYMENU_H

#include <QWidget>
#include <QIcon>
#include <QToolBar>
#include "ApiManager/HttpRequestWorker.h"

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
	private:
		Ui::SystemTrayMenu *ui;
		QToolBar *toolBar;

		void loginToWebGuitarPro();
		void createToolBar();
		QIcon createProfileIcon();
		void syncFileSystem();

	public slots:
		void handleMyCategoriesResult( HttpRequestWorker *worker );
		void handleMyTablaturesResult( HttpRequestWorker *worker );
		void logout();
};

#endif // SYSTEMTRAYMENU_H
