'use strict';

angular.module("terrama2.users")
  .controller("ListController", [
    "$scope",
    "i18n",
    "MessageBoxService",
    "$window",
    function($scope, i18n, MessageBoxService, $window) {
      /**
       * Global configuration
       * @type {Object}
       */
      var config = $window.configuration;
      /**
       * Common title for alert box
       * @type {string}
       */
      var title = i18n.__("Users");

      $scope.MessageBoxService = MessageBoxService;

      $scope.link = function(object) {
        return "/administration/users/edit/" + object.id;
      };

      $scope.i18n = i18n;

      $scope.remove = function(object) {
        return "/administration/users/" + object.id + "/delete";
      };

      $scope.model = config.users;
      $scope.fields = [
        {
          key: "name",
          as: i18n.__("Name")
        },
        {
          key: "email",
          as: i18n.__("E-mail")
        },
        {
          key: "cellphone",
          as: i18n.__("Cellphone")
        },
        {
          key: "username",
          as: i18n.__("User")
        },
        {
          key: "administrator",
          as: i18n.__("Administrator")
        },
      ];
      $scope.iconFn = null;

      $scope.linkToAdd = "/administration/users/new";

      // alert-box
      $scope.alertLevel = "alert-success";
      $scope.alertBox = {
        title: i18n.__("Users"),
        message: "{[ message ]}"
      };
      $scope.close = function() { MessageBoxService.reset(); };

      // callback after remove operation
      $scope.extra = {
        removeOperationCallback: function(err, data) {
          $scope.display = true;
          if (err) {
            $scope.alertLevel = "alert-danger";
            $scope.alertBox.message = err.message;
            return;
          }

          $scope.alertLevel = "alert-success";
          $scope.alertBox.message = data.name + i18n.__(" removed");
        }
      };

      if (config.message) {
        MessageBoxService.success(title, config.message);
      }
    }
  ])
  .controller("UserUpdate", [
    "$scope",
    "UserFactory",
    "i18n",
    "MessageBoxService",
    function($scope, UserFactory, i18n, MessageBoxService) {
      $scope.i18n = i18n;
      $scope.user = {};
      var title = i18n.__("User");
      var hash = Math.random().toString(36);
      $scope.MessageBoxService = MessageBoxService;

      $scope.close = function() {
        MessageBoxService.reset();
      };

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

        if ($scope.form.$invalid) {
          MessageBoxService.danger(title, i18n.__("There are invalid fields on form"));
          return;
        }

        var user = Object.assign({}, $scope.user);

        if (user.password === hash) {
          delete user.password;
        }

        UserFactory.put($scope.user.id, user).success(function(data) {
          window.location.href = $scope.redirectUrl + "?token=" + data.token + "&context="+data.context;
        }).error(function(err) {
          MessageBoxService.danger(title, err.message);
        });
      };
    }])

  .controller("UserRegistration", [
    "$scope",
    "$http",
    "MessageBoxService",
    "i18n",
    function($scope, $http, MessageBoxService, i18n) {
      $scope.isSubmiting = false;
      var title = i18n.__("User");
      $scope.initialization = function(id, redirectUrl) { $scope.redirectUrl = redirectUrl; };

      $scope.close = function() {
        MessageBoxService.reset();
      };

      $scope.user = {};

      $scope.save = function() {
        $scope.$broadcast("formFieldValidation");
        if ($scope.form.$invalid) {
          MessageBoxService.danger(title, i18n.__("There are invalid fields on form"));
          return;
        }

        if ($scope.user.password !== $scope.user.passwordConfirm) {
          MessageBoxService.danger(title, i18n.__("Confirm password must be same password"));
          return;
        }

        $scope.isSubmiting = true;
        $http({
          method: "POST",
          url: "/administration/users/new",
          data: $scope.user
        }).success(function(user) {
          window.location = $scope.redirectUrl + "?token=" + user.token;
        }).error(function(err) {
          MessageBoxService.danger(title, err.message);
        }).finally(function(){
          $scope.isSubmiting = false;
        });
    };
  });