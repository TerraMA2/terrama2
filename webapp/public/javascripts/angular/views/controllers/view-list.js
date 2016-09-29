(function() {
  'use strict';

  angular.module("terrama2.views.controllers.viewlist", [
    "terrama2",
    "terrama2.views.services",
    "terrama2.table",
    "terrama2.components.messagebox.services",
    "terrama2.components.messagebox"])
  .controller("ViewList", ViewList);


  function ViewList($scope, i18n, ViewService, $log, MessageBoxService, $window) {
    var self = this;

    // getting config from template
    var config = $window.configuration || config;

    self.ViewService = ViewService;

    self.MessageBoxService = MessageBoxService;

    // token message
    if (config.message) {
      self.MessageBoxService.success(i18n.__("View"), config.message);
    }

    /**
     * It represents a cached views
     * @type {Object[]}
     */
    self.model = [];

    /**
     * Fields to display in table
     * @type {Object[]}
     */
    self.fields = [{
      key: "name",
      as: i18n.__("Name")
    }, {
      key: "description",
      as: i18n.__("Description")
    }];

    /**
     * A URL to insert a new view
     * @type {string}
     */
    self.linkToAdd = "/configuration/views/new";
    self.link = function(object) {
      return "/configuration/views/edit/" + object.id;
    };

    /**
     * Icon properties to display in table like size, type (img/icon)
     * @type {Object}
     */
    self.iconProperties = {
      type: "img",
      width: 64,
      height: 64
    };

    /**
     * Icon functor to make URL pointing to icon.
     * 
     * @todo It should retrive a overview of layer
     * @returns {string}
     */
    self.icon = function(object) {
      return "/images/map-display.png";
    }

    /**
     * It retrieves all views registered
     */
    self.ViewService.list()
      .then(function(views) {
        self.model = views;
      })

      .catch(function(err) {
        $log.info("Could not list views " + err.toString());
      });
  }

  ViewList.$inject = ["$scope", "i18n", "ViewService", "$log", "MessageBoxService", "$window"];
} ());