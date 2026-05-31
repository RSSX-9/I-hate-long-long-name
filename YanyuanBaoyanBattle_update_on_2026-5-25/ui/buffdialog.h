#pragma once

#include <QDialog>
#include <QVector>

#include "buff.h"

class QButtonGroup;

class BuffDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BuffDialog(const QVector<Buff> &choices,
                        const QString &title,
                        QWidget *parent = nullptr);

    int selectedIndex() const;

private:
    QButtonGroup *m_group = nullptr;
};
