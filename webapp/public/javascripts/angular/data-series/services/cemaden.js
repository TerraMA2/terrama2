define([], () => {
  class CemadenService {
    constructor($http, $q) {
      this.$http = $http;
      this.$q = $q;
      this.url = '';
      this.semantics = {
        dcp_list_uri: '/resources/dados/311_1.json',
        latitude_property: 'latitude',
        longitude_property: 'longitude',
        station_type_id_property: 'idtipoestacao',
        dcp_code_property: 'codestacao',
        uf_property: 'uf',
      }
    }

    parse(text) {
      const json = text.slice(10, text.length - 2);
      const parsedData = JSON.parse(json);
      const rawDCPList = parsedData.estacao;

      let numEstacoes = 0;
      let dcpList = [];
      for (const rawDcp of rawDCPList) {
        if(idtipoestacao && !idtipoestacao.includes(rawDcp[this.semantics.station_type_id_property]))
          continue;

        if(uf && !uf.includes(rawDcp[this.semantics.uf_property]))
          continue;

        const dcp = {
          metadata: {
            alias: rawDcp[this.semantics.dcp_code_property],
            lat: rawDcp[this.semantics.latitude_property],
            long: rawDcp[this.semantics.longitude_property],
          }
        };

        Object.assign(dcp.metadata, rawDcp);

        dcpList.push(dcp);
      }

      return dcpList;
    }

    listDCP(states) {
      let defer = this.$q.defer();

      if (!states)
        states = [];

      this.$http.get('/api/Cemaden', { params: { states: states.join() } })
        .then(response => defer.resolve(response.data))
        .catch(err => defer.reject(err));

      return defer.promise;
    }
  }

  CemadenService.$inject = ['$http', '$q'];

  return CemadenService;
})