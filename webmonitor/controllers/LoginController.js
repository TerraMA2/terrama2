"use strict";

/**
 * Controller responsible for authentication.
 * @class LoginController
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 * @property {object} memberPassport - 'passport' module.
 * @property {object} memberUserToken - 'UserToken' module.
 */
var LoginController = function(app) {

  // 'passport' module
  var memberPassport = require('passport');
  // 'UserToken' module
  var memberUserToken = require('../config/UserToken');

  /**
   * Processes the request and returns a response.
   * @param {json} request - JSON containing the request data
   * @param {json} response - JSON containing the response data
   *
   * @function login
   * @memberof LoginController
   * @inner
   */
  var login = function (request, response, next) {
    memberPassport.authenticate('local', function(err, user, info) {
      if(err)
        console.log(err);

      if(!user)
        return response.json({
          error: info.message
        });
      else {
        request.logIn(user, function(e) {
          if(e)
            return next(e);

          memberUserToken.setToken(user.token);

          return response.json({
            error: null,
            username: user.name,
            token: user.token
          });
        });
      }
    })(request, response, next);
  };

  /**
   * Processes the request and returns a response.
   * @param {json} request - JSON containing the request data
   * @param {json} response - JSON containing the response data
   *
   * @function logout
   * @memberof LoginController
   * @inner
   */
  var logout = function(request, response) {
    request.logout();

    memberUserToken.setToken(null);

    return response.json({ error: null });
  };

  /**
   * Processes the request and returns a response.
   * @param {json} request - JSON containing the request data
   * @param {json} response - JSON containing the response data
   *
   * @function checkAuthentication
   * @memberof LoginController
   * @inner
   */
  var checkAuthentication = function(request, response) {
    return response.json({ isAuthenticated: request.isAuthenticated(), username: (request.isAuthenticated() ? request.session.passport.user.name : null) });
  };

  return {
    login: login,
    logout: logout,
    checkAuthentication: checkAuthentication
  };
};

module.exports = LoginController;