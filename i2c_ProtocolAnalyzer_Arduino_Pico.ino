/*
 *  Constants
 */

constexpr int SDA_PIN = 0;
constexpr int SCL_PIN = 1;
constexpr int VITAL0_PIN = 2;
constexpr int VITAL1_PIN = 3;

constexpr int TRANSACTION_BUFFER_DEPTH = 10;
constexpr int TRANSACTION_MAX_BYTE_LENGTH = 128;
constexpr int CAPTURE_LENGTH = 10;

enum pa_status : int {
  FREE = 0,
  START,
};

/*
 *  Allocating static (global) memory for data capturing
 */

typedef struct {
  uint8_t data;
  uint8_t ack;
} data_ack;

typedef struct {
  data_ack data_byte[TRANSACTION_MAX_BYTE_LENGTH];
  int length;
  int repeated_start;
  int stop;
} transaction;

transaction tr[TRANSACTION_BUFFER_DEPTH];
volatile int transaction_count = 0;

volatile int captured = 0;

int prev_sda = 1;
int prev_scl = 1;
int total_count = 0;

/*
 *  Prototypes
 */

inline void pin_state_change(int sda, int startstop = false);
void show_transactions(int length);

#define SAMPLINF_MONITOR_PERIOD 0xF

/*
 *  Program body
 */

void setup() {
  Serial.begin(115200);  // baudrate setting is ignored : https://arduino-pico.readthedocs.io/en/latest/serial.html
  while (!Serial)
    ;

  Serial.printf("I2C protocol analyzer started\n");
  Serial.printf("  transaction captureing depth    = %6d\n", TRANSACTION_BUFFER_DEPTH);
  Serial.printf("  transaction maximum byte length = %6d\n", TRANSACTION_MAX_BYTE_LENGTH);
  Serial.printf("  memory size for data capturing  = %6d\n", sizeof(tr));

  Serial.printf("[%d] captureing transactions\n", total_count++);

#if 0
  core1_main();
#else
  multicore_launch_core1(core1_main);
#endif

  pinMode(VITAL1_PIN, OUTPUT);

  int toggle = 0;
  int count = 0;
  while (1) {
    show_transactions(captured);

    if (!(count++ & SAMPLINF_MONITOR_PERIOD))
      gpio_put(VITAL1_PIN, (toggle = !toggle));
  }
}


void loop() {
}

void core1_main() {
  int all;
  int sda;
  int scl;
  int count = 0;
  int toggle = false;

  pinMode(SDA_PIN, INPUT_PULLUP);
  pinMode(SCL_PIN, INPUT_PULLUP);
  pinMode(VITAL0_PIN, OUTPUT);

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

    if (!(count++ & SAMPLINF_MONITOR_PERIOD))
      gpio_put(VITAL0_PIN, (toggle = !toggle));
  }
}

inline void pin_state_change(int sda, int ss) {
  static pa_status state = FREE;
  static int bit_count;
  static int byte_count;
  static transaction *t;
  static int repeated_start_flag = 0;

  if (ss) {
    if (sda) {
      state = FREE;

      (tr + transaction_count)->stop = true;

      captured = transaction_count;

      transaction_count++;
      transaction_count %= TRANSACTION_BUFFER_DEPTH;

    } else {
      if (FREE != state) {
        transaction_count++;
        transaction_count %= TRANSACTION_BUFFER_DEPTH;

        (tr + transaction_count)->repeated_start = true;
      } else {
        (tr + transaction_count)->repeated_start = false;
      }

      state = START;
      bit_count = 0;
      byte_count = 0;

      t = tr + transaction_count;
      t->stop = false;
    }
    return;
  }

  if (START == state) {
    switch (bit_count) {
      case 8:
        t->data_byte[byte_count].ack |= sda;
        byte_count++;
        t->length = byte_count;
        break;
      case 0:
        t->data_byte[byte_count].data = (sda & 0x1) << 7;
        t->data_byte[byte_count].ack = repeated_start_flag;
        break;
      default:
        t->data_byte[byte_count].data |= (sda & 0x1) << (7 - bit_count);
        break;
    }
    bit_count++;
    bit_count %= 9;
  }
}

void show_transactions(int captured) {
  static int shown = -1;
  static uint32_t tr_num = 0;
  transaction *t;
  data_ack *addr;

  if (shown == captured)
    return;

  int length = ((captured + TRANSACTION_BUFFER_DEPTH) - shown) % TRANSACTION_BUFFER_DEPTH;

  for (int i = 0; i < length; i++) {
    int t_index = ((shown + i) % TRANSACTION_BUFFER_DEPTH);
    t = tr + t_index;
    addr = &(t->data_byte[0]);

    Serial.printf("#%5d (%2d) : [%c]", tr_num, t->length - 1, t->repeated_start ? 'R' : 'S');
    Serial.printf(" 0x%02X-%c[%c]", addr->data & ~0x01, (addr->data) & 0x01 ? 'R' : 'W', addr->ack ? 'N' : 'A');

    for (int j = 1; j < t->length; j++)
      Serial.printf(" 0x%02X[%c]", t->data_byte[j].data, t->data_byte[j].ack ? 'N' : 'A');

    Serial.printf("%s\n", t->stop ? " [P]" : "");

    tr_num++;
  }

  shown = captured;
}
