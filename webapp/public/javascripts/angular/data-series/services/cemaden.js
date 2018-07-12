define([], () => {
  class CemadenService {
    constructor($http, $q) {
      this.$http = $http;
      this.$q = $q;
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