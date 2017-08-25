/*
  Copyright (C) 2007 National Institute For Space Research (INPE) - Brazil.
  This file is part of TerraMA2 - a free and open source computational
  platform for analysis, monitoring, and alert of geo-environmental extremes.
  TerraMA2 is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License,
  or (at your option) any later version.
  TerraMA2 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public License
  along with TerraMA2. See LICENSE. If not, write to
  TerraMA2 Team at <terrama2-team@dpi.inpe.br>.
*/

/*!
  \file src/unittest/analysis/NovaFriburgoTs.cpp
  \brief test for analysis NovaFriburgo
  \author Bianca Maciel, Jano Simas
*/


#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/impl/Utils.hpp>

#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>
#include <terrama2/core/utility/ServiceManager.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/GeoUtils.hpp>
#include <terrama2/core/utility/CurlWrapperFtp.hpp>

#include <terrama2/core/data-model/DataManager.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetGrid.hpp>


#include <terrama2/services/collector/core/Service.hpp>
#include <terrama2/services/collector/core/Collector.hpp>

#include <terrama2/services/analysis/core/utility/PythonInterpreterInit.hpp>
#include <terrama2/services/analysis/core/Analysis.hpp>
#include <terrama2/services/analysis/core/Service.hpp>
#include <terrama2/services/analysis/core/DataManager.hpp>
#include <terrama2/services/analysis/mock/MockAnalysisLogger.hpp>

#include "NovaFriburgoTs.hpp"

#include <terrama2/Config.hpp>


#include <QString>
#include <QJsonDocument>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QImage>


std::string NovaFriburgoTs::scriptAnalise()
{
    return "# Parametros de entrada\n"
    "c1 = 0.02  # Coesao minima (adimissional)\n"
    "c2 = 0.28  # Coesso maxima (adimissional)\n"
    "t1_entrada = 34   # Angula de atrito minimo (graus)\n"
    "t2_entrada = 39   # Angula de atrito maximo (graus)\n"
    "#	local x1_entrada = 139 # Razao R/T minina vinda da literatura (em metros)\n"
    "#	local x2_entrada = 436 # Razao R/T maxima vinda da literatura (em metro)\n"
    "rw = 1000 # Densidade da agua\n"
    "rs = 2300 # Densidade do solo\n"
    "r = rw/rs  # Densidade da agua/Densidade do solo (kg/m3)\n"
    "# quando nao sabe-se, coloca-se uma constante de 0.5\n"
    "pi = 3.14159265358979\n"

    "t1 = t1_entrada*pi/180    # angula de atrito minimo (graus)\n"
    "t2 = t2_entrada*pi/180    # angula de atrito miximo (graus)\n"

    "#	local hidro_diario = 100.0 # Testes com valores de chuvas arbitrados por nos\n"

    "Tmin = 0.00004	# Transmissividade minima (valores arbitrados por nos)\n"
    "Tmax = 0.00005	# Transmissividade maxima (valores arbitrados por nos)\n"

    "hidro_diario = grid.sample(\"hidro_2011\") or 0\n"
    "x1_entrada = Tmin / (hidro_diario/1000.0)       # Razao Hidro/T minina(em metros)\n"
    "x2_entrada = Tmax / (hidro_diario/1000.0)	# Razao Hidro/T maxima(em metros)\n"

    "x1 = 1/x2_entrada    # Razao R/T minina vinda da literatura (em metros)\n"
    "x2 = 1/x1_entrada    # Razao R/T maxima vinda da literatura (em metros)\n"

    "# Dados de entrada estaticos (raster) (fornecido pelo usuario)\n"
    "a = grid.sample(\"SRTM_a_latlong_sad69\") or 0   # Area de contribuicao (m2)\n"
    "s = grid.sample(\"SRTM_s_latlong_sad69\") or 0   # Declividade (rad)\n"

    "# Dados de entrada dinamicos (climaticos) - avancados\n"
    "#	local radar_PC = grid.sample(\"radar_PC\") or 0\n"
    "#	local chuva = grid.sample(\"hidro\") or 0\n"
    "#	local Prec_eta5 = amostra('Prec_eta05km') or 0\n"
    "			\n"
    "# print('Declividade (s) em radianos : ', s)\n"

    "s_graus = s*180/pi\n"

    "# print('Declividade (s) em graus : ', s_graus)\n"
    "# print('Area de contribuicao (a) : ', a)\n"
    "# print('t1_entrada : ', t1_entrada)\n"
    "# print('t2_entrada : ', t2_entrada)\n"
    "# print('t1 : ', t1)\n"
    "# print('t2 : ', t2)\n"
    "# print('x1_entrada : ', x1_entrada)\n"
    "# print('x2_entrada : ', x2_entrada)\n"
    "# print('x1 : ', x1)\n"
    "# print('x2 : ', x2)\n"

    "# # print('Tmin : ', Tmin)\n"
    "# # print('Tmax : ', Tmax)\n"
    "# # print('hidro_diario : ', hidro_diario)\n"
    "	\n"
    "# Variaveis internas (utilizadas nas contas)\n"
    "# cs, ss, w1, w2, fs2, fs1, cdf1, cdf2, y1, y2, as, si\n"
    "# w1 = None\n"
    "# w2 = None\n"
    "fs1 = None\n"
    "cdf1 = None\n"
    "cdf2 = None\n"
    "y1 = None\n"
    "y2 = None\n"
    "as1 = None\n"
    "si = None\n"
    "\n"
    "sat = 0.0 # valor dummy\n"
    "	\n"
    "if s == 0.0:\n"
    "    sat = 3.0\n"
    "    si = 10\n"
    "    return si\n"
    "		  \n"
    "\n"
    "import math\n"
    "\n"
    "s= math.atan(s)   \n"
    "t1= math.tan(t1)                                                                   \n"
    "t2= math.tan(t2)                                                                    \n"
    "cs = math.cos(s)\n"
    "ss = math.sin(s)\n"
    "		\n"
    "# print('t1 apos tangente: ', t1)\n"
    "# print('t2 apos tangente: ', t2)\n"
    "# print('CS : ', cs)\n"
    "# print('SS : ', ss)\n"
    "\n"
    "if x1 > x2:\n"
    "    w1=x2\n"
    "    x2=x1\n"
    "    x1=w1\n"
    "\n"
    "\n"
    "w2 = x2*a/ss\n"
    "w1 = x1*a/ss\n"
    "sat = w2\n"
    "\n"
    "# print('w1 inicial: ', w1)\n"
    "# print('w2 inicial: ', w2)\n"
    "        \n"
    "if w2 > 1.0:\n"
    "    w2 = 1.0\n"
    "    sat = 2.0\n"
    "\n"
    "        \n"
    "if w1 > 1.0:\n"
    "    w1 = 1.0\n"
    "    sat = 3.0\n"
    "\n"
    "\n"
    "# print('w1, quando maior que 1 : ', w1)\n"
    "# print('w2, quando maior que 1 : ', w2)\n"
    "\n"
    "fs2 = ((c2+cs*(1-w1*r)*t2)/ss)       \n"
    "# print('FS2: ', fs2)\n"
    "\n"
    "if fs2 < 1:\n"
    "    si  =  0\n"
    "else:\n"
    "    fs1 = ((c1+cs*(1-w2*r)*t1)/ss)     \n"
    "    # print('FS1: ', fs1)			\n"
    "\n"
    "    if fs1 >= 1:\n"
    "        si  =  fs1    \n"
    "    else:\n"
    "        if w1 == 1:\n"
    "            x1 = c1/ss\n"
    "            x2 = c2/ss\n"
    "            y1 = cs*(1-r)/ss*t1\n"
    "            y2 = cs*(1-r)/ss*t2\n"
    "            z = 1\n"
    "            # p, mn, mx, d, d1, d2\n"
    "            p = 0.0\n"
    "            mn = min(x1 + y2, x2 + y1)\n"
    "            mx = max(x1 + y2, x2 + y1)\n"
    "            d1 = min(x2 - x1, y2 - y1)\n"
    "            d2 = max(x2 - x1, y2 - y1)\n"
    "            d = z - y1 - x1\n"
    "\n"
    "            if z > (x1 + y1) and z < mn:\n"
    "                p = (d*d)/(2 * (x2 - x1) * (y2 - y1))\n"
    "\n"
    "            if mn <= z and z <= mx:\n"
    "                p = (d - d1/2)/d2\n"
    "\n"
    "            if ((mx < z) and (z < (x2 + y2))):\n"
    "                p = 1-math.pow((z-y2-x2),2)/(2*(x2-x1)*(y2-y1))\n"
    "\n"
    "            if (z >= (x2 + y2)):\n"
    "                p = 1\n"
    "\n"
    "\n"
    "            si = 1 - p       \n"
    "            # print('SI region 1: ', si)					\n"
    "        else:               \n"
    "            if (w2 == 1): \n"
    "                as1 = a/ss\n"
    "                # print('AS region 2: ', as)	\n"
    "                y1 = cs*(1-r)/ss*t1\n"
    "                # print('Y1 region 2: ', y1)	\n"
    "                y2 = cs*(1-r)/ss*t2\n"
    "                # print('Y2 region 2: ', y2)	\n"
    "                x1p = c1/ss\n"
    "                # print('X1p region 2: ', x1p)	\n"
    "                x2p = c2/ss\n"
    "                # print('X2p region 2: ', x2p)	\n"
    "                z = 1\n"
    "                # p, mn, mx, d, d1, d2\n"
    "                p = 0.0\n"
    "                mn = min(x1p + y2, x2p + y1)\n"
    "                mx = max(x1p + y2, x2p + y1)\n"
    "                d1 = min(x2p - x1p, y2 - y1)\n"
    "                d2 = max(x2p - x1p, y2 - y1)\n"
    "                d = z - y1 - x1p\n"
    "                if (z > (x1p + y1) and z < mn):\n"
    "                    p = (d*d)/(2 * (x2p - x1p) * (y2 - y1))\n"
    "\n"
    "                if ((mn <= z) and (z <= mx)):\n"
    "                    p = (d - d1/2)/d2\n"
    "\n"
    "                if ((mx < z) and (z < (x2p + y2))):\n"
    "                    p = 1-math.pow((z-y2-x2p),2)/(2*(x2p-x1p)*(y2-y1))\n"
    "\n"
    "                if (z >= (x2 + y2)):\n"
    "                    p = 1\n"
    "\n"
    "						\n"
    "                # print('X1 region 2: ', x1)	\n"
    "                # print('X2 region 2: ', x2)\n"
    "                # print('P F2s region 2: ', p)	\n"
    "\n"
    "                temp = (x2*as1-1)/(x2*as1-x1*as1)\n"
    "                # print('temp region 2: ', temp)\n"
    "\n"
    "                cdf1 = (x2*as1-1)/(x2*as1-x1*as1)* p\n"
    "                # print('cdf1 region 2: ', cdf1)	\n"
    "                y1 = cs/ss*(1-r)\n"
    "                y2 = cs/ss*(1-x1*as1*r)						\n"
    "                p2 = None\n"
    "\n"
    "                if (x2p < x1p or y2 < y1 or t2 < t1):\n"
    "                    p2 = 1000.0                        \n"
    "                else:\n"
    "                    if (x1p < 0 or y1 < 0 or t1 < 0):\n"
    "                        p2 = 1000.							\n"
    "                    else:\n"
    "                        if (y1 == y2 or t1 == t2):\n"
    "                            y1a = y1*t1\n"
    "                            y2a = y2*t2\n"
    "                            p2 = 0.0\n"
    "                            mn = min(x1p + y2a, x2p + y1a)\n"
    "                            mx = max(x1p + y2a, x2p + y1a)\n"
    "                            d1 = min(x2p - x1p, y2a - y1a)\n"
    "                            d2 = max(x2p - x1p, y2a - y1a)\n"
    "                            d = z - y1a - x1p\n"
    "                            if (z > (x1p + y1a) and z < mn):\n"
    "                                p2 = (d*d)/(2 * (x2p - x1p) * (y2a - y1a))\n"
    "\n"
    "                            if ((mn <= z) and (z <= mx)):\n"
    "                                p2 = (d - d1/2)/d2\n"
    "\n"
    "                            if ((mx < z) and (z < (x2p + y2a))):\n"
    "                                p2 = 1-math.pow((z-y2a-x2p),2)/(2*(x2p-x1p)*(y2a-y1a))\n"
    "\n"
    "                            if (z >= (x2p + y2a)):\n"
    "                                p2 = 1\n"
    "\n"
    "                        else:\n"
    "                            if (x2p == x1p):\n"
    "                                ax = z -x1p\n"
    "                                #  adiv, t\n"
    "                                t = None\n"
    "                                p2 = 0.0\n"
    "                                if ((y1 * t2) > (y2 * t1)):\n"
    "                                    t = y1\n"
    "                                    y1 = t1\n"
    "                                    t1 = t\n"
    "                                    t = y2\n"
    "                                    y2 = t2\n"
    "                                    t2 = t\n"
    "\n"
    "                                adiv = (y2 - y1) * (t2 - t1)\n"
    "                                if ((y1 * t1) < ax and ax <= (y1 * t2)):\n"
    "                                    p2 =   (ax * math.log(ax/(y1 * t1)) - ax + y1 * t1)/adiv\n"
    "\n"
    "                                if ((y1 * t2) < ax and ax <= (y2 * t1)):\n"
    "                                    p2 = (ax * math.log(t2/t1) - (t2 - t1) * y1)/adiv\n"
    "\n"
    "                                if ((y2 * t1) < ax and ax < (y2 * t2)):\n"
    "                                    p2 = (ax * math.log((t2 * y2)/ax) + ax + t1 * y1 - t1 * y2 - t2 * y1)/adiv\n"
    "\n"
    "                                if (ax >= (t2 * y2)):\n"
    "                                    p2 = 1\n"
    "\n"
    "                                \n"
    "                            else:\n"
    "                                ax = z - x1p\n"
    "                                # p3, p4, t, a1, a2, a3, a4, c2, c3, c4, c5 \n"
    "                                p3 = 0.0\n"
    "                                p4 = 0.0\n"
    "                                t = None\n"
    "                                c3 =None\n"
    "                                c4 = None\n"
    "                                if ((y1*t2) > (y2*t1)):\n"
    "                                    t = y1\n"
    "                                    y1 = t1\n"
    "                                    t1 = t\n"
    "                                    t = y2\n"
    "                                    y2 = t2\n"
    "                                    t2 = t\n"
    "\n"
    "                                a1 = y1*t1\n"
    "                                a2 = y1*t2\n"
    "                                a3 = y2*t1\n"
    "                                a4 = y2*t2\n"
    "                                c2 =  - ((a1*a1*math.log(a1/ (y1*t1))/2 - 3*a1*a1/4+y1*t1*a1)/((y2-y1)*(t2-t1)))\n"
    "                                if (a2 == 0):\n"
    "                                    c3  =  0\n"
    "                                else:\n"
    "                                    c3 = ((a2*a2*math.log(a2/(y1*t1))/2 - 3*a2*a2/4+y1*t1*a2)/((y2-y1)*(t2-t1)))+ c2 -((a2*a2*math.log(t2/t1)/2-(t2-t1)*y1*a2)/(y2-y1)*(t2-t1))\n"
    "\n"
    "                                if (a3 == 0):\n"
    "                                    c4  =  0\n"
    "                                else:\n"
    "                                    c4 = ((a3*a3*math.log(t2/t1)/2-(t2-t1)*y1*a3)/(y2-y1)*(t2-t1))+ c3 -((a3*a3*math.log(t2*y2/a3)/2+3*a3*a3/4+t1*y1*a3-t1*y2*a3-t2*y1*a3)/(y2-y1)*(t2-t1))\n"
    "\n"
    "                                c5 = ((a4*a4*math.log(t2*y2/a4)/2+3*a4*a4/4+t1*y1*a4-t1*y2*a4-t2*y1*a4)/(y2-y1)*(t2-t1))+ c4 -(a4)\n"
    "\n"
    "                                if (a1 < ax and ax <= a2):\n"
    "                                    p3 = ((ax*ax*math.log(ax/(y1*t1))/2 - 3*ax*ax/4+y1*t1*ax)/(y2-y1)*(t2-t1))+ c2\n"
    "\n"
    "                                if (a2 < ax and ax <= a3):\n"
    "                                    p3 = ((ax*ax*math.log(t2/t1)/2-(t2-t1)*y1*ax)/(y2-y1)*(t2-t1))+ c3\n"
    "\n"
    "                                if (a3 < ax and ax <  a4):\n"
    "                                    p3 = ((ax*ax*math.log(t2*y2/ax)/2+3*ax*ax/4+t1*y1*ax-t1*y2*ax-t2*y1*ax)/(y2-y1)*(t2-t1))+ c4\n"
    "\n"
    "\n"
    "                                if (ax >= a4):\n"
    "                                    p3 = ax + c5\n"
    "\n"
    "                                ax = z - x2p\n"
    "\n"
    "                                if (a1 < ax and ax <= a2):\n"
    "                                    p4 = ((ax*ax*math.log(ax/(y1*t1))/2 - 3*ax*ax/4+y1*t1*ax)/(y2-y1)*(t2-t1))+ c2\n"
    "\n"
    "                                if (a2 < ax and ax <= a3):\n"
    "                                    p4 = ((ax*ax*math.log(t2/t1)/2-(t2-t1)*y1*ax)/(y2-y1)*(t2-t1))+ c3\n"
    "\n"
    "                                if (a3 < ax and ax <  a4):\n"
    "                                    p4 = ((ax*ax*math.log(t2*y2/ax)/2+3*ax*ax/4+t1*y1*ax-t1*y2*ax-t2*y1*ax)/(y2-y1)*(t2-t1))+ c4\n"
    "\n"
    "\n"
    "                                if (ax >= a4):\n"
    "                                    p4 = ax + c5\n"
    "\n"
    "\n"
    "                                p2 = (p3 - p4) / (x2p - x1p)\n"
    "\n"
    "\n"
    "                cdf2 = (1-x1*as1)/(x2*as1-x1*as1)*p2\n"
    "                # print('cdf2 region 1: ', cdf2)\n"
    "                si = 1-cdf1-cdf2\n"
    "                # print('SI region 2: ', si)\n"
    "\n"
    "            else:\n"
    "\n"
    "                as1 = a/ss\n"
    "                y1 = cs/ss*(1-x2*as1*r)\n"
    "                y2 = cs/ss*(1-x1*as1*r)\n"
    "\n"
    "                x1 = c1/ss\n"
    "                x2 = c2/ss\n"
    "                z = 1\n"
    "\n"
    "                p = None\n"
    "                if (x2 < x1 or y2 < y1 or t2 < t1):\n"
    "                    p = 1000.0                        \n"
    "                else:\n"
    "                    if	(x1 < 0 or y1 < 0 or t1 < 0):\n"
    "                        p = 1000.\n"
    "                    else:\n"
    "                        if (y1 == y2 or t1 == t2):\n"
    "                            y1a = y1*t1\n"
    "                            y2a = y2*t2\n"
    "                            # mn, mx, d, d1, d2   \n"
    "                            p = 0.0\n"
    "                            mn = min(x1 + y2a, x2 + y1a)\n"
    "                            mx = max(x1 + y2a, x2 + y1a)\n"
    "                            d1 = min(x2 - x1, y2a - y1a)\n"
    "                            d2 = max(x2 - x1, y2a - y1a)\n"
    "                            d = z - y1a - x1\n"
    "                            if (z > (x1 + y1a) and z < mn):\n"
    "                                p = (d*d)/(2 * (x2 - x1) * (y2a - y1a))\n"
    "\n"
    "\n"
    "                            if ((mn <= z) and (z <= mx)):\n"
    "                                p = (d - d1/2)/d2\n"
    "\n"
    "                            if ((mx < z) and (z < (x2 + y2a))):\n"
    "                                p = 1-math.pow((z-y2a-x2),2)/(2*(x2-x1)*(y2a-y1a))\n"
    "\n"
    "                            if (z >= (x2 + y2a)):\n"
    "                                p = 1\n"
    "\n"
    "                        else:\n"
    "                            if (x2 == x1):\n"
    "                                ax = z -x1\n"
    "                                # adiv, t \n"
    "                                t = None\n"
    "                                p = 0.0\n"
    "                                if (y1 * t2 > y2 * t1):\n"
    "                                    t = y1\n"
    "                                    y1 = t1\n"
    "                                    t1 = t\n"
    "                                    t = y2\n"
    "                                    y2 = t2\n"
    "                                    t2 = t\n"
    "\n"
    "\n"
    "                                adiv = (y2 - y1) * (t2 - t1)\n"
    "\n"
    "                                if ((y1 * t1) < ax and ax <= (y1 * t2)):\n"
    "                                    p =   (ax * math.log(ax/(y1 * t1)) - ax + y1 * t1)/adiv\n"
    "\n"
    "                                if ((y1 * t2) < ax and ax <= (y2 * t1)):\n"
    "                                    p = (ax * math.log(t2/t1) - (t2 - t1) * y1)/adiv\n"
    "\n"
    "                                if ((y2 * t1) < ax and ax < (y2 * t2)):\n"
    "                                    p = (ax * math.log((t2 * y2)/ax) + ax + t1 * y1 - t1 * y2 - t2 * y1)/adiv\n"
    "\n"
    "                                if (ax >= (t2 * y2)):\n"
    "                                    p = 1\n"
    "\n"
    "\n"
    "                            else:\n"
    "                                ax = z - x1\n"
    "                                # p3, p4, t, a1, a2, a3, a4, c2, c3, c4, c5 \n"
    "                                p3 = 0.0\n"
    "                                p4 = 0.0\n"
    "                                t = None\n"
    "                                c3 = None\n"
    "                                c4 = None	\n"
    "                                if (y1*t2 > y2*t1):\n"
    "                                    t = y1\n"
    "                                    y1 = t1\n"
    "                                    t1 = t\n"
    "                                    t = y2\n"
    "                                    y2 = t2\n"
    "                                    t2 = t\n"
    "\n"
    "\n"
    "                                a1 = y1*t1\n"
    "                                a2 = y1*t2\n"
    "                                a3 = y2*t1\n"
    "                                a4 = y2*t2\n"
    "                                c2 =  - ((a1*a1*math.log(a1/(y1*t1))/2 - 3*a1*a1/4+y1*t1*a1)/((y2-y1)*(t2-t1)))\n"
    "\n"
    "                                if (a2 == 0):\n"
    "                                    c3  =  0\n"
    "                                else:\n"
    "                                    c3 = ((a2*a2*math.log(a2/(y1*t1))/2 - 3*a2*a2/4+y1*t1*a2)/((y2-y1)*(t2-t1)))+ c2 -((a2*a2*math.log(t2/t1)/2-(t2-t1)*y1*a2)/(y2-y1)*(t2-t1))\n"
    "\n"
    "\n"
    "                                if (a3 == 0):\n"
    "                                    c4  =  0\n"
    "                                else:\n"
    "                                    c4 = ((a3*a3*math.log(t2/t1)/2-(t2-t1)*y1*a3)/(y2-y1)*(t2-t1))+ c3 -((a3*a3*math.log(t2*y2/a3)/2+3*a3*a3/4+t1*y1*a3-t1*y2*a3-t2*y1*a3)/(y2-y1)*(t2-t1))\n"
    "\n"
    "\n"
    "                                c5 = ((a4*a4*math.log(t2*y2/a4)/2+3*a4*a4/4+t1*y1*a4-t1*y2*a4-t2*y1*a4)/(y2-y1)*(t2-t1))+ c4 -(a4)\n"
    "\n"
    "                                if (a1 < ax and ax <= a2):\n"
    "                                    p3 = ((ax*ax*math.log(ax/(y1*t1))/2 - 3*ax*ax/4+y1*t1*ax)/(y2-y1)*(t2-t1))+ c2\n"
    "\n"
    "                                if (a2 < ax and ax <= a3):\n"
    "                                    p3 = ((ax*ax*math.log(t2/t1)/2-(t2-t1)*y1*ax)/(y2-y1)*(t2-t1))+ c3\n"
    "\n"
    "                                if (a3 < ax and ax <  a4):\n"
    "                                    p3 = ((ax*ax*math.log(t2*y2/ax)/2+3*ax*ax/4+t1*y1*ax-t1*y2*ax-t2*y1*ax)/(y2-y1)*(t2-t1))+ c4\n"
    "\n"
    "                                if (ax >= a4):\n"
    "                                    p3 = ax + c5\n"
    "\n"
    "\n"
    "                                ax = z - x2\n"
    "\n"
    "                                if (a1 < ax and ax <= a2):\n"
    "                                    p4 = ((ax*ax*math.log(ax/(y1*t1))/2 - 3*ax*ax/4+y1*t1*ax)/(y2-y1)*(t2-t1))+ c2\n"
    "\n"
    "                                if (a2 < ax and ax <= a3):\n"
    "                                    p4 = ((ax*ax*math.log(t2/t1)/2-(t2-t1)*y1*ax)/(y2-y1)*(t2-t1))+ c3\n"
    "\n"
    "                                if (a3 < ax and ax <  a4):\n"
    "                                    p4 = ((ax*ax*math.log(t2*y2/ax)/2+3*ax*ax/4+t1*y1*ax-t1*y2*ax-t2*y1*ax)/(y2-y1)*(t2-t1))+ c4\n"
    "\n"
    "                                if (ax >= a4):\n"
    "                                    p4 = ax + c5\n"
    "\n"
    "\n"
    "                                p = (p3 - p4) / (x2 - x1)\n"
    "\n"
    "\n"
    "                si = 1- p\n"
    "                # print('SI region 3: ', si)\n"
    "\n"
    "\n"
    "if (si > 10.0):\n"
    "    si = 10.0\n"
    "\n"
    "# print('SI FINAL = ',si)	\n"
    "# print(' ')\n"
    "\n"
    "return si\n"
    "\n"
    "# FIM\n"
    "\n";
}

void NovaFriburgoTs::AnalysisTS()
{
    terrama2::core::registerFactories();

    terrama2::services::analysis::core::PythonInterpreterInit pythonInterpreterInit;

    {

      using namespace terrama2::services::analysis::core;

      auto& serviceManager = terrama2::core::ServiceManager::getInstance();
      auto dataManager = std::make_shared<terrama2::services::analysis::core::DataManager>();

      // Starts the service and adds the analysis

      auto loggerCopy = std::make_shared<terrama2::core::MockAnalysisLogger>();

      EXPECT_CALL(*loggerCopy, setConnectionInfo(::testing::_)).WillRepeatedly(::testing::Return());
      EXPECT_CALL(*loggerCopy, setTableName(::testing::_)).WillRepeatedly(::testing::Return());
      EXPECT_CALL(*loggerCopy, getLastProcessTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
      EXPECT_CALL(*loggerCopy, getDataLastTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
      EXPECT_CALL(*loggerCopy, done(::testing::_, ::testing::_)).WillRepeatedly(::testing::Return());
      EXPECT_CALL(*loggerCopy, start(::testing::_)).WillRepeatedly(::testing::Return(0));
      EXPECT_CALL(*loggerCopy, isValid()).WillRepeatedly(::testing::Return(true));

      auto logger = std::make_shared<terrama2::core::MockAnalysisLogger>();

      EXPECT_CALL(*logger, setConnectionInfo(::testing::_)).WillRepeatedly(::testing::Return());
      EXPECT_CALL(*logger, setTableName(::testing::_)).WillRepeatedly(::testing::Return());
      EXPECT_CALL(*logger, getLastProcessTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
      EXPECT_CALL(*logger, getDataLastTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
      EXPECT_CALL(*logger, done(::testing::_, ::testing::_)).WillRepeatedly(::testing::Return());
      EXPECT_CALL(*logger, start(::testing::_)).WillRepeatedly(::testing::Return(0));
      EXPECT_CALL(*logger, clone()).WillRepeatedly(::testing::Return(loggerCopy));
      EXPECT_CALL(*logger, isValid()).WillRepeatedly(::testing::Return(true));

      te::core::URI uri("pgsql://"+TERRAMA2_DATABASE_USERNAME+":"+TERRAMA2_DATABASE_PASSWORD+"@"+TERRAMA2_DATABASE_HOST+":"+TERRAMA2_DATABASE_PORT+"/"+TERRAMA2_DATABASE_DBNAME);
      logger->setConnectionInfo(uri);


      Service service(dataManager);
      serviceManager.setInstanceId(1);
      serviceManager.setLogger(logger);
      serviceManager.setLogConnectionInfo(te::core::URI(uri));
      serviceManager.setInstanceId(1);

      service.setLogger(logger);
      service.start();


      auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();

      /////////////////////////////////////////////////////////////////////////////////////////////////////////
      //     input
      //Servidor de entrada: Dado Coletado, Arquivo de Referencia, Arquivos de modelo de terreno
      //DataProvider information
      ///////////////////////////////////////////////////////////////////////////////////////////////////////

      auto dataProvider = std::make_shared<terrama2::core::DataProvider>();
      dataProvider->id = 1;
      dataProvider->name = "Servidor de Entrada";
      dataProvider->uri = "file://"+TERRAMA2_DATA_DIR+"/";
      dataProvider->dataProviderType = "FILE";
      dataProvider->active = true;

      dataManager->add(dataProvider);

      //semantica
      // DataSeries information
      std::shared_ptr<terrama2::core::DataSeries> dataSeries = std::make_shared<terrama2::core::DataSeries>();
      dataSeries->id = 1;
      dataSeries->name = "Hidro";
      dataSeries->semantics = semanticsManager.getSemantics("GRID-geotiff");
      dataSeries->dataProviderId = dataProvider->id;
      dataSeries->active = true;

      //dado dinamico = hidro_2011
      std::shared_ptr<terrama2::core::DataSetGrid> dataSet = std::make_shared<terrama2::core::DataSetGrid>();
      dataSet->id = 1;
      dataSet->active = true;
      dataSet->format.emplace("mask", "hidro_diario_jan2011/S11216377_%YYYY%MM%DD%hh%mm.tif");
      dataSet->format.emplace("timezone", "UTC+00");

      dataSeries->datasetList.emplace_back(dataSet);

      dataManager->add(dataProvider);

      //ANÁLISE DATA SERIES = hidro_2011

      AnalysisDataSeries hidro;
      hidro.id = 13;
      hidro.alias = "hidro_2011";
      hidro.dataSeriesId = dataProvider->id;
      hidro.type = AnalysisDataSeriesType::ADDITIONAL_DATA_TYPE;



      // semantica e DataSeries information  - SRTM_a_latlong_sad69

      std::shared_ptr<terrama2::core::DataSeries> dataSeriesA = std::make_shared<terrama2::core::DataSeries>();
      dataSeriesA->id = 2;
      dataSeriesA->name = "SRTM_a_latlong_sad69";
      dataSeriesA->semantics = semanticsManager.getSemantics("GRID-static_gdal");
      dataSeriesA->dataProviderId = dataProvider->id;
      dataSeriesA->active = true;

      //dado estatico  - SRTM_a_latlong_sad69
      std::shared_ptr<terrama2::core::DataSetGrid> dataSetA = std::make_shared<terrama2::core::DataSetGrid>();
      dataSetA->id = 2;
      dataSetA->active = true;
      dataSetA->format.emplace("mask", "Rio_Friburgo/SRTM_a_latlong_sad69.tif");

      dataSeriesA->datasetList.emplace_back(dataSetA);
      dataManager->add(dataSeriesA);


      //ANÁLISE DATA SERIES = SRTM_a_latlong_sad69

      AnalysisDataSeries SRTM_a_latlong_sad69;
      SRTM_a_latlong_sad69.id = 2;
      SRTM_a_latlong_sad69.alias = "SRTM_a_latlong_sad69";
      SRTM_a_latlong_sad69.dataSeriesId = dataSeriesA->id;
      SRTM_a_latlong_sad69.type = AnalysisDataSeriesType::ADDITIONAL_DATA_TYPE;



      ////////////////////////////////////////////////////////////
      // semantica e DataSeries information - SRTM_s_latlong_sad69
      ////////////////////////////////////////////////////////////

      std::shared_ptr<terrama2::core::DataSeries> dataSeriesS = std::make_shared<terrama2::core::DataSeries>();
      dataSeriesS->id = 7;
      dataSeriesS->name = "SRTM_s_latlong_sad69";
      dataSeriesS->semantics = semanticsManager.getSemantics("GRID-static_gdal");
      dataSeriesS->dataProviderId = dataProvider->id;
      dataSeriesS->active = true;

      dataManager->add(dataSeriesS);

      //dado estatico  - SRTM_s_latlong_sad69

      std::shared_ptr<terrama2::core::DataSetGrid> dataSetS = std::make_shared<terrama2::core::DataSetGrid>();
      dataSetS->id = 8;
      dataSetS->active = true;
      dataSetS->format.emplace("mask", "Rio_Friburgo/SRTM_s_latlong_sad69.tif");

      dataSeriesS->datasetList.emplace_back(dataSetS);

      //ANÁLISE DATA SERIES = SRTM_s_latlong_sad69

      AnalysisDataSeries SRTM_s_latlong_sad69;
      SRTM_s_latlong_sad69.id = 15;
      SRTM_s_latlong_sad69.alias = "SRTM_s_latlong_sad69";
      SRTM_s_latlong_sad69.dataSeriesId = dataSetS->id;
      SRTM_s_latlong_sad69.type = AnalysisDataSeriesType::ADDITIONAL_DATA_TYPE;


      ///////////////////////////////
      // Dados de saída
      //////////////////////////////

      // DataSeries information - Saida
      std::shared_ptr<terrama2::core::DataSeries> outputDataSeries = std::make_shared<terrama2::core::DataSeries>();
      outputDataSeries->id = 31;
      outputDataSeries->name = "Análise Saida";
      outputDataSeries->semantics = semanticsManager.getSemantics("GRID-geotiff");
      outputDataSeries->dataProviderId = dataProvider->id;
      outputDataSeries->active = true;
      dataManager->add(outputDataSeries);


      // DataSet information
      terrama2::core::DataSet* outputDataSet = new terrama2::core::DataSet();
      outputDataSet->active = true;
      outputDataSet->id = 30;
      outputDataSet->dataSeriesId = outputDataSeries->id;
      outputDataSet->format.emplace("timezone", "UTC+00");
      outputDataSet->format.emplace("srid", "4618");
      outputDataSet->format.emplace("mask", "AnResultTestAutoNF/FS_%YYYY%MM%DD_%hh%mm.tif");

      outputDataSeries->datasetList.emplace_back(outputDataSet);

      std::shared_ptr<terrama2::services::analysis::core::Analysis> analysis = std::make_shared<terrama2::services::analysis::core::Analysis>();
      analysis->id = 9;
      analysis->name = "An_FS_SINMAP";
      analysis->script = scriptAnalise();
      analysis->scriptLanguage = ScriptLanguage::PYTHON;
      analysis->type = AnalysisType::GRID_TYPE;
      analysis->active = true;
      analysis->outputDataSeriesId = outputDataSeries->id;
      analysis->outputDataSetId = outputDataSet->id;
      analysis->serviceInstanceId = serviceManager.instanceId();
      dataManager->add(analysis);

      analysis->schedule.frequency = 24;
      analysis->schedule.frequencyUnit = "hours";


      //Saída em tiff
      std::shared_ptr<AnalysisOutputGrid> outputGrid = std::make_shared<AnalysisOutputGrid>();

      outputGrid->analysisId = analysis->id;
      outputGrid->interpolationMethod = InterpolationMethod::NEARESTNEIGHBOR;
      outputGrid->interestAreaType = InterestAreaType::SAME_FROM_DATASERIES;
      outputGrid->interestAreaDataSeriesId =  SRTM_a_latlong_sad69.id;
      outputGrid->resolutionType = ResolutionType::SAME_FROM_DATASERIES;
      outputGrid->resolutionDataSeriesId =  SRTM_a_latlong_sad69.id;
      outputGrid->interpolationDummy = 0;

      analysis->outputGridPtr = outputGrid;


      //Add as analises dados dinamicos e dados estaticos
      std::vector<AnalysisDataSeries> analysisDataSeriesList;
      analysisDataSeriesList.push_back(hidro);
      analysisDataSeriesList.push_back(SRTM_a_latlong_sad69);
      analysisDataSeriesList.push_back(SRTM_s_latlong_sad69);

      analysis->analysisDataSeriesList = analysisDataSeriesList;

      service.addToQueue(analysis->id, terrama2::core::TimeUtils::stringToTimestamp("2011-01-11T12:00:00.000-02:00", terrama2::core::TimeUtils::webgui_timefacet));


      QTimer timer;
      QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
      timer.start(10000);
      QCoreApplication::exec();


      QDir outputFolder(QString::fromStdString(TERRAMA2_DATA_DIR +"/" + "AnResultTestAutoNF"));
      QImage output(outputFolder.absolutePath()+"/FS_20110111_1400.tif");
      QImage reference(QString::fromStdString(TERRAMA2_DATA_DIR +"/referenciaNF/FS_20110111_1400.tif"));
      QVERIFY(reference == output);

      outputFolder.removeRecursively();
    }
}
