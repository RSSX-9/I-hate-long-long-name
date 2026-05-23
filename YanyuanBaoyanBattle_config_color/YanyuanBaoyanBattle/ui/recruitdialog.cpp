#include "recruitdialog.h"

#include "gameconfig.h"

#include <QDialogButtonBox>
#include <QFont>
#include <QColor>
#include <QBrush>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
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

    QFont font = item->font();
    if (value < GameConfig::AttributeWarningThreshold || value >= GameConfig::AttributeHighThreshold) {
        font.setBold(true);
    }
    item->setFont(font);

    return item;
}
}



RecruitDialog::RecruitDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("开局招生"));
    resize(GameConfig::RecruitDialogWidth, GameConfig::RecruitDialogHeight);
    buildCandidates();

    int defaultRecruitCost = 0;
    for (const auto &candidate : GameConfig::RecruitCandidates) {
        if (candidate.defaultSelected) {
            defaultRecruitCost += candidate.basePrice;
        }
    }

    auto *layout = new QVBoxLayout(this);
    auto *hint = new QLabel(QStringLiteral("开局总经费 %1 金币，其中招生预算 %2 金币；请至少选择 %3 名学生。默认方案花费 %4，能给后续学期留下周转金。")
                                  .arg(GameConfig::StartingFunds)
                                  .arg(GameConfig::RecruitBudget)
                                  .arg(GameConfig::MinRecruitCount)
                                  .arg(defaultRecruitCost), this);
    hint->setWordWrap(true);
    layout->addWidget(hint);

    m_table = new QTableWidget(m_candidates.size(), 6, this);
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

    for (int row = 0; row < m_candidates.size(); ++row) {
        auto *check = new QTableWidgetItem();
        check->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        const bool defaultChecked = (GameConfig::RecruitCandidates[row].defaultSelected);
        check->setCheckState(defaultChecked ? Qt::Checked : Qt::Unchecked);
        m_table->setItem(row, 0, check);

        m_table->setItem(row, 1, new QTableWidgetItem(m_candidates[row].name()));
        m_table->setItem(row, 2, attributeItem(m_candidates[row].score()));
        m_table->setItem(row, 3, attributeItem(m_candidates[row].health()));
        m_table->setItem(row, 4, attributeItem(m_candidates[row].mood()));
        m_table->setItem(row, 5, new QTableWidgetItem(QString::number(m_candidates[row].basePrice())));
    }
    layout->addWidget(m_table);

    m_costLabel = new QLabel(this);
    layout->addWidget(m_costLabel);

    auto *box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addWidget(box);

    connect(m_table, &QTableWidget::itemChanged, this, &RecruitDialog::refreshCost);
    connect(box, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(box, &QDialogButtonBox::accepted, this, [this]() {
        QVector<Student> selected = selectedStudents();
        int cost = 0;
        for (const Student &s : selected) {
            cost += s.basePrice();
        }
        if (selected.size() < GameConfig::MinRecruitCount) {
            QMessageBox::warning(this, QStringLiteral("招生不足"), QStringLiteral("至少需要选择 %1 名学生。").arg(GameConfig::MinRecruitCount));
            return;
        }
        if (cost > GameConfig::RecruitBudget) {
            QMessageBox::warning(this, QStringLiteral("预算不足"), QStringLiteral("招生总价不能超过 %1 金币。").arg(GameConfig::RecruitBudget));
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

    const int left = GameConfig::RecruitBudget - cost;
    m_costLabel->setText(QStringLiteral("已选 %1 人，招生花费 %2，招生预算剩余 %3；开始后还会保留总经费余额。").arg(count).arg(cost).arg(left));
    if (left < 0) {
        m_costLabel->setStyleSheet(QStringLiteral("color: red; font-weight: bold;"));
    } else {
        m_costLabel->setStyleSheet(QString());
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
