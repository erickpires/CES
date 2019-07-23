// --- Variable types defined by length ---
typedef char int8;
typedef byte uint8;
typedef short int16;
typedef unsigned short uint16;
typedef long int32;
typedef unsigned long uint32;

#define nop() __asm__ volatile("nop\n\t")

// Distribuição das portas
//
// * Via de Enderecos = PORTC e PORTL = Pins[37..30] e Pins[49..42]
// * Via de Dados     = PORTF e PORTK = Pins[A0..A7] e Pins[A8..A15]

// * Clock  = PORTE4 = Pins[2]  (saída)
// * SaiBD\ = PORTE5 = Pins[3]  (saída)
// * SaiRE\ = PORTG5 = Pins[4]  (saída)
// * SaiBE\ = PORTE3 = Pins[5]  (saída)
// * EscM\  = PORTH3 = Pins[6]  (saída)
// * UltC\  = PORTH4 = Pins[7]  (saída)
// * ZComp  = PORTH5 = Pins[8]  (saída)
// * SaiBS\ = PORTH6 = Pins[9]  (saída)
// * MaisUm = PORTB4 = Pins[10] (saída)
// * VaiUm  = PORTB5 = Pins[11] (entrada)
// * SaiRP\ = PORTB6 = Pins[12] (saída)
// * EscRP\ = PORTB7 = Pins[13] (saída)
// * EscT\  = PORTJ1 = Pins[14] (saída)


void clock() {
    PORTE |= (1 << 4);
    nop();

    PORTE &= ~(1 << 4);
    nop();
}

void desativa_sai_bd() {
    PORTE |= (1 << 5);
    nop();
}

void desativa_esc_m() {
    PORTH |= (1 << 3);
    nop();
}

void desativa_sai_bt() {
    desativa_esc_m();
}

void desativa_sai_re() {
    PORTG |= (1 << 5);
    nop();
}

void desativa_sai_be() {
    PORTE |= (1 << 3);
    nop();
}

void desativa_sai_bs() {
    PORTH |= (1 << 6);
    nop();
}

void desativa_sai_rp() {
    PORTB |= (1 << 6);
    nop();
}

void desativa_esc_rp() {
    PORTB |= (1 << 7);
    nop();
}

void desativa_esc_t() {
    PORTJ |= (1 << 1);
}

void ativa_sai_re() {
    PORTG &= ~(1 << 5);
    nop();
}

void ativa_sai_be() {
    PORTE &= ~(1 << 3);
    nop();
}

void ativa_sai_bd() {
    PORTE &= ~(1 << 5);
    nop();
}

void ativa_sai_bs() {
    PORTH &= ~(1 << 6);
    nop();
}

void desativa_z_comp() {
    PORTH &= ~(1 << 5);
}

void ativa_z_comp() {
    PORTH |= (1 << 5);
    nop();
}

void ativa_esc_m() {
    PORTH &= ~(1 << 3);
    nop();
}

void ativa_sai_bt() { // a.k.a. EscM
    ativa_esc_m();
}

void ativa_sai_rp() {
    PORTB &= ~(1 << 6);
}

void ativa_esc_rp() {
    PORTB &= ~(1 << 7);
}

void ativa_esc_t() {
    PORTJ &= ~(1 << 1);
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

void desativa_sai_rd() {
    return ult_c_um();
}

void mais_um(uint8 valor) {
    if(valor) {
        PORTB |= (1 << 4);
    } else {
        PORTB &= ~(1 << 4);
    }
}

void vdb_escrita() {
    DDRF = 0xff;
    DDRK = 0xff;
    nop();
}

void vdb_leitura() {
    DDRF = 0x00;
    DDRK = 0x00;
    nop();
}

void veb_leitura() {
    DDRC = 0x00;
    DDRL = 0x00;
}

void setup_controle() {
    DDRE |= (1 << 4);
    DDRE |= (1 << 5);

    DDRG |= (1 << 5);

    DDRE |= (1 << 3);

    DDRH |= (1 << 3);
    DDRH |= (1 << 4);
    DDRH |= (1 << 5);
    DDRH |= (1 << 6);

    DDRB |= (1 << 3);
    DDRB |= (1 << 4);
    DDRB &= ~(1 << 5);
    DDRB |= (1 << 6);
    DDRB |= (1 << 7);

    DDRJ |= (1 << 1);
}

void setup() {
    vdb_leitura();
    veb_leitura();

    setup_controle();

    desativa_sai_bd();
    desativa_sai_be();
    desativa_sai_re();
    desativa_sai_bs();
    desativa_esc_m();

    ult_c_zero();

    desativa_z_comp();
    mais_um(0);

    desativa_sai_rp();
    desativa_esc_rp();

    desativa_esc_t();

    Serial.begin(9600);
}

uint16 le_veb() {
    nop();

    uint8 lower_byte = PINC;
    uint8 upper_byte = PINL;

    upper_byte &= 0x3f;

    return (upper_byte << 8) | lower_byte;
}


bool checa_estabilidade_che(uint16 valor) {
    for(uint8 i = 0; i < 255; i++) {
        uint16 result = le_veb();
        if(result != valor) {
            return false;
        }

        nop();
        nop();
        nop();
        nop();
    }

    return true;
}

void testa_chave_enderecos() {
    ativa_sai_be();

    uint16 valor_esperado = 0x0001;

    for(uint8 i = 0; i <= 14; i++) {
        uint16 result;

        while((result = le_veb()) != valor_esperado) {
            char buff[8];
            sprintf(buff, "%04x", valor_esperado);
            Serial.print("Insira o valor 0x");
            Serial.print(buff);
            Serial.print(" nas chaves de enderecos (valor lido 0x");

            sprintf(buff, "%04x", result);
            Serial.print(buff);
            Serial.println(")");
        }

        if(!checa_estabilidade_che(valor_esperado)) {
            Serial.println("Falha de estabilidade");
            goto END;
        }

        valor_esperado = (valor_esperado << 1) & 0x3fff;
    }

END:
    desativa_sai_be();
}

uint16 le_vdb() {
    nop();

    uint8 lower_byte = PINF;
    uint8 upper_byte = PINK;

    return (upper_byte << 8) | lower_byte;
}

void escreve_vdb(uint16 valor) {
    uint8 lower_byte = valor;
    uint8 upper_byte = valor >> 8;

    PORTF = lower_byte;
    PORTK = upper_byte;

    nop();
}

bool checa_estabilidade_vdb(uint16 valor) {
    for(uint8 i = 0; i < 255; i++) {
        uint16 result = le_vdb();
        if(result != valor) {
            return false;
        }

        nop();
        nop();
        nop();
        nop();
    }

    return true;
}

void testa_chave_de_dados_via_bd() {
    vdb_leitura();
    ativa_sai_bd();

    uint16 valor_esperado = 0x0001;

    for(uint8 i = 0; i <= 16; i++) {
        uint16 result;

        while((result = le_vdb()) != valor_esperado) {
            char buff[8];
            sprintf(buff, "%04x", valor_esperado);
            Serial.print("Insira o valor 0x");
            Serial.print(buff);
            Serial.print(" nas chaves de dados (valor lido 0x");

            sprintf(buff, "%04x", result);
            Serial.print(buff);
            Serial.println(")");
        }

        if(!checa_estabilidade_vdb(valor_esperado)) {
            Serial.println("Falha de estabilidade");
            goto END;
        }

        valor_esperado = (valor_esperado << 1) & 0xffff;
    }

END:
    desativa_sai_bd();
}

uint16 le_rt() {
    nop();

    clock();

    return le_vdb();
}

bool checa_estabilidade_rt(uint16 valor) {
    for(uint8 i = 0; i < 255; i++) {
        uint16 result = le_rt();
        if(result != valor) {
            return false;
        }

        nop();
        nop();
        nop();
        nop();
    }

    return true;
}

void testa_chave_de_dados_via_bs() {
    vdb_leitura();
    desativa_sai_rd();

    ativa_sai_bs();
    ativa_esc_t();
    ativa_sai_bt();

    ativa_z_comp();
    mais_um(0);

    uint16 valor_esperado = 0x0001;

    for(uint8 i = 0; i <= 16; i++) {
        uint16 result;

        while((result = le_rt()) != valor_esperado) {
            char buff[8];
            sprintf(buff, "%04x", valor_esperado);
            Serial.print("Insira o valor 0x");
            Serial.print(buff);
            Serial.print(" nas chaves de dados (valor lido 0x");

            sprintf(buff, "%04x", result);
            Serial.print(buff);
            Serial.println(")");
        }

        if(!checa_estabilidade_rt(valor_esperado)) {
            Serial.println("Falha de estabilidade");
            goto END;
        }

        valor_esperado = (valor_esperado << 1) & 0xffff;
    }

END:
    desativa_sai_bs();
    desativa_sai_bs();
    desativa_esc_t();
    desativa_sai_bt();

    desativa_z_comp();

}

void testa_chaves() {
    testa_chave_enderecos();

    testa_chave_de_dados_via_bd();

    testa_chave_de_dados_via_bs();
}

void incrementa_rp() {
    vdb_leitura();

    ult_c_um();
    ativa_z_comp();
    mais_um(0);

    ativa_sai_rp();
    ativa_sai_re();

    ativa_esc_t();
    ativa_sai_bt();

    uint16 rp_atual = le_rt();

    ativa_esc_rp();

    mais_um(1);

    while(true) {
        mais_um(0);
        desativa_esc_rp();
        nop();
        clock();
        uint16 rp_anterior = le_vdb();
        ativa_esc_rp();
        mais_um(1);

        clock();
        uint16 result = le_vdb() & 0x3fff;
        uint16 veb_atual = le_veb();

        rp_atual = (rp_atual + 1) & 0x3fff;

        if(result != rp_atual) {
            char buff[8];
            sprintf(buff, "%04x", result);
            Serial.print("RP: 0x");
            Serial.print(buff);

            Serial.print(" (deveria ser 0x");
            sprintf(buff, "%04x", rp_atual);
            Serial.print(buff);

            Serial.print(" (vbe= 0x");
            sprintf(buff, "%04x", veb_atual);
            Serial.print(buff);

            Serial.print(" (rp_ant= 0x");
            sprintf(buff, "%04x", rp_anterior);
            Serial.print(buff);

            Serial.println("))");
            break;
        // } else {
        //     char buff[8];
        //     sprintf(buff, "%04x", result);
        //     Serial.print("Ok 0x");
        //     Serial.println(buff);
        }
    }

END:
    desativa_sai_rp();
    desativa_esc_rp();
    desativa_sai_re();

    desativa_esc_t();
    desativa_sai_bt();
}

void testa_rd() {
    ativa_z_comp();

    desativa_sai_bt();

    uint16 counter = 0;
    while(true) {
        uint8 coin = rand() % 2;

        if(coin) {
            mais_um(1);
        } else {
            mais_um(0);
        }

        ult_c_um();
        vdb_escrita();

        escreve_vdb(counter);
        clock();

        ult_c_zero();
        nop();

        ativa_esc_t();
        clock();

        desativa_esc_t();
        ult_c_um();
        vdb_leitura();

        ativa_sai_bt();
        nop();

        uint16 result = le_vdb();
        uint16 expected = counter + coin;

        desativa_sai_bt();


        if(result != expected) {
            Serial.print(expected);
            Serial.print(" != ");
            Serial.println(result);
        } else {
            delay(10);
        }

        counter++;
    }
}

void testa_rt() {
    uint8 error = 0;
    uint16 no_error_counter = 0;
    uint16 counter = 3;
    while(true) {
        ativa_z_comp();
        desativa_sai_bt();

        mais_um(0);

        ult_c_um();
        vdb_escrita();

        escreve_vdb(~counter);
        clock();

        ult_c_zero();
        nop();

        escreve_vdb(0);

        ativa_esc_t();
        // clock();

        // desativa_esc_t();
        clock();

        // while(error);

        ativa_esc_t();
        desativa_z_comp();
        nop();

        clock();

        desativa_esc_t();
        ult_c_um();
        vdb_leitura();

        ativa_sai_bt();
        nop();

        uint16 result = le_vdb();
        uint16 expected = counter;

        desativa_sai_bt();


        if(result != expected) {
            char buff[8];
            sprintf(buff, "0x%04x", expected);
            Serial.print(buff);
            Serial.print(" != ");
            sprintf(buff, "0x%04x", result);
            Serial.print(buff);
            Serial.print(" (no error for: )");
            Serial.println(no_error_counter);

            no_error_counter = 0;
            error = 1;
        } else {
            no_error_counter++;
            // Serial.println("Ok");
            // delay(10);
        }

        counter++;
    }
}

void loop() {
    // testa_chaves();

    while(true) {
        incrementa_rp();
        // testa_rt();
        // testa_rd();
    }
}
