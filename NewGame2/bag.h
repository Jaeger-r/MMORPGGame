#ifndef BAG_H
#define BAG_H

#include "tou.h"
namespace Ui {
class Bag;
}

class Bag : public QWidget
{
    Q_OBJECT

public:
    explicit Bag(QWidget *parent = nullptr);
    ~Bag();
    void addItemtoBag(int row, int column, const QString &iconPath, const QString &tooltipText);
private slots:
    void showItemTooltip(int row, int column);
    void hideTooltip();
protected:
    void onCellEntered(int row, int column);
    void leaveEvent(QEvent *event) override;
private:
    Ui::Bag *ui;
    QLabel *tooltipLabel; // 自定义提示框

private:
    int lastTooltipRow = -1;
    int lastTooltipColumn = -1;
};

#endif // BAG_H
