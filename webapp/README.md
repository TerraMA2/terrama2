# bdqueimadas - Banco de Dados de Queimadas

Este aplicativo permite visualizar os focos (ocorrência do fogo na vegetação) através de um Sistema de Informação Geográfica na Web, com opções de períodos, regiões de interesse, satélites, planos de informação (ex: desmatamento, hidrografia, estradas), além da exportação dos dados em formatos txt, html, shp e kmz.

A versão anterior deste aplicativo encontra-se operacional em: http://www.dpi.inpe.br/proarco/bdqueimadas.

## Dependências

Para executar o BDQueimadas você vai precisar instalar alguns softwares de terceiros. Abaixo listamos quais são os softwares e suas versões:

- **Apache (Obrigatório):** Para que o BDQueimadas funcione é necessário que ele esteja sendo executado por um servidor http. O servidor a ser utilizado é o Apache HTTP Server na versão 2.4 (ou versões superiores). O software pode ser baixado em: https://httpd.apache.org.

## Instruções para Execução

Abaixo mostramos os passos para executar o BDQueimadas:

- Coloque a pasta da aplicação no diretório raiz do Apache.

- Verifique se o caminho para a API TerraMA² está correto no arquivo index.html (bdqueimadas/views/index.html):

```
<script src="http://localhost/terrama2/webapp/js/terrama2.js" type="text/javascript"></script>
<link rel="stylesheet" href="http://localhost/terrama2/webapp/css/terrama2.css">
```
