#include "uistyle.h"

#include <QApplication>
#include <QFont>
#include <QWidget>

namespace UiStyle
{

void applyApplicationStyle(QApplication &app)
{
    app.setFont(QFont(QStringLiteral("Microsoft YaHei UI"), 10));
    app.setStyleSheet(QStringLiteral(R"QSS(
* {
    font-family: "Microsoft YaHei UI", "PingFang SC", "Noto Sans CJK SC", sans-serif;
    color: #3f2521;
}
QMainWindow, QDialog {
    background: #fff5e6;
}
QWidget#GameRoot {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                stop:0 #fff0dc, stop:0.44 #fff7ec, stop:1 #ffe5df);
}
QFrame#HeroPanel {
    border-radius: 28px;
    border: 2px solid rgba(255, 255, 255, 170);
    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                stop:0 #8f000b, stop:0.58 #b41622, stop:1 #f2a54a);
}
QFrame#CardPanel, QFrame#ControlPanel, QFrame#DialogCard {
    border-radius: 22px;
    border: 1px solid rgba(152, 0, 13, 45);
    background: rgba(255, 252, 246, 232);
}
QFrame#MetricCard {
    border-radius: 18px;
    border: 1px solid rgba(152, 0, 13, 38);
    background: rgba(255, 255, 255, 224);
}
QLabel#GameTitle {
    color: #fff7e8;
    font-size: 32px;
    font-weight: 900;
    letter-spacing: 1px;
}
QLabel#HeroSubtitle, QLabel#HeroDescription {
    color: rgba(255, 247, 232, 230);
}
QLabel#HeroSubtitle {
    font-size: 13px;
    font-weight: 800;
    padding: 3px 10px;
    border-radius: 10px;
    background: rgba(255, 255, 255, 45);
}
QLabel#SectionTitle {
    color: #8f000b;
    font-size: 18px;
    font-weight: 900;
}
QLabel#MetricLabel {
    color: #8a6f66;
    font-size: 12px;
    font-weight: 700;
}
QLabel#MetricValue {
    color: #8f000b;
    font-size: 22px;
    font-weight: 900;
}
QLabel#TipLabel {
    color: #7b5a4b;
    font-size: 12px;
    line-height: 150%;
}
QLabel#RateLabel {
    color: #fff8ee;
    font-size: 14px;
    font-weight: 800;
}
QPushButton {
    min-height: 38px;
    padding: 7px 18px;
    border-radius: 17px;
    border: 1px solid rgba(120, 0, 10, 65);
    background: #fff8eb;
    color: #8f000b;
    font-weight: 800;
}
QPushButton:hover {
    background: #ffe8c4;
    border: 1px solid #d84a28;
}
QPushButton:pressed {
    background: #ffd494;
    padding-top: 9px;
    padding-bottom: 5px;
}
QPushButton:disabled {
    color: rgba(115, 91, 82, 125);
    background: rgba(255, 249, 239, 150);
    border: 1px solid rgba(115, 91, 82, 35);
}
QPushButton[variant="primary"] {
    color: #fffaf0;
    border: 1px solid #6f0007;
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                stop:0 #c42a37, stop:1 #8f000b);
}
QPushButton[variant="primary"]:hover {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                stop:0 #d83d47, stop:1 #a50b16);
}
QPushButton[variant="gold"] {
    color: #5d2600;
    border: 1px solid #e09a2d;
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                stop:0 #ffe0a1, stop:1 #f2a54a);
}
QPushButton[variant="danger"] {
    color: #fffaf0;
    border: 1px solid #7d1315;
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                stop:0 #ad2b31, stop:1 #6f0007);
}
QTableWidget {
    border: none;
    border-radius: 16px;
    background: rgba(255, 255, 255, 205);
    gridline-color: rgba(143, 0, 11, 24);
    alternate-background-color: #fff2df;
    selection-background-color: #ffd36b;
    selection-color: #4a271f;
}
QTableWidget::item {
    padding: 8px;
    border-bottom: 1px solid rgba(143, 0, 11, 18);
}

QTableWidget::item:selected {
    background: #ffd36b;
    color: #4a271f;
    font-weight: 900;
}
QFrame#SelectionCard {
    border-radius: 16px;
    border: 1px dashed rgba(143, 0, 11, 70);
    background: rgba(255, 243, 221, 210);
}
QLabel#SelectionName {
    color: #8f000b;
    font-size: 15px;
    font-weight: 900;
}
QLabel#SelectionDetail {
    color: #6b4c43;
    font-size: 12px;
    line-height: 150%;
}
QHeaderView::section {
    min-height: 34px;
    padding: 7px 8px;
    border: none;
    border-right: 1px solid rgba(255, 255, 255, 90);
    background: #8f000b;
    color: #fffaf0;
    font-weight: 900;
}
QScrollBar:vertical {
    border: none;
    background: rgba(143, 0, 11, 20);
    width: 12px;
    margin: 14px 2px 14px 2px;
    border-radius: 6px;
}
QScrollBar::handle:vertical {
    background: #d86b43;
    min-height: 26px;
    border-radius: 6px;
}
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
    height: 0;
}
QTextEdit#EventLog {
    border: none;
    border-radius: 16px;
    background: rgba(73, 25, 25, 230);
    color: #fff0dc;
    padding: 12px;
    selection-background-color: #f2a54a;
}
QProgressBar {
    min-height: 20px;
    border-radius: 10px;
    border: 1px solid rgba(255, 250, 240, 130);
    background: rgba(80, 0, 8, 90);
    color: #fffaf0;
    font-weight: 900;
    text-align: center;
}
QProgressBar::chunk {
    border-radius: 9px;
    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                stop:0 #ffca68, stop:1 #fff2a8);
}
QRadioButton {
    spacing: 8px;
    padding: 8px 10px;
    border-radius: 12px;
}
QRadioButton:hover {
    background: rgba(255, 224, 161, 95);
}
QRadioButton::indicator {
    width: 17px;
    height: 17px;
}
QDialogButtonBox QPushButton {
    min-width: 92px;
}
QMessageBox QLabel {
    min-width: 300px;
}
)QSS"));
}

void polishDialog(QWidget *dialog)
{
    if (!dialog) {
        return;
    }
    dialog->setAttribute(Qt::WA_StyledBackground, true);
}

}
