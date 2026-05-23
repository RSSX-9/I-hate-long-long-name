#include "buffdialog.h"

#include <QButtonGroup>
#include <QDialogButtonBox>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>

BuffDialog::BuffDialog(const QVector<Buff> &choices,
                       const QString &title,
                       QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(title);
    setModal(true);

    auto *layout = new QVBoxLayout(this);
    auto *hint = new QLabel(QStringLiteral("请选择一个 Buff 立即生效，随后会自动触发一个随机事件。"), this);
    hint->setWordWrap(true);
    layout->addWidget(hint);

    m_group = new QButtonGroup(this);

    for (int i = 0; i < choices.size(); ++i) {
        auto *button = new QRadioButton(choices[i].summary(), this);
        m_group->addButton(button, i);
        layout->addWidget(button);
        if (i == 0) {
            button->setChecked(true);
        }
    }

    auto *box = new QDialogButtonBox(QDialogButtonBox::Ok, this);
    connect(box, &QDialogButtonBox::accepted, this, &QDialog::accept);
    layout->addWidget(box);
}

int BuffDialog::selectedIndex() const
{
    return m_group ? m_group->checkedId() : -1;
}
