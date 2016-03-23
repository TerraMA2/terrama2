var RequestFactory = require("../../core/RequestFactory");

module.exports = function(app) {
  return {
    post: function(request, response) {
      var obj = request.body;

      try {
        // requesting for an object to check connection
        var factoryResult = RequestFactory.build(obj);

        factoryResult.request().then(function() {
          response.json({status:200});
        }).catch(function(err) {
          response.status(400);
          response.json({status:400, message:err.message});
        });
      } catch(err) {
        response.status(400);
        response.json({status:400, message:err.message});
      }
    }
  }
};