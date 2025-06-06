#include <U8g2lib.h>
#include <qrcode.h>
#include <pgmspace.h>

#define BTN_A D1
#define BTN_B D2
#define BTN_C D3
U8G2_ST7920_128X64_1_HW_SPI u8g2(
  U8G2_R0,
  D4,
  U8X8_PIN_NONE
);

#define QR_VERSION 1
#define MODULE_SIZE 3
#define QR_PREFIX "KUWEI-"
#define DEBOUNCE_DELAY 30       // 修改为更灵敏的防抖
#define SHORT_PRESS 300         // 新增短按阈值
#define LONG_PRESS 400          // 新增长按阈值
#define FAST_INTERVAL 1        // 新增快速触发间隔

enum SystemMode {
  GROUP_SELECT,
  QR_DISPLAY
};

struct MessageGroup {
  char text[12];
  uint16_t code;
};

// 以下为各组数据，部分组只列出示例
const PROGMEM MessageGroup messagesA[] = {
 {"A1-1",11028},
{"A1-2",11029},
{"A1-3",11030},
{"A1-4",11031},
{"A1-5",11032},
{"A1-6",11033},
{"A1-7",11034},
{"A1-8",11035},
{"A1-9",11036},
{"A1-10",11037},
{"A2-1",11038},
{"A2-2",11039},
{"A2-3",11040},
{"A2-4",11041},
{"A2-5",11042},
{"A2-6",11043},
{"A2-7",11044},
{"A2-8",11045},
{"A2-9",11046},
{"A2-10",11047},
{"A3-1",11048},
{"A3-2",11049},
{"A3-3",11050},
{"A3-4",11051},
{"A3-5",11052},
{"A3-6",11053},
{"A3-7",11054},
{"A3-8",11055},
{"A3-9",11056},
{"A3-10",11057},
{"A4-1",11058},
{"A4-2",11059},
{"A4-3",11060},
{"A4-4",11061},
{"A4-5",11062},
{"A4-6",11063},
{"A4-7",11064},
{"A4-8",11065},
{"A4-9",11066},
{"A4-10",11067},
{"A5-1",11068},
{"A5-2",11069},
{"A5-3",11070},
{"A5-4",11071},
{"A5-5",11072},
{"A5-6",11073},
{"A5-7",11074},
{"A5-8",11075},
{"A5-9",11076},
{"A5-10",11077},
{"A6-1",11078},
{"A6-2",11079},
{"A6-3",11080},
{"A6-4",11081},
{"A6-5",11082},
{"A6-6",11083},
{"A6-7",11084},
{"A6-8",11085},
{"A6-9",11086},
{"A6-10",11087},
{"A7-1",11088},
{"A7-2",11089},
{"A7-3",11090},
{"A7-4",11091},
{"A7-5",11092},
{"A7-6",11094},
{"A7-7",11095},
{"A7-8",11096},
{"A7-9",11097},
{"A7-10",11098},
{"A8-1",11099},
{"A8-2",11100},
{"A8-3",11101},
{"A8-4",11102},
{"A8-5",11103},
{"A8-6",11104},
{"A8-7",11105},
{"A8-8",11106},
{"A8-9",11107},
{"A8-10",11108},
{"A9-1",11109},
{"A9-2",11110},
{"A9-3",11111},
{"A9-4",11112},
{"A9-5",11113},
{"A9-6",11114},
{"A9-7",11115},
{"A9-8",11116},
{"A9-9",11117},
{"A9-10",11118},
{"A10-1",11875},
{"A10-2",11877},
{"A10-3",11878},
{"A10-4",11879},
{"A10-5",11880},
{"A10-6",11881},
{"A10-8",11882},
{"A10-7",11883},
{"A10-9",11929},
{"A10-10",11930}
};
const PROGMEM MessageGroup messagesB[] = {{"B1-1",11464}, {"B1-2",11465}, {"B1-3",11466}};
const PROGMEM MessageGroup messagesC[] = {{"C1-1",11028}, {"C1-2",11029}, {"C1-3",11030}};
const PROGMEM MessageGroup messagesD[] = {{"D1-1",11028}, {"D1-2",11029}, {"D1-3",11030}};
const PROGMEM MessageGroup messagesE[] = {{"E1-1",11028}, {"E1-2",11029}, {"E1-3",11030}};
const PROGMEM MessageGroup messagesF[] = {{"F1-1",11028}, {"F1-2",11029}, {"F1-3",11030}};
const PROGMEM MessageGroup messagesG[] = {{"G1-1",11028}, {"G1-2",11029}, {"G1-3",11030}};
const PROGMEM MessageGroup messagesH[] = {{"H1-1",11028}, {"H1-2",11029}, {"H1-3",11030}};

// 组管理结构
const MessageGroup* allGroups[] = {messagesA, messagesB, messagesC, messagesD, 
                                  messagesE, messagesF, messagesG, messagesH};
const uint8_t groupSizes[] = {
  sizeof(messagesA)/sizeof(MessageGroup),
  sizeof(messagesB)/sizeof(MessageGroup),
  sizeof(messagesC)/sizeof(MessageGroup),
  sizeof(messagesD)/sizeof(MessageGroup),
  sizeof(messagesE)/sizeof(MessageGroup),
  sizeof(messagesF)/sizeof(MessageGroup),
  sizeof(messagesG)/sizeof(MessageGroup),
  sizeof(messagesH)/sizeof(MessageGroup)
};

SystemMode currentMode = GROUP_SELECT;
uint8_t groupIndex = 0;
uint8_t currentIndex = 0;
unsigned long lastDebounce = 0;
unsigned long buttonAPressTime = 0;  // 新增按钮时间跟踪
unsigned long buttonBPressTime = 0;
bool buttonAActive = false;          // 新增按钮状态跟踪
bool buttonBActive = false;

#define READ_FLASH_STRUCT(dest, src) memcpy_P(&dest, &src, sizeof(MessageGroup))

void setup() {
  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);
  pinMode(BTN_C, INPUT_PULLUP);
  digitalWrite(BTN_C, HIGH);
  
  u8g2.begin();
  u8g2.setContrast(180);
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_wqy14_t_gb2312b);
}

// 传入步进值step，短按为1，长按为3
void handleButtonAction(bool isIncrement, uint8_t step = 1) {
  if(currentMode == GROUP_SELECT) {
    groupIndex = isIncrement ? 
      (groupIndex + step) % 8 : 
      (groupIndex - step + 8) % 8;
  } else {
    uint8_t size = groupSizes[groupIndex];
    currentIndex = isIncrement ? 
      (currentIndex + step) % size : 
      (currentIndex - step + size) % size;
  }
}

void handleButtons() {
  bool a = !digitalRead(BTN_A);
  bool b = !digitalRead(BTN_B);
  bool c = !digitalRead(BTN_C);
  unsigned long now = millis();

  // 处理按钮A
  if(a && !buttonAActive) {
    buttonAPressTime = now;
    buttonAActive = true;
  }
  if(!a && buttonAActive) {
    buttonAActive = false;
    if(now - buttonAPressTime < SHORT_PRESS) {
      handleButtonAction(true, 1);   // 短按+1
    }
  }

  // 处理按钮B
  if(b && !buttonBActive) {
    buttonBPressTime = now;
    buttonBActive = true;
  }
  if(!b && buttonBActive) {
    buttonBActive = false;
    if(now - buttonBPressTime < SHORT_PRESS) {
      handleButtonAction(false, 1);  // 短按-1
    }
  }

  // 长按加速处理
  if(buttonAActive && (now - buttonAPressTime > LONG_PRESS)) {
    if((now - lastDebounce) > FAST_INTERVAL) {
      handleButtonAction(true, 3);   // 长按+3
      lastDebounce = now;
    }
  }
  if(buttonBActive && (now - buttonBPressTime > LONG_PRESS)) {
    if((now - lastDebounce) > FAST_INTERVAL) {
      handleButtonAction(false, 3);  // 长按-3
      lastDebounce = now;
    }
  }

  // 按钮C逻辑不变
  if(c && (now - lastDebounce) > DEBOUNCE_DELAY) {
    if(currentMode == GROUP_SELECT) {
      currentMode = QR_DISPLAY;
      currentIndex = 0;
    } else {
      currentMode = GROUP_SELECT;
    }
    lastDebounce = now;
  }
}

void drawGroupSelect() {
  u8g2.firstPage();
  do {
    u8g2.clearBuffer();
    
    u8g2.setFont(u8g2_font_wqy14_t_gb2312b);
    const char* title = "库区选择";
    int titleWidth = u8g2.getUTF8Width(title);
    u8g2.drawUTF8((128 - titleWidth)/2, 16, title);

    u8g2.setFont(u8g2_font_10x20_mf);
    char groupChar = 'A' + groupIndex; // 自动扩展到H
    char groupStr[2] = {groupChar, '\0'};
    int charWidth = u8g2.getStrWidth(groupStr);
    int baseX = (128 - charWidth) / 2;
    u8g2.drawStr(baseX, 45, groupStr);
    
    int symbolOffset = 20;
    u8g2.drawStr(baseX - symbolOffset, 45, "<");
    u8g2.drawStr(baseX + charWidth + symbolOffset - 10, 45, ">");
    
  } while(u8g2.nextPage());
}

void drawQRDisplay() {
  const MessageGroup* currentGroup = nullptr;
  char groupChar = 'A' + groupIndex;

  switch(groupIndex) {
    case 0: currentGroup = messagesA; break;
    case 1: currentGroup = messagesB; break;
    case 2: currentGroup = messagesC; break;
    case 3: currentGroup = messagesD; break;
    case 4: currentGroup = messagesE; break;
    case 5: currentGroup = messagesF; break;
    case 6: currentGroup = messagesG; break;
    case 7: currentGroup = messagesH; break;
  }

  MessageGroup currentItem;
  READ_FLASH_STRUCT(currentItem, currentGroup[currentIndex]);

  char qrContent[20];
  snprintf(qrContent, sizeof(qrContent), "%s%u", QR_PREFIX, currentItem.code);

  QRCode qrcode;
  uint8_t qrBuffer[qrcode_getBufferSize(QR_VERSION)];
  qrcode_initText(&qrcode, qrBuffer, QR_VERSION, ECC_MEDIUM, qrContent);

  u8g2.firstPage();
  do {
    u8g2.clearBuffer();
    
    // 绘制二维码
    for(uint8_t y=0; y<qrcode.size; y++){
      for(uint8_t x=0; x<qrcode.size; x++){
        if(qrcode_getModule(&qrcode, x, y)){
          u8g2.drawBox(x*MODULE_SIZE, y*MODULE_SIZE, MODULE_SIZE, MODULE_SIZE);
        }
      }
    }

    // 显示当前text
    char displayText[12];
    strncpy_P(displayText, currentItem.text, sizeof(displayText));
    displayText[sizeof(displayText)-1] = '\0';
    
    u8g2.setFont(u8g2_font_7x13B_mf);
    int textWidth = u8g2.getStrWidth(displayText);
    u8g2.drawStr(128 - textWidth - 2, 15, displayText);
   
  } while(u8g2.nextPage());
}

void loop() {
  handleButtons();
  currentMode == GROUP_SELECT ? drawGroupSelect() : drawQRDisplay();
  delay(50);
}
