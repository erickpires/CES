// --- Variable types defined by length ---
typedef char int8;
typedef byte uint8;
typedef short int16;
typedef unsigned short uint16;
typedef long int32;
typedef unsigned long uint32;

// Vetor de leitura:
// (Desvia, EscM, UltC, EscP, EscT, EscC, ZComp, Mais1, EntRlg, Rlg\,
// Rlg1, Rlg2, Rlg3, BotParteAtivo, BotParteAtivo\, VaiPartir, VaiPartir,
// VaiParar, EscMem, SaiBD, SaiBE, SaiBS, SaiRE, RlgRI, RlgRC).

// Vetor de escrita:
// (I0, I1, RC, Para, Est1, Parte, MudaP, MudaT, BotParte, Partiu,
// IntPara, BotEscMem, EntRlg\)

// Distribuição das portas:
// * Vetor de leitura: PORTC e PORTL e PORTA e PORTB = (Pins[37..30] e Pins[49..42]) e (Pins[22..29] e Pins[53..50],Pins[10..13])
// * Vetor de escrita: PORTF e PORTK = Pins[A0..A7] e Pins[A8..A15]

// * Clock  = PORTE4 = Pins[2]  (saída)

#define nop() __asm__ volatile("nop\n\t")

void clock() {
    PORTE |= (1 << 4);
    nop();
    nop();

    PORTE &= ~(1 << 4);
    nop();
    nop();
}

bool read_desvia(uint32 values) {
    return (values & (1L << 0)) != 0;
}

bool read_esc_m(uint32 values) {
    return (values & (1L << 1)) != 0;
}

bool read_ult_c(uint32 values) {
    bool result = (values & (1L << 2)) != 0;
    return result;
}

bool read_esc_p(uint32 values) {
    return (values & (1L << 3)) != 0;
}

bool read_esc_t(uint32 values) {
    return (values & (1L << 4)) != 0;
}

bool read_esc_c(uint32 values) {
    return (values & (1L << 5)) != 0;
}

bool read_z_comp(uint32 values) {
    return (values & (1L << 6)) != 0;
}

bool read_mais_1(uint32 values) {
    return (values & (1L << 7)) != 0;
}

bool read_ent_rlg(uint32 values) {
    return (values & (1L << 8)) != 0;
}

bool read_rlg_(uint32 values) {
    return (values & (1L << 9)) != 0;
}

bool read_rlg1(uint32 values) {
    return (values & (1L << 10)) != 0;
}

bool read_rlg2(uint32 values) {
    return (values & (1L << 11)) != 0;
}

bool read_rlg3(uint32 values) {
    return (values & (1L << 12)) != 0;
}

bool read_bot_parte_ativo(uint32 values) {
    return (values & (1L << 13)) != 0;
}

bool read_bot_parte_ativo_(uint32 values) {
    return (values & (1L << 14)) != 0;
}

bool read_vai_partir(uint32 values) {
    return (values & (1L << 15)) != 0;
}

bool read_vai_parar(uint32 values) {
    return (values & (1L << 16)) != 0;
}

bool read_esc_mem(uint32 values) {
    return (values & (1L << 17)) != 0;
}

bool read_sai_bd(uint32 values) {
    bool result =  (values & (1L << 18)) != 0;
    return result;
}

bool read_sai_be(uint32 values) {
    return (values & (1L << 19)) != 0;
}

bool read_sai_bs(uint32 values) {
    return (values & (1L << 20)) != 0;
}

bool read_sai_re(uint32 values) {
    return (values & (1L << 21)) != 0;
}

bool read_sai_rp(uint32 values) {
    return (values & (1L << 22)) != 0;
}

bool read_rlg_ri(uint32 values) {
    return (values & (1L << 23)) != 0;
}

bool read_rlg_rc(uint32 values) {
    return (values & (1L << 24)) != 0;
}

bool get_i0(uint16 inputs) {
    return (inputs & (1 << 0)) != 0;
}

bool get_i1(uint16 inputs) {
    return (inputs & (1 << 1)) != 0;
}

bool get_rc(uint16 inputs) {
    return (inputs & (1 << 2)) != 0;
}

bool get_para(uint16 inputs) {
    return (inputs & (1 << 3)) != 0;
}

bool get_est1(uint16 inputs) {
    return (inputs & (1 << 4)) != 0;
}

bool get_parte(uint16 inputs) {
    return (inputs & (1 << 5)) != 0;
}

bool get_muda_p(uint16 inputs) {
    return (inputs & (1 << 6)) != 0;
}

bool get_muda_t(uint16 inputs) {
    return (inputs & (1 << 7)) != 0;
}

bool get_bot_parte(uint16 inputs) {
    return (inputs & (1 << 8)) != 0;
}

bool get_partiu(uint16 inputs) {
    return (inputs & (1 << 9)) != 0;
}

bool get_int_para(uint16 inputs) {
    return (inputs & (1 << 10)) != 0;
}

bool get_bot_esc_mem(uint16 inputs) {
    return (inputs & (1 << 11)) != 0;
}

bool get_ent_rlg_(uint16 inputs) {
    return (inputs & (1 << 12)) != 0;
}

bool calculate_desvia(uint16 inputs) {
    return (get_i1(inputs) &&
            get_i0(inputs) &&
            get_rc(inputs) &&
            (!get_para(inputs)));
}

bool calculate_esc_m(uint16 inputs) {
    return !((!get_i1(inputs)) &&
             get_i0(inputs) &&
             (!get_est1(inputs)) &&
             (!get_para(inputs)));
}

bool calculate_ult_c(uint16 inputs) {
    bool inner_nor = !((!get_est1(inputs)) || get_para(inputs));

    bool result =  !(calculate_desvia(inputs) || inner_nor || get_parte(inputs));

    return result;
}

bool calculate_esc_p(uint16 inputs) {
    return !(get_muda_p(inputs) ||
             (!(get_est1(inputs) ||
                get_para(inputs))));
}

bool calculate_esc_t(uint16 inputs) {
    return !(get_muda_t(inputs) ||
             (!(get_i0(inputs) ||
                (!get_est1(inputs)) ||
                get_para(inputs))));
}

bool calculate_esc_c(uint16 inputs) {
    return !((!get_i1(inputs)) ||
             get_i0(inputs) ||
             get_para(inputs));
}

bool calculate_z_comp(uint16 inputs) {
    return !(get_i1(inputs) &&
             (!get_i0(inputs)) &&
             get_est1(inputs) &&
             (!get_para(inputs)));
}

bool calculate_mais_1(uint16 inputs) {
    return !((!(get_i1(inputs) ||
                 (!get_est1(inputs)))) ||
              calculate_desvia(inputs) ||
              get_para(inputs));
}

bool calculate_ent_rlg(uint16 inputs) {
    return !get_ent_rlg_(inputs);
}

bool calculate_rlg_(uint16 inputs) {
    return !calculate_ent_rlg(inputs);
}

bool calculate_rlg1(uint16 inputs) {
    return !calculate_rlg_(inputs);
}

bool calculate_rlg2(uint16 inputs) {
    return !calculate_rlg_(inputs);
}

bool calculate_rlg3(uint16 inputs) {
    return !calculate_rlg_(inputs);
}

bool calculate_bot_parte_ativo(uint16 inputs) {
    return !get_bot_parte(inputs);
}

bool calculate_bot_parte_ativo_(uint16 inputs) {
    return !calculate_bot_parte_ativo(inputs);
}

bool calculate_vai_partir(uint16 inputs) {
    return (calculate_bot_parte_ativo(inputs) &&
            (!get_parte(inputs)) &&
            (!get_partiu(inputs)) &&
            get_para(inputs));
}

bool calculate_vai_parar(uint16 inputs) {
    return !((calculate_ult_c(inputs)) ||
             get_para(inputs) ||
             (!get_int_para(inputs)));
}

bool calculate_esc_mem(uint16 inputs) {
    return !((!((calculate_esc_m(inputs)) &&
                (!(get_bot_esc_mem(inputs))))) &&
             (!(calculate_ent_rlg(inputs) &&
                calculate_rlg_(inputs))));
}

bool calculate_sai_bd(uint16 inputs) {
    return !(get_para(inputs) &&
             get_bot_esc_mem(inputs));
}

bool calculate_sai_be(uint16 inputs) {
    return !(get_para(inputs) &&
             (!get_parte(inputs)));
}

bool calculate_sai_bs(uint16 inputs) {
    bool result = !(get_muda_p(inputs) || get_muda_t(inputs));
    return result;
}

bool calculate_sai_re(uint16 inputs) {
    return !((!get_para(inputs)) ||
             get_parte(inputs));
}

bool calculate_sai_rp(uint16 inputs) {
    bool inner_nand2 = !(get_est1(inputs) && (!get_para(inputs)));

    bool result =  !((!calculate_desvia(inputs)) &&
                     inner_nand2 &&
                     (!get_parte(inputs)) &&
                     (calculate_sai_bs(inputs)));

    return result;
}

bool calculate_rlg_ri(uint16 inputs) {
    return !(!(calculate_ult_c(inputs)) &&
             calculate_rlg_(inputs));
}

bool calculate_rlg_rc(uint16 inputs) {
    return !(calculate_esc_c(inputs) &&
             calculate_rlg_(inputs));
}


void test(bool (*testing) (uint32), bool (*reference) (uint16), uint32 read, uint16 input, char* name) {
    bool tested = testing(read);
    bool ref = reference(input);
    if(tested != ref) {
        Serial.print("Error: ");
        Serial.print(name);
        Serial.print(" => val: ");
        Serial.print(tested);
        Serial.print(" ref: ");
        Serial.println(ref);

        char buff[16];
        Serial.print("\tRead: ");
        sprintf(buff, "0x%08lx", read);
        Serial.print(buff);
        Serial.print(" Input: ");
        sprintf(buff, "0x%04x", input);
        Serial.println(buff);
    }
}

#define testa(to_test, input, read) test(read_##to_test, calculate_##to_test, read, input, (char*) #to_test)

void direciona_portas() {
    DDRF = 0xff;
    DDRK = 0xff;

    DDRC = 0x00;
    DDRL = 0x00;
    DDRA = 0x00;
    DDRB = 0x00;

    DDRE |= (1 << 4);
}

void escreve_input(uint16 valor) {
    uint16 inversion_mask = (1 << 6) | (1 << 7) | (1 << 10) | (1 << 11);
    valor = valor ^ inversion_mask;

    uint8 lower_byte = valor;
    uint8 upper_byte = valor >> 8;

    PORTF = lower_byte;
    PORTK = upper_byte;

    clock();
}

uint32 le_output() {
    nop();

    uint32 lower_byte = PINC;
    uint32 middle_lower_byte = PINL;
    uint32 middle_upper_byte = PINA;
    uint32 upper_byte = PINB & 0x01;

    uint32 result = (upper_byte << 24) | (middle_upper_byte << 16) | (middle_lower_byte << 8) | (lower_byte);
    return result;
}

void setup() {
    direciona_portas();
    Serial.begin(9600);
}

static uint16 n_testes = 0;
void loop() {
    Serial.print("Teste nº:");
    Serial.println(n_testes++);

    for(uint16 input = 0; input < (1 << 13); input++) {
        escreve_input(input);
        uint32 saida = le_output();

        testa(desvia, input, saida);
        testa(esc_m, input, saida);
        testa(ult_c, input, saida);
        testa(esc_p, input, saida);
        testa(esc_t, input, saida);
        testa(esc_c, input, saida);
        testa(z_comp, input, saida);
        testa(mais_1, input, saida);
        testa(ent_rlg, input, saida);
        testa(rlg_, input, saida);
        testa(rlg1, input, saida);
        testa(rlg2, input, saida);
        testa(rlg3, input, saida);
        testa(bot_parte_ativo, input, saida);
        testa(bot_parte_ativo_, input, saida);
        testa(vai_partir, input, saida);
        testa(vai_parar, input, saida);
        testa(esc_mem, input, saida);
        testa(sai_bd, input, saida);
        testa(sai_be, input, saida);
        testa(sai_bs, input, saida);
        testa(sai_re, input, saida);
        testa(sai_rp, input, saida);
        testa(rlg_ri, input, saida);
        testa(rlg_rc, input, saida);
    }
}
