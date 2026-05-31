#pragma once

#include <QTextEdit>

class EventLogWidget : public QTextEdit
{
    Q_OBJECT

public:
    explicit EventLogWidget(QWidget *parent = nullptr);

public slots:
    void appendLog(const QString &message);
};
