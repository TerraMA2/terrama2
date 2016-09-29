// Register `phoneList` component, along with its associated controller and template
angular.
  module('viewList').
  component('viewList', {
    templateUrl: 'javascripts/angular/view-list/view-list.template.html',
    controller: function ViewListController($http) {
      var self = this;
      var mapViews = {}

      $http.get('http://private-a9473-terrama2.apiary-mock.com/views').then(function(response){
        self.mapViews = response.data;
        mapViews = response.data;
        console.log(mapViews);

        var menuUrls = {
          "projects": { name: 'Analysis', icon: "fa-book", data:["queimada", "estados"]},
          "static-data": {name: 'Static Data', icon: "fa-cubes", data:mapViews},
          "dynamic-data": {name: 'Dynamic Data', icon: "fa-folder-o", data:["queimada", "estados"]},
          "template": {name: 'Template', icon: "fa-exchange", data:["queimada", "estados"]}
        }
        console.log(mapViews);
        self.menuUrls = menuUrls;
      });

    }
  });