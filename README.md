EDGE_SENSOR:
    IR_TRIGGER

Problem:
    无法处理多机访问

Describe:
AT_CMD 适用于JDY-24M模块AT_Mesh下的UART指令接收和解析
[x]信号量获取 串口数据
[x]使用 union 构建传输的 pack
    倒置输入字符匹配大端序
[x]实现 request "可靠通信"

工程目录：
Tests:
    测试通信和传感器的各个组件的可用性
    定义 XX_test函数,将其放置与相应Task位置作为测试入口
        在测试成功后,注释 //success
迭代流程:
    重构？
        在原实现后添加_old尾缀作为一次提交
    实现:

TODO:
    适应Task的Button

提交规范:
    Change:
    FIX:
    ADD: