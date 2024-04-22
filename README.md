# Projeto VINA

O objetivo desse repositório é implementar o programa VINA, que consiste de um arquivador básico (archiver), isto é, um programa que salva em sequência uma coleção de arquivos (denominados membros) dentro de outro arquivo (denominado archive) cuja estrutura permite recuperar os arquivos originais individualmente.

Os programas tar, zip, rar e arj são arquivadores populares. Além de arquivar arquivos, a maioria deles também comprime dados dos mesmos, para ocupar menos espaço de armazenamento. Para simplificar, o arquivador VINA não comprime os arquivos, apenas os armazena.

### Tecnologias Utilizadas

* C

## Como rodar o projeto ✅

Primeiramente, devemos compilar os arquivos a partir do seguinte comando:

```
make
```

Depois disso, o pacote de software gera o executável vina, que deve ser executado da seguinte forma:

```
vina <opção> <archive> [membro1 membro2 ...]
```
Onde a opção pode ser:

* -i: Insere/acrescenta um ou mais membros ao archive. Se um membro já está no archive, o novo não é inserido. Novos membros são inseridos respeitando a ordem da linha de comando, ao final do archive;

* -a: Mesmo comportamento da opção -i, mas se o arquivo já existe, então a substituição ocorre apenas caso o parâmetro seja mais recente que o arquivado;

* -x: extrai os membros indicados de archive. Se os membros não forem indicados, todos devem ser extraídos. A extração consiste em ler o membro de archive e criar um arquivo correspondente, com conteúdo idêntico, em disco;

* -r: remove os membros indicados de archive;

* -c: lista o conteúdo de archive em ordem, incluindo as propriedades de cada membro (nome, UID, permissões, tamanho e data de modificação) e sua ordem no arquivo;

* -h: Imprime uma pequena mensagem de ajuda com as opções disponíveis e encerra.

Exemplos de comandos:

```
//inclui os arquivos "arq.txt", "foto.jpg" e "despesas.ods" em backup.vr
vina -i backup.vr arq.txt foto.jpg despesas.ods

//atualiza "despesas.ods" em backup.vr, se o arquivo externo for mais recente
vina -a backup.vr despesas.ods

//extrai o arquivo arq.txt de backup.vr
vina -x backup.vr arq.txt

//extrai todos os arquivos de backup.vr
vina -x backup.vr
```

Você sempre pode confirmar as mudanças realizadas no archive com a opção -c.

```
//lista os arquivos dentro do archive
vina -c backup.vr
```

## 📌 Informações importantes sobre a aplicação 📌

* O início ou final do archive (.vr) tem uma área de diretório. Todas as informações sobre os membros necessárias para a manipulação do archive, estão armazenadas única e exclusivamente na área de diretório. A parte restante contém apenas dados dos membros.

* Para cada membro, são armazenadas as seguintes informações: nome, UID (user ID), permissões, tamanho, data de modificação, ordem no arquivo e localização.

* A ordem de um membro no arquivo é dada pela ordem de inserção.

## ⏭️ Próximos passos

Gostaria de implementar uma forma de alterar a ordem dos membros dentro do archive. Entretanto, ainda não vi uma certa utilidade nessa mudança.
