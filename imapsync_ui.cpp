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
		//Get the args with which we're going to run imapsync
		QStringList args = fetchArgs();
		if(args.isEmpty()){
			QMessageBox::critical(this, "Error", "Missing mandatory config (host1 or host2)");
			return;
		}

		QStringList accounts = fetchAccounts();
		if(accounts.isEmpty()){
			QMessageBox::critical(this, "Error", "Missing mandatory config (accounts)");
			return;
		}

		QListIterator<QString>itr(accounts);
		while(itr.hasNext()){
			//Save the constructed args
			QString cmd = args.join(" "),
					user1 = itr.next(),
					password1 = itr.next(),
					user2 = itr.next(),
					password2 = itr.next();

			final_args.append(
				QString("%1 --user1 %2 --password1 %3 --user2 %4 --password2 %5").arg(cmd, user1, password1, user2, password2)
			);
		}

		nargs = 0;

		//Start processing the final_args list
		processFinalArgs();
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

	if(ui->include->toPlainText().length() > 0){
		QStringList include = ui->include->toPlainText().split("\n");
		foreach (QString folder, include){
			args.append( QString("--include ").append(folder) );
		}
	}

	if(ui->exclude->toPlainText().length() > 0){
		QStringList exclude = ui->exclude->toPlainText().split("\n");
		foreach (QString folder, exclude){
			args.append( QString("--exclude ").append(folder) );
		}
	}

	return args;
}

QStringList ImapSync_UI::fetchAccounts(){
	QStringList lines1, lines2;
	QStringList users, users1, users2;

	if(ui->users1->toPlainText().length() > 0){
		lines1 = ui->users1->toPlainText().split("\n");
		foreach (QString userpass, lines1){
			users1.append(userpass.split(";"));
		}
	}

	if(ui->users2->toPlainText().length() > 0){
		lines2 = ui->users2->toPlainText().split("\n");
		foreach (QString userpass, lines2){
			users2.append(userpass.split(";"));
		}
	}

	if(users1.length() != users2.length()){
		return QStringList();
	}else{
		QListIterator<QString>itr1(users1);
		QListIterator<QString>itr2(users2);
		while(itr1.hasNext() && itr2.hasNext()){
			users.append(itr1.next()); //user1
			users.append(itr1.next()); //password1
			users.append(itr2.next()); //user2
			users.append(itr2.next()); //password2
		}
		return users;
	}
}

void ImapSync_UI::onStdoutAvailable(){
	QByteArray arr = process->readAllStandardOutput();
	ui->output->appendPlainText(QString(arr));
}

void ImapSync_UI::processFinalArgs(){
	//Run imapsync
	QString cmd = QString("%1 %2").arg("./imapsync.pl", final_args.takeFirst());
	qDebug() << cmd;
	process->start(cmd, QIODevice::ReadWrite | QIODevice::Text);
	if(!process->waitForStarted()){
		QMessageBox::critical(this, "Error", "Executing program failed (check console output)");
	}else{
		onStarted();
	}
}

void ImapSync_UI::onStarted(){
	qDebug() << "Running!";
	ui->sync->setText("Cancel");
}

void ImapSync_UI::onFinished(int, QProcess::ExitStatus){
	qDebug() << "Finishing!";
	process->close();
	ui->sync->setText("Sync!");

	if(final_args.length() > 0){
		ui->progress->setValue( (100 * (nargs - final_args.length())) / nargs );
		processFinalArgs();
	}else{
		nargs = 0;
		ui->progress->setValue(0);
	}
}
