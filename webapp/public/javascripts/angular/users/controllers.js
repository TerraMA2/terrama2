'use strict';

angular.module("terrama2.users")
  .controller("UserUpdate", ["$scope", "UserFactory", "i18n",
    function($scope, UserFactory, i18n) {
      $scope.i18n = i18n;
      $scope.user = {};
      var hash = Math.random().toString(36);

      $scope.initialization = function(userId, redirectUrl) {
        $scope.redirectUrl = redirectUrl;
        if (!userId) {
          return;
        }

        UserFactory.get(userId, {}).success(function(user) {
          $scope.user = user;
          // resetting password for security reasons
          $scope.user.password = hash;
          $scope.user.passwordConfirm = $scope.user.password;
        }).error(function(err) {
          console.log(err);
        });
      };

      $scope.save = function() {
        // emitting fields validation
        $scope.$broadcast("formFieldValidation");

        if ($scope.form.$invalid) { return; }

        var user = Object.assign({}, $scope.user);

        if (user.password === hash) {
          delete user.password;
        }

        UserFactory.put($scope.user.id, user).success(function(data) {
          window.location.href = $scope.redirectUrl + "?token=" + data.token + "&context="+data.context;
        }).error(function(err) {
          console.log(err);
        });
      };
    }])

  .controller("UserRegistration", function($scope, $http) {
    $scope.isSubmiting = false;
    $scope.errorFound = "";
    $scope.initialization = function(id, redirectUrl) { $scope.redirectUrl = redirectUrl; };

    $scope.user = {};

    $scope.save = function() {
      $scope.$broadcast("formFieldValidation");
      if ($scope.form.$invalid) { return; }

      if ($scope.user.password !== $scope.user.passwordConfirm) {
        return;
      }

      $scope.isSubmiting = true;
      $http({
        method: "POST",
        url: "/administration/users/new",
        data: $scope.user
      }).success(function(user) {
        $scope.errorFound = "";
        window.location = $scope.redirectUrl + "?token=" + user.token;
      }).error(function(err) {
        alert(err.message);
        $scope.errorFound = err.message;
        console.log(err);
        form.name.$invalid = true;
      }).finally(function(){
        $scope.isSubmiting = false;
      });
    };
  });