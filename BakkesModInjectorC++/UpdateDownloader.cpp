#include "UpdateDownloader.h"
#include <qfile.h>
#include <qnetworkreply.h>
#include "windowsutils.h"
#include <cstdio>
#include <qmessagebox.h>
UpdateDownloader::UpdateDownloader(std::string downloadUrl, std::string packageUrl)
{
	this->updateUrl = downloadUrl;
	this->packageUrl = packageUrl;
	networkManager = new QNetworkAccessManager(this);
	connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finishedSlot(QNetworkReply*)));
	connect(networkManager, SIGNAL(downloadProgress(qint64, qint64)),
		this, SLOT(updateProgress(qint64, qint64)));
}


UpdateDownloader::~UpdateDownloader()
{
}

void UpdateDownloader::StartDownload()
{
	QUrl url(QString(updateUrl.c_str()));
	networkManager->get(QNetworkRequest(url));
}


void UpdateDownloader::updateProgress(qint64 read, qint64 total)
{
	percentComplete = read / total * 100;
}

void UpdateDownloader::finishedSlot(QNetworkReply *reply)
{
	if (reply->error() == QNetworkReply::NoError) {
		if (WindowsUtils::FileExists(packageUrl))
		{
			if (remove(packageUrl.c_str()) != 0)
			{
				QMessageBox msgBox;
				msgBox.setText("Could not delete update package, probably need admin privileges. I might crash now.");
				msgBox.show();
			}
		}
		QFile other(packageUrl.c_str());
		other.open(QIODevice::WriteOnly);
		other.write(reply->readAll());
		other.flush();
		other.close();
	}
	completed = true;
	reply->deleteLater();
}