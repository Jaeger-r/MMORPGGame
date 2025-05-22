#include "bag.h"
#include "ui_bag.h"

Bag::Bag(QWidget *parent) :
    QWidget(parent),
    tooltipLabel(new QLabel(this)),
    ui(new Ui::Bag)
{
    ui->setupUi(this);

    // 设置表格外观
    QPalette palette = ui->bag1->palette();
    palette.setColor(QPalette::Base, Qt::white);
    ui->bag1->setPalette(palette);
    ui->bag1->setStyleSheet("QTableWidget::item { border: 1px solid black; }");
    ui->bag1->setRowCount(13);
    ui->bag1->setColumnCount(10);
    ui->bag1->setShowGrid(true);
    ui->bag1->verticalHeader()->setVisible(false);
    ui->bag1->horizontalHeader()->setVisible(false);
    ui->bag1->verticalHeader()->setDefaultSectionSize(40);
    ui->bag1->horizontalHeader()->setDefaultSectionSize(40);
    ui->bag1->setIconSize(QSize(35, 35));
    ui->bag1->setSelectionMode(QAbstractItemView::NoSelection);

    // 启用鼠标追踪（保证 `cellEntered` 事件可以正确触发）
    ui->bag1->setMouseTracking(true);
    ui->bag1->viewport()->setMouseTracking(true);

    // 连接信号槽
    connect(ui->bag1, &QTableWidget::cellEntered, this, &Bag::showItemTooltip);
    connect(ui->bag1, &QTableWidget::cellEntered, this, &Bag::onCellEntered);

    // 初始化 tooltipLabel（黑底、白字、圆角）
    tooltipLabel->setStyleSheet("background-color: rgba(0, 0, 0, 200); color: white; padding: 5px; border-radius: 5px;");
    tooltipLabel->setAlignment(Qt::AlignCenter);
    tooltipLabel->setWindowFlags(Qt::ToolTip); // 使其类似于系统提示框
    tooltipLabel->setVisible(false);
    // addItemtoBag(0,0,QCoreApplication::applicationDirPath() + "/icons/weapon/sword.bmp","哈哈");
    // addItemtoBag(0,1,QCoreApplication::applicationDirPath() + "/icons/armor/armor2.bmp","哈哈");
    // addItemtoBag(0,2,QCoreApplication::applicationDirPath() + "/icons/armor/boots1.bmp","哈哈");
    // addItemtoBag(0,3,QCoreApplication::applicationDirPath() + "/icons/armor/necklace1.bmp","哈哈");
    // addItemtoBag(0,4,QCoreApplication::applicationDirPath() + "/icons/armor/ring1.bmp","哈哈");
    // addItemtoBag(0,5,QCoreApplication::applicationDirPath() + "/icons/armor/armor1.bmp","哈哈");
}

Bag::~Bag()
{
    delete ui;
}
//"/Users/zhangwenjie/Desktop/qt/newgame/weapon/sword.png"
void Bag::addItemtoBag(int row, int column, const QString &iconPath, const QString &tooltipText)
{
    // 创建图标标签
    QLabel *iconLabel = new QLabel();
    iconLabel->setPixmap(QPixmap(iconPath).scaled(35, 35, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    iconLabel->setAlignment(Qt::AlignCenter);
    ui->bag1->setCellWidget(row, column, iconLabel);

    // 创建 QTableWidgetItem 并设置 Tooltip 数据
    QTableWidgetItem *item = new QTableWidgetItem();
    item->setData(Qt::UserRole, tooltipText);
    ui->bag1->setItem(row, column, item);
}

void Bag::showItemTooltip(int row, int column)
{
    QTableWidgetItem *item = ui->bag1->item(row, column);
    if (item) {
        QString tooltipText = item->data(Qt::UserRole).toString();
        if (!tooltipText.isEmpty()) {
            tooltipLabel->setText(tooltipText);
            tooltipLabel->adjustSize(); // 自动调整大小
            QPoint pos = ui->bag1->viewport()->mapToGlobal(ui->bag1->visualItemRect(item).bottomRight()); // 计算相对窗口的位置
            tooltipLabel->move(pos + QPoint(10, 10)); // 避免遮挡鼠标
            tooltipLabel->setVisible(true);
            // 记录当前显示 tooltip 的行列
            lastTooltipRow = row;
            lastTooltipColumn = column;
        }
    }
}

// **隐藏物品详细信息**
void Bag::hideTooltip()
{
    tooltipLabel->setVisible(false);
}
void Bag::leaveEvent(QEvent *event)
{
    hideTooltip();
    QWidget::leaveEvent(event);
}
void Bag::onCellEntered(int row, int column)
{
    // 如果进入了新单元格，并且不是当前显示 tooltip 的物品，则隐藏 tooltip
    if (row != lastTooltipRow || column != lastTooltipColumn) {
        hideTooltip();
    }
}
