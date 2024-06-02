#include "loginwindow.h"
#include "./ui_loginwindow.h"
#include <QGraphicsEffect>
#include <QMessageBox>
#include <QWidget>
#include <fstream>
#include <QtQuick/QQuickView>
#include <QQmlApplicationEngine>
// #include"sign_up.h"
#include "Account/basicAccount.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
    ui->idEdit->hide();
    ui->nameEdit->hide();
    ui->passwdConfirm->hide();

    QObject::connect(ui->sign_up_btn, SIGNAL(clicked()), this, SLOT(signup_click()));
    QObject::connect(ui->loginBtn, SIGNAL(clicked()), this, SLOT(login_click()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

static bool checkPasswdStrength(const QString &passwd) {
    if (passwd.length() < 8) {
        return false;
    }
    if (passwd.contains(QRegExp("[0-9]"))) {
        if (passwd.contains(QRegExp("[a-z]"))) {
            if (passwd.contains(QRegExp("[A-Z]"))) {
                return true;
            }
        }
    }
    return false;
}

void MainWindow::signup_click() {
    if (ui->idEdit->isHidden()) {
        ui->idEdit->show();
        ui->nameEdit->show();
        ui->passwdConfirm->show();
        return;
    }
    QString phone = ui->phoneEdit->text();
    QString passwd = ui->passwdEdit->text();
    QString id = ui->idEdit->text();
    QString name = ui->nameEdit->text();
    if (phone.isEmpty()) {
        QMessageBox::information(this,"Title","手机号不能为空");
        return;
    }
    if (id.isEmpty()) {
        QMessageBox::information(this,"Title","身份证号不能为空");
        return;
    }
    if (name.isEmpty()) {
        QMessageBox::information(this,"Title","姓名不能为空");
        return;
    }
    if (passwd != ui->passwdConfirm->text()) {
        QMessageBox::information(this,"Title","两次密码输入不一致");
        return;
    }
    // 检查密码强度
    if (!checkPasswdStrength(passwd)) {
        QMessageBox::information(this,"Title","密码强度不足：至少需要8位，包含大小写字母和数字");
        return;
    }
    bms::BasicAccount account(name.toStdString(), passwd.toStdString(), phone.toStdString(), id.toStdString());
    account.store(account.datafile());
        QMessageBox::information(this,"Title","注册成功！");
}

void MainWindow::login_click() {
    // 如果用于注册的输入框显示，则隐藏
    if (ui->idEdit->isVisible()) {
        ui->idEdit->hide();
        ui->nameEdit->hide();
        ui->passwdConfirm->hide();
        return;
    }
    // 获取用户输入的账号和密码
    QString phone = ui->phoneEdit->text();
    QString password = ui->passwdEdit->text();

    bms::BasicAccount user(phone.toStdString(), password.toStdString());
    // 拼接用户信息文件的路径
    std::string filename = user.datafile();

    /*
     * 以下代码用于测试登录功能
        // // qml引擎
        // QQmlApplicationEngine* engine = new QQmlApplicationEngine;
        // // 加载qml文件
        // engine->load(QUrl(QStringLiteral("qrc:/dashboard.qml")));
        // // 获取qml窗口
    */

    std::ifstream file(filename);

    if (file.good()) {
        // 文件存在，登录成功
        user.load(filename);
        // 创建独立的qml窗口
        // qml引擎
        QQmlApplicationEngine* engine = new QQmlApplicationEngine;
        // 加载qml文件
        engine->load(QUrl(QStringLiteral("qrc:/dashboard.qml")));
    } else {
        // 文件不存在，登录失败
        QMessageBox::information(this,"Title", "登录失败，请检查手机号和密码是否正确");
    }

}

void MainWindow::on_clear_button_clicked()
{
    ui->nameEdit->clear();
    ui->idEdit->clear();
    ui->phoneEdit->clear();
    ui->passwdEdit->clear();
    ui->passwdConfirm->clear();
}

