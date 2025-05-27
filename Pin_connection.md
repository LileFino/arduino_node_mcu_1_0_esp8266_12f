

### 主要外设引脚分配

| 设备               | 定义名 | 板载引脚名 | 实际GPIO编号 | 说明           |
|--------------------|--------|------------|--------------|----------------|
| 按钮A（上一组/上一项） | BTN_A  | D1         | GPIO5        |                |
| 按钮B（下一组/下一项） | BTN_B  | D2         | GPIO4        |                |
| 按钮C（切换模式）      | BTN_C  | D3         | GPIO0        |                |
| OLED屏幕(ST7920接口)  | -      | D4         | GPIO2        | SPI数据线（DIN）|
| OLED屏幕(ST7920接口)  | -      | 默认HW SPI | SCK=GPIO14, MOSI=GPIO13 | 使用硬件SPI接口 |

### 具体连接说明

1. **按钮A**  
   - NodeMCU D1 (GPIO5) — 连接到按钮A的一端  
   - 另一端接地（GND）
   - 此引脚设置为 `INPUT_PULLUP`，建议按钮与GND之间接线

2. **按钮B**  
   - NodeMCU D2 (GPIO4) — 连接到按钮B的一端  
   - 另一端接地（GND）
   - 此引脚设置为 `INPUT_PULLUP`，建议按钮与GND之间接线

3. **按钮C**  
   - NodeMCU D3 (GPIO0) — 连接到按钮C的一端  
   - 另一端接地（GND）
   - 此引脚设置为 `INPUT_PULLUP`，建议按钮与GND之间接线

4. **OLED 屏幕 (ST7920, 硬件SPI模式)**
   - `D4` (GPIO2)：数据/命令选择线（U8g2库构造函数第2参数）
   - SCK：`D5` (GPIO14) — SPI时钟线
   - MOSI：`D7` (GPIO13) — SPI数据线
   - GND：接地
   - VCC：接3.3V或5V（根据你的屏幕要求）
   - 其他脚（如CS、RST），依据实际屏幕模块接法与库用法连接

### 代码中的相关部分

```c++
#define BTN_A D1    // GPIO5
#define BTN_B D2    // GPIO4
#define BTN_C D3    // GPIO0
U8G2_ST7920_128X64_1_HW_SPI u8g2(
  U8G2_R0,
  D4,                // GPIO2，数据/命令线
  U8X8_PIN_NONE      // 没有复位脚
);
// SPI接口自动使用NodeMCU硬件SPI引脚（D5=CLK，D7=MOSI）
```

### 总结接线表

| 模块    | NodeMCU引脚 | ESP8266 GPIO | 说明           |
|---------|-------------|--------------|----------------|
| 按钮A   | D1          | GPIO5        | 一端接D1，一端接GND |
| 按钮B   | D2          | GPIO4        | 一端接D2，一端接GND |
| 按钮C   | D3          | GPIO0        | 一端接D3，一端接GND |
| OLED D/C| D4          | GPIO2        | 数据/命令选择  |
| OLED SCK| D5          | GPIO14       | SPI时钟        |
| OLED MOSI| D7         | GPIO13       | SPI数据        |
| OLED GND| GND         | -            | 接地           |
| OLED VCC| 3.3V或5V    | -            | 电源           |
