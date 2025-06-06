# arduino_node_mcu_1_0_esp8266

#当前版本 V0.9.1

这个程序是为基于 NodeMCU ESP8266-12F 的硬件设计的库区二维码显示器。其主要功能如下：

1. **分组选择与显示**：通过三个按钮（A、B、C），用户可以选择不同的库区分组（A~H），并在每组内切换不同的库位编号。
2. **二维码生成与显示**：程序会根据当前选择的库区与编号，生成一个特定格式（如 "KUWEI-11028"）的字符串，并用 QRCode 库生成二维码，通过 128x64 的液晶屏（U8g2库支持）显示出来。
3. **按钮操作逻辑**：
   - 按钮A/B短按：切换分组或编号（+1/-1）。
   - 按钮A/B长按：快速切换（+3/-3）。
   - 按钮C：在分组选择界面和二维码显示界面切换。
4. **数据管理**：每组库位信息以结构体数组形式存储于flash中，通过指针和索引管理切换。

**总结：**  
该程序实现了通过物理按键选择库区和库位，并实时生成并显示对应二维码，适用于仓库管理等场景下的二维码快速定位与显示。
