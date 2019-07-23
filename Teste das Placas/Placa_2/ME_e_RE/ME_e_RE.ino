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
// Entradas:VDB e SS

// Distribuição das portas
//
// * Via de Enderecos = PORTF e PORTK = Pins[A0..A7] e Pins[A8..A15]
// * Via de Dados     = PORTA e PORTB = Pins[22..29] e Pins[53..50],Pins[10..13]
// * Saída do Somador = PORTC e PORTL = Pins[37..30] e Pins[49..42]

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

void vdb_escrita() {
    DDRA = 0xff;
    DDRB = 0xff;
    nop();
}

void ss_escrita() {
    DDRC = 0xff;
    DDRL = 0xff;
}

void veb_leitura() {
    DDRF = 0x00;
    DDRK = 0x00;
}

void setup() {
    vdb_escrita();
    ss_escrita();
    veb_leitura();

    controle_escrita();

    desativa_sai_bd();
    desativa_esc_m();
    desativa_sai_re();
    desativa_sai_be();


    Serial.begin(9600);
}

void clock() {
    PORTE |= (1 << 4);
    nop();

    PORTE &= ~(1 << 4);
    nop();
}

void ativa_sai_re() {
    PORTG &= ~(1 << 5);
    nop();
}

void ult_c_zero() {
    PORTH &= ~(1 << 4);
    nop();
}

void ult_c_um() {
    PORTH |= (1 << 4);
    nop();
}

void escreve_vdb(uint16 valor) {
    uint8 lower_byte = valor;
    uint8 upper_byte = valor >> 8;

    PORTA = lower_byte;
    PORTB = upper_byte;

    nop();
}

void escreve_ss(uint16 valor) {
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

    upper_byte &= 0x3f;

    return (upper_byte << 8) | lower_byte;
}

bool checa_veb(uint16 valor) {
    valor_lido = le_veb();

    return valor == valor_lido;
}

bool testa_me() {
    ativa_sai_re();

    for(uint16 i = 0; i <= MAX_END; i++) {
        uint16 complemento = MAX_END - i;

        escreve_vdb(i);
        escreve_ss(complemento);

        ult_c_zero();
        nop();
        clock();

        bool result = checa_veb(i);
        if(!result) {
            Serial.print("Falha ao checar ME com: ");
            Serial.println(i);
            Serial.print("Valor lido: ");
            Serial.println(valor_lido);
            Serial.println("UltC: 0");
            reporta_diff(i, valor_lido);

            Serial.println("\tEntrada por VDB");

            desativa_sai_re();
            return 0;
        }

        ult_c_um();
        nop();
        clock();

        result = checa_veb(complemento);
        if(!result) {
            Serial.print("Falha ao checar ME com: ");
            Serial.println(complemento);
            Serial.print("Valor lido: ");
            Serial.println(valor_lido);
            Serial.println("UltC: 1");
            reporta_diff(complemento, valor_lido);

            Serial.println("\tEntrada por SS");

            desativa_sai_re();
            return 0;
        }
    }

    desativa_sai_re();
    return 1;
}

void loop() {
    delay(500);

    bool result;

    Serial.print("Testando ME... ");
    result = testa_me();
    if(!result) { return; }
    Serial.println("Ok");

}
