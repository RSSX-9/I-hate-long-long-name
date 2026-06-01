# UI 优化说明

本版本只调整 Qt Widgets 界面与资源，不修改核心经营、考试、事件和结算逻辑。

## 主要改动

- 主界面改为卡通风、北大红主题：顶部游戏横幅、状态卡片、圆角面板、统一按钮样式。
- 增加保研进度条，清晰展示当前保研人数与胜利目标。
- 学生表格改为游戏化培养面板：属性颜色、居中排版、风险提示、培养建议更直观。
- 事件日志改为深色游戏日志面板，自动滚动到最新事件。
- 招生、Buff、替考对话框统一为卡片式弹窗，提高视觉一致性。
- 新增 SVG 图标资源，放置于 `resources/ui/`，并在 `resources.qrc` 中注册。
- 新增 `ui/uistyle.h/.cpp` 统一管理全局样式，便于后续继续换肤。

## 新增资源

- `resources/ui/app_icon.svg`
- `resources/ui/recruit.svg`
- `resources/ui/next.svg`
- `resources/ui/exam.svg`
- `resources/ui/sell.svg`
- `resources/ui/log.svg`
- `resources/ui/help.svg`

## 集成说明

直接使用 `YanyuanBaoyanBattle.pro` 打开项目即可。若使用 Qt Creator，请重新运行 qmake / 清理后构建，避免旧构建缓存影响资源加载。
