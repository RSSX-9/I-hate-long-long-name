#include "examdialog.h"

#include "gameconfig.h"
#include "uistyle.h"

#include <QAbstractButton>
#include <QButtonGroup>
#include <QFrame>
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
    setWindowTitle(QStringLiteral("附身答题"));
    resize(GameConfig::ExamDialogWidth + 80, GameConfig::ExamDialogHeight + 60);
    setModal(true);
    UiStyle::polishDialog(this);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(18, 18, 18, 18);

    auto *card = new QFrame(this);
    card->setObjectName(QStringLiteral("DialogCard"));
    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(22, 20, 22, 20);
    layout->setSpacing(12);

    m_titleLabel = new QLabel(QStringLiteral("正在附身【%1】：共 %2 题。保持冷静，选出最稳答案。")
                                  .arg(studentName)
                                  .arg(m_questions.size()), card);
    m_titleLabel->setObjectName(QStringLiteral("SectionTitle"));
    m_titleLabel->setWordWrap(true);
    layout->addWidget(m_titleLabel);

    m_feedbackEmojiLabel = new QLabel(QStringLiteral("📝"), card);
    m_feedbackEmojiLabel->setAlignment(Qt::AlignCenter);
    m_feedbackEmojiLabel->setStyleSheet(QStringLiteral("font-size: 34px; padding: 4px 0 2px 0;"));
    layout->addWidget(m_feedbackEmojiLabel);

    m_promptLabel = new QLabel(card);
    m_promptLabel->setWordWrap(true);
    m_promptLabel->setStyleSheet(QStringLiteral("font-weight: 900; font-size: %1px; color: #3f2521;").arg(GameConfig::ExamPromptFontSize + 1));
    layout->addWidget(m_promptLabel);

    m_optionsLayout = new QVBoxLayout();
    m_optionsLayout->setSpacing(6);
    layout->addLayout(m_optionsLayout);

    m_resultLabel = new QLabel(card);
    m_resultLabel->setObjectName(QStringLiteral("TipLabel"));
    m_resultLabel->setWordWrap(true);
    m_resultLabel->setMinimumHeight(58);
    layout->addWidget(m_resultLabel);

    m_submitButton = new QPushButton(QStringLiteral("提交本题"), card);
    m_submitButton->setProperty("variant", "primary");
    m_submitButton->setCursor(Qt::PointingHandCursor);
    layout->addWidget(m_submitButton, 0, Qt::AlignRight);
    connect(m_submitButton, &QPushButton::clicked, this, &ExamDialog::submitCurrent);

    root->addWidget(card);

    if (m_questions.isEmpty()) {
        m_feedbackEmojiLabel->setText(QStringLiteral("😅"));
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
            m_feedbackEmojiLabel->setText(QStringLiteral("🤔"));
            m_resultLabel->setStyleSheet(QStringLiteral("color: #c36b00; font-weight: 700;"));
            m_resultLabel->setText(QStringLiteral("请先选择一个选项。"));
            return;
        }

        const Question &q = m_questions[m_currentIndex];
        for (QAbstractButton *button : m_optionGroup->buttons()) {
            button->setEnabled(false);
        }

        if (selected == q.answerIndex) {
            ++m_correctCount;
            m_feedbackEmojiLabel->setText(QStringLiteral("😊"));
            m_resultLabel->setStyleSheet(QStringLiteral("color: #2e7d32; font-weight: 800; line-height: 150%;"));
            m_resultLabel->setText(QStringLiteral("回答正确！%1\n\n继续保持，这一题拿下了。")
                                   .arg(q.explanation));
        } else {
            m_feedbackEmojiLabel->setText(QStringLiteral("😢"));
            m_resultLabel->setStyleSheet(QStringLiteral("color: #b23a35; font-weight: 800; line-height: 150%;"));
            m_resultLabel->setText(QStringLiteral("回答错误。正确答案是：%1。%2\n\n别灰心，下一题继续加油。")
                                   .arg(q.options[q.answerIndex], q.explanation));
        }

        m_currentSubmitted = true;
        if (m_currentIndex == m_questions.size() - 1) {
            m_submitButton->setText(QStringLiteral("完成附身"));
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
    m_feedbackEmojiLabel->setText(QStringLiteral("📝"));
    m_promptLabel->setText(QStringLiteral("第 %1/%2 题：%3")
                           .arg(m_currentIndex + 1)
                           .arg(m_questions.size())
                           .arg(q.prompt));
    m_resultLabel->clear();
    m_resultLabel->setStyleSheet(QString());
    m_submitButton->setText(QStringLiteral("提交本题"));

    m_optionGroup = new QButtonGroup(this);
    for (int i = 0; i < q.options.size(); ++i) {
        auto *button = new QRadioButton(QStringLiteral("%1. %2")
                                        .arg(QChar(QChar::fromLatin1(static_cast<char>('A' + i))))
                                        .arg(q.options[i]), this);
        button->setCursor(Qt::PointingHandCursor);
        m_optionGroup->addButton(button, i);
        m_optionsLayout->addWidget(button);
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
