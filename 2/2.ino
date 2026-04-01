// 定義接腳
const int btn1 = 22;         
const int btn2 = 23;         
const int com1 = 30;         
const int com2 = 31;         
const int segments[] = {38, 39, 40, 41, 42, 43, 44}; // A, B, C, D, E, F, G
const int dotPin = 45;       // DP (小數點)

// 共陽極 0-9 編碼 (0為亮, 1為暗)
const byte numbers[10][7] = {
  {0,0,0,0,0,0,1}, {1,0,0,1,1,1,1}, {0,0,1,0,0,1,0}, {0,0,0,0,1,1,0}, {1,0,0,1,1,0,0},
  {0,1,0,0,1,0,0}, {0,1,0,0,0,0,0}, {0,0,0,1,1,1,1}, {0,0,0,0,0,0,0}, {0,0,0,0,1,0,0}
};

// 狀態變數
bool show1 = false;          
bool show2 = false;          
int count1 = 0;              // 0 -> 9
int count2 = 9;              // 9 -> 0

int lastBtn1 = LOW;
int lastBtn2 = LOW;

unsigned long lastUpdate1 = 0;
unsigned long lastUpdate2 = 0;
const int interval = 500;    // 0.5秒換一次數字

void setup() {
  pinMode(btn1, INPUT);
  pinMode(btn2, INPUT);
  pinMode(com1, OUTPUT);
  pinMode(com2, OUTPUT);
  pinMode(dotPin, OUTPUT);

  // 初始化：PNP HIGH 為關閉
  digitalWrite(com1, HIGH);
  digitalWrite(com2, HIGH);
  digitalWrite(dotPin, HIGH); // 共陽極 HIGH 為不亮

  for (int i = 0; i < 7; i++) {
    pinMode(segments[i], OUTPUT);
    digitalWrite(segments[i], HIGH);
  }
}

void loop() {
  int cur1 = digitalRead(btn1);
  int cur2 = digitalRead(btn2);

  // 1. 同時按下判斷：全暗
  if (cur1 == HIGH && cur2 == HIGH) {
    show1 = false;
    show2 = false;
  } 
  else {
    // 按鈕 1：由低變高時，重新從 0 開始數
    if (cur1 == HIGH && lastBtn1 == LOW) {
      delay(20); // 去彈跳
      show1 = true;
      count1 = 0;
      lastUpdate1 = millis();
    }
    // 按鈕 2：由低變高時，重新從 9 開始數
    if (cur2 == HIGH && lastBtn2 == LOW) {
      delay(20); // 去彈跳
      show2 = true;
      count2 = 9;
      lastUpdate2 = millis();
    }
  }
  lastBtn1 = cur1;
  lastBtn2 = cur2;

  // 2. 計數邏輯
  unsigned long now = millis();
  if (show1 && (now - lastUpdate1 >= interval)) {
    count1 = (count1 + 1) % 10;
    lastUpdate1 = now;
  }
  if (show2 && (now - lastUpdate2 >= interval)) {
    count2 = count2 - 1;
    if (count2 < 0) count2 = 9;
    lastUpdate2 = now;
  }

  // 3. 掃描顯示 (Multiplexing)
  if (show1) {
    displayNum(com1, count1);
    delay(5); // 停留 5ms
  }
  
  if (show2) {
    displayNum(com2, count2);
    delay(5); // 停留 5ms
  }

  // 若都沒開啟則確保關閉
  if (!show1 && !show2) {
    digitalWrite(com1, HIGH);
    digitalWrite(com2, HIGH);
  }
}

// 顯示單一數字函式
void displayNum(int comPin, int num) {
  // 消隱 (關閉所有 COM 避免殘影)
  digitalWrite(com1, HIGH);
  digitalWrite(com2, HIGH);

  // 設置段位
  for (int i = 0; i < 7; i++) {
    digitalWrite(segments[i], numbers[num][i]);
  }
  
  // 第二顆(以及所有)的 dot 永不亮
  digitalWrite(dotPin, HIGH); 

  // 開啟目標位數 (PNP 給 LOW)
  digitalWrite(comPin, LOW);
}
