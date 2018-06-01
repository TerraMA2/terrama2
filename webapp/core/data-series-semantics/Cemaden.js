const http = require('http');
const Promise = require('./../Promise');

class Cemaden {
  constructor() {
    this.TIPO_ESTACAO = {
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

    this.idtipoestacao = [this.TIPO_ESTACAO.PLUVIOMETRICA.id];
    this.dataProviderUri = 'http://sjc.salvar.cemaden.gov.br';
    this.semantics = {
      dcp_list_uri: '/resources/dados/311_1.json',
      latitude_property: 'latitude',
      longitude_property: 'longitude',
      station_type_id_property: 'idtipoestacao',
      dcp_code_property: 'codestacao',
      uf_property: 'uf',
    };
  }

  listDCP(ufs) {
    return new Promise((resolve, reject) => {
      let dcpList = [];

      http.get(this.dataProviderUri + this.semantics.dcp_list_uri, (res) => {
        console.log('Got response: ' + res.statusCode);

        let rawData = '';

        res.on('data', (chunk) => { rawData += chunk; });

        res.on('end', () => {
          try {
            const json = rawData.slice(10, rawData.length - 2);
            const parsedData = JSON.parse(json);
            const rawDCPList = parsedData.estacao;

            let numEstacoes = 0;

            for (const rawDcp of rawDCPList) {
              if(this.idtipoestacao && !this.idtipoestacao.includes(rawDcp[this.semantics.station_type_id_property]))
                continue;

              if(ufs && !ufs.includes(rawDcp[this.semantics.uf_property]))
                continue;

              const dcp = {
                // metadata: {
                  ...rawDcp,
                  alias: rawDcp[this.semantics.dcp_code_property],
                  lat: rawDcp[this.semantics.latitude_property],
                  long: rawDcp[this.semantics.longitude_property],
                // }
              }

              dcpList.push(dcp);
            }

            return resolve(dcpList);
          } catch (error) {
            return reject(error);
          }
        });
      }).on('error', error => reject(error));
    });
  }
}

const cemaden = new Cemaden();

module.exports = cemaden;