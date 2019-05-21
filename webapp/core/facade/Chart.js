var DataManager = require("./../DataManager");
class ChartFacade {
  async list(viewId){
    return await DataManager.orm.models.Charts.findAll({"view_id": viewId});
  }

  async saveAll(charts, viewId){
    try {
      charts.forEach(async chart=>{
        chart.view_id = viewId;
        await DataManager.orm.models.Charts.create(chart);
      });
    }catch(error){
      return false;
    }
  }
  async delete(chart){
    try{
      const deletedChart = await DataManager.orm.models.Charts.delete(chart);
    }catch(error){
      return false;
    }
  }
}
module.exports = ChartFacade;