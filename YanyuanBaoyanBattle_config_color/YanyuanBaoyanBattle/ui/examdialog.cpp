#include "examdialog.h"

#include "gameconfig.h"

#include <QButtonGroup>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>

ExamDialog::ExamDialog(const QVector<Question> &questions,
                       const QString &studentName,
                       QWidget *parent)
    : QDialog(parent),
      m_questions(questions)
{
    setWindowTitle(QStringLiteral("替考答题"));
    resize(GameConfig::ExamDialogWidth, GameConfig::ExamDialogHeight);
    setModal(true);

    auto *layout = new QVBoxLayout(this);
    m_titleLabel = new QLabel(QStringLiteral("正在为【%1】替考：共 %2 题。").arg(studentName).arg(m_questions.size()), this);
    m_titleLabel->setWordWrap(true);
    layout->addWidget(m_titleLabel);

    m_promptLabel = new QLabel(this);
    m_promptLabel->setWordWrap(true);
    m_promptLabel->setStyleSheet(QStringLiteral("font-weight: bold; font-size: %1px;").arg(GameConfig::ExamPromptFontSize));
    layout->addWidget(m_promptLabel);

    m_optionsLayout = new QVBoxLayout();
    layout->addLayout(m_optionsLayout);

    m_resultLabel = new QLabel(this);
    m_resultLabel->setWordWrap(true);
    layout->addWidget(m_resultLabel);

    m_submitButton = new QPushButton(QStringLiteral("提交本题"), this);
    layout->addWidget(m_submitButton);
    connect(m_submitButton, &QPushButton::clicked, this, &ExamDialog::submitCurrent);

    if (m_questions.isEmpty()) {
        m_resultLabel->setText(QStringLiteral("题库为空，本次正确率为 0。"));
        m_submitButton->setText(QStringLiteral("关闭"));
        connect(m_submitButton, &QPushButton::clicked, this, &QDialog::accept);
    } else {
        renderQuestion();
    }
}

double ExamDialog::accuracy() const
{
    if (m_questions.isEmpty()) {
        return 0.0;
    }
    return static_cast<double>(m_correctCount) / static_cast<double>(m_questions.size());
}

void ExamDialog::submitCurrent()
{
    if (m_questions.isEmpty()) {
        accept();
        return;
    }

    if (!m_currentSubmitted) {
        const int selected = m_optionGroup->checkedId();
        if (selected < 0) {
            m_resultLabel->setText(QStringLiteral("请先选择一个选项。"));
            return;
        }

        const Question &q = m_questions[m_currentIndex];
        if (selected == q.answerIndex) {
            ++m_correctCount;
            m_resultLabel->setText(QStringLiteral("回答正确！%1").arg(q.explanation));
        } else {
            m_resultLabel->setText(QStringLiteral("回答错误。正确答案是：%1。%2")
                                   .arg(q.options[q.answerIndex], q.explanation));
        }

        m_currentSubmitted = true;
        if (m_currentIndex == m_questions.size() - 1) {
            m_submitButton->setText(QStringLiteral("完成替考"));
        } else {
            m_submitButton->setText(QStringLiteral("下一题"));
        }
        return;
    }

    if (m_currentIndex == m_questions.size() - 1) {
        accept();
        return;
    }

    ++m_currentIndex;
    m_currentSubmitted = false;
    renderQuestion();
}

void ExamDialog::renderQuestion()
{
    clearOptions();

    const Question &q = m_questions[m_currentIndex];
    m_promptLabel->setText(QStringLiteral("第 %1/%2 题：%3")
                           .arg(m_currentIndex + 1)
                           .arg(m_questions.size())
                           .arg(q.prompt));
    m_resultLabel->clear();
    m_submitButton->setText(QStringLiteral("提交本题"));

    m_optionGroup = new QButtonGroup(this);
    for (int i = 0; i < q.options.size(); ++i) {
        auto *button = new QRadioButton(QStringLiteral("%1. %2")
                                        .arg(QChar(QChar::fromLatin1(static_cast<char>('A' + i))))
                                        .arg(q.options[i]), this);
        m_optionGroup->addButton(button, i);
        m_optionsLayout->addWidget(button);
        if (i == 0) {
            button->setChecked(true);
        }
    }
}

void ExamDialog::clearOptions()
{
    if (m_optionGroup) {
        delete m_optionGroup;
        m_optionGroup = nullptr;
    }

    while (QLayoutItem *item = m_optionsLayout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
}
