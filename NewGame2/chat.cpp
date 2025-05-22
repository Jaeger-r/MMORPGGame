#include "chat.h"
#include "ui_chat.h"

Chat::Chat(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Chat)
{
    ui->setupUi(this);
    playerName = "none";

}

Chat::~Chat()
{
    delete ui;
}

void Chat::setName(QString name)
{
    playerName = name;
}
QString Chat::getName()
{
    return playerName;
}

void Chat::on_SendButton_clicked()
{
    QString UserMessage = ui->EnterBox->toPlainText().trimmed();  // 去除前后空白
    if (UserMessage.isEmpty()) {
        QMessageBox::warning(this,"发送","不能发送空消息");
        return;
    }
    QString username = getName();

    ui->EnterBox->clear();
    STRU_CHAT_RQ scq;

    //memcpy(scq.player_Name,username.toStdString().c_str(),username.toStdString().length());
    //memcpy(scq.szbuf,UserMessage.toStdString().c_str(),UserMessage.toStdString().length());
    strcpy(scq.player_Name,username.toStdString().c_str());
    strcpy(scq.szbuf,UserMessage.toStdString().c_str());

    m_Core->sendData(m_Core->getSocket(),(char*)&scq,sizeof(scq));

}

void Chat::slot_chat(STRU_CHAT_RS *szbuf)
{
    //接收消息
    QString senderName = QString::fromUtf8(szbuf->player_Name);
    QString message = QString::fromUtf8(szbuf->szbuf);

    // 格式化显示消息的内容，发送者名称 + 消息
    QString fromattedMessage = QString("%1: %2").arg(senderName).arg(message);

    //显示到控件上
    QListWidgetItem *newItem = new QListWidgetItem(fromattedMessage);
    ui->ChatBox->addItem(newItem);

    // 自动滚动到最新消息
    ui->ChatBox->scrollToBottom();

}
