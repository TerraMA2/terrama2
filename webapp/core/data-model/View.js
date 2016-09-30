(function() {
  'use strict';

  // Dependency
  var AbstractClass = require("./AbstractData");
  var URIBuilder = require("./../UriBuilder");
  var RegisteredView = require("./RegisteredView");

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
     * View description
     * @type {string}
     */
    this.description = params.description;
    /**
     * View server URI. The server may be a GeoServer
     * @type {string}
     */
    this.uri = params.uri;
    /**
     * View style script
     * @type {string}
     */
    this.script = params.script;
    /** 
     * Registered view retrieved from tcp or database
     * @type {RegisteredView}
     */
    this.registeredView = {};
    /**
     * Data series associated
     * @type {DataSeries}
     */
    this.dataSeries = params.data_series_id;
    /**
     * Schedule associated
     * @type {Schedule}
     */
    this.schedule = params.schedule || {};
  }

  View.prototype = Object.create(AbstractClass.prototype);
  View.prototype.constructor = View;
  /**
   * It builds a database representation of View
   */
  View.prototype.rawObject = function() {
    return this.toObject();
  }

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
      uri: this.uri,
      script: this.script,
      data_series_id: this.dataSeries,
      schedule: this.schedule instanceof AbstractClass ? this.schedule.toObject() : this.schedule
    });
  };

  module.exports = View;
} ());