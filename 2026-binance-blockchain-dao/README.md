# Criando um Servidor Discord Server e uma DAO: Guia Completo

## Conteúdo
1. [Introdução](#introduction)
2. [Crie seu servidor Discord](#creating-your-discord-server)
3. [Compreendendo as DAOs](#understanding-daos)
4. [Crie sua DAO](#creating-your-dao)
5. [Integrando o Discord com sua DAO](#integrating-discord-with-your-dao)
6. [Melhores práticas e segurança](#best-practices-and-security)


## Introdução

Este guia mostra os passos para a criação de um servidor Discord e o estabelecimento de uma **Organização Autônoma Descentralizada** (*Decentralized Autonomous Organization - DAO*), e conectando-os de modo que sua comunidade Discord seja restrita por tokens e governada pelos membros da DAO.

**O que precisaremos:**
- Uma conta no Discord
- Uma carteira de criptomoedas (recomenda-se a MetaMask)
- Alguma criptomoeda para o gas fees (ETH, Polygon MATIC, etc.)
- Compreenção básica dos conceitos sobre blockchain


## Criando seu servidor Discord

### Passo 1: Criando o servidor

1. **Abra o Discord** (aplicativo para desktop ou servidor web)
2. Clique no botão **"+"** na barra lateral esquerda
3. Selecione **"Create My Own"**
4. Escolha o tipo do servidor:
   - "For a club or community" (recomendado para DAOs)
5. **Dê um nome para seu servidor** (ex., "MyDAO Community")
6. Faça o upload de um **ícone para o servidor** (opcional mas recomendado)
7. Clique em **"Create"**

### Passo 2: Efetue os ajustes básicos

1. Clique no nome do **seu servidor** no topo à esquerda
2. Selecione **"Server Settings"**
3. Configure o seguinte:
   - **Overview**: Adicione uma descrição para o servidor
   - **Roles**: Crie as autorizações iniciais (Admin, Moderator, Member, etc.)
   - **Channels**: Organize seus canais por categoria
   - **Moderation**: Ajuste as regras do AutoMod
   - **Community**: Habilite os recursos da comunidade por verificação

### Passo 3: Crie os canais essenciais

Crie estes canais básicos para uma DAO:

**Canais de texto:**
- `#announcements` - Atualizações importantes (postagem somente para administradores)
- `#general` - Discussões gerais
- `#introductions` - Instruções para novos membros
- `#proposals` - Discussões das propostas para a DAO
- `#voting` - Votações ativas e resultados
- `#resources` - Links e documentações
- `#support` - Ajuda e questões 

**Canais de voz:**
- `General Voice`
- `DAO Meetings`
- `AFK Channel`

### Passo 4: Ajude as funções e as permissões

1. Entre em **Server Settings > Roles**
2. Crie as seguintes funções:
   - **Founder/Admin** - Permissões completas
   - **DAO Member** - Membros que possuem tokens de governança
   - **Contributor** - Membros ativos na comunidade
   - **Visitor** - Acesso limitado para somente leitura

3. Para cada função, configure as permissões:
   - DAO Members: Podem postar na maioria dos canais, participam nas discussões
   - Visitors: Acesso somente leitura nos canais
   - Admins: Acesso completo para gerenciamento


## Entendendo as DAOs

### O que é uma DAO?

Uma *Decentralized Autonomous Organization (DAO)* é uma organização governada pelos contratos inteligentes (*smart contracts*) na blockchain. Os membros possuem tokens de governança que garantem direito aos votos e as decisões são feitas coletivamente através de propostas e votações.

### Principais componentes de uma DAO:

1. **Token de governança** - Representa o poder de votação e sociedade
2. **Tesouraria** - Fundos compartilhados controlados pela DAO
3. **Smart contracts** - Código que executam as decisões automaticamente
4. **Sistema de propostas** - Como os membros sugerem e votam nas mudanças
5. **Comunidade** - Os membros que participam na governança

### Plataformas Populares para DAOs:

- **Aragon** - Amigável e baseado em Ethereum
- **DAOstack** - Modelo de consenso holográfico
- **Snapshot** - Votação fora da rede e sem *gas fee*
- **Colony** - Governança baseada em reputação
- **Gnosis Safe + Snapshot** - Combinação comum para tesouraria + votação


## Crie sua DAO

### Método 1: Usando Aragon (recomendado para iniciantes)

#### Passo 1: Conecte sua carteira

1. Entre em [aragon.org](https://aragon.org)
2. Clique em **"Create a DAO"**
3. Conecte sua **MetaMask** ou outra carteira Web3
4. Escolha sua rede (Ethereum, Polygon, Arbitrum, etc.)
   - *Nota: Polygon é a rede mais econômica em gas fees*

#### Passo 2: Escolha um modelo

1. Selecione um modelo de DAO:
   - **Company** - Votação ponderada pelos tokens
   - **Membership** - Cada membro é um voto
   - **Reputation** - Votação baseada pelo mérito

2. Para a maioria das DAOS, escolha **"Company"**

#### Passo 3: Configure os parâmetros da DAO

1. **Nomeie sua DAO** (ex., "MyDAO")
2. **Ajuste os detalhes dos tokens de governança:**
   - Nome do token (ex., "MyDAO Token")
   - Símbolo do token (ex., "MYDAO")
   - Total de tokens (ex., 1.000.000 tokens)
   - Proprietários iniciais dos tokens e quantidades

3. **Configure os parâmetros para as votações:**
   - Suporte necessário: 50% (porcentagem dos votos necessários para aprovação)
   - Quorum mínimo: 15% (participação mínima necessária)
   - Duração da votação: 7 dias
   - Atraso na execução: 1 dia (tempo antes das propostas aprovadas serem executadas)

#### Passo 4: Distribuição dos tokens

1. Adicione os proprietários iniciais dos tokens:
   - Entre os endereços das carteiras
   - Especifique a quantidade dos tokens para cada proprietário
   - Considere reservar tokens para futura distribuição

2. Revise as distribuições com cuidado
3. Clique em **"Launch Your DAO"**
4. Confirme as transações em sua carteira
5. Aguarde a confirmação (isto pode demorar vários minutos)

#### Passo 5: Acesse o painel de controle da DAO

1. Uma vez disponibilizada a DAO, você receberá a URL (ex., `mydao.aragon.eth`)
2. Bookmark esta URL para facilitar o acesso
3. Explore o painel de controle da sua DAO:
   - **Finance** - Gerenciamento da tesouraria
   - **Tokens** - Detentores dos tokens e distribuição
   - **Voting** - Crie e vote nas propostas
   - **Settings** - configurações gerais da DAO

### Método 2: Usando Snapshot + Gnosis Safe

Este método separa a votação (Snapshot) do gerenciamento da tesouraria  (Gnosis Safe).

#### Criando um *Snapshot Space*:

1. Entre em [snapshot.org](https://snapshot.org)
2. Clique em **"Create Space"**
3. Conecte sua carteira
4. Configure seu espaço:
   - Nome do espaço
   - Token de votação (pode usar um token ERC-20 já existente)
   - Estratégia de votos (saldo de tokens, delegação, etc.)
   - Administradores e moderadores

5. Personalize os ajustes do espaço
6. Publique seu espaço

#### Ajuste do *Gnosis Safe* para a tesouraria:

1. Entre em [safe.global](https://safe.global)
2. Clique em **"Create Safe"**
3. Selecione sua rede
4. Adicione as carteiras (endereço das carteiras que podem aprovar as transações)
5. Ajuste os limites (número de aprovações necessárias)
6. Disponibiliza no Safe
7. Adicione recursos (crypto) no seu Safe


## Integrando o Discord com sua DAO

### Passo 1: Coleta de tokens com *Collab.Land*

**Collab.Land** é um *bot* popular para a coleta de tokens para servidores do Discord.

1. Entre em [collab.land](https://collab.land)
2. Clique em **"Add to Discord"**
3. Selecione seu servidor no Discord
4. Autorize o bot
5. O bot criará um canal chamado `#collabland-config`

#### Configure a coleta dos tokens:

1. Em `#collabland-config`, digite: `/admin`
2. Selecione **"Token Gating"**
3. Escolha **"Add TGR"** (*Token Gated Role*)
4. Configure suas regras:
   - **Blockchain**: Selecione a blockchain da sua DAO (Ethereum, Polygon, etc.)
   - **Token Address**: Seu endereço do token do contrato de governança
   - **Token Type**: ERC-20 (para a maioria dos tokens de govenança)
   - **Minimum Balance**: Quantos tokens são necessários
   - **Role**: Selecione a função "DAO Member" que você criou anteriormente

5. Grave a regra
6. Teste conectando sua carteira em `#collabland-join`

### Passo 2: Coleta de tokens alternativa com *Guild.xyz*

1. Entre em [guild.xyz](https://guild.xyz)
2. Crie uma nova *guilda*
3. Conecte seu servidor Discord
4. Necessidade de ajuste:
   - Requerimentos para manutenção dos tokens
   - Requerimentos de NFTs
   - Outras credenciais Web3

5. Crie as funções baseadas nos requerimentos
6. Verificação dos membros no site Guild.xyz

### Passo 3: Integre o sistema de votação

#### Usando o Discord Webhook:

1. Em seu servidor Discord, entre em **Channel Settings** para configurar o canal `#voting`
2. Selecione **"Integrations" > "Webhooks"**
3. Crie um novo webhook, copie o URL
4. No Snapshot, entre nas configurações do seu espaço
5. Adicione o URL do webhood do Discord
6. Configure as notificações para:
   - Novas propostas
   - Finalização das propostas
   - Resultados das propostas

#### Integração manual:

1. Crie um canal `#proposals` dedicado
2. Quando criar uma proposta no Snapshot:
   - Compartilhe o link direto no Discord
   - Fixe as propostas importantes
   - Use o recurso *poll* do Discord para a verificação dos sentimentos

### Passo 4: Configure os *bots* de gerenciamento da DAO

#### Opções de Bots de Governança:

**1. Polls do Discord para votações informais:**
- Use o recurso nativo poll do Discord para verificações de sentimentos não vinculantes
- Exemplo: `/poll "Devemos expandir para este novo mercado?"`

**2. Bot personalizado com Webhooks:**
- Conecte com a API da sua DAO
- Automaticalmente envie novas propostas
- Avalie a pariticipação nas votações
- Anuncie os resultados

**3. Coordinape para recompensa aos contribuidores:**
- Instale o bot Coordinape do Discord
- Execute um ciclo regular de recompensa aos contribuidores
- Distribua os tokens com base no reconhecimento dos pares

### Passo 5: Crie a documentação do canal do DAO

No seu servidor do Discord, crie um canal `#dao-info` com:

```
**Bem-vindo ao [nome da DAO]!**

**O que é nossa DAO?**
[Breve descrição da missão e objetivos da DAO]

**Como participar:**
1. Retenha [X] tokens de governança para acessar os canais dos membros
2. Participe das discussões em #general e #proposals
3. Vote nas propostas em: [link do seu Snapshot/Aragon]
4. Veja #announcements para obter informações importantes

**Informação do token:**
- Nome do token: [token]
- Símbolo do token: [símbolo]
- Endereço do contrato: `[0x...]`
- Como obter os tokens: [links da DEX ou informações]

**Processos de governança:**
1. Discussão na comunidade (mínimo de 3 dias)
2. Formulação da proposta submetida
3. Período de votação (7 dias)
4. Execução (se aprovado)

**Links importantes:**
- Painel do DAO: [Link]
- Snapshot Space: [Link]
- Tesouraria: [Link]
- Documentação: [Link]

**Verificaçao:**
Entre em #collabland-join para verificar sua posse de tokens e obtenção de funções na DAO!
```


## Melhhores práticas e segurança

### Segurança no Discord:

1. **Habilite a autênticação de dois fatores (2FA)**
   - Server Settings > Safety Setup > Require 2FA

2. **Ajuste os níveis de verificação**
   - Requer um email verificado
   - Requer verificação por telefone para as ações de maior risco

3. **Use AutoMod**
   - Filtre os links de spam e scam
   - Bloqueie as tentativas comuns de phishing
   - Ajuste os filtros de teclado

4. **Informe os membros**
   - Melhores práticas para segurança do PIN
   - Avisos sobre scans de DM (administradores do Discord nunca devem priorizar DM)
   - Crie um canal `#security-tips`

### Segurança da DAO:

1. **Carteiras com múltiplas assinaturas**
   - Use Gnosis Safe para tesouraria
   - Requer múltiplas aprovações para transações
   - Distribua as chaves somente para membros confiáveis

2. **Auditoria dos smart contracts**
   - Antes de lançar, faça uma auditoria nos contratos
   - Use templates bem-testados e auditados
   - Nunca apresse a implementação

3. **Governança transparente**
   - Documente todas as decisões
   - Torne público os relatórios financeiros
   - Comunique as atualizações regularmente

4. **Decentralização gradual**
   - Comece com algum controle centralizado
   - Gradualmente aumente a governança da comunidade
   - Implemente medidas de segurança e de emergência

### Melhores práticas de integração:

1. **Lembretes para verificações regulares**
   - Lembre os membros para verificar suas carteiras
   - Atualize os requisitos de acesso por token conforme necessário

2. **Comunicação Clara**
   - Sempre anuncie as votações de governança no Discord
   - Compartilhe atualizações importantes em outros canais
   - Use eventos do Discord para prazos de votação

3. **Integração de Membros**
   - Crie um guia de integração
   - Atribua uma "equipe de boas-vindas" para ajudar novos membros
   - Realize chamadas regulares com a comunidade

4. **Planos de Backup**
   - Documente todas as configurações de bots
   - Tenha administradores de backup com acesso
   - Faça backups regulares dos canais importantes


## Solução de Problemas Comuns

**Problema: Membros não conseguem verificar tokens**
   - Verifique se o endereço do contrato do token está correto
   - Certifique-se de que a rede blockchain corresponde
   - Confirme se os membros estão usando a carteira correta

**Problema: Bot não responde**
   - Verifique as permissões do bot no Discord
   - Confirme se o bot está online
   - Reconecte a autorização do bot

**Problema: Baixa participação nas votações**
   - Envie notificações do Discord sobre as votações
   - Considere períodos de votação mais curtos
   - Aumente o engajamento com discussões


## Conclusão

Agora você criou um servidor Discord integrado com seu DAO! Lembre-se:

- Comece pequeno e cresça de forma orgânica
- Ouça sua comunidade
- Faça iterações no seu processo de governança
- Mantenha as melhores práticas de segurança
- Continue aprendendo e melhorando

**Próximos Passos:**
1. Convide seus membros fundadores
2. Crie sua primeira proposta
3. Realize sua primeira votação da comunidade
4. Comece a construir a cultura do seu DAO

Boa sorte na sua jornada com o DAO!
