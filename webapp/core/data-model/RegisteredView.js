(function() {
  'use strict';

  // Dependencies
  var AbstractClass = require("./AbstractData");
  var Utils = require("./../Utils");
  var View = require("./View");
  var URIBuilder = require("./../UriBuilder");
  var DataSeriesType = require("./../Enums").DataSeriesType;
  var ViewSourceType = require("./../Enums").ViewSourceType;
  /**
   * Default URI syntax
   * @type {Enums.Uri}
   */
  var URISyntax = require("./../Enums").Uri;

  /**
   * @param {Object} params
   * @param {number} params.id
   * @param {string} params.workspace
   * @param {Sequelize.Model[]} params.layers
   */
  function RegisteredView(params) {
    AbstractClass.call(this, {'class': 'RegisteredView'});
    /**
     * Registered View identifier
     * @type {number}
     */
    this.id = params.id;
    /**
     * It defines a maps server workspace. (GeoServer)
     * @type {string}
     */
    this.workspace = params.workspace;
    /**
     * It defines a base URI of layers retrieved from maps server handler
     * @type {string}
     */
    this.uri = params.uri;
    /**
     * It defines a list of layers registered
     * @type {string[]}
     */
    this.layers = [];
    /**
     * It defines a TerraMA² View object parent
     * @type {View}
     */
    this.view = null;
    /**
     * It defines a TerraMA² Data Series Object linked from View
     * @type {DataSeries}
     */
    this.dataSeries = null;
    /**
     * It defines Data Series Type (static, dynamic, analysis)
     * @type {string}
     */
    this.dataSeriesType = null;
    /**
     * It defines a URI object struct.
     * @type {Object}
     * @private
     */
    this.$uriObject = {};

    // setting layers
    this.setLayers(params.Layers || params.layers);
    // setting parent view
    this.setView(params.View || params.view);
  }
  // Javascript Object Inheritance Way
  RegisteredView.prototype = Object.create(AbstractClass.prototype);
  RegisteredView.prototype.constructor = RegisteredView;

  /**
   * It sets layers to cache.
   * @todo Improve this doc
   * @param {Sequelize.Model[]|Object[]}
   */
  RegisteredView.prototype.setLayers = function(layers) {
    var output = [];
    layers.forEach(function(layer) {
      var obj = layer;
      if (Utils.isFunction(layer.get)) {
        obj = layer.get();
      }
      output.push(obj);
    });
    this.layers = output;
  };
  /**
   * It sets the data series type of View
   *
   * @todo Improve it.
   * @param {string} dsType - Data Series Type
   */
  RegisteredView.prototype.setDataSeriesType = function(dsType) {
    switch(dsType){
      case ViewSourceType.STATIC:
        this.dataSeriesType = "static";
        break;
      case ViewSourceType.DYNAMIC:
        this.dataSeriesType = "dynamic";
        break;
      case ViewSourceType.ANALYSIS:
        this.dataSeriesType = "analysis";
        break;
      case ViewSourceType.ALERT:
        this.dataSeriesType = "alert";
        break;
    }
  };
  /**
   * It sets TerraMA² Data Series object to the RegisteredView instance
   *
   * @param {DataSeries} dataSeries - A TerraMA² Data Series model built
   */
  RegisteredView.prototype.setDataSeries = function(dataSeries) {
    this.dataSeries = dataSeries;
  };
  /**
   * It sets parent view
   *
   * @throws {Error} When view is invalid
   * @param {Sequelize.Model|View} view - TerraMA² View
   */
  RegisteredView.prototype.setView = function(view) {
    var viewObj = view;
    if (Utils.isFunction(view.get)) {
      viewObj = view.get();
    }
    this.view = new View(viewObj);
  };
  /**
   * Get real representation of RegisteredView
   *
   * @returns {Object}
   */
  RegisteredView.prototype.rawObject = function() {
    return this.toObject();
  };
  /**
   * Retrieves a standard representation used in TCP communication
   *
   * @returns {Object}
   */
  RegisteredView.prototype.toObject = function() {
    var uriObject = URIBuilder.buildObject(this.uri, URISyntax);
    var uri = Utils.format("%s://%s:%s%s", uriObject[URISyntax.SCHEME].toLowerCase(),
                                         uriObject[URISyntax.HOST],
                                         uriObject[URISyntax.PORT],
                                         uriObject[URISyntax.PATHNAME]);

    var params = {};
    if (this.dataSeries) {
      var semantics = this.dataSeries.data_series_semantics;
      if (semantics && semantics.data_series_type_name === DataSeriesType.GRID) {
        var mask = this.dataSeries.dataSets[0].format.mask;
        params.mask = mask;
      }
    }

    return Object.assign(AbstractClass.prototype.toObject.call(this), {
      id: this.id,
      name: this.view.name,
      private: this.view.private,
      workspace: this.workspace,
      uriGeoserver: uri,
      layers: this.layers.map(function(layer) { return layer.name; }),
      user: this.$uriObject[URISyntax.USER],
      password: this.$uriObject[URISyntax.PASSWORD],
      serverType: "geoserver", // TODO: change it. It should be received from c++ service or even during view registration
      type: this.dataSeriesType,
      params: params
    });
  };

  module.exports = RegisteredView;
} ());
