(function() {
  'use strict';

  angular.module('terrama2.analysis.services', ['terrama2'])
    .constant("AnalysisType", {
      DCP: 1,
      MONITORED: 2,
      GRID: 3
    })
    .service("AnalysisOperators", ["BaseService", "$log", function(BaseService, $log) {
      var self = this;
      this.$data = {};

      this.init = function() {
        return BaseService.$request("/javascripts/angular/analysis/data/operators.json", "GET", {})
          .then(function(data) {
            self.$data = data;
          })
          .catch(function(err) {
            $log.log(err);
            throw error;
          });
      }
    }])
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
  AnalysisService.prototype.init = function(restriction) {
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