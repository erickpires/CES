// --- Variable types defined by length ---
typedef char int8;
typedef byte uint8;
typedef short int16;
typedef unsigned short uint16;
typedef long int32;
typedef unsigned long uint32;

#define MAX_END 16383

#define nop() __asm__ volatile("nop\n\t")

// Saidas: VEB
// Entradas: CHE

// Distribuição das portas
//
// * Via de Enderecos    = PORTF e PORTK = Pins[A0..A7] e Pins[A8..A15]
// * Chaves de Enderecos = PORTC e PORTL = Pins[37..30] e Pins[49..42]

// * Clock = PORTE4 = Pins[2]
// * SaiBD = PORTE5 = Pins[3]
// * SaiRE = PORTG5 = Pins[4]
// * SaiBE = PORTE3 = Pins[5]
// * EscM  = PORTH3 = Pins[6]
// * UltC  = PORTH4 = Pins[7]

static uint16 valor_lido;

void reporta_diff(uint16 expected, uint16 received) {
    char buff[8];
    uint16 diff = expected ^ received;
    sprintf(buff, "0x%04x", diff);

    uint8 popcnt = 0;
    uint8 mask = 0x01;
    for(uint8 _ = 0; _ < 16; _++) {
        if(diff & mask) { popcnt++; }

        mask = mask << 1;
    }

    Serial.print("\t\tDiff: ");
    Serial.print(buff);
    Serial.print(" PopCnt: ");
    Serial.println(popcnt);
}

void desativa_sai_bd() {
    PORTE |= (1 << 5);
    nop();
}

void desativa_esc_m() {
    PORTH |= (1 << 3);
    nop();
}

void desativa_sai_re() {
    PORTG |= (1 << 5);
    nop();
}

void desativa_sai_be() {
    PORTE |= (1 << 3);
    nop();
}

void controle_escrita() {
    DDRE |= (1 << 4);
    DDRE |= (1 << 5);
    DDRG |= (1 << 5);
    DDRE |= (1 << 3);
    DDRH |= (1 << 3);
    DDRH |= (1 << 4);
}

void che_escrita() {
    DDRC = 0xff;
    DDRL = 0xff;
}

void veb_leitura() {
    DDRF = 0x00;
    DDRK = 0x00;
}

void setup() {
    che_escrita();
    veb_leitura();

    controle_escrita();

    desativa_sai_bd();
    desativa_esc_m();
    desativa_sai_re();
    desativa_sai_be();

    Serial.begin(9600);
}

void ativa_sai_be() {
    PORTE &= ~(1 << 3);
    nop();
}

void escreve_chave_de_enderecos(uint16 valor) {
    uint8 lower_byte = valor;
    uint8 upper_byte = valor >> 8;

    PORTC = lower_byte;
    PORTL = upper_byte;

    nop();
}

uint16 le_veb() {
    nop();

    uint8 lower_byte = PINF;
    uint8 upper_byte = PINK;

    return (upper_byte << 8) | lower_byte;
}

bool checa_veb(uint16 valor) {
    valor_lido = le_veb();

    return valor == valor_lido;
}

bool testa_be() {
    ativa_sai_be();

    for(uint16 i = 0; i <= MAX_END; i++) {
        escreve_chave_de_enderecos(i);

        bool result = checa_veb(i);
        if(!result) {
            Serial.print("Falha ao checar BE com: ");
            Serial.println(i);
            Serial.print("Valor lido: ");
            Serial.println(valor_lido);
            reporta_diff(i, valor_lido);

            desativa_sai_be();
            return 0;
        }
    }

    desativa_sai_be();
    return 1;
}

void loop() {
    delay(500);

    bool result;

    Serial.print("Testando BE... ");
    result = testa_be();
    if(!result) { return; }
    Serial.println("Ok");
}
