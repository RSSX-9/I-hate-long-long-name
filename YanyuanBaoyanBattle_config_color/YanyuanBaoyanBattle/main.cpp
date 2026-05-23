#include <QApplication>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // ----- 全局现代化 UI QSS 皮肤注入 -----
    app.setStyleSheet(R"(
        /* 全局字体与现代背景 */
        QWidget {
            font-family: "Segoe UI", "Helvetica Neue", "Microsoft YaHei", sans-serif;
            font-size: 13px;
            color: #2c3e50;
            background-color: #f4f6f9;
        }

        /* 主窗口白底纯净设计 */
        QMainWindow {
            background-color: #ffffff;
        }

        /* 顶部看板指标卡片 */
        QFrame#MetricCard {
            background-color: #ffffff;
            border: 1px solid #e2e8f0;
            border-radius: 10px;
            padding: 12px;
        }

        /* 主操作按钮 */
        QPushButton {
            background-color: #3b82f6;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 8px 18px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #2563eb;
        }
        QPushButton:pressed {
            background-color: #1d4ed8;
        }
        QPushButton:disabled {
            background-color: #cbd5e1;
            color: #94a3b8;
        }

        /* 功能性按钮特殊色设计 */
        QPushButton#examButton { background-color: #10b981; }
        QPushButton#examButton:hover { background-color: #059669; }
        QPushButton#examButton:pressed { background-color: #047857; }

        QPushButton#sellButton { background-color: #f59e0b; }
        QPushButton#sellButton:hover { background-color: #d97706; }
        QPushButton#sellButton:pressed { background-color: #b45309; }

        QPushButton#recruitButton { background-color: #8b5cf6; }
        QPushButton#recruitButton:hover { background-color: #7c3aed; }

        /* 电子表格美化 */
        QTableWidget {
            background-color: #ffffff;
            border: 1px solid #e2e8f0;
            gridline-color: #f1f5f9;
            border-radius: 8px;
        }
        QHeaderView::section {
            background-color: #f8fafc;
            color: #64748b;
            padding: 8px;
            border: none;
            border-bottom: 2px solid #e2e8f0;
            font-weight: bold;
        }
        QTableWidget::item {
            padding: 6px;
        }
        QTableWidget::item:selected {
            background-color: #eff6ff;
            color: #2563eb;
        }

        /* 日志窗口 */
        QTextEdit {
            background-color: #ffffff;
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            padding: 10px;
            font-family: "Consolas", "Courier New", monospace;
        }

        /* 分割条美化 */
        QSplitter::handle {
            background-color: #e2e8f0;
            margin: 0px 4px;
        }
    )");

    MainWindow window;
    window.resize(1150, 750);
    window.show();
    return app.exec();
}
