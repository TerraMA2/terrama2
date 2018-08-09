const http = require('http');
const Promise = require('./../Promise');
const SemanticsFactory = require('./Factory');
const URL = require('url');

/**
 * Class responsible to retrieve and to parse Cemaden DCPS from remote server.
 *
 * Requires semantics "DCP-json_cemaden" loaded. Otherwise, an exception will be thrown
 */
class Cemaden {
  constructor() {
    this.semantics = SemanticsFactory.build({ code: "DCP-json_cemaden" });
  }

  /**
   * Communicate with Cemaden API Server. Parses the Cemaden data to TerraMA² DCPS
   *
   * @param {string[]} ufs List of brazilian states to filter
   * @param {DataProvider} dataProvider DataProvider credentials for download
   * @param {string[]} types List of Stations to retrieve
   * @returns {Promise<Array<any>>} Promise of DCPs
   */
  listDCP(ufs, dataProvider, types) {
    return new Promise((resolve, reject) => {
      if (!dataProvider && !dataProvider.hasOwnProperty("uri"))
        return reject(new Error("No URI set in DataProvider"));

      const uri = dataProvider.uri + this.semantics.metadata.dcp_list_uri;
      http.get(uri, (res) => {
        let rawData = '';

        res.on('data', (chunk) => { rawData += chunk; });

        res.on('end', () => {
          try {
            let dcpList = [];
            let stationIds = [];
            if (!Array.isArray(types))
              stationIds.push(types);
            else
              stationIds = types;

            const json = rawData.slice(10, rawData.length - 2);
            const parsedData = JSON.parse(json);
            const rawDCPList = parsedData.estacao;

            for (const rawDcp of rawDCPList) {
              if (stationIds && !stationIds.includes(rawDcp[this.semantics.metadata.station_type_id_property]))
                continue;

              if (ufs && !ufs.includes(rawDcp[this.semantics.metadata.uf_property]))
                continue;

              const dcp = {
                ...rawDcp,
                alias: rawDcp[this.semantics.metadata.dcp_code_property],
                lat: rawDcp[this.semantics.metadata.latitude_property],
                long: rawDcp[this.semantics.metadata.longitude_property],
              }

              dcpList.push(dcp);
            }

            return resolve(dcpList);
          } catch (error) {
            const uriObject = new URL.parse(uri);
            const url = uriObject.protocol + "//" + uriObject.host + uriObject.pathname;
            return reject(new Error("Invalid DCP Cemaden. Is it a valid Cemaden DataProvider? - " + url));
          }
        });
      }).on('error', error => reject(error));
    });
  }
}

module.exports = Cemaden;
