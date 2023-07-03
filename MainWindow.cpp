#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//СОздание базы данных
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./DBChat");
     query = new QSqlQuery(db);
    if(db.open())
    {
        m_chat = ui->textBrowser->toPlainText();
        m_chat.append("DB is open\n");
        ui->textBrowser->setText(m_chat);
    }
    else
    {
        m_chat = ui->textBrowser->toPlainText();
        m_chat.append("DB is NO open\n");
        ui->textBrowser->setText(m_chat);
    }
    query = new QSqlQuery(db);
    query->exec("CREATE TABLE USERS(LOGIN TEXT, PASSWORD TEXT);");
     query->exec("CREATE TABLE MESSAGES(SENDER TEXT, RECIEVER TEXT, MESSAGE TEXT);");
    model = new QSqlTableModel(this,db);
    modelmes = new QSqlTableModel(this,db);
    model->setTable("USERS");
    modelmes->setTable("MESSAGES");
    model->select();
    modelmes->select();
    ui->tableView->setModel(model);
    ui->tableView_2->setModel(modelmes);

    //Вывод в лог всех зарегестрированных пользователей
    query->exec("SELECT LOGIN FROM USERS");
    m_chat.append("Users: \n");
    ui->textBrowser->setText(m_chat);
    while(query->next())
 {
        QString name = query->value(0).toString() + "\n";
        m_chat = ui->textBrowser->toPlainText();
        m_chat.append(name);
        ui->textBrowser->setText(m_chat);
 }

 //добавление хэшей в контейнер с пользователями

 query->exec("SELECT * FROM USERS");
 QString cheklog;
 QString chekpass;
 while(query->next())
 {
        cheklog = query->value(0).toString();
        chekpass = query->value(1).toString();
        Hash user_pass = sha1(chekpass.toStdString());
          user_passwords.insert(cheklog, user_pass);

 }



    //Создание сервера
    s = new Server;

    connect(s,&Server::seyconnect, this,&MainWindow::getstart);
    connect(this,&MainWindow::sendtoClient, s,&Server::slotsendtoClient);
}

MainWindow::~MainWindow()
{
    delete s;
    delete ui;
}

void MainWindow::printMes(QString mes)
{
    ui->textBrowser->setText(mes);
}
    //Получение запросов от клиентов
void MainWindow::getstart(QString mes)
{
    QString message;
    QString sender;
    QString reciever;
    QString user_login;
    QString user_pass;
    Hash user_pass_sha1;
    int counter = 1;

    //обработка запроса на подключение нового зарегестрированного пользователя
    if(mes[0]=='1')
    {
        for(int i =1; i<mes.length();i++)
        {
            if(mes[i]=='<')
            {
                while(mes[i+counter]!='>')
                {
                    user_login += mes[i+counter++];

                }
            }
            counter =1;
            if(mes[i]=='[')
            {
                while(mes[i+counter]!=']')
                {
                    user_pass += mes[i+counter++];
                }
                counter =1;
            }
        }
         user_pass_sha1 = sha1(user_pass.toStdString());
        user_passwords.insert(user_login, user_pass_sha1);
        // user_pass_sha1.c;

        query->exec("INSERT INTO USERS VALUES('" + user_login + "','" + user_pass + "');");
        ui->tableView->setModel(model);
        model->setTable("USERS");
        model->select();
        ui->tableView->setModel(model);
        m_chat = ui->textBrowser->toPlainText();
        m_chat.append(mes);
        ui->textBrowser->setText(m_chat);
    }

    //обработка запроса на получение списка пользователей
    if(mes[0]=='3')
    {
    QString user_login;
    int counter =1;
    for(int i =1; i<mes.length();i++)
    {
        if(mes[i]=='<')
            {
                while(mes[i+counter]!='>')
                {
                    user_login += mes[i+counter++];

                }
            }
        counter =1;
    }
    query->exec("SELECT LOGIN FROM USERS");
    QString name;
    while(query->next())
    {
         name +="F<" +user_login + ">[" + query->value(0).toString() + "] ";
    }
    m_chat = ui->textBrowser->toPlainText();
    m_chat.append(" toclient " + name);
    ui->textBrowser->setText(m_chat);
    emit sendtoClient(name);
    }

    //запрос на Вход по логину
    if(mes[0]=='2')
    {
    QString res = "G";
    for(int i =1; i<mes.length();i++)
    {
        if(mes[i]=='<')
         {
                while(mes[i+counter]!='>')
                {
                    user_login += mes[i+counter++];
                }
                res+=user_login;
        }
        counter =1;
        if(mes[i]=='[')
        {
                while(mes[i+counter]!=']')
                {
                    user_pass += mes[i+counter++];
                }
                counter =1;
        }
    }
    query->exec("SELECT * FROM USERS");
    QString cheklog;
    QString chekpass;
    while(query->next())
        {
        cheklog = query->value(0).toString();
        chekpass = query->value(1).toString();
        if(cheklog == user_login &&chekpass==user_pass )
        {
              res = "logok";
        }
    }
    emit sendtoClient(res);
    }

    //Обработка запроса на отправку сообщения для всех
    if(mes[0]=='5')
    {
        for(int i =1; i<mes.length();i++)
    {
        if(mes[i]=='<')
        {
                while(mes[i+counter]!='>')
                {
                    sender += mes[i+counter++];
                }
        }
        counter =1;

        if(mes[i]==':')
        {
                while(mes[i+counter]!='\n')
                {
                    message += mes[i+counter++];
                }
        }
        counter =1;
        }
         emit sendtoClient(mes);
        m_chat = ui->textBrowser->toPlainText();
        m_chat.append(" toclient " + mes);
        ui->textBrowser->setText(m_chat);

        query->exec("INSERT INTO MESSAGES VALUES('" + sender + "','all','" +message + "');");
        sender.clear();
        message.clear();
        ui->tableView_2->setModel(modelmes);
        modelmes->setTable("MESSAGES");
        modelmes->select();
        ui->tableView_2->setModel(modelmes);
    }

    //обработка запроса на отправку приватных сообщений
    if(mes[0]=='6')
    {
    for(int i =1; i<mes.length();i++)
        {
        if(mes[i]=='<')
        {
                while(mes[i+counter]!='>')
                {
                    sender += mes[i+counter++];
                }
        }
        counter =1;

        if(mes[i]=='[')
        {
                while(mes[i+counter]!=']')
                {
                    reciever += mes[i+counter++];
                }
        }
         counter =1;

        if(mes[i]==':')
         {
                while(mes[i+counter]!='\n')
                {
                    message += mes[i+counter++];
                }
        }
        counter =1;
    }
        emit sendtoClient(mes);
        m_chat = ui->textBrowser->toPlainText();
        m_chat.append(" toclient " + mes);
        ui->textBrowser->setText(m_chat);
        query->exec("INSERT INTO MESSAGES VALUES('" + sender + "','" + reciever +  "','" +message + "');");
        sender.clear();
        message.clear();
        ui->tableView_2->setModel(modelmes);
        modelmes->setTable("MESSAGES");
        modelmes->select();
        ui->tableView_2->setModel(modelmes);
    }

}

void MainWindow::on_tableView_2_clicked(const QModelIndex &index)
{
    row = index.row();
}

void MainWindow::on_pushButton_2_clicked()
{
    modelmes->removeRow(row);
    modelmes->setTable("MESSAGES");
    modelmes->select();
    ui->tableView_2->setModel(modelmes);
}

void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
     row = index.row();
}

void MainWindow::on_pushButton_clicked()
{
     model->removeRow(row);
     model->setTable("USERS");
     model->select();
     ui->tableView->setModel(model);
}

