define([
  "TerraMA2WebApp/common/services/index"
], function(commonApp) {
  var moduleName = "terrama2.users.services";
  /**
   * It handles UserService DAO over API
   * @class UserService
   */
  function UserService(BaseService) {
    this.BaseService = BaseService;
    this.url = BASE_URL + "api/users";
    /**
     * It stores available users
     * @type {any[]}
     */
    this.model = [];
  }

  UserService.prototype.init = function(params) {
    var self = this;
    var defer = self.BaseService.$q.defer();

    self.BaseService.$request(self.url, "GET", {params: params})
      .then(function(response) {
        self.model = response.data;
        return defer.resolve(response.data);
      });

    return defer.promise;
  };

  UserService.prototype.list = function(restriction) {
    return this.BaseService.$list(this.model, restriction);
  };

  UserService.prototype.get = function(restriction) {
    return this.BaseService.get(this.model, restriction);
  };

  UserService.prototype.update = function(userId, userObject) {
    var self = this;
    var defer = self.BaseService.$q.defer();

    self.BaseService.$request(self.url + "/" + userId, "PUT", {data: userObject})
      .then(function(response) {
        return defer.resolve(response.data);
      });

    return defer.promise;

  };

  UserService.prototype.create = function(userObject) {
    var self = this;
    var defer = self.BaseService.$q.defer();

    self.BaseService.$request(self.url, "POST", {data: userObject})
      .then(function(response) {
        self.model.push(response.data);
        return defer.resolve(response.data);
      });

    return defer.promise;
  };

  UserService.$inject = ["BaseService"];

  angular.module(moduleName, [commonApp])
    .service("UserService", UserService);

  return moduleName;
});