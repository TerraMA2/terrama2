define(function(){
  function AlertService(BaseService, $q){
    var self = this;
    this.BaseService = BaseService;
    this.$q = $q;
    this.$baseUrl = "/api/Alert";
    this.model = [];

    /**
     * It performs a alert creation on API call and stores in cache
     * 
     * @param {Object} alertObject - A alert values
     * @returns {ng.IPromise}
     */
    this.create = function(alertObject) {
      var defer = self.$q.defer();
      self.BaseService.$request(self.$baseUrl, "POST", {
        data: alertObject,
        headers: {
          "Content-Type": "application/json"
        }
      })
      .then(function(response) {
        self.model.push(response.data);
        return defer.resolve(response.data);
      })
      .catch(function(err) {
        return defer.reject(err);
      })

      return defer.promise;
    };
  }

  AlertService.$inject = ["BaseService", "$q"];

  return AlertService;
});