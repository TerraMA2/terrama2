define(function(){
  function InterpolatorService(BaseService, $q){
    var self = this;
    this.BaseService = BaseService;
    this.$q = $q;
    this.$baseUrl = BASE_URL + "api/Interpolator";
    this.model = [];

    /**
     * It retrieves all Interpolators and cache them in model.
     * 
     * @param {Object} restriction
     * @returns {angular.IPromise<Object[]>}
     */
    this.init = function(restriction){
      var defer = self.BaseService.$q.defer();

      self.BaseService.$request(self.$baseUrl, "GET", {params: restriction})
        .then(function(response){
          self.model = response.data;
          return defer.resolve(response.data);
        });

      return defer.promise;
    };

    /**
     * It performs an Interpolator creation on API call and stores in cache
     * 
     * @param {Object} interpolatorObject - An Interpolator values
     * @returns {ng.IPromise}
     */
    this.create = function(interpolatorObject){
      var defer = self.BaseService.$q.defer();

      self.BaseService.$request(self.$baseUrl, "POST", {
        data: interpolatorObject,
        headers: {
          "Content-Type": "application/json"
        }
      })
      .then(function(response){
        self.model.push(response.data);
        return defer.resolve(response.data);
      })
      .catch(function(err){
        var errMessage = "Error creating Interpolator. \n" + (err.data ? err.data.message : "");
        return defer.reject(errMessage);
      });

      return defer.promise;
    };

    /**
     * It performs an interpolator update on API call and stores in cache
     * 
     * @param {number} interpolatorId - An interpolator identifier
     * @param {Object} interpolatorObject - An interpolator values
     * @returns {ng.IPromise}
     */
    this.update = function(interpolatorId, interpolatorObject){
      var defer = self.BaseService.$q.defer();

      self.BaseService.$request(self.$baseUrl + "/" + interpolatorId, "PUT", {
        data: interpolatorObject
      })
      .then(function(response){
        return defer.resolve(response.data);
      })
      .catch(function(err){
        var errMessage = "Error updating Interpolator. \n" + (err.data ? err.data.message : "");
        return defer.resolve(errMessage);
      });
      return defer.promise;
    };

    /**
     * It list interpolators on API call.
     * 
     * @param {Object} viewObject - A view values
     * @returns {ng.IPromise}
     */
    this.list = function(restriction){
      return self.BaseService.$filter('filter')(self.model, restriction);
    };

  }

  InterpolatorService.$inject = ["BaseService", "$q"];
  
  return InterpolatorService;
});