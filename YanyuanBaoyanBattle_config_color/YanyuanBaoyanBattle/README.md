# 《燕园保研战》Qt 大作业源码

这是一个可直接用 Qt Creator 打开的 Qt Widgets + qmake 项目。

## 已实现功能

- 开局经费：总经费 3000 金币，其中招生预算 1000 金币，至少选择 4 人。
- 8 学期流程：学期初、Buff、事件、替考、期中、期末、结算。
- 学生属性：成绩、健康、心理、市场价、状态。
- Buff 系统：3 选 1，支持全体或随机学生效果。
- 随机事件系统：全体或随机学生影响。
- 替考答题：每学期最多 3 人，每人 5 道题，正确率转化为成绩提升。
- 出售/转学：结算阶段可出售在读学生。
- 休学：任一属性低于 10 自动休学并扣 200 金币。
- 胜负判定：第 8 学期结算后，保研率达到 60% 胜利。

## 用 Qt Creator 打开

1. 解压本项目。
2. 打开 Qt Creator。
3. 选择 `File` -> `Open File or Project...`。
4. 选择 `YanyuanBaoyanBattle.pro`。
5. 选择 Desktop Qt Kit。
6. 点击左下角绿色运行按钮。

## 如果中文乱码

请确认：
- 所有 `.cpp/.h/.json` 文件用 UTF-8 保存。
- Qt Creator：`Tools` -> `Options` -> `Text Editor` -> `Behavior` -> `Default encoding` 设为 UTF-8。

## 建议答辩展示点

- 面向对象：Student、Buff、Event、ExamSystem、GameManager、多个 Dialog。
- STL/Qt 容器：QVector、QStringList、std::shuffle。
- 文件读取：questions.json 通过 Qt Resource 加载。
- 信号槽：GameManager 与 MainWindow 解耦。
- 自定义界面：RecruitDialog、BuffDialog、ExamDialog、EventLogWidget。
- 运算符重载：Student::operator<。
- 随机数：Buff/Event 抽取与考试波动。

## 平衡性调整说明

- 将实际开局总经费从 10000 金币下调为 3000 金币，避免学校资金过多。
- 保留 1000 金币招生预算；默认勾选方案花费 980 金币，开始后约剩 2020 金币。
- 以 4 名学生为例，8 学期基础生活费约 1600 金币，剩余资金可用于少量替考、事件损失或休学公关，整体更接近“能生存但不宽裕”。

## 本版结构/UI 改进说明

- 新增 `libcore/gameconfig.h`：集中管理开局经费、招生预算、保研阈值、生活费、替考成本、考试波动、售价系数、Buff/Event 数值、招生候选人和 UI 显示阈值。
- 主界面学生表和招生界面的成绩/健康/心理属性增加颜色：高属性为绿色，偏低为橙色，危险为红色，非在读学生为灰色。
- 本版没有继续调整玩法平衡数值，只是把已有数值集中到配置文件，并增强显示效果。
