#ifndef IMAPSYNC_UI_H
#define IMAPSYNC_UI_H

#include <QMainWindow>
#include <QProcess>
#include <QDebug>
#include <QMessageBox>

namespace Ui {
class ImapSync_UI;
}

class ImapSync_UI : public QMainWindow
{
	Q_OBJECT

public:
	explicit ImapSync_UI(QWidget *parent = 0);
	~ImapSync_UI();

public slots:
	void sync();
	void onStdoutAvailable();
	void onStarted();
	void onFinished(int, QProcess::ExitStatus status);

private:
	Ui::ImapSync_UI *ui;
	QProcess *process;
	QStringList fetchArgs();
	QStringList fetchAccounts();
	QStringList final_args;
	void processFinalArgs();
};

#endif // IMAPSYNC_UI_H
