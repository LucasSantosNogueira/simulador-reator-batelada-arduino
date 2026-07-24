// --- MAPEAMENTO DE PINOS ---
const int segmentos[7] = {2, 3, 4, 5, 6, 7, 8}; // Display [A,B,C,D,E,F,G]
const int pinoRed   = 9;
const int pinoGreen = 10;
const int pinoBlue  = 11;
const int pinoBotaoStart = 12;
const int pinoBotaoModo  = 13;
const int pinoSensorTemp = A0;

// --- MÁQUINA DE ESTADOS (Fases do Reator) ---
enum Fases { STANDBY, CARGA, REACAO, DESCARGA, EMERGENCIA };
Fases faseAtual = STANDBY;

// --- VARIÁVEIS DE CONTROLE ---
bool modoAutomatico = true;
unsigned long tempoEstado = 0;
int progressoCarga = 0;
int tempoReacaoRestante = 5; // 5 segundos de reação estável necessários

// Matriz do display para os caracteres: 0, 1, 2, 3 e 9
const byte mapaNumeros[5][7] = {
  {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, LOW},  // 0
  {LOW, HIGH, HIGH, LOW, LOW, LOW, LOW},     // 1
  {HIGH, HIGH, LOW, HIGH, HIGH, LOW, HIGH},  // 2
  {HIGH, HIGH, HIGH, HIGH, LOW, LOW, HIGH},  // 3
  {HIGH, HIGH, HIGH, HIGH, LOW, HIGH, HIGH}   // 9 (Emergência)
};

void setup() {
  Serial.begin(9600);
  
  // Configura saídas
  for (int i = 0; i < 7; i++) pinMode(segmentos[i], OUTPUT);
  pinMode(pinoRed, OUTPUT); pinMode(pinoGreen, OUTPUT); pinMode(pinoBlue, OUTPUT);
  
  // Configura entradas com resistor interno
  pinMode(pinoBotaoStart, INPUT_PULLUP);
  pinMode(pinoBotaoModo, INPUT_PULLUP);
  
  Serial.println("==========================================");
  Serial.println("  SISTEMA AUTOMATIZADO DE REATOR INICIADO  ");
  Serial.println("==========================================");
}

void loop() {
  // 1. LEITURA PERMANENTE DOS SENSORES E BOTÕES
  int leituraCrua = analogRead(pinoSensorTemp);
  int temperatura = map(leituraCrua, 0, 1023, 20, 120); // Escala de temperatura de 20°C a 120°C

  // Botão de alternar modo (Auto/Manual) - Funciona em quase todas as fases
  if (digitalRead(pinoBotaoModo) == LOW) {
    modoAutomatico = !modoAutomatico;
    Serial.print("-> Modo de Operacao alterado para: ");
    Serial.println(modoAutomatico ? "AUTOMATICO" : "MANUAL INTERVENTIVO");
    delay(250); // Debounce
  }

  // 2. LÓGICA DA MÁQUINA DE ESTADOS DO REATOR
  switch (faseAtual) {
    
    case STANDBY:
      atualizarInterface(0); // Mostra '0' no display
      
      if (digitalRead(pinoBotaoStart) == LOW) {
        faseAtual = CARGA;
        progressoCarga = 0;
        Serial.println("\n[FASE 1] Iniciando carga de reagentes na batelada...");
        delay(250);
      }
      break;

    case CARGA:
      atualizarInterface(1); // Mostra '1' no display
      
      // Simula o enchimento do reator químico
      progressoCarga += 10;
      Serial.print("Volume do Reator: ");
      Serial.print(progressoCarga);
      Serial.println("%");
      
      if (progressoCarga >= 100) {
        faseAtual = REACAO;
        tempoReacaoRestante = 5;
        tempoEstado = millis();
        Serial.println("\n[FASE 2] Carga concluida. Aqueca o reator ate a faixa verde (70C - 90C)!");
      }
      delay(400); // Ritmo de enchimento
      break;

    case REACAO:
      atualizarInterface(2); // Mostra '2' no display
      
      Serial.print("Temperatura Atual: ");
      Serial.print(temperatura);
      Serial.println(" C");

      // Verificação de segurança (Intertravamento por sobretemperatura)
      if (temperatura > 92) {
        faseAtual = EMERGENCIA;
        Serial.println("\n!!! ALARME CRÍTICO !!! Superaquecimento no reator!");
        break;
      }

      // Verifica se está na faixa correta de reação
      if (temperatura >= 70 && temperatura <= 90) {
        if (millis() - tempoEstado >= 1000) {
          tempoEstado = millis();
          tempoReacaoRestante--;
          Serial.print("-> Temperatura ideal! Mantendo reacao. Tempo restante: ");
          Serial.print(tempoReacaoRestante);
          Serial.println("s");
        }
        
        if (tempoReacaoRestante <= 0) {
          faseAtual = DESCARGA;
          tempoEstado = millis();
          Serial.println("\n[FASE 3] Reacao concluida com sucesso! Abrindo valvula de descarga.");
        }
      } else {
        // Se sair da temperatura ideal, reinicia o cronômetro da reação
        tempoEstado = millis();
        if (temperatura < 70) {
          Serial.println(" Alerta: Temperatura muito baixa para iniciar a reacao.");
        }
      }
      delay(300);
      break;

    case DESCARGA:
      atualizarInterface(3); // Mostra '3' no display
      Serial.println("Descarregando produto final valioso...");
      
      // Pisca o LED para simular a bomba/válvula escoando
      for(int i=0; i<6; i++) {
        digitalWrite(pinoGreen, LOW); delay(250);
        digitalWrite(pinoGreen, HIGH); delay(250);
      }
      
      faseAtual = STANDBY;
      Serial.println("\n[FASE 0] Batelada finalizada! Reator limpo e em Standby.\n");
      break;

    case EMERGENCIA:
      atualizarInterface(4); // Mostra '9' no display e LED Vermelho fixo
      
      // Para sair do alarme: temperatura deve baixar E operador deve apertar START
      if (temperatura < 60) {
        Serial.println("Resfriamento detectado. Pressione START para resetar o intertravamento de segurança.");
        
        if (digitalRead(pinoBotaoStart) == LOW) {
          faseAtual = STANDBY;
          Serial.println("\n[RESET] Alarme resetado com sucesso pelo operador.");
          delay(250);
        }
      } else {
        Serial.print(" !!! REATOR TRANCADO !!! Aguarde resfriar abaixo de 60C. Temperatura atual: ");
        Serial.print(temperatura);
        Serial.println(" C");
      }
      delay(500);
      break;
  }
}

// --- FUNÇÃO DE GERENCIAMENTO DA INTERFACE VISUAL ---
void atualizarInterface(int indiceNumero) {
  // Atualiza os segmentos do display de 7 seg
  for (int i = 0; i < 7; i++) {
    digitalWrite(segmentos[i], mapaNumeros[indiceNumero][i]);
  }
  
  // Controle das cores da Torre de Sinalização Industrial (LED RGB)
  if (faseAtual == EMERGENCIA) {
    // ERRO CRÍTICO: LED Vermelho
    digitalWrite(pinoRed, HIGH); digitalWrite(pinoGreen, LOW); digitalWrite(pinoBlue, LOW);
  } 
  else if (!modoAutomatico) {
    // MODO MANUAL: LED Azul
    digitalWrite(pinoRed, LOW); digitalWrite(pinoGreen, LOW); digitalWrite(pinoBlue, HIGH);
  } 
  else {
    // MODO AUTOMÁTICO SEGURO: LED Verde
    digitalWrite(pinoRed, LOW); digitalWrite(pinoGreen, HIGH); digitalWrite(pinoBlue, LOW);
  }
}
