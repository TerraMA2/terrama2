(function() {
  'use strict';

  angular.module("terrama2.dataseries.services", ["terrama2"])
    .service("DataSeriesService", DataSeriesService);
  
  /**
   * Data Series service DAO
   * 
   * @class DataSeriesService
   * @param {BaseService} BaseService - A TerraMA² base service request
   */
  function DataSeriesService(BaseService) {
    this.BaseService = BaseService;
    this.url = "/api/DataSeries";
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
  DataSeriesService.$inject = ["BaseService"];
} ());