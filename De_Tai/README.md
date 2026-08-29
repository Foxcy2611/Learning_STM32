# DANH SÁCH ĐỀ TÀI BÀI TẬP LỚN
## Vi điều khiển STM32F103 — Ngoại vi cơ bản

**Ngoại vi đã học:** GPIO, NVIC, ADC, TIMER, DMA, SPI, I2C, FLASH, UART

---

### Thông tin nhóm mẫu

| Trường | Nội dung |
|---|---|
| GROUP_NAME | Embedded Team |
| MEMBERS | Van An, Van Hung, Van Long |
| PROJECT_NAME | Smart Light System |
| HARDWARE_USED | STM32, BH1750, LCD16x2 |
| SOFTWARE_USED | Keil, Cmisis Library |
| DATE | 2025-03-25 |

---

## Đề 1: Cảnh báo vật cản / đo khoảng cách (HC-SR04)

- **Ngoại vi STM32 sử dụng:** TIMER (delay µs + đếm polling), GPIO, I2C, FLASH, NVIC
- **Module sử dụng:** HC-SR04 (siêu âm) + LCD1602 (I2C)
- **Nội dung yêu cầu:**
  - Phát xung Trigger 10µs bằng GPIO (dùng Timer tạo delay chính xác µs)
  - Đo độ rộng xung Echo bằng polling: bật Timer đếm free-running khi Echo lên mức cao, đọc giá trị Timer khi Echo xuống mức thấp, suy ra khoảng cách
  - Hiện khoảng cách lên LCD, cập nhật liên tục
  - Buzzer/LED cảnh báo khi khoảng cách nhỏ hơn ngưỡng an toàn
  - Ngưỡng cảnh báo được thiết lập và lưu vào FLASH nội — đọc lại ngưỡng đã lưu mỗi khi khởi động, không cần thiết lập lại từ đầu

## Đề 2: Đo nhiệt độ - độ ẩm cảnh báo (DHT11)

- **Ngoại vi STM32 sử dụng:** GPIO (1-wire), TIMER, I2C, FLASH, NVIC
- **Module sử dụng:** DHT11 + LCD1602 (I2C)
- **Nội dung yêu cầu:**
  - Đọc nhiệt độ, độ ẩm từ DHT11 mỗi 2 giây, hiện lên LCD
  - Đặt ngưỡng cảnh báo nhiệt độ tối đa qua nút GPIO, lưu ngưỡng vào FLASH
  - LED/còi bật khi vượt ngưỡng, LCD hiện "CẢNH BÁO"

## Đề 3: Điểm danh bằng thẻ RFID (RC522)

- **Ngoại vi STM32 sử dụng:** SPI, GPIO, TIMER, FLASH, UART, NVIC
- **Module sử dụng:** RFID RC522 + LCD1602 (I2C)
- **Nội dung yêu cầu:**
  - Đọc UID thẻ qua SPI khi quẹt thẻ
  - So sánh UID với danh sách tối đa ~10 UID hợp lệ lưu trong FLASH
  - LCD hiện "Hợp lệ" kèm số thứ tự, hoặc "Từ chối" nếu UID lạ
  - Gửi log qua UART dạng: `UID: xx xx xx xx - OK/FAIL` mỗi lần quẹt

## Đề 4: Máy tính cầm tay đơn giản (Keypad + LCD)

- **Ngoại vi STM32 sử dụng:** GPIO, TIMER, I2C, NVIC
- **Module sử dụng:** Keypad 8x8 + LCD1602 (I2C)
- **Nội dung yêu cầu:**
  - Quét phím ma trận 4x4 lấy số (0-9) và phép toán (+, -, *, /)
  - Hỗ trợ phép tính 2 toán hạng 1 chữ số, dạng `a [op] b =`
  - LCD dòng 1 hiện biểu thức đang nhập (VD: 5+3), dòng 2 hiện kết quả khi nhấn =
  - Phím C dùng để xóa, nhập lại phép tính mới

## Đề 5: Cảnh báo góc nghiêng (MPU6050)

- **Ngoại vi STM32 sử dụng:** I2C, TIMER, UART, NVIC
- **Module sử dụng:** MPU6050 + LED 7 đoạn (74HC595 - SPI)
- **Nội dung yêu cầu:**
  - Đọc Accel X/Y/Z qua I2C mỗi 200ms bằng ngắt Timer
  - Tính góc nghiêng xấp xỉ (atan2 hoặc so sánh biên độ trục), hiện 1 chữ số làm tròn lên LED 7 đoạn
  - Khi góc nghiêng vượt ngưỡng đặt trước, gửi cảnh báo chi tiết (giá trị chính xác) qua UART

## Đề 6: Lịch vạn niên (RTC ngoài DS3231)

- **Ngoại vi STM32 sử dụng:** I2C, GPIO, NVIC
- **Module sử dụng:** DS1307 + LCD1602 (I2C) + LED
- **Nội dung yêu cầu:**
  - Đọc ngày/giờ thực từ chip DS1307 qua I2C
  - LCD hiện đồng thời: giờ:phút:giây (dòng 1) và ngày/tháng/năm + thứ (dòng 2)
  - Định kỳ: Kết thúc 10s đếm blink 1 led ngoài

## Đề 7: Cảnh báo mức ánh sáng, ghi log (LDR)

- **Ngoại vi STM32 sử dụng:** ADC, DMA, TIMER, FLASH, UART, NVIC
- **Module sử dụng:** Biến trở + LED 7 đoạn 4 số
- **Nội dung yêu cầu:**
  - Biến trở mô phỏng module đọc giá trị ánh sáng
  - Lấy mẫu ADC giá trị ánh sáng liên tục qua DMA
  - Mỗi 5 giây ghi 1 giá trị log vào FLASH dạng bộ đệm vòng (circular buffer, giữ tối đa ~50 mẫu gần nhất)
  - LED 7 đoạn hiện mức sáng hiện tại

---

## Yêu cầu thêm về thông tin người làm

**Thiết lập UART:** Cấu hình UART cho STM32F103C8 nhận lệnh

- **Tên lệnh:** `INFO_REQUEST`
- **Yêu cầu:** Trả về thông tin gửi lên HERCULES bao gồm:
  - `Name`: Tên người thực hiện
  - `Project Name`: Tên dự án
  - `Hardware Used`: Tên danh sách phần cứng sử dụng
  - `Software Used`: Phần mềm, thư viện liên quan sử dụng
  - `Date`: Ngày hoàn thành dự án

**Ví dụ:**

```
-> INFO_REQUEST

<= Name: ...
   Project Name: ...
   Hardware Used: ...
   Software Used: ...
   Date: ...
```

> **Note:** Bất kỳ kỹ thuật nào để thu chuỗi ký tự: Ký tự kết thúc, Ring Buffer, …
