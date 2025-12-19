#define LED_PIN 8

void setup() {
  // Configure pin as output using C
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  uint32_t bitmask = 1 << LED_PIN;

  // Turn LED ON (set bit)
  asm volatile (
    "li t0, %[addr]\n"     // Load address of GPIO_OUT_W1TS_REG in t0
    "lw t1, %[mask]\n"     // Load bitmask in t1
    "sw t1, 0(t0)\n"       // Store mask to address
    :                      // Return values
    : [addr] "i" (GPIO_OUT_W1TS_REG), [mask] "m" (bitmask) // Variables: [addr] is an immediate constant, [mask] is a memory operand
    : "t0", "t1"           // Registers used
  );

  delay(500);

  // Turn LED OFF (clear bit)
  asm volatile (
    "li t0, %[addr]\n"
    "lw t1, %[mask]\n"
    "sw t1, 0(t0)\n"
    :
    : [addr] "i" (GPIO_OUT_W1TC_REG), [mask] "m" (bitmask)
    : "t0", "t1"
  );

  delay(500);
}
