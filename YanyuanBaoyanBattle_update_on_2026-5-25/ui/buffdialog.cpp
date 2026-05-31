#include "buffdialog.h"

#include "uistyle.h"

#include <QButtonGroup>
#include <QDialogButtonBox>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>

BuffDialog::BuffDialog(const QVector<Buff> &choices,
                       const QString &title,
                       QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(title);
    resize(560, 360);
    setModal(true);
    UiStyle::polishDialog(this);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(18, 18, 18, 18);
    root->setSpacing(12);

    auto *card = new QFrame(this);
    card->setObjectName(QStringLiteral("DialogCard"));
    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(20, 18, 20, 18);
    layout->setSpacing(10);

    auto *heading = new QLabel(title, card);
    heading->setObjectName(QStringLiteral("SectionTitle"));
    layout->addWidget(heading);

    auto *hint = new QLabel(QStringLiteral("请选择一个 Buff 立即生效，随后会自动触发一个随机事件。不同 Buff 会影响学生属性和经营节奏。"), card);
    hint->setObjectName(QStringLiteral("TipLabel"));
    hint->setWordWrap(true);
    layout->addWidget(hint);

    m_group = new QButtonGroup(this);

    for (int i = 0; i < choices.size(); ++i) {
        auto *button = new QRadioButton(choices[i].summary(), card);
        button->setCursor(Qt::PointingHandCursor);
        m_group->addButton(button, i);
        layout->addWidget(button);
        if (i == 0) {
            button->setChecked(true);
        }
    }

    auto *box = new QDialogButtonBox(QDialogButtonBox::Ok, card);
    if (auto *ok = box->button(QDialogButtonBox::Ok)) {
        ok->setText(QStringLiteral("确认选择"));
        ok->setProperty("variant", "primary");
        ok->setCursor(Qt::PointingHandCursor);
    }
    connect(box, &QDialogButtonBox::accepted, this, &QDialog::accept);
    layout->addWidget(box);

    root->addWidget(card);
}

int BuffDialog::selectedIndex() const
{
    return m_group ? m_group->checkedId() : -1;
}
