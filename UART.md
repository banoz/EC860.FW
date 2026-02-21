# UART Interface

## Hardware

| Pin  | Function | Description |
|------|----------|-------------|
| P1_4 | TXD0     | UART TX     |
| P1_5 | RXD0     | UART RX     |

## Serial Configuration

| Parameter | Value              |
|-----------|--------------------|
| Mode      | UART, 8N1          |
| Baud rate | 57600 bps          |
| Bit order | LSB first          |
| BRG value | 0x15 (21)          |
| Clock     | f1 = 20 MHz        |
| Formula   | f1 / 16 / (BRG+1) |

Baud rate = 20,000,000 / 16 / 22 = 56,818 bps (≈ 57,600).

## Interrupts

UART0 transmit and receive interrupts are enabled at priority level 1.

| Vector | Function             |
|--------|----------------------|
| 17     | UART0 TX interrupt   |
| 18     | UART0 RX interrupt   |

Received bytes are placed into a 32-byte ring buffer by the RX interrupt.
Transmitted bytes are pulled from a 32-byte ring buffer by the TX interrupt.

## Packet Format

Both command (RX) and response (TX) packets share the same framing:

```
| 0xAA | Byte 0 | Byte 1 | Byte 2 | Byte 3 | Byte 4 | Checksum |
```

- **Start byte**: `0xAA` — must not appear in the payload.
- **Checksum**: XOR of bytes 0 – 4.

### Command Packet (RX)

Sent to the device to control outputs.

| Byte | Field          | Type          | Description               |
|------|----------------|---------------|---------------------------|
| 0    | pump           | unsigned char | Pump control              |
| 1    | valves         | unsigned char | Valve control             |
| 2    | target_temp_c  | unsigned char | Target coffee temperature |
| 3    | target_temp_s  | unsigned char | Target steam temperature  |
| 4    | leds           | unsigned char | LED control               |
| 5    | checksum       | unsigned char | XOR of bytes 0 – 4        |

### Response Packet (TX)

Sent back after a valid command is received.

| Byte | Field    | Type          | Description                       |
|------|----------|---------------|-----------------------------------|
| 0    | millis   | unsigned char | Timestamp (wraps at 255)          |
| 1    | temp_c   | unsigned char | Coffee boiler temperature (raw)   |
| 2    | temp_s   | unsigned char | Steam boiler temperature (raw)    |
| 3    | pressure | signed char   | Pressure reading                  |
| 4    | switches | unsigned char | Switch states                     |
| 5    | checksum | unsigned char | XOR of bytes 0 – 4                |

## Communication Flow

1. Host sends a command packet (start byte + 5 data bytes + checksum).
2. Firmware validates the checksum.
3. On success the received fields are applied to the system state and a response packet is transmitted with the current sensor readings.
4. Invalid packets (bad checksum or overflow) are silently discarded.
