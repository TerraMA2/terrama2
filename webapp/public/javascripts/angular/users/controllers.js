define([
  "TerraMA2WebApp/users/services",
  "TerraMA2WebApp/alert-box/app",
  "TerraMA2WebApp/table/table"
], function(servicesApp, messageBoxApp, tableApp) {

'use strict';

    function ListController($scope, i18n, MessageBoxService, $window, $timeout) {
      /**
       * Global configuration
       * @type {Object}
       */
      var config = $window.configuration;
      /**
       * Common title for alert box
       * @type {string}
       */
      var title = "Users";

      $scope.MessageBoxService = MessageBoxService;

      $scope.link = function(object) {
        return BASE_URL + "administration/users/edit/" + object.id;
      };

      $scope.i18n = i18n;

      $scope.remove = function(object) {
        return BASE_URL + "administration/users/" + object.id + "/delete";
      };

      $scope.model = config.users;

      $timeout(function() {
        $scope.model.forEach(function(instance) {
          instance.administrator = i18n.__(instance.administrator);
        });
      }, 500);

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
      
      $scope.iconFn = function(object) {
        switch(object.administrator) {
          case i18n.__("Yes"):
            return BASE_URL + "images/user/admin/user.png"
            break;
          case i18n.__("No"):
          default:
            return BASE_URL + "images/user/nadmin/user.png"
            break;
        }
      };

      $scope.iconProperties = {
        type: "img",
        width: 20,
        height: 20
      };

      $scope.linkToAdd = BASE_URL + "administration/users/new";

      $scope.close = function() { MessageBoxService.reset(); };

      // callback after remove operation
      $scope.extra = {
        canRemove: true,
        removeOperationCallback: function(err, data) {
          if(err) {
            return MessageBoxService.danger(i18n.__(title), i18n.__(err.message));
          }
          MessageBoxService.success(i18n.__(title), data.name + i18n.__(" removed"));
        }
      };

      if(config.message) {
        var messageArray = config.message.split(" ");
        var tokenCodeMessage = messageArray[messageArray.length - 1];
        messageArray.splice(messageArray.length - 1, 1);

        $timeout(function() {
          var finalMessage = messageArray.join(" ") + " " + i18n.__(tokenCodeMessage);
          MessageBoxService.success(i18n.__(title), finalMessage);
        }, 1000);
      }
    }

    ListController.$inject = ["$scope", "i18n", "MessageBoxService", "$window", "$timeout"];

    function UserUpdate($scope, UserService, i18n, MessageBoxService) {
      $scope.i18n = i18n;
      $scope.user = {};
      var title = "User";
      var hash = Math.random().toString(36);
      $scope.MessageBoxService = MessageBoxService;

      $scope.close = function() {
        MessageBoxService.reset();
      };

      $scope.initialization = function(userId, redirectUrl) {
        $scope.redirectUrl = redirectUrl;

        if(!userId) return;

        UserService.init({id: userId}).then(function(user) {
          $scope.user = user[0];
          // resetting password for security reasons
          $scope.user.password = hash;
          $scope.user.passwordConfirm = $scope.user.password;
        }).catch(function(response) {
          console.log(response);
        });
      };

      $scope.save = function() {
        // emitting fields validation
        $scope.$broadcast("formFieldValidation");

        if ($scope.form.$invalid) {
          MessageBoxService.danger(i18n.__(title), i18n.__("There are invalid fields on form"));
          return;
        }

        var user = Object.assign({}, $scope.user);

        if (user.password === hash) {
          delete user.password;
        }

        UserService.update($scope.user.id, user).then(function(data) {
          window.location.href = $scope.redirectUrl + "?token=" + data.token + "&context="+data.context;
        }).catch(function(err) {
          MessageBoxService.danger(i18n.__(title), i18n.__(err.message));
        });
      };
    }

    UserUpdate.$inject = ["$scope", "UserService", "i18n", "MessageBoxService"];

    function UserRegistration($scope, $http, MessageBoxService, i18n, UserService) {
      $scope.isSubmiting = false;
      var title = "User";
      $scope.initialization = function(id, redirectUrl) { $scope.redirectUrl = redirectUrl; };

      $scope.close = function() {
        MessageBoxService.reset();
      };

      $scope.user = {};

      $scope.save = function() {
        $scope.$broadcast("formFieldValidation");
        if ($scope.form.$invalid) {
          MessageBoxService.danger(i18n.__(title), i18n.__("There are invalid fields on form"));
          return;
        }

        if ($scope.user.password !== $scope.user.passwordConfirm) {
          MessageBoxService.danger(i18n.__(title), i18n.__("The password confirmation must be identical to the password"));
          return;
        }

        $scope.isSubmiting = true;
        UserService.create($scope.user).then(function(response) {
          window.location = $scope.redirectUrl + "?token=" + response.token;
        }).catch(function(err) {
          MessageBoxService.danger(i18n.__(title), i18n.__(err.message));
        }).finally(function(){
          $scope.isSubmiting = false;
        });
      };
    }

    UserRegistration.$inject = ["$scope", "$http", "MessageBoxService", "i18n", "UserService"];

    var moduleName = "terrama2.users.controllers";

    angular.module(moduleName, [servicesApp, messageBoxApp, tableApp])
      .controller("UserListController", ListController)
      .controller("UserUpdateController", UserUpdate)
      .controller("UserRegistrationController", UserRegistration);

    return moduleName;
});