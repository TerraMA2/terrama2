define([], function(){
  "use strict";

  function AlertList($scope, i18n, $q, AlertService){
    var self = this;
    self.i18n = i18n;

    self.AlertService = AlertService;
    self.model = [
      {
        name: "Alert 1",
        color: "blue"
      },
      {
        name: "Alert 2",
        color: "yellow"
      },
      {
        name: "Alert 3",
        color: "red"
      }
    ];

    self.fields = ['name'];
    self.linkToAdd = "/configuration/alerts/new";
    self.link = function(object) {
      return "";
    };

    self.iconProperties = {
      type: "icon"
    };

    self.icon = function(object) {
      if (object.color === 'blue')
        return "fa fa-check label-primary";

      if (object.color === 'yellow')
        return "fa fa-eye label-warning";

      if (object.color === 'red')
        return "fa fa-exclamation label-danger";

      return "";
    }

    $q.all([AlertService.init()])
      .then(function(){
        self.model = AlertService.list();
        /**
         * Functor to make URL to remove selected view
         * @param {Object}
         */
        self.remove = function(object) {
          return "/api/Alert/" + object.id + "/delete";
        };
      })
      .catch(function(err) {
        $log.log("Could not load alerts due " + err.toString() + ". Please refresh page (F5)");
      });
  }

  AlertList.$inject = ["$scope", "i18n", "$q", "AlertService"];

  return AlertList;
});