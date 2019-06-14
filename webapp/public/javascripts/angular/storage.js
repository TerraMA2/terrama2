define([
    "TerraMA2WebApp/common/app"
], function (coreModule) {
    var moduleName = "storage";

    const storageComponent = {
        controller: function($scope, i18n) {
            $scope.i18n = i18n;
            $scope.css={boxType: "box-solid"};
            this.save = () => {
                console.log(this.storage);
            }
        },
        templateUrl: "/dist/templates/templates/storage-component.html"
    };

    storageComponent.controller.$inject = ["$scope", "i18n"];

    var terrama2Module = angular.module(moduleName, [coreModule])
        .component("storageComponent", storageComponent)

    return terrama2Module.name;
});