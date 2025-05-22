#ifndef CHAT_H
#define CHAT_H

#include <QWidget>
#include "packdef.h"
#include "TCPNet.h"
#include "core.h"

namespace Ui {
class Chat;
}

class Chat : public QWidget
{
    Q_OBJECT
public:
    explicit Chat(QWidget *parent = nullptr);
    ~Chat();
public:
    void setCore(ICore*pCore) {
        m_Core = pCore;
    }
    void setName(QString name);
    QString getName();
public slots:
    void slot_chat(STRU_CHAT_RS*szbuf);
private slots:
    void on_SendButton_clicked();

private:
    Ui::Chat *ui;
    ICore *m_Core;
    QString playerName;
};

#endif // CHAT_H
