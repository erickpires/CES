// --- Variable types defined by length ---

typedef char int8;
typedef byte uint8;
typedef short int16;
typedef unsigned short uint16;
typedef long int32;
typedef unsigned long uint32;
typedef uint8 bool;

//
// Distribuição das portas
//
// * Chaves de Dados  = PORTF e PORTK = Pins[A0..A7] e Pins[A8..A15]
// * Via de Dados     = PORTA e PORTB = Pins[22..29] e Pins[53..50],Pins[10..13]
// * Saída do Somador = PORTC e PORTL = Pins[37..30] e Pins[49..42]

// * Clock            = PORTE4 = Pins[2]
// * SaiBufferChaves  = PORTE5 = Pins[3]
// * SaiRD            = PORTG5 = Pins[4]
// * SaiRT            = PORTE3 = Pins[5]
// * SaiRP            = PORTH3 = Pins[6]
// * ZComp            = PORTH4 = Pins[7]
// * MaisUm           = PORTH5 = Pins[8]
// * VaiUm            = PORTH6 = Pins[9]

#define nop() __asm__ volatile("nop\n\t")

static bool chaves_ativadas = false;
static bool rp_ativado      = false;
static bool rd_ativado      = false;

static volatile uint16 valor_lido;

void clock() {
    PORTE |= (1 << 4);
    nop();

    PORTE &= ~(1 << 4);
    nop();
}

void ativa_chave_de_dados() {
    if(rp_ativado || rd_ativado) {
        Serial.println("Mais de uma entrada ativa ao mesmo tempo.");
        while(true);
    }

    PORTE &= ~(1 << 5);
    chaves_ativadas = true;

    nop();
}

void desativa_chave_de_dados() {
    PORTE |= (1 << 5);
    chaves_ativadas = false;

    nop();
}

void escreve_chave_de_dados(uint16 valor) {
    uint8 lower_byte = valor;
    uint8 upper_byte = valor >> 8;

    PORTF = lower_byte;
    PORTK = upper_byte;

    nop();
}

void ativa_zcomp() {
    PORTH |= (1 << 4);

    nop();
}

void desativa_zcomp() {
    PORTH &= ~(1 << 4);

    nop();
}

void ativa_mais_um() {
    PORTH |= (1 << 5);

    nop();
}

void desativa_mais_um() {
    PORTH &= ~(1 << 5);

    nop();
}

void ativa_rd() {
    if(rp_ativado || chaves_ativadas) {
        Serial.println("Mais de uma entrada ativa ao mesmo tempo.");
        while(true);
    }

    PORTG &= ~(1 << 5);
    rd_ativado = true;

    nop();
}

void desativa_rd() {
    PORTG |= (1 << 5);
    rd_ativado = false;

    nop();
}

void escreve_em_rd(uint16 valor) {
    uint8 lower_byte = valor;
    uint8 upper_byte = valor >> 8;

    PORTA = lower_byte;
    PORTB = upper_byte;
    nop();

    clock();

    nop();
}

void ativa_rp() {
    if(chaves_ativadas || rd_ativado) {
        Serial.println("Mais de uma entrada ativa ao mesmo tempo.");
        while(true);
    }

    PORTH &= ~(1 << 3);
    rp_ativado = true;

    nop();
}

void desativa_rp() {
    PORTH |= (1 << 3);
    rp_ativado = false;

    nop();
}

void ativa_rt() {
    PORTE |= (1 << 3);

    nop();
}

uint16 le_somador() {
    nop();

    uint8 lower_byte = PINC;
    uint8 upper_byte = PINL;

    return (upper_byte << 8) | lower_byte;
}

uint8 le_vai_um() {
    return (PINH & (1 << 6));
}

bool checa_somador(uint16 valor) {
    valor_lido = le_somador();

    return valor == valor_lido;
}

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

bool testa_zero() {
    escreve_chave_de_dados(0);
    ativa_zcomp();
    desativa_mais_um();
    ativa_chave_de_dados();

    bool result = checa_somador(0);
    if(!result) {
        Serial.println("Falha ao testar zero pelas chaves de dados.");
        reporta_diff(0, valor_lido);
    }

    desativa_chave_de_dados();
    return result;
}

bool testa_chaves_de_dados() {
    ativa_zcomp();
    desativa_mais_um();
    ativa_chave_de_dados();

    for(uint16 i = 1; i != 0; i++) {
        escreve_chave_de_dados(i);

        bool result = checa_somador(i);
        if(!result) {
            Serial.print("Falha ao testar chaves de dados com: ");
            Serial.println(i);
            Serial.print("Valor lido: ");
            Serial.println(valor_lido);
            reporta_diff(i, valor_lido);

            desativa_chave_de_dados();

            return 0;
        }
    }

    desativa_chave_de_dados();
    return 1;
}

bool testa_mais_um() {
    ativa_zcomp();
    ativa_mais_um();
    ativa_chave_de_dados();

    uint16 i = 0;
    do {
        escreve_chave_de_dados(i);

        bool result = checa_somador(i + 1);
        if(!result) {
            Serial.print("Falha ao testar mais um com: ");
            Serial.println(i);
            Serial.print("Valor lido: ");
            Serial.println(valor_lido);
            reporta_diff(i, valor_lido);

            desativa_chave_de_dados();
            return 0;
        }
    } while(++i != 0);

    desativa_chave_de_dados();
    return 1;
}


bool testa_rd() {
    ativa_zcomp();
    desativa_mais_um();
    ativa_rd();

    uint16 i = 0;
    do {
        escreve_em_rd(i);
        bool result = checa_somador(i);
        if(!result) {
            Serial.print("Falha ao testar RD com: ");
            Serial.println(i);
            Serial.print("Valor lido: ");
            Serial.println(valor_lido);
            reporta_diff(i, valor_lido);

            desativa_rd();
            return false;
        }

    } while(++i != 0);

    desativa_rd();
    return true;
}

bool testa_rp() {
    ativa_zcomp();
    desativa_mais_um();

    escreve_chave_de_dados(0);
    ativa_chave_de_dados();
    clock();
    desativa_chave_de_dados();


    ativa_rp();
    bool result = checa_somador(0);
    if(!result) {
        Serial.print("Falha ao checar RP com: ");
        Serial.println(0);
        Serial.print("Valor lido: ");
        Serial.println(valor_lido);
        reporta_diff(0, valor_lido);

        //        while(true);

        desativa_rp();
        return false;
    }

    ativa_mais_um();
    result = checa_somador(1);
    if(!result) {
        Serial.println("Falha ao checar MaisUm");
        reporta_diff(1, valor_lido);

        desativa_rp();
        return false;
    }

    for(uint16 i = 1; i < 0x4000; i++) {
        nop();
        nop();
        bool result = checa_somador(i);
        if(!result) {
            Serial.print("Falha ao checar RP com: ");
            Serial.println(i);
            Serial.print("Valor lido: ");
            Serial.println(valor_lido);
            reporta_diff(i, valor_lido);

            // while(true);

            desativa_rp();
            return false;
        }

        clock();
    }

    desativa_rp();
    return true;
}

bool testa_rt() {
    desativa_mais_um();
    ativa_zcomp();
    escreve_chave_de_dados(0);
    ativa_chave_de_dados();

    clock();

    desativa_zcomp();
    ativa_mais_um();

    bool result = checa_somador(0);
    if(!result) {
        Serial.print("Falha ao checar RT com: ");
        Serial.println(0);
        Serial.print("Valor lido: ");
        Serial.println(valor_lido);
        reporta_diff(0, valor_lido);

        desativa_chave_de_dados();
        return 0;
    }

    for(uint16 i = 1; i != 0; i++) {
        desativa_mais_um();
        ativa_zcomp();
        escreve_chave_de_dados(i);

        clock();

        escreve_chave_de_dados(0);
        desativa_zcomp();
        ativa_mais_um();

        uint16 esperado = 0 - i;
        result = checa_somador(esperado);
        if(!result) {
            Serial.print("Falha ao testar RT com: ");
            Serial.println(i);
            Serial.print("Valor lido: ");
            Serial.println(valor_lido);
            reporta_diff(i, valor_lido);

            desativa_chave_de_dados();
            return 0;
        }
    }

    desativa_chave_de_dados();
    return 1;
}

bool testa_somadores() {
    desativa_mais_um();
    ativa_chave_de_dados();

    for(uint8 somador = 0; somador < 4; somador++) {
        for(uint16 direito = 0; direito < 16; direito++) {
            uint16 lado_direito = direito << (4 * somador);

            ativa_zcomp();
            escreve_chave_de_dados(~(lado_direito));
            clock();

            desativa_zcomp();

            for(uint16 esquerdo = 0; esquerdo < 16; esquerdo++) {
                uint16 lado_esquerdo = esquerdo << (4 * somador);

                escreve_chave_de_dados(lado_esquerdo);

                uint16 lido = le_somador();
                uint8 vai_um = le_vai_um() != 0;

                uint16 expected = (lado_esquerdo + lado_direito);
                uint8  expected_vai_um = lado_esquerdo > (0xffff - lado_direito);

                if(lido != expected || vai_um != expected_vai_um) {
                    Serial.print(lado_esquerdo);
                    Serial.print(" + ");
                    Serial.print(lado_direito);
                    Serial.print(" = ");
                    Serial.print(lido);
                    Serial.print('(');
                    Serial.print(vai_um);
                    Serial.print(") == ");
                    Serial.print(expected);
                    Serial.print('(');
                    Serial.print(expected_vai_um);
                    Serial.println(')');
                    reporta_diff(expected, valor_lido);

                    desativa_chave_de_dados();
                    return false;
                }
            }
        }
    }

    desativa_chave_de_dados();
    return true;
}

void setup() {
    Serial.begin(9600);

    DDRF = 0xff;
    DDRK = 0xff;

    DDRA = 0xff;
    DDRB = 0xff;

    DDRC = 0x00;
    DDRL = 0x00;

    DDRE |= (1 << 4);
    DDRE |= (1 << 5);
    DDRG |= (1 << 5);
    DDRE |= (1 << 3);
    DDRH |= (1 << 3);
    DDRH |= (1 << 4);
    DDRH |= (1 << 5);
    DDRH &= ~(1 << 6);

    ativa_rt();
    desativa_zcomp();
    desativa_chave_de_dados();
    desativa_rp();
    desativa_rd();
    desativa_mais_um();
    clock();
}

void loop() {
    delay(2000);

    bool result;

    Serial.print("Testando Zero... ");
    result = testa_zero();
    if(!result) { return; }
    Serial.println("Ok");

    Serial.print("Testando Chaves de Dados... ");
    result = testa_chaves_de_dados();
    if(!result) { return; }
    Serial.println("Ok");

    Serial.print("Testando Mais Um... ");
    result = testa_mais_um();
    if(!result) { return; }
    Serial.println("Ok");


    Serial.print("Testando RD... ");
    result = testa_rd();
    if(!result) { return; }
    Serial.println("Ok");

    /*
    */
    Serial.print("Testando RP... ");
    result = testa_rp();
    if(!result) { return; }
    Serial.println("Ok");
    Serial.print("Testando RT... ");
    result = testa_rt();
    if(!result) { return; }
    Serial.println("Ok");

    /*
    */
    Serial.print("Testando Somadores...");
    result = testa_somadores();
    if(!result) { return; }
    Serial.println("Ok");

}
