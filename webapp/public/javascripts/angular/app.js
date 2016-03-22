var terrama2Application = angular.module("terrama2", []);

// It avoid conflicts with template engine tags. The angular tags will be {[ someAngularCommand ]}
terrama2Application.config(['$interpolateProvider', function($interpolateProvider) {
    $interpolateProvider.startSymbol('{[');
    $interpolateProvider.endSymbol(']}');
}]);
