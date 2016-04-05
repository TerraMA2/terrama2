module.exports = function(app) {
  return {
    get: function (request, response) {
      return response.render('administration/users');
    },

    new: function (request, response) {
      return response.render('administration/user');
    },

    edit: function (request, response) {
      console.log(request.params);
      return response.render('administration/user', {user: request.params});
    }
  };
};