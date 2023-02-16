#ifndef SYSTEMTRAYMENU_H
#define SYSTEMTRAYMENU_H

#include <QWidget>
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

	private:
		Ui::SystemTrayMenu *ui;

		void displayMyTablatures();

	public slots:
		void handleMyTablaturesResult( HttpRequestWorker *worker );
};

#endif // SYSTEMTRAYMENU_H
