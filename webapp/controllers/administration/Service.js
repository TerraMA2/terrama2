module.exports = function(app) {
  return {
    get: function (request, response) {
      return response.render('administration/services');
    },

    new: function (request, response) {
      return response.render('administration/service');
    },
    
    edit: function(request, response) {
      console.log(request.params);  
      return response.render('administration/service');
    }
  };
};