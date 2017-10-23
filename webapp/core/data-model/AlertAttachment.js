(function() {
  'use strict';

  // Dependencies

  /**
   * TerraMA² BaseClass of Data Model
   * @type {AbstractData}
   */
  var BaseClass = require("./AbstractData");
  var URIBuilder = require("./../UriBuilder");
  var URISyntax = require("./../Enums").Uri;
  var Utils = require("./../Utils");

  /**
   * TerraMA² AlertAttachment Data Model representation
   * 
   * @class AlertAttachment
   */
  var AlertAttachment = function(params) {
    BaseClass.call(this, {'class': 'AlertAttachment'});
    /**
     * AlertAttachment#id
     * @type {number}
     */
    this.id = params.id;
    /**
     * @name AlertAttachment#y_max
     * @type {number}
     */
    this.y_max = params.y_max;
    /**
     * @name AlertAttachment#y_min
     * @type {number}
     */
    this.y_min = params.y_min;
    /**
     * @name AlertAttachment#x_max
     * @type {number}
     */
    this.x_max = params.x_max;
    /**
     * @name AlertAttachment#x_min
     * @type {number}
     */
    this.x_min = params.x_min;
    /**
     * @name AlertAttachment#srid
     * @type {number}
     */
    this.srid = params.srid;
    /**
     * @name AlertAttachment#alert_id
     * @type {number}
     */
    this.alert_id = params.alert_id;
    /**
     * @name Alert#attachedViews
     * @type {array}
     */
    this.attachedViews = params.AlertAttachedViews || [];
  };

  AlertAttachment.prototype = Object.create(BaseClass.prototype);
  AlertAttachment.prototype.constructor = AlertAttachment;

  AlertAttachment.prototype.setAttachedViews = function(attachedViews) {
    this.attachedViews = attachedViews;
  };

  AlertAttachment.prototype.toObject = function() {
    var attachedViewsArray = [];

    this.attachedViews.forEach(function(attachedView) {
      attachedViewsArray.push({
        id: attachedView.dataValues.id,
        layer_order: attachedView.dataValues.layer_order,
        alert_attachment_id: attachedView.dataValues.alert_attachment_id,
        view_id: attachedView.dataValues.view_id
      });
    });

    return Object.assign(BaseClass.prototype.toObject.call(this), {
      id: this.id,
      y_max: this.y_max,
      y_min: this.y_min,
      x_max: this.x_max,
      x_min: this.x_min,
      srid: this.srid,
      alert_id: this.alert_id,
      attachedViews: attachedViewsArray
    });
  };

  AlertAttachment.prototype.rawObject = function() {
    var toObject = this.toObject();
    return toObject;
  };

  AlertAttachment.prototype.toService = function() {
    var viewObject = {
      ymax: this.y_max,
      ymin: this.y_min,
      xmax: this.x_max,
      xmin: this.x_min,
      srid: this.srid
    };

    if(this.attachedViews.length > 0) {
      var originalUri = null;
      var serviceAttachedViews = [];

      for(var i = 0, serviceMetadataLength = this.attachedViews[0].View.ServiceInstance.ServiceMetadata.length; i < serviceMetadataLength; i++) {
        if(this.attachedViews[0].View.ServiceInstance.ServiceMetadata[i].dataValues.key === "maps_server") {
          originalUri = this.attachedViews[0].View.ServiceInstance.ServiceMetadata[i].dataValues.value;
          break;
        }
      }

      if(originalUri) {
        var uriObject = URIBuilder.buildObject(originalUri, URISyntax);

        if(!isNaN(uriObject[URISyntax.PORT])) {
          var uri = Utils.format(
            "%s://%s:%s%s",
            uriObject[URISyntax.SCHEME].toLowerCase(),
            uriObject[URISyntax.HOST],
            uriObject[URISyntax.PORT],
            uriObject[URISyntax.PATHNAME]
          );
        } else {
          var uri = Utils.format(
            "%s://%s%s",
            uriObject[URISyntax.SCHEME].toLowerCase(),
            uriObject[URISyntax.HOST],
            uriObject[URISyntax.PATHNAME]
          );
        }

        viewObject.geoserver_uri = uri + "/ows";
      } else {
        viewObject.geoserver_uri = null;
      }

      for(var i = 0, attachedViewsLength = this.attachedViews.length; i < attachedViewsLength; i++) {
        serviceAttachedViews.push(
          {
            view_id: this.attachedViews[i].View.dataValues.id,
            workspace: "terrama2_" + this.attachedViews[i].View.dataValues.id // It's hardcoded now, but that isn't right, in the future this should come from the database
          }
        );
      }

      viewObject.layers = serviceAttachedViews;
    }

    return Object.assign({}, viewObject);
  }

  module.exports = AlertAttachment;
} ());