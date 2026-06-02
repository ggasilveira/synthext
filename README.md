# Synthext
Graphical application to synthesize audio with text input.

UFRGS - Software Development assignment.
Lectured by prof. [Marcelo Pimenta](https://www.inf.ufrgs.br/~mpimenta/).

## Authors
- Guilherme Guimarães
- Humberto Enchaki
- Ícaro Argonauta


## Build Instructions
1. Install CMake and g++ toolchain for your platform.
3. Install [FluidSynth](https://github.com/FluidSynth/fluidsynth).
2. Run `mkdir build && cd build && cmake .. && make`


## Estrutura do Repositório

- apps:
  Essa pasta contém o código da interface gráfica. Esse código não
  consegue seguir muito boas práticas de desenvolvimento devido a
  limitações da biblioteca GUI utilizada.

- synthlib:
  Essa pasta contém o código de domínio do projeto (a parte importante
  do trabalho), e está dividida em subpastas:

  - include/synthlib:
    Essa pasta contém os headers do projeto, declarando todas as classes
    e interfaces.
  
  - lib/include:
    Essa pasta contém headers de bibliotecas externas utilizadas. Não é
    código de nossa autoria.

  - src:
    Contém os arquivos .c implementando as funções declaradas nos headers,
    e os respectivos testes unitários dos módulos.
