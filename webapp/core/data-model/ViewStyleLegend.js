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
     * ViewStyleLegend Type identifier
     * @name ViewStyleLegend#typeId
     * @type {number}
     */
    this.typeId = params.type_id || params.typeId;
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
     * It defines the ViewStyleLegend colors used
     * @name ViewStyleLegend#colors
     * @type {any}
     */
    this.colors = params.colors || [];
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

  /**
   * It builds a standardized TCP format
   * 
   * @returns {Object}
   */
  ViewStyleLegend.prototype.toObject = function() {
    return Object.assign(AbstractClass.prototype.toObject.call(this), {
      id: this.id,
      column: this.column || null,
      type_id: this.typeId,
      colors: this.colors,
      band_number: isNumber(this.band_number) ? this.band_number : null
    });
  };

  module.exports = ViewStyleLegend;
} ());