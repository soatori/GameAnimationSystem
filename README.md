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
        - 解绑场景翻越方块转换,使用接口 `BPI_Traversable_Ledges` 消息函数
    - 输入代理
        - `InputAction` 打包代理事件函数
        - 创建 `ProduceIpuit` 中使用到的 `Get_IA` 代理函数 (Input - `Get_TwinStick_AimDirection` / `Get_Move`)
    - 待机转动
        - 修正角色旋转模式，缩小旋转响应夹角
        - 添加 `IdleState` `OnRotationIdle` 预处理旋转状态，待机延迟停止旋转跟随
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
- [x] 基础引用优化
- [X] 待机角色转动
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

## GASP插件化修改日志

![image.png](https://alidocs.oss-cn-zhangjiakou.aliyuncs.com/res/Lk3lbmbbR1J77Om9/img/6027b5e9-a5a6-4cc0-9205-8baf510fd929.png)

GASP 使用 Mover 组件会使用 _网络预测_

*   原模板项目中使用设置 - ++迁移使用插件时注意修改项目设置++
    
    *   全局-首选 Tick 策略
        
        *   独立
            
    *   固定 tick-模拟代理网络 LOD
        
        *   已插值
            
    *   固定 tick 帧率（可选）
        
        *   原 60
            
        *   倍速  64/128
            

---

![image.png](https://alidocs.oss-cn-zhangjiakou.aliyuncs.com/res/Lk3lbmbbR1J77Om9/img/9ad0d070-ef24-42d1-b4d1-4672ba0a5708.png)

插件中将原SandboxCharacter\_Mover 的 InputAction 事件的控制，独立打包成事件

---

![image.png](https://alidocs.oss-cn-zhangjiakou.aliyuncs.com/res/Lk3lbmbbR1J77Om9/img/315c91f6-8249-4e60-b432-7b12d5a1d12b.png)

SandboxCharacter\_Mover 中默认的 InputAction 是写死在 ProduceIpuit 需要的函数中

*   插件版使用 Get\_IA 代理函数，实际场景需要重载该函数
    
    *   Input - ++Get\_TwinStick\_AimDirection++ / ++Get\_Move++
        
*   MoverInputProducerInterface(移动器输入生产者接口)-ProduceIpuit 
    
    *   ++_To find where Mover takes inputs, open the "Produce Input" function in the INTERFACES tab._++
        
    *   要了解 Mover 从何处获取输入信息，请在“++接口++”选项卡中打开“++Produce Input++”功能。
        

---

插件基础角色 BaseCharacter\_Mover 仅包含基础执行逻辑

美术资产分离(角色网格体和角色动画) 放置在子类 Hero 中，减轻基础扩展的原始体积

移除AC\_VisualOverrideManager减少内存引用

---

解绑AC\_TraversalLogic\_Mover 中获取可翻越墙体的类型转换，使用BPI\_Traversable\_Ledges接口获取结果

![image.png](https://alidocs.oss-cn-zhangjiakou.aliyuncs.com/res/Lk3lbmbbR1J77Om9/img/d03357c8-9b64-4ea1-8e9b-0523fe5b7531.png)

关卡方块 LevelBlock\_Traversable 将原始函数GetLedgeTransforms /  FindLedgeClosestToActor 替换为BPI\_Traversable\_Ledges 接口，并实现接口函数 IsTraversable，原函数逻辑不变，通过调用接口消息 以达到解绑内存目的

![image.png](https://alidocs.oss-cn-zhangjiakou.aliyuncs.com/res/Lk3lbmbbR1J77Om9/img/d7a47236-0025-4ae5-b6dd-2b32f8de6bec.png)

原始AC\_TraversalLogic 组件顺应函数修改，使用接口消息函数,其他逻辑不变

![image.png](https://alidocs.oss-cn-zhangjiakou.aliyuncs.com/res/Lk3lbmbbR1J77Om9/img/92e99711-0c29-4651-aa80-1f6a9aaf2413.png)

---

创建 S\_AutoIdelStat![image.png](https://alidocs.oss-cn-zhangjiakou.aliyuncs.com/res/Lk3lbmbbR1J77Om9/img/91ee5842-e61c-4808-92fc-2b132e1df8af.png)

创建函数 OnRotationIdle 预修改旋转模式

![image.png](https://alidocs.oss-cn-zhangjiakou.aliyuncs.com/res/Lk3lbmbbR1J77Om9/img/9237986c-d7b3-42ac-a6e8-2a5aaa63b89b.png)

![image.png](https://alidocs.oss-cn-zhangjiakou.aliyuncs.com/res/Lk3lbmbbR1J77Om9/img/4fdbb669-a946-4ae1-b81d-57ab2d55f9d7.png)

![image.png](https://alidocs.oss-cn-zhangjiakou.aliyuncs.com/res/Lk3lbmbbR1J77Om9/img/5c451008-60a6-4f07-9c82-024c6c2aa795.png)

修正Get\_OrientationIntent，在无 MoveInput 输入时,Strafe 的旋转跟随，并 减少触发角度 的判定

![image.png](https://alidocs.oss-cn-zhangjiakou.aliyuncs.com/res/Lk3lbmbbR1J77Om9/img/34e2f90e-6c2f-4a31-a839-17c0c4dde218.png)

![image.png](https://alidocs.oss-cn-zhangjiakou.aliyuncs.com/res/Lk3lbmbbR1J77Om9/img/abdce52c-bec3-4424-95c1-1345a005859f.png)

![image.png](https://alidocs.oss-cn-zhangjiakou.aliyuncs.com/res/Lk3lbmbbR1J77Om9/img/dbfba356-acd4-4941-86e4-fd91cb97cc0a.png)

---

原角色、CMC、游戏模式、控制器模板

移至子文件夹
