(function() {
  'use strict';

  angular.module('terrama2.analysis.services', ['terrama2'])
    .constant("AnalysisType", {
      DCP: 1,
      MONITORED: 2,
      GRID: 3
    })
    .service("AnalysisService", AnalysisService);
  
  /**
   * @class AnalysisService
   */
  function AnalysisService(BaseService, AnalysisType) {
    /**
     * It defines a cached analysis list retrieved from remote host or even created
     * @type {Analysis[]}
     */
    this.model = [];
    /**
     * It defines target API URL
     * @type {string} 
     */
    this.url = "/api/Analysis";
    /**
     * It defines enum for Analysis 
     * @type {AnalysisType}
     */
    this.types = AnalysisType;
    /**
     * It defines a TerraMA² base dao
     * @type {BaseService}
     */
    this.BaseService = BaseService;
  }
  /**
   * It initializes Analysis DAO service, retrieving analysis instances and caching them
   * 
   * @param {Object} restriction - A query restriction
   * @returns {angular.Promise<Analysis[]>}
   */
  AnalysisService.init = function(restriction) {
    return this.BaseService.$request(this.url, "GET", {params: restriction});
  };

  /**
   * It retrives cached analysis. You can filter given query restriction.
   * 
   * @example
   * AnalysisService
   *   .init()
   *   .then((data) => {
   *     console.log(AnalysisService.list({type_id: AnalysisType.GRID})); // it will display all grid analysis 
   *   })
   * 
   *  
   * @param {Object} restriction - A query restriction
   * @returns {Analysis[]}
   */
  AnalysisService.prototype.list = function(restriction) {
    return this.BaseService.$list(this.model, restriction);
  };

  AnalysisService.prototype.get = function(restriction) {
    return this.BaseService.get(this.model, restriction);
  };

  /**
   * It sends a built analysis in order to validate in backend
   * 
   * @param {Object} analysisObject - A javascript object with analysis values
   * @returns {angular.IPromise<any>} Response object with status code to identify state mode
   */
  AnalysisService.prototype.validate = function(analysisObject) {
    return this.BaseService.$request(this.url + "/validate", "POST", {data: analysisObject});
  };

  /**
   * It creates a analysis resource on remote host using API POST and then retrieve the resource created.
   * 
   * @param {Object} analysisObject - A javascript object with analysis values
   * @returns {angular.IPromise<Analysis>}
   */
  AnalysisService.prototype.create = function(analysisObject) {
    var defer = this.BaseService.$q.defer();
    var self = this;
    this.BaseService
      .$request(this.url, "POST", {data: analysisObject})
      .then(function(response) {
        self.model.push((response || {}).result);
        return defer.resolve(response);
      })
      .catch(function(err) {
        return defer.reject(err);
      });
    return defer.promise;
  };

  /**
   * It performs update analysis on remote host using TerraMA² PUT API
   * @todo Update cached analysis
   */
  AnalysisService.prototype.update = function(analysisId, analysisObject) {
    return this.BaseService.$request(this.url + "/" + analysisId, "PUT", {data: analysisObject});
  };

  AnalysisService.$inject = ["BaseService", "AnalysisType"];
} ());