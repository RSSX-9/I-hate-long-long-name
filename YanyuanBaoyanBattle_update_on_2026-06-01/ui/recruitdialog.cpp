#include "recruitdialog.h"

#include "gameconfig.h"
#include "uistyle.h"

#include <QAbstractItemView>
#include <QBrush>
#include <QColor>
#include <QDialogButtonBox>
#include <QFont>
#include <QFrame>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

namespace {
QColor attributeColor(int value)
{
    if (value < GameConfig::AttributeDangerThreshold) {
        return QColor(GameConfig::AttributeDangerColor);
    }
    if (value < GameConfig::AttributeWarningThreshold) {
        return QColor(GameConfig::AttributeWarningColor);
    }
    if (value >= GameConfig::AttributeHighThreshold) {
        return QColor(GameConfig::AttributeHighColor);
    }
    return QColor(GameConfig::AttributeNormalColor);
}

QTableWidgetItem *attributeItem(int value)
{
    auto *item = new QTableWidgetItem(QString::number(value));
    item->setForeground(QBrush(attributeColor(value)));
    item->setTextAlignment(Qt::AlignCenter);

    QFont font = item->font();
    if (value < GameConfig::AttributeWarningThreshold || value >= GameConfig::AttributeHighThreshold) {
        font.setBold(true);
    }
    item->setFont(font);

    return item;
}

QTableWidgetItem *centeredItem(const QString &text)
{
    auto *item = new QTableWidgetItem(text);
    item->setTextAlignment(Qt::AlignCenter);
    return item;
}
}

RecruitDialog::RecruitDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("开局招生"));
    resize(GameConfig::RecruitDialogWidth + 80, GameConfig::RecruitDialogHeight + 90);
    UiStyle::polishDialog(this);
    buildCandidates();

    int defaultRecruitCost = 0;
    for (const auto &candidate : GameConfig::RecruitCandidates) {
        if (candidate.defaultSelected) {
            defaultRecruitCost += candidate.basePrice;
        }
    }

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(18, 18, 18, 18);

    auto *card = new QFrame(this);
    card->setObjectName(QStringLiteral("DialogCard"));
    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(22, 20, 22, 20);
    layout->setSpacing(12);

    auto *title = new QLabel(QStringLiteral("选择你的第一届燕园学员"), card);
    title->setObjectName(QStringLiteral("SectionTitle"));
    layout->addWidget(title);

    auto *hint = new QLabel(QStringLiteral("开局总经费 %1 金币，请至少选择 %2 名学生。默认方案花费 %3，剩余金币可用于后续运营。")
                                .arg(GameConfig::StartingFunds)
                                .arg(GameConfig::MinRecruitCount)
                                .arg(defaultRecruitCost), card);
    hint->setObjectName(QStringLiteral("TipLabel"));
    hint->setWordWrap(true);
    layout->addWidget(hint);

    m_table = new QTableWidget(m_candidates.size(), 6, card);
    m_table->setHorizontalHeaderLabels({
        QStringLiteral("选择"),
        QStringLiteral("姓名"),
        QStringLiteral("成绩"),
        QStringLiteral("健康"),
        QStringLiteral("心理"),
        QStringLiteral("价格")
    });
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->verticalHeader()->setVisible(false);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setAlternatingRowColors(true);
    m_table->setShowGrid(false);

    for (int row = 0; row < m_candidates.size(); ++row) {
        auto *check = new QTableWidgetItem();
        check->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        check->setTextAlignment(Qt::AlignCenter);
        const bool defaultChecked = (GameConfig::RecruitCandidates[row].defaultSelected);
        check->setCheckState(defaultChecked ? Qt::Checked : Qt::Unchecked);
        m_table->setItem(row, 0, check);

        m_table->setItem(row, 1, centeredItem(m_candidates[row].name()));
        m_table->setItem(row, 2, attributeItem(m_candidates[row].score()));
        m_table->setItem(row, 3, attributeItem(m_candidates[row].health()));
        m_table->setItem(row, 4, attributeItem(m_candidates[row].mood()));
        m_table->setItem(row, 5, centeredItem(QString::number(m_candidates[row].basePrice())));
        m_table->setRowHeight(row, 38);
    }
    layout->addWidget(m_table);

    m_costLabel = new QLabel(card);
    m_costLabel->setObjectName(QStringLiteral("TipLabel"));
    layout->addWidget(m_costLabel);

    auto *box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, card);
    if (auto *ok = box->button(QDialogButtonBox::Ok)) {
        ok->setText(QStringLiteral("开始经营"));
        ok->setProperty("variant", "primary");
        ok->setCursor(Qt::PointingHandCursor);
    }
    if (auto *cancel = box->button(QDialogButtonBox::Cancel)) {
        cancel->setText(QStringLiteral("返回"));
        cancel->setCursor(Qt::PointingHandCursor);
    }
    layout->addWidget(box);

    root->addWidget(card);

    connect(m_table, &QTableWidget::itemChanged, this, &RecruitDialog::refreshCost);
    connect(box, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(box, &QDialogButtonBox::accepted, this, [this]() {
        QVector<Student> selected = selectedStudents();
        if (selected.size() < GameConfig::MinRecruitCount) {
            QMessageBox::warning(this, QStringLiteral("招生不足"), QStringLiteral("至少需要选择 %1 名学生。").arg(GameConfig::MinRecruitCount));
            return;
        }
        accept();
    });

    refreshCost();
}

QVector<Student> RecruitDialog::selectedStudents() const
{
    QVector<Student> selected;
    for (int row = 0; row < m_candidates.size(); ++row) {
        const QTableWidgetItem *item = m_table->item(row, 0);
        if (item && item->checkState() == Qt::Checked) {
            selected << m_candidates[row];
        }
    }
    return selected;
}

void RecruitDialog::refreshCost()
{
    int cost = 0;
    int count = 0;
    for (int row = 0; row < m_candidates.size(); ++row) {
        const QTableWidgetItem *item = m_table->item(row, 0);
        if (item && item->checkState() == Qt::Checked) {
            cost += m_candidates[row].basePrice();
            ++count;
        }
    }

    const int remaining = GameConfig::StartingFunds - cost;
    m_costLabel->setText(QStringLiteral("已选 %1 人 · 招生花费 %2 · 剩余金币 %3").arg(count).arg(cost).arg(remaining));
    if (remaining < 0) {
        m_costLabel->setStyleSheet(QStringLiteral("color: #c62828; font-weight: 900;"));
    } else {
        m_costLabel->setStyleSheet(QStringLiteral("color: #7b5a4b; font-weight: 800;"));
    }
}

void RecruitDialog::buildCandidates()
{
    m_candidates.clear();
    for (const auto &candidate : GameConfig::RecruitCandidates) {
        m_candidates << Student(QString::fromUtf8(candidate.name),
                                candidate.score,
                                candidate.health,
                                candidate.mood,
                                candidate.basePrice);
    }
}
