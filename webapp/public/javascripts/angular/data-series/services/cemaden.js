define([], () => {
  /**
   * Class responsible to communicate with backend in order to retrieve Cemaden DCPs
   */
  class CemadenService {
    // Injectors
    constructor($http, $q) {
      this.$http = $http;
      this.$q = $q;
    }

    /**
     * Retrieves Cemaden DCPS from backend
     *
     * @param {string|string[]} states
     * @param {number} dataProviderId
     * @param {number[]} stations
     * @returns {Promise<Array<any>>} Promise of DCPs
     */
    listDCP(states, dataProviderId, stations) {
      let defer = this.$q.defer();

      if (!states)
        states = [];

      this.$http.get('/api/Cemaden', { params: { states: states.join(), provider: dataProviderId, stations } })
        .then(response => defer.resolve(response.data))
        .catch(err => {
          defer.reject(new Error((err.data.error || "Invalid DCP Cemaden")))
        });

      return defer.promise;
    }
  }

  CemadenService.$inject = ['$http', '$q'];

  return CemadenService;
})