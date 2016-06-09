angular.module("terrama2.schedule", ['terrama2'])
  .directive("terrama2Schedule", function() {
    return {
      restrict: 'E',
      templateUrl: '/javascripts/angular/data-series/templates/schedule.html',
      scope: {
        model: '=model'
      },
      controller: ['$scope', 'i18n', function($scope, i18n) {
        $scope.i18n = i18n;
        $scope.isFrequency = false;
        $scope.isSchedule = false;

        $scope.onScheduleChange = function(value) {
          var resetHelper = function(i) {
            if (i == 1) {
              delete $scope.model.schedule;
              delete $scope.model.schedule_retry;
              delete $scope.model.schedule_retry_unit;
              delete $scope.model.schedule_timeout;
              delete $scope.model.schedule_timeout_unit;
              $scope.isFrequency = true;
              $scope.isSchedule = false;
            } else if (i == 2) {
              delete $scope.model.frequency;
              delete $scope.model.frequency_unit;
              $scope.isFrequency = false;
              $scope.isSchedule = true;
            }
          };

          switch(value) {
            case "seconds":
            case "minutes":
            case "hours":
              resetHelper(1);
              $scope.minSchedule = 0;
              $scope.maxSchedule = 2147483648; // setting max value to schedule (int32)
              break;
            case "weeks":
              resetHelper(2);
              $scope.minSchedule = 1;
              $scope.maxSchedule = 7;
              break;
            case "monthly":
              resetHelper(2);
              $scope.minSchedule = 1;
              $scope.maxSchedule = 31;
              break;
            case "yearly":
              resetHelper(2);
              $scope.minSchedule = 1;
              $scope.maxSchedule = 366;
              break;
            default:
              $scope.minSchedule = 0;
              $scope.maxSchedule = 0;
              break;
          }
        };
      }]
    }
  });
