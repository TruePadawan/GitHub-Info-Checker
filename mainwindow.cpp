#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QInputDialog>
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    netManager = new QNetworkAccessManager(this);
    netReply = nullptr;
    repoReply = nullptr;
    img = new QPixmap();
    setFixedSize(606,469);
}

void MainWindow::clearValues()
{
    ui->picLabel->clear();
    ui->usernameLabel->clear();
    ui->nameLabel->clear();
    ui->repoList->clear();
    ui->repoBox->setValue(0);
    ui->bioEdit->clear();
    ui->followerBox->setValue(0);
    ui->followingBox->setValue(0);
    ui->typeLabel->clear();
    dataBuffer.clear();
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_usernameButton_clicked()
{
    auto username = QInputDialog::getText(this,"Github Username","Enter your GitHub Username");
    if(!username.isEmpty()){
        clearValues();
        QNetworkRequest req{QUrl(QString("https://api.github.com/users/%1").arg(username))};
        QNetworkRequest repoReq{QUrl(QString("https://api.github.com/users/%1/repos").arg(username))};
        netReply = netManager->get(req);
        connect(netReply,&QNetworkReply::readyRead,this,&MainWindow::readData);
        connect(netReply,&QNetworkReply::finished,this,&MainWindow::finishReading);
        repoReply = netManager->get(repoReq);
        connect(repoReply,&QNetworkReply::readyRead,this,&MainWindow::readDataForRepo);
        connect(repoReply,&QNetworkReply::finished,this,&MainWindow::finishedGettingRepos);
    }
}

void MainWindow::readData()
{
    dataBuffer.append(netReply->readAll());
}

void MainWindow::readDataForRepo()
{
    dataBuffer.append(repoReply->readAll());
}

void MainWindow::finishedGettingRepos()
{
    if(repoReply->error() != QNetworkReply::NoError){
        qDebug() << "Error Getting List of Repo: " << netReply->errorString();
        QMessageBox::warning(this,"Error",QString("Request[Error] : %1").arg(netReply->errorString()));
    }else{
        QJsonArray repoInfo = QJsonDocument::fromJson(dataBuffer).array();
        ui->repoBox->setValue(repoInfo.size());
        for(int i{0}; i < ui->repoBox->value(); ++i){
            auto repo = repoInfo.at(i).toObject();
//            if(repo.value("name") == QJsonValue::Undefined){
//                qDebug() << "No Repo associated with Account!";
//                QMessageBox::information(this,"No Repo","No Repository associated with current account!");
//                return;
//            }
            QString repoName = repo.value("name").toString();
            ui->repoList->addItem(repoName);
        }
    }
}

void MainWindow::finishReading()
{
    if(netReply->error() != QNetworkReply::NoError){
        qDebug() << "Error : " << netReply->errorString();
        QMessageBox::warning(this,"Error",QString("Request[Error] : %1").arg(netReply->errorString()));
    }else{

        //CONVERT THE DATA FROM A JSON DOC TO A JSON OBJECT
        QJsonObject userJsonInfo = QJsonDocument::fromJson(dataBuffer).object();

        //SET USERNAME
        QString login = userJsonInfo.value("login").toString();
        ui->usernameLabel->setText(login);

        // SET DISPLAY NAME
        QString name = userJsonInfo.value("name").toString();
        ui->nameLabel->setText(name);

        //SET BIO
        auto bio = userJsonInfo.value("bio").toString();
        ui->bioEdit->setText(bio);

        //SET FOLLOWER AND FOLLOWING COUNT
        auto follower = userJsonInfo.value("followers").toInt();
        auto following = userJsonInfo.value("following").toInt();
        ui->followerBox->setValue(follower);
        ui->followingBox->setValue(following);

        //SET ACCOUNT TYPE
        QString type = userJsonInfo.value("type").toString();
        ui->typeLabel->setText(type);

        //SET PICTURE
        auto picLink = userJsonInfo.value("avatar_url").toString();
        QNetworkRequest link{QUrl(picLink)};
        netReply = netManager->get(link);
        connect(netReply,&QNetworkReply::finished,this,&MainWindow::setUserImage);
        dataBuffer.clear();
    }
}

void MainWindow::setUserImage()
{
    qDebug() << "Pixmap stuff starts";
    img->loadFromData(netReply->readAll());
    QPixmap temp = img->scaled(ui->picLabel->size());
    ui->picLabel->setPixmap(temp);
    qDebug() << "Pixmap end";
}
