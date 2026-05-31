#include "eventlog.h"

#include <QDateTime>
#include <QTextCursor>

EventLogWidget::EventLogWidget(QWidget *parent)
    : QTextEdit(parent)
{
    setObjectName(QStringLiteral("EventLog"));
    setReadOnly(true);
    setMinimumWidth(330);
    setPlaceholderText(QStringLiteral("事件与 Buff 日志会显示在这里。"));
}

void EventLogWidget::appendLog(const QString &message)
{
    const QString time = QDateTime::currentDateTime().toString(QStringLiteral("hh:mm:ss"));
    append(QStringLiteral("<span style='color:#f2a54a;font-weight:700;'>[%1]</span> %2").arg(time, message.toHtmlEscaped()));
    moveCursor(QTextCursor::End);
}
