#ifndef SYSTEMTRAYMENU_H
#define SYSTEMTRAYMENU_H

#include <QWidget>

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

		void init();
};

#endif // SYSTEMTRAYMENU_H
