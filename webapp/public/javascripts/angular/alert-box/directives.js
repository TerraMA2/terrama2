'use strict';

angular.module("terrama2.components.messagebox", ['terrama2'])
  .run(function($templateCache) {
    // <terrama2-alert-box>
    $templateCache.put('message-box.html',
    '<div ng-if="display" class="alert alert-dismissible" ng-class="alertLevel" terrama2-fluid ng-transclude>' +
      '<button type="button" class="close" ng-click="close()">×</button>' +
      '<h4><i class="icon fa" ng-class="alertIcon()"></i> {{ title }}</h4>' +
      '<div class="row">' +
        '<div class="col-md-12">{{ message }}</div>'+
        '<div class="col-md-12" ng-if="isAnyExtra()">'+
          '<button type="button" class="btn btn-primary pull-right" ng-click="extra.confirmButtonFn(extra.object)">' +
            '{{ extra.confirmButtonName ||  "Confirm" }}' +
          '</button>' +
        '</div>' +
      '</div>' +
    '</div>');

    // <terrama2-modal>
    $templateCache.put('modal.html',
    '<div class="{{ css }}" ng-class="klass" id="{{ modalId }}" tabindex="-1" role="dialog" aria-labelledby="my{{ modalId }}">' +
      '<div class="modal-dialog" ng-class="modalType" role="document">' +
        '<div class="modal-content">' +
          '<div class="modal-header">' +
            '<button type="button" class="close" data-dismiss="modal" aria-label="Close">' +
              '<span aria-hidden="true">×</span>' +
            '</button>' +
            '<h4 class="modal-title">{{ title }}</h4>' +
          '</div>' +
          '<div class="modal-body" ng-transclude="bodySlot">' +

          '</div>' +
          '<div class="modal-footer" ng-transclude="footerSlot">' +
            '<button type="button" class="btn btn-default" data-dismiss="modal">Close</button>' +
          '</div>' +
        '</div>' +
      '</div>' +
    '</div>');
  })
  .directive("terrama2MessageBox", function() {
    return {
      restrict: "AE",
      replace: true,
      transclude: true,
      templateUrl: "/javascripts/angular/alert-box/templates/alert-box.html",
      scope: {
        errorFound: "=errorFound",
        message: "=",
        title: "=",
        close: "&"
      }
    };
  })

  /**
   * It will replace terrama2-alert-box and terrama2-message-box
   *
   */
  .directive("terrama2AlertBox2", function() {
    return {
      restrict: "E",
      transclude: true,
      templateUrl: "message-box.html",
      scope: {
        handler: '=handler', // it should be a AlertBox
        close: '&?',
        extra: '=?extra'
      },
      controller: ["$scope", function($scope) {
        const ALERT_LEVELS = {
          INFO: "info",
          WARNING: 'warning',
          DANGER: 'danger',
          SUCCESS: 'success'
        };
        $scope.$watch("handler", function(value) {
          if (!value) {
            return;
          }

          var alertBox = value;
          $scope.display = alertBox.display;
          $scope.title = alertBox.title;
          $scope.message = alertBox.message;
          $scope.alertLevel = alertBox.level;
          $scope.extra = alertBox.extra;
        }, true)

        $scope.isAnyExtra = function() {
          return Object.keys($scope.extra || {}).length > 0;
        };
        $scope.alertIcon = function() {
          switch($scope.alertLevel) {
            case ALERT_LEVELS.INFO:
              return "fa-info";
            case ALERT_LEVELS.WARNING:
              return "fa-exclamation-triangle";
            case ALERT_LEVELS.DANGER:
              return "fa-times";
            case ALERT_LEVELS.SUCCESS:
              return "fa-check";
            default:
              return "fa-info";
          }
        };
      }]
    }
  })

  .directive("terrama2AlertBox", function() {
    return {
      restrict: "E",
      templateUrl: "message-box.html",
      transclude: true,
      scope: {
        alertLevel: '=alertLevel',
        title: "=title",
        message: "=message",
        close: "&?close",
        display: "=displayHandler",
        extra: '=?extra'
      },
      controller: ["$scope", function($scope) {
        const ALERT_LEVELS = {
          INFO: "info",
          WARNING: 'warning',
          DANGER: 'danger',
          SUCCESS: 'success'
        };

        // setting default close function
        //if (!$scope.close) {
          $scope.close = function() {
            $scope.display = false;
          };
        //}

        $scope.$watch("display", function(val) {
          console.log(val);
        });

        $scope.isAnyExtra = function() {
          return Object.keys($scope.extra || {}).length > 0;
        };

        $scope.alertIcon = function() {
          switch($scope.alertLevel) {
            case ALERT_LEVELS.INFO:
              return "fa-info";
            case ALERT_LEVELS.WARNING:
              return "fa-exclamation-triangle";
            case ALERT_LEVELS.DANGER:
              return "fa-times";
            case ALERT_LEVELS.SUCCESS:
              return "fa-check";
            default:
              return "fa-info";
          }
        };
      }]
    };
  })

  .directive("terrama2ModalBox", function() {
    return {
      restrict: 'E',
      transclude: true,/*{
        "bodySlot": "terrama2Content",
        "footerSlot": "terrama2Button"
      },*/
      templateUrl: '/javascripts/angular/alert-box/templates/modal.html',
      scope: {
        title: '=title',
        modalId: '=modalId',
        // klass: "=class",
        modalType: '=modalType',
        properties: '=?properties'
      },
      controller: ["$scope", function($scope) {
        if ($scope.properties === undefined) { $scope.properties = {}; }
        $scope.css = $scope.properties.css || "modal fade";
      }]
    };
  })

  .directive("terrama2RemoveModal", function() {
    return {
      restrict: "E",
      template: "<terrama2-modal-box title='title' modal-id='\"removalID\"' properties='properties'>" +
                  "<terrama2-content>Are you sure?</terrama2-content>" +
                  "<terrama2-button class='btn btn-primary' ng-click='onRemovalClick'>OK</terrama2-button>" +
                  "<terrama2-button class='btn btn-primary' ng-click='onCancelClick'>Cancel</terrama2-button>" +
                "</terrama2-modal-box>",
      scope: {
        title: '=',
        properties: '=?',
        onRemovalClick: '@',
        onCancelClick: '@'
      },
      link: function(scope, element, attrs) {
        console.log(scope);
      }
    };
  });