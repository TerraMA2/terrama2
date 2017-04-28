(function() {
  'use strict';

  // Dependency
  var AbstractClass = require("./AbstractData");

  /**
   * It defines a TerraMA² View representation. 
   * @class View
   */
  function View(params) {
    // Calling base abstraction
    AbstractClass.call(this, {'class': 'View'});
    /**
     * View identifier
     * @type {number}
     */
    this.id = params.id;
    /**
     * View name
     * @type {string}
     */
    this.name = params.name;
    /**
     * TerraMA² Project identifier
     * @type {number}
     */
    this.projectId = params.project_id || params.projectId;
    /**
     * View description
     * @type {string}
     */
    this.description = params.description;
    /**
     * View style script
     * @type {string}
     */
    this.style = params.style;
    /** 
     * Registered view retrieved from tcp or database
     * @type {RegisteredView}
     */
    this.registeredView = {};
    /**
     * Data series associated
     * @type {DataSeries}
     */
    this.dataSeriesId = params.data_series_id;

    this.dataSeries = params.dataSeries || {};
    /**
     * Schedule type associated
     * @type {Schedule}
     */
    this.scheduleType = params.schedule_type;
    /**
     * Schedule associated
     * @type {Schedule}
     */
    this.schedule = params.schedule || {};
    /**
     * Conditional Schedule associated
     * @type {Schedule}
     */
    this.conditionalSchedule = params.conditionalSchedule || {};
    /**
     * View state
     * @type {boolean}
     */
    this.active = params.active;
    /**
     * TerraMA² Service Instance Identifier
     * @type {number}
     */
    this.serviceInstanceId = params.service_instance_id || params.serviceInstanceId;
    /**
     * It defines View Legend
     * @type {any}
     */
    this.legend = params.legend || null;
  }

  View.prototype = Object.create(AbstractClass.prototype);
  View.prototype.constructor = View;

  View.prototype.setLegend = function(legend) {
    this.legend = legend;
  };

  /**
   * It builds a database representation of View
   */
  View.prototype.rawObject = function() {
    var obj = this.toObject();
    delete obj.dataseries_id;
    obj.data_series_id = this.dataSeriesId;
    return obj;
  };

  /**
   * It builds a standardized TCP format
   * 
   * @returns {Object}
   */
  View.prototype.toObject = function() {
    return Object.assign(AbstractClass.prototype.toObject.call(this), {
      id: this.id,
      name: this.name,
      description: this.description,
      style: this.style,
      dataseries_id: this.dataSeriesId,
      dataSeries: this.dataSeries instanceof AbstractClass ? this.dataSeries.toObject() : {},
      schedule: this.schedule instanceof AbstractClass ? this.schedule.toObject() : {},
      conditional_schedule: this.conditionalSchedule instanceof AbstractClass ? this.conditionalSchedule.toObject() : {},
      active: this.active,
      service_instance_id: this.serviceInstanceId,
      project_id: this.projectId,
      legend: this.legend instanceof AbstractClass ? this.legend.toObject() : this.legend,
      schedule_type: this.scheduleType
    });
  };

  module.exports = View;
} ());