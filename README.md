# **Projeto Blockchain 🔐**

Esse projeto foi desenvolvido para a disciplina de Estrutura de Dados 2, utilizando conceitos de Blockchain e Hash. A linguagem utilizada para o desenvolvimento do projeto foi C.

O objetivo do projeto é simular a mineração de N blocos em uma Blockchain.

## Desenvolvedor 👨‍🎤

- [Ricardo Rebelo Junior](https://www.github.com/Rebel0R) - Graduando em Ciência da Computação pela UTFPR-PG, com conhecimentos em Design Gráfico e Desenvolvimento Web.

## Utilizando o código 💻

- É importante que em sua máquina o OpenSSL esteja instalado, pois é graças a ele que a simulação de criptografia será realizada.

```bash
#include "openssl/crypto.h"
#include "openssl/sha.h"

```

- Para a criação de números aleatórios foi utilizada a biblioca MTwister, a documentação completa você encontra aqui [Clique aqui!](https://github.com/ESultanik/mtwister):

```bash
#include "mtwister.h"
#include "mtwister.c"
```

- Para gerar um arquivo .txt de todas os blocos mineradas com base no arquivo "blocosMinerados.bin" execute este comando no terminal:

```bash
gcc ./a.out > blocosMinerados.txt
```
