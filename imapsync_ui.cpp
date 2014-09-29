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
	delete process;
	delete ui;
}


void ImapSync_UI::sync(){
	if(process->state() == QProcess::Running){
		onFinished(0, QProcess::CrashExit);
	}else{
		QString imapsync = "echo"; //"./imapsync.pl";

		//Get the args with which we're going to run imapsync
		QStringList args = fetchArgs();
		qDebug() << args;

		if(args.isEmpty()){
			QMessageBox::critical(this, "Error", "Missing mandatory config (host1 or host2)");
			return;
		}

		//Run imapsync
		process->start(imapsync, args, QIODevice::ReadWrite | QIODevice::Text);
		if(!process->waitForStarted()){
			qDebug() << "executing program failed with exit code" << process->exitCode();
		}else{
			onStarted();
		}
	}
}

QStringList ImapSync_UI::fetchArgs(){
	QStringList args;

	//Check for server 1 config
	if(ui->host1->text().length() == 0){
		return QStringList();
	}
	args.append( QString("--host1 ").append(ui->host1->text()) );
	if(ui->port1->text().length() > 0){
		args.append( QString("--port1 ").append(ui->port1->text()) );
	}
	if(ui->ssl1->isChecked()){
		args.append("--ssl1");
	}
	if(ui->tls1->isChecked()){
		args.append("--tls1");
	}

	//Check for server 2 config
	if(ui->host2->text().length() == 0){
		return QStringList();
	}
	args.append( QString("--host2 ").append(ui->host2->text()) );
	if(ui->port2->text().length() > 0){
		args.append( QString("--port2 ").append(ui->port2->text()) );
	}
	if(ui->ssl2->isChecked()){
		args.append("--ssl2");
	}
	if(ui->tls2->isChecked()){
		args.append("--tls2");
	}

	//Check extra config
	if(ui->justfolders->isChecked()){
		args.append("--justfolders");
	}

	if(ui->justfoldersizes->isChecked()){
		args.append("--justfoldersizes");
	}

	if(ui->justconnect->isChecked()){
		args.append("--justconnect");
	}

	if(ui->syncacls->isChecked()){
		args.append("--syncacls");
	}

	if(ui->dry->isChecked()){
		args.append("--dry");
	}

	return args;
}

void ImapSync_UI::onStdoutAvailable(){
	QByteArray arr = process->readAllStandardOutput();
	ui->output->appendPlainText(QString(arr));
}

void ImapSync_UI::onStarted(){
	qDebug() << "Running!";
	ui->sync->setText("Cancel");
}

void ImapSync_UI::onFinished(int, QProcess::ExitStatus){
	qDebug() << "Finishing!";
	process->close();
	ui->sync->setText("Sync!");
}
