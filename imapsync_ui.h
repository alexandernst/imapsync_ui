#ifndef IMAPSYNC_UI_H
#define IMAPSYNC_UI_H

#include <QMainWindow>
#include <QProcess>
#include <QDebug>

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
	void onFinished(int, QProcess::ExitStatus);

private:
	Ui::ImapSync_UI *ui;
	QProcess *process;
};

#endif // IMAPSYNC_UI_H
