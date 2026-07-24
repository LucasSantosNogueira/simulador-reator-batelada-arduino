# Simulador de Reator em Batelada (ISA-88) com Arduino

Este projeto consiste em um simulador dinâmico de um Reator Químico em Batelada (Batch Reactor) controlado por Arduino, projetado sob os conceitos de gerenciamento de receitas e controle sequencial da norma internacional **ISA-88**.

## ⚙️ Funcionalidades Operacionais

O sistema gerencia o reator através de uma **Máquina de Estados Finitos (FSM)** dividida em 5 estágios visíveis na Interface Homem-Máquina (Display de 7 Segmentos) e na Torre de Sinalização (LED RGB):

- **Fase 0 (Standby):** Display indica `0`. Sistema limpo e aguardando comando do operador. LED indicador em Verde (Automático) ou Azul (Manual).
- **Fase 1 (Carga):** Display indica `1`. Simulação do preenchimento volumétrico dos reagentes (0% a 100%) monitorado via Serial.
- **Fase 2 (Reação):** Display indica `2`. Fase crítica de controle térmico. O operador deve ajustar o potenciômetro para manter a temperatura na faixa ideal de **70°C a 90°C** por 5 segundos.
- **Fase 3 (Descarga):** Display indica `3`. Abertura da válvula de escoamento do produto final (LED RGB pulsante).
- **Alarme (Emergência):** Display indica `9` e LED Vermelho fixo. Ativado por intertravamento caso a temperatura ultrapasse **92°C**. O sistema é completamente bloqueado até que o reator resfrie abaixo de 60°C e o botão START seja pressionado para Reset.

## 🛠️ Hardware Utilizado

- 1x Arduino Uno R3
- 1x Display de 7 Segmentos (5161AS - Catodo Comum)
- 1x LED RGB (Catodo Comum)
- 1x Potenciômetro Linear de 10kΩ (Transmissor de Temperatura)
- 2x Chaves Tácteis (Push-buttons para Start e Seleção de Modo)
- 5x Resistores de 220Ω (Proteção dos segmentos e anodos do LED)
