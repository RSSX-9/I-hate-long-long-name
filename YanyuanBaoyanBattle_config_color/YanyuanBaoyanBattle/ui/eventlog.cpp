#include "eventlog.h"

#include <QDateTime>

EventLogWidget::EventLogWidget(QWidget *parent)
    : QTextEdit(parent)
{
    setReadOnly(true);
    setMinimumWidth(330);
    setPlaceholderText(QStringLiteral("事件与 Buff 日志会显示在这里。"));
}

void EventLogWidget::appendLog(const QString &message)
{
    const QString time = QDateTime::currentDateTime().toString(QStringLiteral("hh:mm:ss"));
    append(QStringLiteral("[%1] %2").arg(time, message));
}
