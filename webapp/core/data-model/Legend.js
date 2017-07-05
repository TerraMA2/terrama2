(function() {
  'use strict';

  // Dependencies

  /**
   * TerraMA² BaseClass of Data Model
   * @type {AbstractData}
   */
  var BaseClass = require('./AbstractData');
  /**
   * TerraMA² Global Utility module
   * @type {Utils}
   */
  var Utils = require("./../Utils");

  /**
   * TerraMA² Legend Data Model representation
   * 
   * @class Legend
   */
  var Legend = function(params) {
    BaseClass.call(this, {'class': 'Legend'});
    /**
     * Legend Identifier
     * @type {number}
     */
    this.id = params.id;
    /**
     * Project Identifier
     * @type {number}
     */
    this.project_id = params.project_id;
    /**
     * Legend name
     * @type {string}
     */
    this.name = params.name;
    /**
     * Legend description
     * @type {string}
     */
    this.description = params.description;

    this.levels = [];


    if (params.levels || params.legendLevels){
      this.levels = params.levels || params.legendLevels;
    } else if (params.LegendLevels) {
      var levelsList = [];
      params.LegendLevels.forEach(function(level){
        levelsList.push(level.get());
      });
      this.levels = levelsList;
    }

  };

  /**
   * It sets legend levels data.
   * @param {Sequelize.Model[]|Object[]}
   */
  Legend.prototype.setLegendLevels = function(LegendLevels) {
    var output = [];
    LegendLevels.forEach(function(LegendLevel) {
      var obj = LegendLevel;
      if (Utils.isFunction(LegendLevel.get)) {
        obj = LegendLevel.get();
      }
      output.push(obj);
    });
    this.levels = output;
  };
  Legend.prototype = Object.create(BaseClass.prototype);
  Legend.prototype.constructor = Legend;

  Legend.prototype.toObject = function() {
    return Object.assign(BaseClass.prototype.toObject.call(this), {
      id: this.id,
      project_id: this.project_id,
      name: this.name,
      description: this.description,
      levels: this.levels
    });
  };

  Legend.prototype.toService = function() {
    var levelsCopy =[];
    //Removing unnecessary properties to send to service
    this.levels.forEach(function(level){
      delete level.id;
      delete level.legend_id;
      levelsCopy.push(level);
    });
    return Object.assign(BaseClass.prototype.toObject.call(this), {
      id: this.id,
      name: this.name,
      project_id: this.project_id,
      description: this.description,
      levels: levelsCopy
    });
  };

  Legend.prototype.rawObject = function() {
    var toObject = this.toObject();
    return toObject;
  };

  module.exports = Legend;

} ());