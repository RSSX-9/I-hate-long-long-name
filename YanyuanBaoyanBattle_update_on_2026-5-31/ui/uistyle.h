#pragma once

class QApplication;
class QWidget;

namespace UiStyle
{
void applyApplicationStyle(QApplication &app);
void polishDialog(QWidget *dialog);
}
