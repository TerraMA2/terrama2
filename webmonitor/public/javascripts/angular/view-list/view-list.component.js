// Register `phoneList` component, along with its associated controller and template
angular.
  module('viewList').
  component('viewList', {
    templateUrl: 'javascripts/angular/view-list/view-list.template.html',
    controller: function ViewListController($http) {
      var self = this;
      var mapViews = {};

      $http.get('javascripts/angular/view-list/view-list.json').then(function(response){
        self.mapViews = response.data;
        mapViews = response.data;

        var groupedData = _.groupBy(mapViews, function(d){return d.type});

        var menuUrls = [
          { name: 'Analysis', icon: "fa-book", data: groupedData.Analysis},
          { name: 'Static Data', icon: "fa-cubes", data: groupedData.Static},
          { name: 'Dynamic Data', icon: "fa-folder-o", data: groupedData.Dynamic},
          { name: 'Template', icon: "fa-exchange", data: groupedData.Template}
        ]
        self.menuUrls = menuUrls;
      });

    }
  });