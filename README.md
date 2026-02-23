# GameAnimationSystem Plugin | Unreal Engine 5.7

GameAnimationSample 打包插件化
扩展分层代理

## Features

- Game Animation Sample
- Mover
- GameplayCamera
- Motion Matching
- 基础角色
    - 内存引用
        - 创建 `BaseCharacter_Mover` 仅包含基础执行逻辑,减轻基础扩展的原始体积
        - 设计 将美术资产分离放置在 子类(例:`BaseCharacter_Hero`)
        - 移除 `AC_VisualOverrideManager` 减少内存引用
    - 输入代理
        - `InputAction` 打包代理事件函数
        - 创建 `ProduceIpuit` 中使用到的 `Get_IA` 代理函数 (Input - `Get_TwinStick_AimDirection` / `Get_Move`)
- 网络预测
    - 固定 tick 帧率（可选）
        - 60 (标准)
        - 64 / 128 （倍率）

## Migrating Plugin

将插件迁移到您自己的虚幻引擎项目中，您可以按照以下步骤操作：

1. 复制到您的项目 Plugins 文件夹中
2. `/Plugins/*/Config/` 将这些文件与你的项目文件合并 `/Config/`
3. 复制 `/Plugins/*/Config/DefaultEngine.ini` 的内容到项目 `/Config/DefaultEngine.ini` 的末尾
4. 复制 `/Plugins/*/Config/DefaultNetworkPrediction.ini` 到项目 `/Config/` 文件夹里
5. 复制 `/Plugins/*/Config/Tags/GameplayTags_GASP.ini` 到项目 `/Config/` 文件夹里
6. 启动项目后，会提示错误信息添加 碰撞 设置 ，点击消息末尾的“添加”按钮

> 注意 `DataDrivenConsoleVariableSettings`控制台变量 、 `Tags`标签 和 `Network Prediction`网络预测 必须正确配置

## Plan

- [x] 代理输入分离
- [ ] 基础引用优化
- [ ] 待机角色转动
- [ ] 掩避模式扩展
- [ ] 动画叠加图层
- [ ] 第一人称视角
- [ ] 库存武器插槽
- [ ] 逻辑蓝图转C++
- [ ] 兼容通用输入
- [ ] 动作兼容GAS
- [ ] 数据加载配置

## License

[UE-Only Content - Licensed for Use Only with Unreal Engine-based Products](https://www.unrealengine.com/en-US/eula/content)
[Game Animation Sample | Fab](https://www.fab.com/listings/880e319a-a59e-4ed2-b268-b32dac7fa016)