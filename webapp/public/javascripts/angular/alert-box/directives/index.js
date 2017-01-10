define([
  "TerraMA2WebApp/alert-box/common/index",
  "TerraMA2WebApp/alert-box/directives/alertbox",
  "TerraMA2WebApp/alert-box/directives/modalbox"
], function(commonModule, terrama2AlertBox2, terrama2ModalBox) {
  var moduleName = "terrama2.components.messagebox.directives";

  angular.module(moduleName, [commonModule])
    .directive("terrama2AlertBox2", terrama2AlertBox2)
    .directive("terrama2ModalBox", terrama2ModalBox)
    .run(["$templateCache", function($templateCache) {
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
    }]);

  return moduleName;
});