#pragma once

#include <QDialog>
#include <QVector>

#include "student.h"

class QLabel;
class QTableWidget;

class RecruitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RecruitDialog(QWidget *parent = nullptr);
    QVector<Student> selectedStudents() const;

private slots:
    void refreshCost();

private:
    QTableWidget *m_table = nullptr;
    QLabel *m_costLabel = nullptr;
    QVector<Student> m_candidates;

    void buildCandidates();
};
