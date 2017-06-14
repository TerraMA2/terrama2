(function() {
  'use strict';

  // Dependency
  var AbstractClass = require("./AbstractData");
  var isNumber = require("./../Utils").isNumber;

  /**
   * It defines a TerraMA² ViewStyleLegend Legend representation. 
   * @class ViewStyleLegend
   */
  function ViewStyleLegend(params) {
    // Calling base abstraction
    AbstractClass.call(this, {'class': 'ViewStyleLegend'});
    /**
     * ViewStyleLegend identifier
     * @name ViewStyleLegend#name
     * @type {number}
     */
    this.id = params.id;
    /**
     * ViewStyleLegend column
     * @name ViewStyleLegend#column
     * @type {string}
     */
    this.column = params.column;
    /**
     * ViewStyleLegend raster band number
     * @name ViewStyleLegend#band_number
     * @type {string}
     */
    this.band_number = params.band_number;
    /**
     * ViewStyleLegend type (Gradient, Interval and Value)
     * @name ViewStyleLegend#type
     * @type {string}
     */
    this.type = params.type;
    /**
     * ViewStyleLegend dummy value for raster cells
     * @name ViewStyleLegend#dummy
     * @type {number}
     */
    this.dummy = params.dummy;
    /**
     * It defines the ViewStyleLegend colors used
     * @name ViewStyleLegend#colors
     * @type {any}
     */
    this.colors = params.colors || [];
    this.metadata = params.metadata || {};
  }

  ViewStyleLegend.prototype = Object.create(AbstractClass.prototype);
  ViewStyleLegend.prototype.constructor = ViewStyleLegend;

  ViewStyleLegend.prototype.addColor = function(color) {
    this.colors.push(color);
  };

  /**
   * It builds a database representation of ViewStyleLegend
   */
  ViewStyleLegend.prototype.rawObject = function() {
    return this.toObject();
  };

  ViewStyleLegend.prototype.setMetadata = function(formats) {
    var formatOutput = {};
    if (formats instanceof Array) {
      formats.forEach(function(format) {
        formatOutput[format.key] = format.value;
      });
    } else {
      formatOutput = formats;
    }  
    this.metadata = formatOutput;
  };

  /**
   * It builds a standardized TCP format
   * 
   * @returns {Object}
   */
  ViewStyleLegend.prototype.toObject = function() {
    return Object.assign(AbstractClass.prototype.toObject.call(this), {
      id: this.id,
      column: this.column || null,
      type: this.type,
      colors: this.colors,
      dummy: this.dummy,
      metadata: this.metadata,
      band_number: isNumber(this.band_number) ? this.band_number : null
    });
  };

  module.exports = ViewStyleLegend;
} ());