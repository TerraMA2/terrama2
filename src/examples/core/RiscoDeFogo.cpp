
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/Raii.hpp>

#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/raster/Raster.h>
#include <terralib/raster/Band.h>
#include <terralib/raster/Grid.h>

#include <vector>
#include <memory>
#include <cmath>

//TerraLib
#include <terralib/dataaccess/dataset/DataSetTypeConverter.h>
#include <terralib/dataaccess/dataset/DataSetType.h>
#include <terralib/datatype/TimeInstantTZ.h>
#include <terralib/memory/DataSet.h>


namespace terrama2
{
  namespace core
  {
    class RiscoDeFogo
    {

      public:
        RiscoDeFogo() = default;
        virtual ~RiscoDeFogo() = default;
        double valorAVegetacao(double tipo_vegetacao);
        double converteIGBP_INPE(double igbp);
        double maxPSE(double igbp, double pse);
        double XYLinhaCol(double x, double y, const std::__cxx11::string &path, const std::__cxx11::string &filename) const;

    };
  }
}

double terrama2::core::RiscoDeFogo::valorAVegetacao(double tipo_vegetacao)
{
    if (tipo_vegetacao == 0 || tipo_vegetacao == 11 || tipo_vegetacao == 13 || tipo_vegetacao == 15 || tipo_vegetacao == 16)
        return 0;
    else if (tipo_vegetacao == 1 || tipo_vegetacao == 3 || tipo_vegetacao == 5)
        return  2;
    else if (tipo_vegetacao == 2)
        return 1.5;
    else if (tipo_vegetacao == 4)
        return 1.72;
    else if (tipo_vegetacao == 6 || tipo_vegetacao == 8)
        return 2.4;
    else if (tipo_vegetacao == 7 or tipo_vegetacao == 9)
        return  3;
    else if (tipo_vegetacao == 10)
        return 6;
    else if (tipo_vegetacao == 12 || tipo_vegetacao == 14)
        return  4;
    return  0;
}

double terrama2::core::RiscoDeFogo::converteIGBP_INPE(double igbp)
{
    if (igbp == 0 || igbp ==  11 || igbp == 13 || igbp == 15|| igbp == 16)
        return 0;

    if (igbp == 10)
        return 1;

    if (igbp == 12 || igbp == 14)
        return 2;

    if (igbp == 7 || igbp == 9)
        return 3;

    if (igbp == 6 || igbp == 8)
        return 4;

    if (igbp == 1 || igbp ==  5 || igbp == 3)
        return 5;

    if (igbp == 4)
        return 6;

    if (igbp == 2)
        return 7;

    //return None;
    return 0;
}

double terrama2::core::RiscoDeFogo::maxPSE(double igbp, double pse)
{
    double ibge = converteIGBP_INPE(igbp);
    if (ibge == 1)
    {
        if(pse < 30)
            return pse;
        else
        {
            pse = 30;
            return pse;
        }
    }
    else if (ibge == 2)
    {
        if (pse < 45)
           return pse;
        else
        {
            pse = 45;
            return pse;
        }
    }
    else if (ibge == 3)
    {
        if (pse < 60)
           return pse;
        else
        {
            pse = 60;
            return pse;
        }
    }
    else if (ibge == 4)
    {
        if (pse < 75)
           return pse;
        else
        {
            pse = 75;
            return pse;
        }
    }
    else if (ibge == 5)
    {
        if (pse < 90)
           return pse;
        else
        {
            pse = 90;
            return pse;
        }
    }
    else if (ibge == 6)
    {
        if (pse < 105)
           return pse;
        else
        {
            pse = 105;
            return pse;
        }
    }
    else if (ibge == 7)
    {
        if (pse < 120)
            return pse;
        else
        {
            pse = 120;
            return pse;
        }
    }

    return 0;
}

double terrama2::core::RiscoDeFogo::XYLinhaCol(double x, double y, const std::string& path, const std::string& filename) const
{

    //const auto& prec = *(precipitacao.rbegin()+i);
    std::shared_ptr<te::da::DataSource> datasource(te::da::DataSourceFactory::make("GDAL", "file://"+path+filename));

    //RAII for open/closing the datasource
    terrama2::core::OpenClose<std::shared_ptr<te::da::DataSource> > openClose(datasource);

    std::shared_ptr<te::da::DataSourceTransactor> transactor(datasource->getTransactor());
    std::shared_ptr<te::da::DataSet> dataset(transactor->getDataSet(filename));

    std::shared_ptr<te::rst::Raster> raster(dataset->getRaster(0));

    te::rst::Grid* grid = raster->getGrid();
    te::rst::Band* band = raster->getBand(0);

    double colD, rowD;
    grid->geoToGrid(x, y, colD, rowD);
    int col = std::round(colD);
    int row = std::round(rowD);


    double value;
    band->getValue(col, row, value);

    return value;
}

int main(int argc, char *argv[])
{
    terrama2::core::RiscoDeFogo* riscodefogo = new terrama2::core::RiscoDeFogo();

    terrama2::core::TerraMA2Init terrama2Init("", 0);
    {
        double x = -44.166;
        double y = -19.424;

        std::vector<std::string> precipitacao = {
            "S10648241_201703191200.tif",
            "S10648241_201703201200.tif",
            "S10648241_201703211200.tif",
            "S10648241_201703221200.tif",
            "S10648241_201703231200.tif",
            "S10648241_201703241200.tif",
            "S10648241_201703251200.tif",
            "S10648241_201703261200.tif",
            "S10648241_201703271200.tif",
            "S10648241_201703281200.tif",
            "S10648241_201703291200.tif",
            "S10648241_201703301200.tif",
            "S10648241_201703311200.tif",
            "S10648241_201704011200.tif",
            "S10648241_201704021200.tif",
            "S10648241_201704031200.tif",
            "S10648241_201704041200.tif",
            "S10648241_201704051200.tif",
            "S10648241_201704061200.tif",
            "S10648241_201704071200.tif",
            "S10648241_201704081200.tif",
            "S10648241_201704091200.tif",
            "S10648241_201704101200.tif",
            "S10648241_201704111200.tif",
            "S10648241_201704121200.tif",
            "S10648241_201704131200.tif",
            "S10648241_201704141200.tif",
            "S10648241_201704151200.tif",
            "S10648241_201704161200.tif",
            "S10648241_201704171200.tif",
            "S10648241_201704181200.tif",
            "S10648241_201704191200.tif",
            "S10648241_201704201200.tif",
            "S10648241_201704211200.tif",
            "S10648241_201704221200.tif",
            "S10648241_201704231200.tif",
            "S10648241_201704241200.tif",
            "S10648241_201704251200.tif",
            "S10648241_201704261200.tif",
            "S10648241_201704271200.tif",
            "S10648241_201704281200.tif",
            "S10648241_201704291200.tif",
            "S10648241_201704301200.tif",
            "S10648241_201705011200.tif",
            "S10648241_201705021200.tif",
            "S10648241_201705031200.tif",
            "S10648241_201705041200.tif",
            "S10648241_201705051200.tif",
            "S10648241_201705061200.tif",
            "S10648241_201705071200.tif",
            "S10648241_201705081200.tif",
            "S10648241_201705091200.tif",
            "S10648241_201705101200.tif",
            "S10648241_201705111200.tif",
            "S10648241_201705121200.tif",
            "S10648241_201705131200.tif",
            "S10648241_201705141200.tif",
            "S10648241_201705151200.tif",
            "S10648241_201705161200.tif",
            "S10648241_201705171200.tif",
            "S10648241_201705181200.tif",
            "S10648241_201705191200.tif",
            "S10648241_201705201200.tif",
            "S10648241_201705211200.tif",
            "S10648241_201705221200.tif",
            "S10648241_201705231200.tif",
            "S10648241_201705241200.tif",
            "S10648241_201705251200.tif",
            "S10648241_201705261200.tif",
            "S10648241_201705271200.tif",
            "S10648241_201705281200.tif",
            "S10648241_201705291200.tif",
            "S10648241_201705301200.tif",
            "S10648241_201705311200.tif",
            "S10648241_201706011200.tif",
            "S10648241_201706021200.tif",
            "S10648241_201706031200.tif",
            "S10648241_201706041200.tif",
            "S10648241_201706051200.tif",
            "S10648241_201706061200.tif",
            "S10648241_201706071200.tif",
            "S10648241_201706081200.tif",
            "S10648241_201706091200.tif",
            "S10648241_201706101200.tif",
            "S10648241_201706111200.tif",
            "S10648241_201706121200.tif",
            "S10648241_201706131200.tif",
            "S10648241_201706141200.tif",
            "S10648241_201706151200.tif",
            "S10648241_201706161200.tif",
            "S10648241_201706171200.tif",
            "S10648241_201706181200.tif",
            "S10648241_201706191200.tif",
            "S10648241_201706201200.tif",
            "S10648241_201706211200.tif",
            "S10648241_201706221200.tif",
            "S10648241_201706231200.tif",
            "S10648241_201706241200.tif",
            "S10648241_201706251200.tif",
            "S10648241_201706261200.tif",
            "S10648241_201706271200.tif",
            "S10648241_201706281200.tif",
            "S10648241_201706291200.tif",
            "S10648241_201706301200.tif",
            "S10648241_201707011200.tif",
            "S10648241_201707021200.tif",
            "S10648241_201707031200.tif",
            "S10648241_201707041200.tif",
            "S10648241_201707051200.tif",
            "S10648241_201707061200.tif",
            "S10648241_201707071200.tif",
            "S10648241_201707081200.tif",
            "S10648241_201707091200.tif",
            "S10648241_201707101200.tif",
            "S10648241_201707111200.tif",
            "S10648241_201707121200.tif",
            "S10648241_201707131200.tif",
            "S10648241_201707141200.tif",
            "S10648241_201707151200.tif",
            "S10648241_201707161200.tif"
        };

        std::string umidade = "/home/bianca/Teste/dados_amb/saida/umidade/";
        std::string nomeArquivoUmidade = "UMRS201707161820.tif";

        std::string temperatura = "/home/bianca/Teste/dados_amb/saida/temperatura/";
        std::string nomeArquivoTemperatura = "TEMP201707161820.tif";

        std::string resultado = "/home/bianca/Teste/dados_amb/saida/resultado/";
        std::string nomeArquivoResultado = "result201707162120.tif";

        std::string landcover = "/home/bianca/Teste/dados_amb/entrada/rf/landcover/";
        std::string nomeArquivoLand = "landcover_2012.tif";

        std::string gabarito = "/home/bianca/Teste/dados_amb/entrada/";
        std::string nomeArquivoGab = "RF.20170716.tif";



        double prec1      = 0;
        double prec2      = 0;
        double prec3      = 0;
        double prec4      = 0;
        double prec5      = 0;
        double prec6_10   = 0;
        double prec11_15  = 0;
        double prec16_30  = 0;
        double prec31_60  = 0;
        double prec61_90  = 0;
        double prec91_120 = 0;

        std::string path = "/home/bianca/Teste/dados_amb/saida/precipitacao/";
        for(int i = 0; i < 120; ++i)
        {
            const auto& prec = *(precipitacao.rbegin()+i);
            std::shared_ptr<te::da::DataSource> datasource(te::da::DataSourceFactory::make("GDAL", "file://"+path+prec));

            //RAII for open/closing the datasource
            terrama2::core::OpenClose<std::shared_ptr<te::da::DataSource> > openClose(datasource);

            std::shared_ptr<te::da::DataSourceTransactor> transactor(datasource->getTransactor());
            std::shared_ptr<te::da::DataSet> dataset(transactor->getDataSet(prec));

            std::shared_ptr<te::rst::Raster> raster(dataset->getRaster(0));

            te::rst::Grid* grid = raster->getGrid();
            te::rst::Band* band = raster->getBand(0);

            double colD, rowD;
            grid->geoToGrid(x, y, colD, rowD);
            int col = std::round(colD);
            int row = std::round(rowD);

            double value;
            band->getValue(col, row, value);

            int dia = i+1;
            if(dia == 1)
                prec1      += value;
            if(dia == 2)
                prec2      += value;
            if(dia == 3)
                prec3      += value;
            if(dia == 4)
                prec4      += value;
            if(dia == 5)
                prec5      += value;
            if(dia > 5 && dia <= 10)
                prec6_10   += value;
            if(dia > 10 && dia <= 15)
                prec11_15  += value;
            if(dia > 15 && dia <= 30)
                prec16_30  += value;
            if(dia > 30 && dia <= 60)
                prec31_60  += value;
            if(dia > 60 && dia <= 90)
                prec61_90  += value;
            if(dia > 90 && dia <= 120)
                prec91_120 += value;

        }
        // 2 - calc fatores de precipitações
        double fp1 = exp(-0.14 * prec1);
        double fp2 = exp(-0.07 * prec2);
        double fp3 = exp(-0.04 * prec3);
        double fp4 = exp(-0.03 * prec4);
        double fp5 = exp(-0.02 * prec5);
        double fp6_10 = exp(-0.01 * prec6_10);
        double fp11_15 = exp(-0.008 * prec11_15);
        double fp16_30 = exp(-0.004 * prec16_30);
        double fp31_60 = exp(-0.002 * prec31_60);
        double fp61_90 = exp(-0.001 * prec61_90);
        double fp91_120 = exp(-0.0007 * prec91_120);

        //3 - calc dias de secura
        double pse = 105. * fp1 * fp2 * fp3 * fp4 * fp5 * fp6_10 *  fp11_15 * fp16_30 * fp31_60 * fp61_90 * fp91_120;

        //4 - risco de fogo básico
        //tipo_vegetacao = grid.sample("lancover");

        double tipo_vegetacao = riscodefogo->XYLinhaCol(x, y, landcover, nomeArquivoLand);

        double a = riscodefogo->valorAVegetacao(tipo_vegetacao);
        std::cout << "A: " << a << std::endl;

        // Ajuste
        double PSE = riscodefogo->maxPSE(tipo_vegetacao, pse);
        std::cout << "PSE: " << PSE << std::endl;

        //rb = 0.9 * (1. + math.sin((a*pse))) / 2.
        double rb = 0.9 * (1. + std::sin((a*PSE-90.)*3.1416/180.)) * 0.5;
        if(rb > 0.9)
            rb = 0.9;

        std::cout << "RB: " << rb << std::endl;

        //5 - fator umidade

        //ur = grid.history.interval.sum("umidade", "1d", "0d", 0)
        double ur = riscodefogo->XYLinhaCol(x,y, umidade, nomeArquivoUmidade);
        std::cout << "UR: " << ur << std::endl;
        double fu = ur * -0.006 + 1.3;
        std::cout << "FU: " << fu << std::endl;


        //6 - fator temperatura
        double tempMax = riscodefogo->XYLinhaCol(x,y,temperatura, nomeArquivoTemperatura);
        std::cout << "TEMPMAX: " << tempMax << "\t" << tempMax-273.15 << std::endl;

        //grid.history.interval.sum("temperatura", "1d", "0d", 0);
        double ft = (tempMax-273.15) * 0.02 + 0.4;
        std::cout << "FT: " << ft << std::endl;

        //7 - gerar risco observatorio
        double rf = rb * ft * fu;

        if(rf > 1)
            rf = 1;

        std::cout << "RF: " << rf << std::endl;



        double res = riscodefogo->XYLinhaCol(x,y, resultado, nomeArquivoResultado);
        std::cout << "RESULTADO: " << res << std::endl;

        double gab = riscodefogo->XYLinhaCol(x,y,gabarito, nomeArquivoGab);
        std::cout << "GABARITO: " << gab << std::endl;
    }

    return 0;
}
