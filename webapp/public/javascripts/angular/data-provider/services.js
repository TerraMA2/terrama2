define([
  "TerraMA2WebApp/common/services/index"
], function(commonServiceApp) {
  'use strict';
 
  /**
   * TerraMA² Data Provider DAO
   * 
   * @class DataProviderService
   * 
   * @property {BaseService} BaseService - TerraMA² Base Service DAO
   * @property {string} url - TerraMA² API URL
   * @property {DataProvider[]} model - TerraMA² cached list  
   */
  function DataProviderService(BaseService) {
    this.BaseService = BaseService;
    this.url = "/api/DataProvider";
    this.model = [];
  }

  // Angular Dependencies Injector
  DataProviderService.$inject = ["BaseService"];

  /**
   * It prepares data provider service cache, loading all data providers with given restriction from remote host
   * 
   * @param {Object} restriction - A query restriction
   * @returns {angular.IPromise<Object[]>}
   */
  DataProviderService.prototype.init = function(restriction) {
    var defer = this.BaseService.$q.defer();
    var self = this;
    this.BaseService
      .$request(this.url, "GET", {params: restriction})
      .then(function(response) {
        self.model = response.data;
        return defer.resolve(response.data);
      })
      .catch(function(err) {
        return defer.reject(err);
      });
    return defer.promise;
  };

  /**
   * It performs a filter from cached data providers
   * 
   * @param {Object} restriction - A query restriction
   * @returns {DataProvider[]}
   */
  DataProviderService.prototype.list = function(restriction) {
    return this.BaseService.$list(this.model, restriction);
  };

  DataProviderService.$inject = ["BaseService"];

  var moduleName = "terrama2.providers.services";

  angular.module(moduleName, [commonServiceApp])
    .service('DataProviderService', DataProviderService);

  return moduleName;
});