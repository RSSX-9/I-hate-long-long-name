# 燕园保研战

## 项目简介

**燕园保研战** 是一款基于 **Qt/C++** 开发的校园经营模拟游戏。玩家需要在有限的学期和资金内招募、培养学生，通过合理安排资源、应对随机事件和完成答题挑战，最终让足够数量的学生达到保研条件。

游戏融合了校园经营、学生养成、随机事件和程设学习答题等内容，适合作为 Qt/C++ 课程实践项目。

## 运行环境

* Qt 5 或 Qt 6
* C++17
* Qt Creator
* qmake

## 运行方式

使用 Qt Creator 打开项目文件：

```text
YanyuanBaoyanBattle.pro
```

执行 qmake 后编译运行即可。

也可以在命令行中运行：

```bash
qmake
make
```

## 游戏目标

玩家需要在规定学期内，使至少一定数量的学生满足保研条件。

保研条件主要包括：

* 学生仍处于在读状态
* 成绩达到要求
* 健康达到要求
* 心理状态达到要求

如果最终达标人数不足，则游戏失败。

## 基本玩法

游戏开始时，玩家需要从候选学生中进行招生。每名学生都有不同的初始成绩、健康、心理和招生成本。

游戏按学期推进。每个学期中，玩家需要经历不同阶段，并根据当前情况做出决策。

主要操作包括：

* 招募学生
* 推进学期阶段
* 选择 Buff
* 应对随机事件
* 参加考试结算
* 进行答题替考
* 在结算阶段协助学生转学发展

玩家需要平衡学生的成绩、健康和心理状态，避免学生休学，同时合理使用金币和替考机会。

## 答题机制

游戏中包含程设相关答题内容。玩家可以在特定阶段为学生进行替考答题。

答题正确率会影响学生成绩提升幅度。正确率越高，成绩提升越多。替考需要消耗金币，并且每学期次数有限。

## 项目结构

```text
YanyuanBaoyanBattle/
├── main.cpp
├── YanyuanBaoyanBattle.pro
├── questions.json
├── resources.qrc
├── libcore/
│   ├── student.cpp / student.h
│   ├── gamemanager.cpp / gamemanager.h
│   ├── buff.cpp / buff.h
│   ├── event.cpp / event.h
│   └── examsystem.cpp / examsystem.h
├── ui/
│   ├── mainwindow.cpp / mainwindow.h
│   ├── recruitdialog.cpp / recruitdialog.h
│   ├── buffdialog.cpp / buffdialog.h
│   ├── examdialog.cpp / examdialog.h
│   ├── eventlog.cpp / eventlog.h
│   └── uistyle.cpp / uistyle.h
└── resources/
    └── ui/
```

## 主要模块

* `GameManager`：负责游戏流程控制、阶段推进、胜负判断等。
* `Student`：负责学生属性和状态管理。
* `Buff`：负责 Buff 效果定义与应用。
* `Event`：负责随机事件定义。
* `ExamSystem`：负责题库读取和答题系统。
* `ui`：负责界面显示与用户交互。

## 配置说明

游戏中的主要参数集中在：

```text
libcore/gameconfig.h
```

可以在该文件中调整初始资金、学期数量、保研要求、替考次数、生活费、学生候选数据等内容。

题库文件为：

```text
questions.json
```

可以通过修改该文件扩展答题内容。

## 项目说明

本项目主要用于 Qt/C++ 课程实践和小游戏开发练习。项目尽量将核心逻辑和界面显示分离，方便后续维护、修改和扩展。
