/*
 *  Constants
 */

constexpr int SDA_PIN = 0;
constexpr int SCL_PIN = 1;
constexpr int MONITOR_PIN = 2;

constexpr int TRANSACTION_CAPTURE_LENGTH = 64;
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

transaction tr[TRANSACTION_CAPTURE_LENGTH];

int prev_sda = 1;
int prev_scl = 1;
int total_count = 0;

/*
 *  Prototypes
 */

inline void pin_state_change(int sda, int startstop = false);
void show_transactions(int length);

/*
 *  Program body
 */

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;

  Serial.printf("I2C protocol analyzer started\n");
  Serial.printf("  transaction captureing depth    = %6d\n", TRANSACTION_CAPTURE_LENGTH);
  Serial.printf("  transaction maximum byte length = %6d\n", TRANSACTION_MAX_BYTE_LENGTH);
  Serial.printf("  memory size for data capturing  = %6d\n", sizeof(tr));

  pinMode(SDA_PIN, INPUT_PULLUP);
  pinMode(SCL_PIN, INPUT_PULLUP);
  pinMode(MONITOR_PIN, OUTPUT);

  Serial.printf("[%d] captureing %d transactions\n", total_count++, CAPTURE_LENGTH);
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
      gpio_put(MONITOR_PIN, (toggle = !toggle));

    count++;
  }
}

inline void pin_state_change(int sda, int ss) {
  static pa_status state = FREE;
  static int bit_count;
  static int byte_count;
  static int transaction_count = 0;
  static transaction *t;
  static int repeated_start_flag = 0;

  if (ss) {
    if (sda) {
      state = FREE;

      (tr + transaction_count)->stop = true;

      transaction_count++;

      //      Serial.printf("tr: %2d\n", transaction_count);

      if (CAPTURE_LENGTH < transaction_count) {
        show_transactions(CAPTURE_LENGTH);
        transaction_count = 0;
        Serial.printf("[%d] captureing %d transactions\n", total_count++, CAPTURE_LENGTH);
      }

    } else {
      if (FREE != state) {
        transaction_count++;
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

void show_transactions(int length) {
  transaction *t;
  data_ack *addr;

  for (int i = 0; i < length; i++) {
    t = tr + i;
    addr = &(t->data_byte[0]);

    Serial.printf("#%2d (%2d) : [%c]", i, t->length - 1, t->repeated_start ? 'R' : 'S');
    Serial.printf(" 0x%02X-%c[%c]", addr->data & ~0x01, (addr->data) & 0x01 ? 'R' : 'W', addr->ack ? 'N' : 'A');

    for (int j = 1; j < t->length; j++)
      Serial.printf(" 0x%02X[%c]", t->data_byte[j].data, t->data_byte[j].ack ? 'N' : 'A');

    Serial.printf("%s\n", t->stop ? " [P]" : "");
  }
}
