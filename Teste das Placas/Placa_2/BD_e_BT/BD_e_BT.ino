// --- Variable types defined by length ---
typedef char int8;
typedef byte uint8;
typedef short int16;
typedef unsigned short uint16;
typedef long int32;
typedef unsigned long uint32;

#define nop() __asm__ volatile("nop\n\t")

// Saidas: VDB
// Entradas: SRT e CHD

// Distribuição das portas
//
// * Chaves de Dados  = PORTF e PORTK = Pins[A0..A7] e Pins[A8..A15]
// * Via de Dados     = PORTA e PORTB = Pins[22..29] e Pins[53..50],Pins[10..13]
// * Saída de RT      = PORTC e PORTL = Pins[37..30] e Pins[49..42]

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

void vdb_leitura() {
    DDRA = 0x00;
    DDRB = 0x00;
    nop();
}

void chd_escrita() {
    DDRF = 0xff;
    DDRK = 0xff;
    nop();
}

void srt_escrita() {
    DDRC = 0xff;
    DDRL = 0xff;
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

void setup() {
    vdb_leitura();
    chd_escrita();
    srt_escrita();

    controle_escrita();

    desativa_sai_bd();
    desativa_esc_m();
    desativa_sai_re();
    desativa_sai_be();

    Serial.begin(9600);
}

void ativa_sai_bd() {
    PORTE &= ~(1 << 5);
    nop();
}

void ativa_esc_m() {
    PORTH &= ~(1 << 3);
    nop();
}

void escreve_chave_de_dados(uint16 valor) {
    uint8 lower_byte = valor;
    uint8 upper_byte = valor >> 8;

    PORTF = lower_byte;
    PORTK = upper_byte;

    nop();
}

void escreve_srt(uint16 valor) {
    uint8 lower_byte = valor;
    uint8 upper_byte = valor >> 8;

    PORTC = lower_byte;
    PORTL = upper_byte;

    nop();
}

uint16 le_vdb() {
    nop();

    uint8 lower_byte = PINA;
    uint8 upper_byte = PINB;

    return (upper_byte << 8) | lower_byte;
}

bool checa_vdb(uint16 valor) {
    valor_lido = le_vdb();

    return valor == valor_lido;
}

bool testa_bd() {
    ativa_sai_bd();

    escreve_chave_de_dados(0);

    bool result = checa_vdb(0);

    if(!result) {
        Serial.print("Falha ao checar BT com: ");
        Serial.println(0);
        Serial.print("Valor lido: ");
        Serial.println(valor_lido);
        reporta_diff(0, valor_lido);

        desativa_sai_bd();
        return 0;
    }

    for(uint16 i = 1; i != 0; i++) {
        escreve_chave_de_dados(i);

        result = checa_vdb(i);
        if(!result) {
            Serial.print("Falha ao checar BT com: ");
            Serial.println(i);
            Serial.print("Valor lido: ");
            Serial.println(valor_lido);
            reporta_diff(i, valor_lido);

            desativa_sai_bd();
            return 0;
        }
    }

    desativa_sai_bd();
    return 1;
}

bool testa_bt() {
    ativa_esc_m();

    escreve_srt(0);

    bool result = checa_vdb(0);

    if(!result) {
        Serial.print("Falha ao checar BT com: ");
        Serial.println(0);
        Serial.print("Valor lido: ");
        Serial.println(valor_lido);
        reporta_diff(0, valor_lido);

        desativa_esc_m();
        return 0;
    }

    for(uint16 i = 1; i != 0; i++) {
        escreve_srt(i);

        result = checa_vdb(i);
        if(!result) {
            Serial.print("Falha ao checar BT com: ");
            Serial.println(i);
            Serial.print("Valor lido: ");
            Serial.println(valor_lido);
            reporta_diff(i, valor_lido);

            desativa_esc_m();
            return 0;
        }
    }

    desativa_esc_m();
    return 1;
}

void loop() {
    delay(500);

    bool result;

    Serial.print("Testando BD... ");
    result = testa_bd();
    if(!result) { return; }
    Serial.println("Ok");

    Serial.print("Testando BT... ");
    result = testa_bt();
    if(!result) { return; }
    Serial.println("Ok");
}
