const http = require("http");

const TIPO_ESTACAO = {
  PLUVIOMETRICA: {
    name: 'Pluviométrica',
    id: 1
  },
  HIDROLOGICA: {
    name: 'Hidrológica',
    id: 3
  },
  AGROMETEOROLOGICA: {
    name: 'Agrometeorológica',
    id: 4
  },
  ACQUA: {
    name: 'Acqua',
    id: 5
  },
  GEOTECNICA: {
    name: 'Geotécnica',
    id: 10
  }
}
const idtipoestacao = [TIPO_ESTACAO.PLUVIOMETRICA.id];
const uf = ['MG','SP'];

http.get('http://sjc.salvar.cemaden.gov.br/resources/dados/311_1.json', (res) => {
    console.log("Got response: " + res.statusCode);

    let rawData = '';
    res.on('data', (chunk) => { rawData += chunk; });
    res.on('end', () => {
      try {
        console.log('Data length: ' + rawData.length);
        const json = rawData.slice(10, rawData.length - 2);        
        const parsedData = JSON.parse(json);
        const dcpList = parsedData.estacao;

        let numEstacoes = 0;
        for (const iterator of dcpList) {
          if(idtipoestacao
              && !idtipoestacao.includes(iterator.idtipoestacao)) {
            continue;
          }
          if(uf
            && !uf.includes(iterator.uf)) {
          continue;
        }

          numEstacoes++;
        }

        console.log("Numero de estações: "+numEstacoes);

      } catch (e) {
        console.error(e.message);
      }
    });
  }).on('error', (e) => {
    console.log("Got error: " + e.message);
  });
