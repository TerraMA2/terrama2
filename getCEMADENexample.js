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

const dataProviderUri = 'http://sjc.salvar.cemaden.gov.br';

const semantics = {
  dcp_list_uri: '/resources/dados/311_1.json',
  latitude_property: 'latitude',
  longitude_property: 'longitude',
  station_type_id_property: 'idtipoestacao',
  dcp_code_property: 'codestacao',
  uf_property: 'uf',
}

let dcpList = [];

http.get(dataProviderUri+semantics.dcp_list_uri, (res) => {
    console.log("Got response: " + res.statusCode);

    let rawData = '';
    res.on('data', (chunk) => { rawData += chunk; });
    res.on('end', () => {
      try {
        console.log('Data length: ' + rawData.length);
        const json = rawData.slice(10, rawData.length - 2);        
        const parsedData = JSON.parse(json);
        const rawDCPList = parsedData.estacao;

        let numEstacoes = 0;
        for (const rawDcp of rawDCPList) {
          if(idtipoestacao
              && !idtipoestacao.includes(rawDcp[semantics.station_type_id_property])) {
            continue;
          }
          if(uf
            && !uf.includes(rawDcp[semantics.uf_property])) {
            continue;
          }

          const dcp = {
            metadata: {
              ...rawDcp,
              alias: rawDcp[semantics.dcp_code_property]
            },
            position: {
              lat: rawDcp[semantics.latitude_property],
              long: rawDcp[semantics.longitude_property],
            }
          }

          dcpList.push(dcp);
        }

        console.log("Numero de estações: "+dcpList.length);

      } catch (e) {
        console.error(e.message);
      }
    });
  }).on('error', (e) => {
    console.log("Got error: " + e.message);
  });
