require.config({
  paths: {
    'angular' : '../../bower_components/angular/angular.min.js',
    'application': 'application'
  },
  shim: {
    'application': {
      deps: ['angular']
    }
  }
});
require(["TerraMA2WebApp/application", function(app) {
  app.init();
}]);