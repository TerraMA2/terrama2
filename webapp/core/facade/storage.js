var DataManager = require("./../DataManager");
class storageFacade {
  async list(){
    const storages = await DataManager.orm.models.Storages.findAll({});
    return storages;
  }

  async save(storage){
    try{
      const createdStorage = await DataManager.orm.models.Storages.create(storage);
      return createdStorage;
    }catch(error){
      return false;
    }
  }
}
module.exports = storageFacade;