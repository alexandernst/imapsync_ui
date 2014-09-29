#include "imapsync_ui.h"
#include "ui_imapsync_ui.h"

ImapSync_UI::ImapSync_UI(QWidget *parent) : QMainWindow(parent), ui(new Ui::ImapSync_UI){
	ui->setupUi(this);

	process = new QProcess(this);

	connect(ui->sync, SIGNAL(clicked()), this, SLOT(sync()));

	connect(process, SIGNAL(readyReadStandardOutput()), SLOT(onStdoutAvailable()));
	connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(onFinished(int, QProcess::ExitStatus)));
}

ImapSync_UI::~ImapSync_UI(){
	delete ui;
}


void ImapSync_UI::sync(){
	if(process->state() == QProcess::Running){
		process->close();
		ui->sync->setText("Sync!");
	}else{
		QString imapsync = "ping"; //"./imapsync.pl";
		QStringList args;
		args << "google.es";
		process->start(imapsync, args, QIODevice::ReadWrite | QIODevice::Text);
		if(!process->waitForStarted()){
			qDebug() << "executing program failed with exit code" << process->exitCode();
		}else{
			qDebug() << "Running!";
			ui->sync->setText("Cancel");
		}
	}
}

void ImapSync_UI::onStdoutAvailable(){
	QByteArray arr = process->readAllStandardOutput();
	//qDebug() << arr;
	ui->output->appendPlainText(QString(arr));
}

void ImapSync_UI::onFinished(int, QProcess::ExitStatus){

}
