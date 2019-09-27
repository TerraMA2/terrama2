  var DataManager = require('../../core/DataManager');
  var bcrypt = require('bcrypt');

  /**
   * Injecting NodeJS App configuration AS dependency. It retrieves a Views controllers API
   *
   * @param {Object}
   * @returns {Object}
   */
  module.exports = function(app) {
    return {
      login: async (request, response) => {
        DataManager.getUser({'username': request.body.params.username}).then(function(userObj) {
          if(userObj === null)
            return response.json({
              error: true,
              message: 'Usuário não encontrado.',
              user: null
            });

          var hashedPassword = bcrypt.hashSync(request.body.params.password, userObj.salt);

          if(userObj.password === hashedPassword) {
            var user = {
              id: userObj.id,
              name: userObj.name,
              email: userObj.email,
              username: userObj.username,
              administrator: userObj.administrator,
              token: userObj.token
            };

            return response.json({
              error: false,
              message: "Login realizado com sucesso!",
              user: user
            });
          }

          return response.json({
            error: true,
            message: 'Senha incorreta.',
            user: null
          });
        }).catch(function(err) {
          return response.json({
            error: true,
            message: 'Não foi possível realizar o login.',
            user: null
          });
        });
      },
    }
} ();
