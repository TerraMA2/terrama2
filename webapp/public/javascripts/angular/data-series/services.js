(function() {
  'use strict';

  angular.module("terrama2.dataseries.services", ["terrama2"])
    /**
     * It defines a TerraMA² Data Series Type available.
     * 
     * @readonly
     * @enum {string}
     */
    .constant("DataSeriesType", {
      DCP: 'DCP',
      OCCURRENCE: 'OCCURRENCE',
      GRID: 'GRID',
      ANALYSIS_MONITORED_OBJECT: 'ANALYSIS_MONITORED_OBJECT',
      GEOMETRIC_OBJECT: 'GEOMETRIC_OBJECT',
      POSTGIS: 'POSTGIS'
    })
    .service("DataSeriesService", DataSeriesService);
  
  /**
   * Data Series service DAO
   * 
   * @class DataSeriesService
   * @param {BaseService} BaseService - A TerraMA² base service request
   * @param {DataSeriesType} DataSeriesType - A const TerraMA² enum for handling data series type
   */
  function DataSeriesService(BaseService, DataSeriesType) {
    this.BaseService = BaseService;
    this.url = "/api/DataSeries";
    this.DataSeriesType = DataSeriesType
  }

  /**
   * It retrieves all data series from API
   * 
   * @param {Object} restriction - a query restriction
   * @returns {ng.IPromise}
   */
  DataSeriesService.prototype.list = function(restriction) {
    return this.BaseService.$request(this.url, "GET", {params: restriction});
  };

  /**
   * It creates a Data Series on remote API
   * 
   * @param {Object} dataSeriesObject - A data series object values to save
   * @returns {ng.IPromise}
   */
  DataSeriesService.prototype.create = function(dataSeriesObject) {
    return this.BaseService.$request(this.url, "POST", {
      data: dataSeriesObject
    });
  };

  // Angular Injecting Dependency
  DataSeriesService.$inject = ["BaseService", "DataSeriesType"];
} ());