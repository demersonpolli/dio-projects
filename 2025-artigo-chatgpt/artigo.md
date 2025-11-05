# Aplicações com bancos de dados à moda antiga

![Sistema de banco de dados em mainframe. Wikipedia Creative Commons](https://brewminate.com/wp-content/uploads/2022/04/042822-08-History-Technology.jpg)

## Legado do dBase e do Clipper: Pioneiros da programação com banco de dados

No início dos anos 1980 e 1990 as linguagens **dBase** e **Clipper** eram as ferramentas dominantes para o gerenciamento e desenvolvimento de aplicações com o uso de bancos de dados. A linguagem dBase foi desenvolvida no final da década de 1970 e oferecia aos desenvolvedores uma forma acessível de trabalhar com dados relacionais, combinando um gerenciador de banco de dados com uma linguagem de progração fácil de aprender. A linguagem foi desenvolvida inicialmente para o sistema operacional CP/M (*control program/monitor*) e, posteriormente, para o DOS (*disk operating system*).

A lingugagem Clipper surgiu inicialmente como um compilador para os códigos do dBase, uma vez que esta era uma linguagem interpretada e exigia que a linguagem (dBase) estivesse instalada na máquina para que os programas fossem utilizados. O Clipper (versão inicial *Winter' 84*) apareceu inicialmente como um compilador para códigos escritos em dBase, gerando um arquivo executável autônomo. A linguagem Clipper, no entanto, a partir da verão *Summer' 87* se desenvolve em uma linguagem diferente do dBase (embora mantivesse a compatibilidade).

Juntas estas duas ferramentas tornaram-se o padrão para programação comercial, permitindo aos desenvolvedores criar aplicações de controle de estoque, folha de pagamento e muito mais.


### A Evolução: Desafios e herança

Com a popularização do *Windows 3.1*, primeira versão do Windows a ser amplamente adotada, e com o surgimento das linguagens orientadas a objeto na segunda metade dos anos 1990, o dBase começaram a perder espaço para linguagens mais modernas como o *Visual Basic*, o *Delphi* e os sistemas de bancos de dados relacionais baseados no SQL (*structured query language* -- linguagem estruturada de busca). No entanto a simplicidade, foco na manipulação de dados e paradigma procedural de programação continua apelativo mesmo atualmente. Por conta disto, atualmente tem surgido alguns clones da linguagem Clipper, dentre as quais se destacam o [Xbase++](https://www.alaska-software.com/), o [Harbour](https://harbour.github.io/) e o [FoxInCloud](http://foxincloud.com/)  (este último uma versão em núvem do FoxPro, dialeto do dBase desenvolvido pela Microsoft). Estes softwares buscam modernizar o ecosistema fornecendo suporte para sistemas operacionais modernos e introduz novos recursos como programação orientada a objetos e integração com bancos de dados mais modernos (o banco de dados original do dBase é o famoso arquivo DBF).

### Forks Modernos: Uma mistura de tradição e inovação

Os forks modernos do dBASE e do Clipper, como o **Harbour** e o **xBase++**, buscam manter a compatibilidade com as aplicações legadas enquanto incorporam paradigmas contemporâneos de programação. Eles permitem aos desenvolvedores criar soluções multiplataforma, integrados com serviços de núvem (*cloud services*) e utilizar bibliotecas modernas. Estas iterações modernas mesclam a simplicidade das primeiras linguagens focadas em bancos de dados e o poder das ferramentas modernas de desenvolvimento, garantindo que as empresas que possuem sistemas legados possam continuar a inovar sem perder décadas de códigos e expertise de negócio.

### Um tira-gosto

O famoso programa "Olá, Mundo" escrito em dBase/Clipper é tão simples quanto isto:

```dbase
?"Olá mundo!"
```
Um formulário, por sua vez, é construído muito simplesmente com o comando `@ linha, coluna SAY "texto" GET variavel`:
```clipper
@ 01, 02 SAY "  Nome:" GET Nome
@ 02, 02 SAY "E-mail:" GET Email
READ
```

### Nota

Este artigo foi criado com o uso do [ChatGPT](https://chatgpt.com) e posteriormente foi revisado e enriquecido por mim (humano).