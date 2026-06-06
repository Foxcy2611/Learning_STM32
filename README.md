# 🚀 Hướng Dẫn Lập Trình STM32F103 với SPL (Standard Peripheral Library)

Mục tiêu của series này giúp nắm vững bản chất kiến trúc lõi ARM Cortex-M3, hiểu cách cấu hình các ngoại vi thông qua thư viện SPL và thực hành 1 số project.

---

## 🛠️ Công Cụ Yêu Cầu (Prerequisites)
Cần chuẩn bị:

### 1. Phần cứng (Hardware)
*   Mạch phát triển: **STM32F103C8T6 (Blue Pill)** .
*   Mạch nạp: **ST-Link V2**.
*   Các module thực hành: LED, Nút bấm, MPU6050, Module UART (CP2102/FT232), Motor Driver.

### 2. Phần mềm & Môi trường (Software & Environment)
*   **Truyền thống:** Keil C MDK-ARM (ARM Compiler V5).

---

## 📚 Lộ Trình (Syllabus)

### Phần 1: Khởi động & Nhập môn
*   **Bài 1:** Giới thiệu cấu trúc vi điều khiển STM32F103 & Lõi ARM Cortex-M3.
*   **Bài 2:** Setup môi trường lập trình (Keil C & VS Code/CMake). Tạo project SPL đầu tiên từ con số 0.
*   **Bài 3:** Hiểu về Clock Tree trong STM32. Cấu hình System Clock.

### Phần 2: Ngoại vi cơ bản (Basic Peripherals)
*   **Bài 4:** GPIO Output - Điều khiển nhấp nháy LED, cấu hình Push-Pull / Open-Drain.
*   **Bài 5:** GPIO Input - Đọc nút bấm, xử lý rung phím (Debounce).
*   **Bài 6:** Ngắt ngoài (EXTI) & Bộ điều khiển ngắt lồng nhau (NVIC).

### Phần 3: Timer & Analog
*   **Bài 7:** Timer cơ bản (SysTick Timer & General Purpose Timer). Tạo hàm Delay và Millis chính xác.
*   **Bài 8:** PWM (Pulse Width Modulation) - Điều khiển độ sáng LED, xuất xung điều khiển động cơ.
*   **Bài 9:** ADC (Analog-to-Digital Converter) - Đọc giá trị cảm biến Analog.
*   **Bài 10:** DMA (Direct Memory Access) - Tăng tốc độ truyền dữ liệu ADC mà không tốn CPU.

### Phần 4: Các chuẩn giao tiếp (Communication Protocols)
*   **Bài 11:** UART/USART - Giao tiếp máy tính, in dữ liệu debug.
*   **Bài 12:** I2C - Giao tiếp với cảm biến góc nghiêng (ví dụ: MPU6050).
*   **Bài 13:** SPI - Giao tiếp tốc độ cao (Module LoRa, thẻ nhớ SD, màn hình).

### Phần 5: Xây dựng các Project

---
