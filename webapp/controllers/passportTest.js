module.exports = function(app) {
  function passportTest(request, response) {
    console.log('lala');
    response.render("passportTest");
  }

  return passportTest;
};
