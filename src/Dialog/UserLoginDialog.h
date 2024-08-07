#ifndef DIALOG_USER_LOGIN_H
#define DIALOG_USER_LOGIN_H

#include <QDialog>
#include "ApiManager/CommandState.h"
#include "Application/WgpWaitingSpinner.h"

namespace Ui {
	class UserLoginDialog;
}

class UserLoginDialog : public QDialog
{
    Q_OBJECT

	public:
		explicit UserLoginDialog( QWidget *parent = 0 );
		~UserLoginDialog();

		QString database();

	private:
	    Ui::UserLoginDialog *ui;
	    WgpWaitingSpinner *waitingSpinner;

	public slots:
		void save();
		void handleAuthResult( CommandState *state );
};

#endif // DIALOG_USER_LOGIN_H
