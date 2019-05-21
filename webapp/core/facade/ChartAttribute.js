var DataManager = require("../DataManager");
class ChartAttributeFacade {
  async list(chartId){
    const charts = await DataManager.orm.models.ChartAttributes.findAll({"chart_id": chartId});
    return charts;
  }

  async saveAll(charts, viewId){
    try {
      charts.forEach(async chart=>{
        chart.chart_id = viewId;
        await DataManager.orm.models.ChartAttributes.create(chart);
      });
    }catch(error){
      return false;
    }
  }
  async delete(chart){
    try{
      const deletedChart = await DataManager.orm.models.ChartAttributes.delete(chart);
    }catch(error){
      return false;
    }
  }
}
module.exports = ChartFacade;