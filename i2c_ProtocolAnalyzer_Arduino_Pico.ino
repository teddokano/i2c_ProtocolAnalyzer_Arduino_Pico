#include <stdarg.h>

constexpr int SDA_PIN = 0;
constexpr int SCL_PIN = 1;

int prev_sda = 1;
int prev_scl = 1;

#define CAPTURE_LENGTH 10

enum pa_status : int {
  FREE = 0,
  START,
};

typedef struct {
  uint8_t data;
  uint8_t ack;
} data_ack;

typedef struct {
  data_ack data_byte[64];
  int length;
} transactions;

transactions tr[64];

inline void pin_state_change(int sda, int startstop = false);
void zprintf(const char *format, ...);

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;

  zprintf("Hello, world!");

  pinMode(SDA_PIN, INPUT_PULLUP);
  pinMode(SCL_PIN, INPUT_PULLUP);
  pinMode(2, OUTPUT);
}
#define SAMPLINF_MONITOR_PERIOD 0xF
void loop() {
  int all;
  int sda;
  int scl;
  int count = 0;
  int toggle = false;
  while (true) {

#if 0
    sda = gpio_get(SDA_PIN);
    scl = gpio_get(SCL_PIN);
#else
    all = gpio_get_all();
    sda = all & 0x01;
    scl = (all >> 1) & 0x1;
#endif

    if ((prev_sda != sda) && (prev_scl && scl))
      pin_state_change(sda, true);
    else if (!prev_scl && scl)
      pin_state_change(sda, false);

    prev_sda = sda;
    prev_scl = scl;

    if (!(count & SAMPLINF_MONITOR_PERIOD))
      gpio_put(2, toggle = !toggle);


    count++;
  }
}

inline void pin_state_change(int sda, int ss) {
  static pa_status state = FREE;
  static int bit_count;
  static int byte_count;
  static int transaction_count = 0;
  static transactions *t;

  if (ss) {
    if (sda) {
      state = FREE;
      transaction_count++;

      zprintf("tr: %2d\n", transaction_count);

      if (CAPTURE_LENGTH < transaction_count) {
        show_transactions(CAPTURE_LENGTH);
        transaction_count = 0;
      }

    } else {
      if (FREE != state)
        transaction_count++;

      state = START;
      bit_count = 0;
      byte_count = 0;

      t = tr + transaction_count;
    }
    return;
  }

  if (START == state) {
    switch (bit_count) {
      case 8:
        t->data_byte[byte_count].ack = sda;
        byte_count++;
        t->length = byte_count;
        break;
      case 0:
        t->data_byte[byte_count].data = (sda & 0x1) << 7;
        break;
      default:
        t->data_byte[byte_count].data |= (sda & 0x1) << (7 - bit_count);
        break;
    }
    bit_count++;
    bit_count %= 9;
  }
}

void show_transactions(int length) {
  transactions *t;
  data_ack *addr;

  for (int i = 0; i < length; i++) {
    t = tr + i;
    zprintf("#%2d (%2d) : [S]", i, t->length - 1);


    addr = &(t->data_byte[0]);

    //    zprintf(" 0x%02X(%02X)%c[%c]", addr->data & ~0x01, addr->data >> 1, (addr->data) & 0x01 ? 'R' : 'W', addr->ack ? 'N' : 'A');
    zprintf(" 0x%02X-%c[%c]", addr->data & ~0x01, (addr->data) & 0x01 ? 'R' : 'W', addr->ack ? 'N' : 'A');

    for (int j = 1; j < t->length; j++)
      zprintf(" 0x%02X[%c]", t->data_byte[j].data, t->data_byte[j].ack ? 'N' : 'A');

    zprintf(" [P]\n");
  }
}

#define MAX_STR_LENGTH 120
void zprintf(const char *format, ...) {
  char s[MAX_STR_LENGTH];
  va_list args;

  va_start(args, format);
  vsnprintf(s, MAX_STR_LENGTH, format, args);
  va_end(args);

  Serial.print(s);
}
