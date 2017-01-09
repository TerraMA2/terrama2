define([], function() {
  /**
   * It retrieves all fields available from specific implementation to display in GUI interface
   * 
   * @class FieldRetriever
   * @property {BaseService} BaseService - TerraMA² Base Request module
   * @property {angular.IQService} $q - Angular promiser module
   */
  function FieldRetriever(BaseService, $q) {
    this.BaseService = BaseService;
    this.$q = $q;
  }

  FieldRetriever.prototype.retrieve = function(params) {
    var defer = this.$q.defer();

    this.BaseService.$request("/uri", "GET", {params: params})
      .then(function(response) {
        return defer.resolve(response.data.data);
      })
      .catch(function(response) {
        return defer.reject(response.data);
      });

    return defer.promise;
  };

  FieldRetriever.$inject = ["BaseService", "$q"];

  return FieldRetriever;
});