module.exports = function(app) {

  function indexController(request, response) {
    response.render('index');
  }

  return indexController;
};
